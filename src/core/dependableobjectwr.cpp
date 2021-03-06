/*************************************************************************************/
/*      Copyright 2009 Barcelona Supercomputing Center                               */
/*                                                                                   */
/*      This file is part of the NANOS++ library.                                    */
/*                                                                                   */
/*      NANOS++ is free software: you can redistribute it and/or modify              */
/*      it under the terms of the GNU Lesser General Public License as published by  */
/*      the Free Software Foundation, either version 3 of the License, or            */
/*      (at your option) any later version.                                          */
/*                                                                                   */
/*      NANOS++ is distributed in the hope that it will be useful,                   */
/*      but WITHOUT ANY WARRANTY; without even the implied warranty of               */
/*      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                */
/*      GNU Lesser General Public License for more details.                          */
/*                                                                                   */
/*      You should have received a copy of the GNU Lesser General Public License     */
/*      along with NANOS++.  If not, see <http://www.gnu.org/licenses/>.             */
/*************************************************************************************/

#include "dependableobjectwr.hpp"

#include "llvm/GlobalVariable.h"
#include "llvm/Type.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include <iostream>
#include <sstream>

#include <stdio.h>

using namespace nanos;

static inline void InitializeLLVM()
{
	static bool done_before = false;
	if (!done_before)
	{
		llvm::InitializeNativeTarget();
	}
}

DOWorkRepresentation::DOWorkRepresentation(const unsigned char llvmir_start[], const unsigned char llvmir_end[], const unsigned char llvm_function[])
	: _llvmir_start(llvmir_start), _llvmir_end(llvmir_end)
{
	if (llvmir_start == NULL)
		return;

	if (llvmir_start == llvmir_end)
		std::cout << "WARNING: llvmir_start == llvmir_end\n";

	InitializeLLVM();

	// Parse IR text
	std::string ir(llvmir_start, llvmir_end);

	llvm::SMDiagnostic err;
	llvm::MemoryBuffer *buf = llvm::MemoryBuffer::getMemBuffer(ir);
	_module = ParseIR(buf, err, _context);

	 _packed_name = std::string((const char*)llvm_function);
	std::stringstream ss;
	ss << _packed_name << "_unpacked";
	_unpacked_name = ss.str();

	llvm::verifyModule(*_module);
	detectPointerArguments();
}

template <typename T>
static inline T *dereference_type(T *ptr_type)
{
	typename T::element_iterator it = ptr_type->element_begin();
	return static_cast<T *>(*it);
}

void DOWorkRepresentation::detectPointerArguments()
{
	llvm::Function *packed_func = _module->getFunction(_packed_name);
	const llvm::Argument &arg = *(packed_func->arg_begin());
	const llvm::Type *arg_type = arg.getType();
	const llvm::Type *struct_type = arg_type->getContainedType(0);
	const llvm::StructType *struct_t = llvm::cast<llvm::StructType>(struct_type);

	for (llvm::StructType::element_iterator it = struct_t->element_begin(), E = struct_t->element_end(); it != E; ++it)
		_isPointerArgument.push_back((*it)->isPointerTy());
}

DOWorkRepresentation::DOWorkRepresentation(const DOWorkRepresentation &work_representation)
{

}

DOWorkRepresentation::~DOWorkRepresentation()
{

}

static inline llvm::Value *argumentToValue(llvm::ValueSymbolTable &table, llvm::Argument &arg)
{
	const llvm::StringRef name = arg.getName();
	return table.lookup(name);
} 

static inline llvm::Value *createAlignedAlloca(llvm::IRBuilder<> &builder, llvm::Type *type,
                                       unsigned align, const llvm::Twine &name)
{
	llvm::AllocaInst *inst = builder.CreateAlloca(type, 0, name);
	inst->setAlignment(align);
	return inst;
}

static inline llvm::Value *createAlignedAlloca(llvm::IRBuilder<> &builder, llvm::Argument &arg,
                                               unsigned align, const llvm::Twine &name)
{
	return createAlignedAlloca(builder, arg.getType(), align, name);
}

static inline llvm::Value *createAlignedStore(llvm::IRBuilder<> &builder, llvm::Value *value,
                                              llvm::Value *ptr, unsigned align)
{
	llvm::StoreInst *inst = builder.CreateStore(value, ptr);
	inst->setAlignment(align);
	return inst;
}

static inline llvm::Value *createAlignedLoad(llvm::IRBuilder<> &builder, llvm::Value *value,
                                      unsigned align, const llvm::Twine &name)
{
	llvm::LoadInst *inst = builder.CreateLoad(value, name); 
	inst->setAlignment(align);
	return inst;
}

static void unsupportedTypeMsg(const char *type)
{
	std::cout << "Error: Type " << type << " is not supported\n";
}

static void unsupportedWidthMsg(const char *type, unsigned width)
{
	std::cout << "Error: Type " << type << " with width " << width << " is not supported\n";
}

#define UNSUPPORTEDTYPE(type) \
	case llvm::Type::type: \
		unsupportedTypeMsg(#type); \
		break;

static inline llvm::Value *generateConstantValue(llvm::LLVMContext &context, llvm::Argument &arg, void *data, bool ptr)
{
	const llvm::Type *type = arg.getType()->getContainedType(0);
	const llvm::Type::TypeID id = type->getTypeID();

	switch (id)
	{
		case llvm::Type::FloatTyID:
		{
			float value = (ptr) ? (**(float **)data) : (*(float *)data);
			return llvm::ConstantInt::get(llvm::Type::getFloatTy(context), value);
		}
		break;
		case llvm::Type::DoubleTyID:
		{
			double value = (ptr) ? (**(double **)data) : (*(double *)data);
			return llvm::ConstantInt::get(llvm::Type::getDoubleTy(context), value);
		}
		break;
		case llvm::Type::IntegerTyID:
		{
			const llvm::IntegerType *int_type = llvm::cast<llvm::IntegerType>(type);
			const unsigned width = int_type->getBitWidth();

			double value = (ptr) ? (**(int **)data) : (*(int *)data);

			if (width == 32){
				return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), value);}
			else if (width == 64){
				return llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), value);}
			else
				unsupportedWidthMsg("Integer", width);
		}
		break;
		UNSUPPORTEDTYPE(VoidTyID)
		UNSUPPORTEDTYPE(X86_FP80TyID)
		UNSUPPORTEDTYPE(FP128TyID)
		UNSUPPORTEDTYPE(PPC_FP128TyID)
		UNSUPPORTEDTYPE(LabelTyID)
		UNSUPPORTEDTYPE(MetadataTyID)
		UNSUPPORTEDTYPE(X86_MMXTyID)
		UNSUPPORTEDTYPE(FunctionTyID)
		UNSUPPORTEDTYPE(StructTyID)
		UNSUPPORTEDTYPE(ArrayTyID)
		UNSUPPORTEDTYPE(PointerTyID)
		UNSUPPORTEDTYPE(VectorTyID)
		UNSUPPORTEDTYPE(NumTypeIDs)
	}
	return NULL;
}

static inline void storeConstantToPtr(llvm::IRBuilder<> &builder, llvm::ValueSymbolTable &table,
                                      llvm::LLVMContext &context, DOWorkRepresentation::NameGenerator &name,
                                      llvm::Argument &arg, void *data, bool ptr)
{
	llvm::Value *arg_value = argumentToValue(table, arg);
	llvm::Value *X = createAlignedAlloca(builder, arg, 8, name.next());
	createAlignedStore(builder, arg_value, X, 8);
	llvm::Value *Xptr = createAlignedLoad(builder, X, 8, name.next());
	llvm::Value *constant = generateConstantValue(context, arg, data, ptr);
	createAlignedStore(builder, constant, Xptr, 8);
}

static void optimise(llvm::Module *module)
{
	llvm::PassManager PM;
	llvm::PassManagerBuilder Builder;
	Builder.OptLevel = 1;
	Builder.populateModulePassManager(PM);
	PM.run(*module);
}

typedef llvm::Function::arg_iterator arg_iterator;

static inline void hardcode(llvm::Module *module, llvm::Function *function, void *data, std::vector<char> *satisfiedArguments, std::vector<bool> &_isPointerArgument)
{
	void **data_it = static_cast<void **>(data);
	DOWorkRepresentation::NameGenerator name("_");
	llvm::LLVMContext &context = function->getContext();
	llvm::IRBuilder<> builder(function->begin()->begin());
	llvm::ValueSymbolTable &table = function->getValueSymbolTable();

	int i;
	for (arg_iterator it = function->arg_begin(), E = function->arg_end(); it != E; ++it, ++data_it, ++i){

		if (satisfiedArguments == NULL || static_cast<bool>(satisfiedArguments->at(i)))
			storeConstantToPtr(builder, table, context, name, *it, data_it, _isPointerArgument.at(i));
	}

	optimise(module);
	llvm::verifyFunction(*function);
}

static inline bool canRunNow(llvm::Function *function, std::vector<char> &satisfiedArguments)
{
	llvm::ValueSymbolTable &table = function->getValueSymbolTable();
	arg_iterator args = function->arg_begin();

	int arg_num = 0;
	int satisfied_count = 0;
	for (std::vector<char>::iterator it = satisfiedArguments.begin(), E = satisfiedArguments.end(); it != E; ++it, ++arg_num)
	{
		if (*it == 0)
		{
			llvm::Value *arg_value = argumentToValue(table, *args);
			if (arg_value->use_empty())
			{
				// Not used, so can prune
				satisfiedArguments[arg_num] = 1;
				++satisfied_count;
			}
		} else {
			++satisfied_count;
		}
		args++;
	}

	bool result = (satisfied_count == arg_num );
	return result;
}

static inline DOWorkRepresentation::JITFunc doJIT(llvm::Module *module, llvm::Function *function)
{
	std::string Error;
	llvm::ExecutionEngine *EE = llvm::ExecutionEngine::createJIT(module, &Error);
	if (!EE) {
		llvm::errs() << "unable to make execution engine: " << Error << "\n";
	}

	void* void_func = EE->getPointerToFunction(function);
	return reinterpret_cast<DOWorkRepresentation::JITFunc>(void_func);
}

DOWorkRepresentation::JITFunc DOWorkRepresentation::JITCompile(void *data, std::vector<char> *satisfiedArguments)
{
	if (_llvmir_start == NULL)
		return NULL;

	llvm::Module *module = llvm::CloneModule(_module);
	llvm::Function *unpacked_func = module->getFunction(_unpacked_name);
	llvm::Function *packed_func = module->getFunction(_packed_name);
	hardcode(module, unpacked_func, data, satisfiedArguments, _isPointerArgument);
	JITFunc generated_function = doJIT(module, packed_func);
	return generated_function;
}