/*
	*** Dissolve Module Registry
	*** src/module/registry.h
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

#ifndef DISSOLVE_MODULEREGISTRY_H
#define DISSOLVE_MODULEREGISTRY_H

#include "module/list.h"
#include "templates/reflist.h"

// Forward Declarations
class Module;

// Module Registry
class ModuleRegistry
{
	private:
	// Reference list of lists of registered Module instances
	static RefList< List<Module>, bool> instanceLists_;

	public:
	// Register master instances for all Modules
	static void registerModules();
	// De-register master (and other) instances for all Modules
	static void deRegisterModules();
	// Register instance list for Module
	static void registerInstanceList(List<Module>& instanceList);
};

// Module Registrar
template <class M> class ModuleRegistrar
{
	public:
	// Constructor
	ModuleRegistrar()
	{
		M* mainInstance = new M;
		ModuleList::registerMasterInstance(mainInstance);
		ModuleRegistry::registerInstanceList(mainInstance->instances());
	};

	private:

};

#endif

