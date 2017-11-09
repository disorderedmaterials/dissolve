/*
	*** Module Keyword - Complex
	*** src/modules/modulekeyword_complex.h
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

#ifndef DUQ_MODULEKEYWORDCOMPLEX_H
#define DUQ_MODULEKEYWORDCOMPLEX_H

#include "module/keyworddata.h"
#include "module/keywordbase.h"

// Forward Declarations
/* none */

// Keyword with Complex Data
class ComplexModuleKeyword : public ModuleKeywordBase, public ModuleKeywordData<int>
{
	public:
	// Constructors
	ComplexModuleKeyword(int minArgs, int maxArgs = -1);
	// Destructor
	~ComplexModuleKeyword();


	/*
	 * Data
	 */
	public:
	// Duplicate the keyword's data in the supplied GenericList
	void duplicateInList(GenericList& targetList, const char* prefix);
	// Return whether the current data value has ever been set
	bool set();


	/*
	 * Data Validation
	 */
	public:
	// Validate supplied value
	bool isValid(int value);


	/*
	 * Arguments
	 */
	private:
	// Minimum number of arguments accepted
	int minArguments_;
	// Maximum number of arguments accepted
	int maxArguments_;
	public:
	// Return minimum number of arguments accepted
	int minArguments();
	// Return maximum number of arguments accepted
	int maxArguments();
	// Parse arguments from supplied LineParser, starting at argument offset specified
	bool parseArguments(LineParser& parser, int startArg);
};

#endif

