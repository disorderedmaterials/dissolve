/*
	*** Keyword - Module
	*** src/keywords/module.h
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

#ifndef DISSOLVE_KEYWORD_MODULE_H
#define DISSOLVE_KEYWORD_MODULE_H

#include "keywords/data.h"
#include "module/module.h"
#include "classes/coredata.h"
#include "base/lineparser.h"

// Forward Declarations
class ModuleValue;
class ProcedureModule;

// Keyword with ProcedureModule base class
class ModuleKeywordBase
{
	public:
	// Constructor
	ModuleKeywordBase(const char* moduleType);
	// Destructor
	virtual ~ModuleKeywordBase();


	/*
	 * Target Module
	 */
	private:
	// Target module type to allow
	const char* moduleType_;

	public:
	// Return target module type to allow
	const char* moduleType() const;
	// Set the target module
	virtual bool setModule(Module* module) = 0;
	// Return the current target module as the base class
	virtual Module* baseModule() const = 0;


	/*
	 * Access to KeywordBase
	 */
	public:
	// Return option mask for keyword
	virtual int optionMask() const = 0;
};

// Keyword with ProcedureModule
template <class M> class ModuleKeyword : public ModuleKeywordBase, public KeywordData<M*>
{
	public:
	// Constructor
	ModuleKeyword(const char* moduleType, M* module = NULL) : ModuleKeywordBase(moduleType), KeywordData<M*>(KeywordBase::ModuleData, module)
	{
	}
	// Destructor
	~ModuleKeyword()
	{
	}


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments() const
	{
		return 1;
	}
	// Return maximum number of arguments accepted
	int maxArguments() const
	{
		return 1;
	}
	// Parse arguments from supplied LineParser, starting at given argument offset
	bool read(LineParser& parser, int startArg, const CoreData& coreData)
	{
		Module* module = coreData.findModule(parser.argc(startArg));
		if (!module) return Messenger::error("Module '%s' given to keyword %s doesn't exist.\n", parser.argc(startArg), KeywordBase::name());

		return setModule(module);
	}
	// Write keyword data to specified LineParser
	bool write(LineParser& parser, const char* keywordName, const char* prefix)
	{
		// No need to write the keyword if the module pointer is null
		if (KeywordData<M*>::data_ == NULL) return true;

		if (!parser.writeLineF("%s%s  '%s'\n", prefix, KeywordBase::name(), KeywordData<M*>::data_->uniqueName())) return false;

		return true;
	}


	/*
	 * Target Module
	 */
	public:
	// Set the target module
	bool setModule(Module* module)
	{
		if (!module) return false;

		// Attempt to case the supplied pointer to a Module of our type
		M* castModule = dynamic_cast<M*>(module);
		if (!castModule) return Messenger::error("Module '%s' given to keyword %s is of the wrong type (%s) - only a module of type '%s' can be accepted.\n", module->uniqueName(), KeywordBase::name(), module->type(), moduleType());

		KeywordData<M*>::data_ = castModule;

		KeywordData<M*>::set_ = true;

		return true;
	}
	// Return the current target module as the base class
	Module* baseModule() const
	{
		return KeywordData<M*>::data_;
	}
	// Return the current target module
	M* module() const
	{
		return KeywordData<M*>::data_;
	}


	/*
	 * Access to KeywordBase
	 */
	public:
	// Return option mask for keyword
	int optionMask() const
	{
		return KeywordBase::optionMask();
	}


	/*
	 * Object Management
	 */
	protected:
	// Prune any references to the supplied ProcedureModule in the contained data
	void removeReferencesTo(Module* module)
	{
		if (KeywordData<M*>::data_ == module) KeywordData<M*>::data_ = NULL;
	}
};

#endif

