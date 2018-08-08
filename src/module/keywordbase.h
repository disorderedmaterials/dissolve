/*
	*** Module Keyword Base Class
	*** src/module/keywordbase.h
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

#ifndef DISSOLVE_KEYWORDBASE_H
#define DISSOLVE_KEYWORDBASE_H

#include "base/charstring.h"
#include "module/keyworddata.h"
#include "templates/listitem.h"

// Forward Declarations
class ProcessPool;

// Module Keyword Base Class
class ModuleKeywordBase : public ListItem<ModuleKeywordBase>
{
	public:
	// Keyword Data Type
	enum KeywordDataType { ComplexData, BoolData, IntegerData, DoubleData, CharStringData, AtomTypeSelectionData, BroadeningFunctionData, IsotopologueListData, PairBroadeningFunctionData, WindowFunctionData, XYDataStoreData };
	// Constructor
	ModuleKeywordBase(KeywordDataType type);
	// Destructor
	virtual ~ModuleKeywordBase();
	// Return DataType name
	static const char* keywordDataType(KeywordDataType kdt);


	/*
	 * Keyword Description
	 */
	private:
	// Data type stored by keyword
	KeywordDataType type_;
	// Keyword string
	CharString keyword_;
	// Arguments string (for information)
	CharString arguments_;
	// Description of keyword, if any
	CharString description_;
	// Flags to apply if reinstated as a GenericListItem (i.e. in a Module)
	int genericItemFlags_;

	public:
	// Set name, description, and item flags
	void set(const char* keyword, const char* description, const char* arguments, int genericItemFlags);
	// Return data type stored by keyword
	KeywordDataType type();
	// Return keyword name
	const char* keyword();
	// Return arguments string
	const char* arguments();
	// Return keyword description
	const char* description();
	// Return flags to apply if reinstated as a GenericListItem (i.e. in a Module)
	int genericItemFlags();
	// Return whether the data has ever been set
	virtual bool isSet() = 0;


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	virtual int minArguments() = 0;
	// Return maxnimum number of arguments accepted
	virtual int maxArguments() = 0;
	// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
	virtual bool parseArguments(LineParser& parser, int startArg, ProcessPool& procPool) = 0;
	// Write keyword data to specified LineParser
	virtual bool write(LineParser& parser, const char* prefix) = 0;


	/*
	 * Conversion
	 */
	public:
	// Return value (as bool)
	virtual bool asBool();
	// Return value (as int)
	virtual int asInt();
	// Return value (as double)
	virtual double asDouble();
	// Return value (as string)
	virtual const char* asString();
};

#endif

