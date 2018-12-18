/*
	*** Module Registrar
	*** src/module/registrar.h
	Copyright T. Youngs 2012-2018

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_MODULEREGISTRAR_H
#define DISSOLVE_MODULEREGISTRAR_H

#include "module/list.h"
#include "templates/reflist.h"

// Forward Declarations
class Module;

// Module Registrar
template <class M> class ModuleRegistrar
{
	public:
	// Constructor
	ModuleRegistrar(ModuleList& moduleList, RefList< List<Module>, bool>& instancesList_)
	{
		M* mainInstance = new M;
		moduleList.registerMasterInstance(mainInstance);
		instancesList_.add(&mainInstance->instances());
	};
};

#endif

