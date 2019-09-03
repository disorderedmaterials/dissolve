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
class Collect1DProcedureNode;
class Collect2DProcedureNode;
class Process1DProcedureNode;
class Process2DProcedureNode;
class SelectProcedureNode;
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
	// SelectNode for site A
	SelectProcedureNode* selectA_;
	// SelectNode for site B
	SelectProcedureNode* selectB_;
	// SelectNode for site C
	SelectProcedureNode* selectC_;
	// Collect1DNode for B-C RDF
	Collect1DProcedureNode* collectDistance_;
	// Collect1DNode for A-B-C angle histogram
	Collect1DProcedureNode* collectAngle_;
	// Collect2DNode for distance-angle data
	Collect2DProcedureNode* collectDAngle_;
	// Process1DNode for B-C RDF
	Process1DProcedureNode* processDistance_;
	// Process1DNode for A-B-C angle histogram
	Process1DProcedureNode* processAngle_;
	// Process2DNode for distance-angle data
	Process2DProcedureNode* processDAngle_;

	private:
	// Update names of processed quantities
	void updateNodeNames();


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	ModuleWidget* createWidget(QWidget* parent, Dissolve& dissolve);
};

#endif

