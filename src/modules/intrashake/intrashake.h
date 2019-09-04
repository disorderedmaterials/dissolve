/*
	*** IntraShake Module
	*** src/modules/intrashake/intrashake.h
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

#ifndef DISSOLVE_MODULE_INTRASHAKE_H
#define DISSOLVE_MODULE_INTRASHAKE_H

#include "module/module.h"

// Forward Declarations
/* none */

// IntraShake Module
class IntraShakeModule : public Module
{
	/*
	 * Module for performing MC moves on intramolecular terms within Molecules
	 */

	public:
	// Constructor
	IntraShakeModule();
	// Destructor
	~IntraShakeModule();


	/*
	 * Instances
	 */
	public:
	// Create instance of this module
	Module* createInstance() const;


	/*
	 * Definition
	 */
	public:
	// Return type of module
	const char* type() const;
	// Return category for module
	const char* category() const;
	// Return brief description of module
	const char* brief() const;
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	int nTargetableConfigurations() const;


	/*
	 * Initialisation
	 */
	protected:
	// Perform any necessary initialisation for the Module
	void initialise();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	KeywordBase::ParseResult parseComplexKeyword(KeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);


	/*
	 * Processing
	 */
	private:
	// Run main processing
	bool process(Dissolve& dissolve, ProcessPool& procPool);
};

#endif

