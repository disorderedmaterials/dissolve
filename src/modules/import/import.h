/*
	*** Import Module
	*** src/modules/import/import.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_IMPORTMODULE_H
#define DUQ_IMPORTMODULE_H

#include "module/module.h"
#include "modules/import/formats.h"

// Forward Declarations
/* none */

// Import Module
class ImportModule : public Module
{
	public:
	// Constructor
	ImportModule();
	// Destructor
	~ImportModule();


	/*
	 * Instances
	 */
	protected:
	// List of all instances of this Module type
	static List<Module> instances_;

	public:
	// Return list of all created instances of this Module
	List<Module>& instances();
	// Create instance of this module
	Module* createInstance();


	/*
	 * Definition
	 */
	public:
	// Return name of module
	const char* name();
	// Return brief description of module
	const char* brief();
	// Return instance type for module
	InstanceType instanceType();
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	int nTargetableConfigurations();
	// Whether the Module has a pre-processing stage
	bool hasPreProcessing();
	// Whether the Module has a processing stage
	bool hasProcessing();
	// Whether the Module has a post-processing stage
	bool hasPostProcessing();
	// Modules upon which this Module depends to have run first
	const char* dependentModules();
	// Set up supplied dependent module (only if it has been auto-added)
	bool setUpDependentModule(Module* depMod);


	/*
	 * Options
	 */
	protected:
	// Set up options for Module
	void setUpKeywords();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix);


	/*
	 * Processing
	 */
	private:
	// Run pre-processing stage
	bool preProcess(DUQ& duq, ProcessPool& procPool);
	// Run main processing
	bool process(DUQ& duq, ProcessPool& procPool);
	// Run post-processing stage
	bool postProcess(DUQ& duq, ProcessPool& procPool);


	/*
	 * Static Functions - Coordinates
	 */
	public:
	// Read coordinates in specified format
	static bool readCoordinates(ImportModuleFormats::CoordinateFormat format, LineParser& parser, Array< Vec3<double> >& r);
	// Read xyz coordinates from specified file
	static bool readXYZCoordinates(LineParser& parser, Array< Vec3<double> >& r);
	// Read DL_POLY coordinates from specified file
	static bool readDLPOLYCoordinates(LineParser& parser, Array< Vec3<double> >& r);
	// Read EPSR ATO coordinates from specified file
	static bool readEPSRCoordinates(LineParser& parser, Array< Vec3<double> >& r);


	/*
	 * Static Functions - Forces
	 */
	public:
	// Read forces in specified format
	static bool readForces(ImportModuleFormats::ForceFormat format, LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Read simple forces from specified file
	static bool readSimpleForces(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Read DL_POLY forces from specified file
	static bool readDLPOLYForces(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz);
};

#endif

