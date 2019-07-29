/*
	*** Calculate Distance-Angle Module
	*** src/modules/calculate/dangle/dangle.h
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

#ifndef DISSOLVE_CALCULATEMODULE_DANGLE_H
#define DISSOLVE_CALCULATEMODULE_DANGLE_H

#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class SpeciesSite;

// Calculate Distance-Angle Module
class CalculateDAngleModule : public Module
{
	public:
	// Constructor
	CalculateDAngleModule();
	// Destructor
	~CalculateDAngleModule();


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
	 * Options
	 */
	protected:
	// Set up options for Module
	void setUpKeywords();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);


	/*
	 * Processing
	 */
	private:
	// Run set-up stage
	bool setUp(Dissolve& dissolve, ProcessPool& procPool);
	// Run main processing
	bool process(Dissolve& dissolve, ProcessPool& procPool);


	/*
	 * Functions / Data
	 */
	private:
	// Analysis procedure to be run
	Procedure analyser_;
	// List of Sites 'A'
	RefList<SpeciesSite,bool> aSites_;
	// List of Sites 'B'
	RefList<SpeciesSite,bool> bSites_;
	// List of Sites 'C'
	RefList<SpeciesSite,bool> cSites_;

	public:
	// Return name of B..C RDF
	const char* rdfBCResultName() const;
	// Return name of A-B...C angle distribution
	const char* angleABCResultName() const;
	// Return name of resulting distance-angle map
	const char* resultName() const;


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	ModuleWidget* createWidget(QWidget* parent, Dissolve& dissolve);
};

#endif

