/*
	*** dUQ Module Interface
	*** src/modules/module.cpp
	Copyright T. Youngs 2012-2016

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

#include "modules/module.h"
#include <cstring>

// Module Types
const char* ModuleTypeKeywords[Module::nModuleTypes] = { "Analysis", "Correlation", "Evolution", "Fit" };
Module::ModuleType Module::moduleType(const char* s)
{
	for (int n=0; n<Module::nModuleTypes; ++n) if (strcmp(s, ModuleTypeKeywords[n]) == 0) return (Module::ModuleType) n;
	return Module::nModuleTypes;
}
const char* Module::moduleType(Module::ModuleType mt)
{
	return ModuleTypeKeywords[mt];
}