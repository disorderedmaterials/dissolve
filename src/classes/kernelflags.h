/*
	*** Kernel Flags
	*** src/classes/kernelflags.h
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_KERNELFLAGS_H
#define DUQ_KERNELFLAGS_H

// Forward Declarations
/* none */

// Kernel Flags
class KernelFlags
{
	public:
	// Flags
	enum Flags
	{
		NoFlags = 0,
		ExcludeSelfFlag = 1,
		ExcludeIGEJFlag = 2,
		ExcludeIntraGreaterThan = 4,
		ApplyMinimumImageFlag = 8
	};
};

#endif
