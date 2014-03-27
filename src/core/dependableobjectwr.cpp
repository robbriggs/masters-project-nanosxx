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

using namespace nanos;

DOWorkRepresentation::DOWorkRepresentation(const unsigned char llvmir_start[], const unsigned char llvmir_end[], const unsigned char llvm_function[])
	: _llvmir_start(llvmir_start), _llvmir_end(llvmir_end)
{
	if (llvmir_start == NULL)
		return;

	_func_name = std::string((const char*)llvm_function);

	if (llvmir_start == llvmir_end)
		std::cout << "WARNING: llvmir_start == llvmir_end\n";

	// Parse IR text
	std::string ir(llvmir_start, llvmir_end);
	llvm::SMDiagnostic err;
	llvm::MemoryBuffer *buf = llvm::MemoryBuffer::getMemBuffer(ir);

	_module = ParseIR(buf, err, _context);

	std::stringstream ss;
	ss << _func_name << "_unpacked";
	_llvm_func = _module->getFunction(ss.str());
	_table = &_llvm_func->getValueSymbolTable();
	
	//std::cout << "*****\n******\n*******\nLooking for " << _func_name << std::endl;
}

DOWorkRepresentation::DOWorkRepresentation(const DOWorkRepresentation &work_representation)
{

}

DOWorkRepresentation::~DOWorkRepresentation()
{

}

void DOWorkRepresentation::JITCompile(void *data)
{
	hardcode(data);
}
/*
static llvm::Constant *create_int_element(llvm::Type &element_type, void *data)
{
	printf("data : %p\n",data);
	int *cast_data = (int *)data;
	printf("cast_data : %p\n", cast_data);
	uint64_t value = static_cast<uint64_t>(*cast_data);
	return llvm::ConstantInt::get(&element_type, *cast_data);
}

template <typename T>
static inline T *dereference_type(T *ptr_type)
{
	typename T::element_iterator it = ptr_type->element_begin();
	return static_cast<T *>(*it);
}

static std::string add_global_var(llvm::Module* module, llvm::StructType *type, void *data)
{
	std::vector<llvm::Constant *> struct_internal;
	std::cout <<"About to deref\n";
	llvm::StructType *struct_type = dereference_type(type);
	const unsigned struct_size = struct_type->getNumElements();

	llvm::StructType::element_iterator element_type_it = struct_type->element_begin();
	for (unsigned i = 0; i < struct_size; ++i, element_type_it++)
	{
		std::cout <<"Loop start\n";
		element_type->dump();
		llvm::Type *element_type = *element_type_it;
		const llvm::Type::TypeID id = element_type->getTypeID();
	}*/
	/*
	std::vector<llvm::Constant *> struct_internal;
	llvm::StructType::element_iterator ptr_it = type->element_begin();
	llvm::StructType *element = (llvm::StructType*)*ptr_it;
	llvm::StructType::element_iterator it = element->element_begin();
	const unsigned struct_size = element->getNumElements();

	void **data_ptr = (void **)data;

	for (unsigned i = 0; i < struct_size; ++i, it++)
	{
		llvm::Type &element_type = **it;
		element_type.dump();
		const llvm::Type::TypeID id = element_type.getTypeID();
		std::cout << "TYPE IS " << id << std::endl;
		llvm::Constant *new_value;

		switch (id)
		{
		case(llvm::Type::IntegerTyID):
			new_value = create_int_element(element_type, data);
		default:
			std::cout << "Type Not found\n";
		}
		new_value->dump();

	}

	llvm::Twine name("____hard_data____");
	llvm::ArrayRef<llvm::Constant *> arrayref(struct_internal);
	llvm::GlobalVariable *global = new llvm::GlobalVariable(*module, (llvm::Type *)type, true, llvm::GlobalValue::CommonLinkage, *arrayref.data(), name);
	return "hard_data";*/
//		return "hard_data";
//}



template <typename T>
static inline T *dereference_type(T *ptr_type)
{
	typename T::element_iterator it = ptr_type->element_begin();
	return static_cast<T *>(*it);
}

static inline llvm::Value *argumentToValue(llvm::ValueSymbolTable *table, llvm::Argument &arg)
{
	const llvm::StringRef name = arg.getName();
	return table->lookup(name);
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
std::cout << "It is " << id << "I thought " << llvm::Type::IntegerTyID << "\n";
	switch (id)
	{
	case llvm::Type::IntegerTyID:
		//int *cast_data = (int *)data;
	std::cout << "It gets here\n";
		return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 99/**cast_data*/);
		break;
	default:
		std::cout << "AHHHHH\n";
		break;
	}
}

inline void DOWorkRepresentation::storeConstantToPtr(llvm::IRBuilder<> &builder, NameGenerator &name, llvm::Argument &arg, void *data)
{
	llvm::Value *arg_value = argumentToValue(_table, arg);
	llvm::Value *X = createAlignedAlloca(builder, arg, 8, name.next());
	createAlignedStore(builder, arg_value, X, 8);
	llvm::Value *Xptr = createAlignedLoad(builder, X, 8, name.next());
	llvm::Value *constant = generateConstantValue(_context, arg, data);
	createAlignedStore(builder, constant, Xptr, 8);
}

static void optimise(llvm::Module *module)
{
	llvm::PassManager PM;
	llvm::PassManagerBuilder Builder;
	Builder.OptLevel = 1;
	Builder.DisableUnitAtATime = false;
	Builder.DisableUnrollLoops = false;
	Builder.populateModulePassManager(PM);
	PM.run(*module);
}

void DOWorkRepresentation::hardcode(void *data)
{
	if (_llvmir_start == NULL)
		return;

	
	_llvm_func->dump();

	NameGenerator name("_h");

	llvm::FunctionType *func_type = _llvm_func->getFunctionType();
	llvm::ValueSymbolTable &table = _llvm_func->getValueSymbolTable();

	llvm::Argument *arg_it = _llvm_func->arg_begin();
	


//{
	llvm::IRBuilder<> builder(_llvm_func->begin()->begin()); // Must destruct at end
	storeConstantToPtr(builder, name, *arg_it, data);

//}
	
	optimise(_module);
   llvm::verifyFunction(*_llvm_func);
   //_module->dump();
   _llvm_func->dump();
 
}