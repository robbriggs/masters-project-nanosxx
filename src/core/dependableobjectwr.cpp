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

using namespace nanos;

DOWorkRepresentation::DOWorkRepresentation(const unsigned char llvmir_start[], const unsigned char llvmir_end[])
	: _llvmir_start(llvmir_start), _llvmir_end(llvmir_end)
{
	if (llvmir_start == NULL)
		return;

	// Parse IR text
	std::string ir(llvmir_start, llvmir_end);
	llvm::SMDiagnostic err;
	llvm::MemoryBuffer *buf = llvm::MemoryBuffer::getMemBuffer(ir);
	_module = ParseIR(buf, err, _context);
}

DOWorkRepresentation::DOWorkRepresentation(const DOWorkRepresentation &work_representation)
{

}

DOWorkRepresentation::~DOWorkRepresentation()
{

}