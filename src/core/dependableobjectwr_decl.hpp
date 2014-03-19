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

#ifndef _NANOS_DEPENDABLE_OBJECT_WR_DECL
#define _NANOS_DEPENDABLE_OBJECT_WR_DECL

#include "llvm/Analysis/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/ValueSymbolTable.h"


namespace nanos
{

   class DOWorkRepresentation
   {
      private:
      const unsigned char *_llvmir_start;
      const unsigned char *_llvmir_end;
      llvm::LLVMContext _context;
      llvm::Module* _module;

      public:
        DOWorkRepresentation(const unsigned char llvmir_start[], const unsigned char llvmir_end[]);
        DOWorkRepresentation(const DOWorkRepresentation& work_representation);
        ~DOWorkRepresentation();

        bool has_ir() const;

   };

 }
#endif

