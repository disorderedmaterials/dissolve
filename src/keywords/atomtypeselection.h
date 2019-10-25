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
#include "classes/atomtypelist.h"
#include "templates/reflist.h"

// Forward Declarations
class Configuration;

// Keyword with AtomTypeList Data
class AtomTypeSelectionKeyword : public KeywordData<AtomTypeList&>
{
	public:
	// Constructor
	AtomTypeSelectionKeyword(AtomTypeList& selection_, const RefList<Configuration>& sourceConfigurations);
	// Destructor
	~AtomTypeSelectionKeyword();


	/*
	 * Data
	 */
	private:
	// Source Configurations from which we take our valid AtomTypes
	const RefList<Configuration>& sourceConfigurations_;

	public:
	// Determine whether current data is 'empty', and should be considered as 'not set'
	bool isDataEmpty() const;
	// Check selection and make sure it is consistent based on the source Configurations
	void checkSelection();
	// Return selection after checking it for validity
	AtomTypeList& selection();


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments() const;
	// Return maximum number of arguments accepted
	int maxArguments() const;
	// Parse arguments from supplied LineParser, starting at given argument offset
	bool read(LineParser& parser, int startArg, const CoreData& coreData);
	// Write keyword data to specified LineParser
	bool write(LineParser& parser, const char* keywordName, const char* prefix);


	/*
	 * Object Management
	 */
	protected:
	// Prune any references to the supplied AtomType in the contained data
	void removeReferencesTo(AtomType* at);
};

#endif

