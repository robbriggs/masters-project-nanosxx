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
	if (llvmir_start == NULL){
		std::cout << "++ Starting DOWR with NO LLVM\n";
		return;
	} else {
		std::cout << "++ Starting DOWR with LLVM\n";
	}

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
	std::cout << "End of construct\n";
	
	//std::cout << "*****\n******\n*******\nLooking for " << _func_name << std::endl;
}

DOWorkRepresentation::DOWorkRepresentation(const DOWorkRepresentation &work_representation)
{

}

DOWorkRepresentation::~DOWorkRepresentation()
{

}

template <typename T>
static inline T *dereference_type(T *ptr_type)
{
	typename T::element_iterator it = ptr_type->element_begin();
	return static_cast<T *>(*it);
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

static inline llvm::Value *generateConstantValue(llvm::LLVMContext &context, llvm::Argument &arg, void *data)
{
	const llvm::Type *type = arg.getType()->getContainedType(0);
	const llvm::Type::TypeID id = type->getTypeID();

	switch (id)
	{
	case llvm::Type::IntegerTyID:
		return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 99/**cast_data*/);
		break;
	default:
		std::cout << "WARNING: Type not known in generateConstantValue() for \n";
		break;
	}

	return NULL;
}

static inline void storeConstantToPtr(llvm::IRBuilder<> &builder, llvm::ValueSymbolTable &table, llvm::LLVMContext &context, DOWorkRepresentation::NameGenerator &name, llvm::Argument &arg, void *data)
{
	llvm::Value *arg_value = argumentToValue(table, arg);
	llvm::Value *X = createAlignedAlloca(builder, arg, 8, name.next());
	createAlignedStore(builder, arg_value, X, 8);
	llvm::Value *Xptr = createAlignedLoad(builder, X, 8, name.next());
	llvm::Value *constant = generateConstantValue(context, arg, data);
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

static inline void hardcode(llvm::Module *module, llvm::Function *function, void *data)
{std::cout << "In hardcode\n";
	DOWorkRepresentation::NameGenerator name("_");
	llvm::LLVMContext &context = function->getContext();
	llvm::FunctionType *func_type = function->getFunctionType();
	llvm::IRBuilder<> builder(function->begin()->begin());
	llvm::ValueSymbolTable &table = function->getValueSymbolTable();

	for (arg_iterator it = function->arg_begin(), E = function->arg_end(); it != E; ++it) 
		storeConstantToPtr(builder, table, context, name, *it, data);
	
	optimise(module);
   llvm::verifyFunction(*function);
}

static inline DOWorkRepresentation::JITFunc doJIT(llvm::Module *module, llvm::Function *function)
{
	std::string Error;
	llvm::ExecutionEngine *EE = llvm::ExecutionEngine::createJIT(module, &Error);
	if (!EE) {
		std::cout << "\n\n\n\n\n\n\n!!!!!!!!!!\n";
		llvm::errs() << "unable to make execution engine: " << Error << "\n";
	}

	void* void_func = EE->getPointerToFunction(function);
	return reinterpret_cast<DOWorkRepresentation::JITFunc>(void_func);
}

void DOWorkRepresentation::JITCompile(void *data)
{
	if (_llvmir_start == NULL){
		std::cout << "-- Hardcode scared as no llvm_start";
		return;
	}

	llvm::Module *module = llvm::CloneModule(_module);
	llvm::Function *unpacked_func = module->getFunction(_unpacked_name);
	llvm::Function *packed_func = module->getFunction(_packed_name);
	hardcode(module, unpacked_func, data);
	JITFunc generated_function = doJIT(module, packed_func);
}