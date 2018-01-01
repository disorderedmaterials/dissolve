/*
	*** dUQ Module Registry
	*** src/module/registry.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_MODULEREGISTRY_H
#define DUQ_MODULEREGISTRY_H

#include "module/list.h"

// Forward Declarations
class Module;

// Module Registry
class ModuleRegistry
{
	public:
	// Constructor
	ModuleRegistry();
};

// Module Registrar
template <class M> class ModuleRegistrar
{
	public:
	// Constructor
	ModuleRegistrar()
	{
		ModuleList::registerMasterInstance(new M);
	};
};

#endif

