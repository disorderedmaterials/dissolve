/*
	*** Keyword - AtomType Selection
	*** src/keywords/atomtypeselection.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_KEYWORD_ATOMTYPESELECTION_H
#define DISSOLVE_KEYWORD_ATOMTYPESELECTION_H

#include "keywords/data.h"
#include "keywords/base.h"
#include "classes/atomtypelist.h"
#include "templates/reflist.h"

// Forward Declarations
class Configuration;

// Keyword with IsotopologueReference Data
class AtomTypeSelectionModuleKeyword : public ModuleKeywordData<AtomTypeList&>
{
	public:
	// Constructor
	AtomTypeSelectionModuleKeyword(AtomTypeList& selection_, RefList<Configuration>& sourceConfigurations);
	// Destructor
	~AtomTypeSelectionModuleKeyword();


	/*
	 * Data
	 */
	private:
	// Source Configurations from which we take our valid AtomTypes
	RefList<Configuration>& sourceConfigurations_;

	public:
	// Check selection and make sure it is consistent based on the source Configurations
	void checkSelection();
	// Return selection after checking it for validity
	AtomTypeList& selection();


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments();
	// Return maximum number of arguments accepted
	int maxArguments();
	// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
	bool read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool);
	// Write keyword data to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif

