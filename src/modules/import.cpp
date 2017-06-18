/*
	*** Import Module
	*** src/modules/import.cpp
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

#include "modules/import.h"
#include "main/duq.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Static Members
List<Module> ImportModule::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
ImportModule::ImportModule() : Module()
{
	// Add to instances list and set unique name for this instance
	instances_.own(this);
	uniqueName_.sprintf("%s%02i", name(), instances_.nItems()-1);

	// Setup variables / control parameters
	// Boolean options must be set as 'bool(false)' or 'bool(true)' rather than just 'false' or 'true' so that the correct overloaded add() function is called
	//options_.add("WriteConfig", bool(false));
}

// Destructor
ImportModule::~ImportModule()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& ImportModule::instances()
{
	return instances_;
}

// Create instance of this module
Module* ImportModule::createInstance()
{
	return new ImportModule;
}

/*
 * Definition
 */

// Return name of module
const char* ImportModule::name()
{
	return "Import";
}

// Return brief description of module
const char* ImportModule::brief()
{
	return "Import data to the target Configuration";
}

// Return instance type for module
Module::InstanceType ImportModule::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool ImportModule::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool ImportModule::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool ImportModule::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* ImportModule::dependentModules()
{
	return "";
}

// Setup supplied dependent module (only if it has been auto-added)
bool ImportModule::setupDependentModule(Module* depMod)
{
	return true;
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int ImportModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	return -1;
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int ImportModule::nTargetableConfigurations()
{
	return -1;
}

/*
 * Method
 */

// Perform setup tasks for module
bool ImportModule::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool ImportModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool ImportModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Import data to the target Configuration(s)
	 */

	// Loop over target Configurations
	for (RefListItem<Configuration,bool>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next)
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item;

		// Retrieve control parameters from Configuration
		// bool writeConfig = GenericListHelper<bool>::retrieve(cfg->moduleData(), "WriteConfig", uniqueName(), options_.valueAsBool("WriteConfig"));

		/*
		 * Import Something
		 */
// 		if (writeConfig)
// 		{
// 			Messenger::print("Import: Writing DL_POLY CONFIG file for Configuration '%s'...\n", cfg->name());
// 
// 			// Only the pool master saves the data
// 			if (procPool.isMaster())
// 			{
// 				// Construct the filename
// 				CharString filename("%s.CONFIG", cfg->niceName());
// 
// 				// Open the file
// 				LineParser parser;
// 				if (!parser.openOutput(filename))
// 				{
// 					parser.closeFiles();
// 					procPool.stop();
// 					return false;
// 				}
// 				else if (!writeConfigurationDLPOLY(parser, cfg, cfg->name()))
// 				{
// 					Messenger::print("Import: Failed to export DL_POLY CONFIG file.\n");
// 					parser.closeFiles();
// 					procPool.stop();
// 					return false;
// 				}
// 
// 				procPool.proceed();
// 			}
// 			else if (!procPool.decision()) return false;
// 
// 			Messenger::print("Import: Finished writing DL_POLY CONFIG file.\n");
// 		}
	}

	return true;
}

// Execute post-processing stage
bool ImportModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

/*
 * Static Functions
 */

// Read simple forces from specified file
bool ImportModule::readSimpleForces(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Read force information through the specified line parser.
	 * Assumed format is as follows:
	 * 
	 * Line 1:    nAtoms
	 * Line 2:    <atom id>    <fx>    <fy>    <fz>
	 *   ...
	 */

	// Read in number of atoms and initiliase arrays
	if (!parser.getArgsDelim(LineParser::Defaults)) return false;
	int nAtoms = parser.argi(0);
	Messenger::print(" --> Expecting forces for %i atoms.\n", nAtoms);
	fx.initialise(nAtoms);
	fy.initialise(nAtoms);
	fz.initialise(nAtoms);

	for (int n=0; n<nAtoms; ++n)
	{
		if (!parser.getArgsDelim(LineParser::Defaults)) return false;
		fx[n] = parser.argd(0);
		fy[n] = parser.argd(1);
		fz[n] = parser.argd(2);
	}

	return true;
}

// Read DL_POLY forces from specified file
bool ImportModule::readDLPOLYForces(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Read DL_POLY force information through the specified line parser.
	 * We assume CONFIG or REVCON format:
	 * 
	 * Line 1:    Title
	 * Line 2:    keytrj   imcon    natoms    []
	 * Line 3-5:  cell matrix (if imcon > 0)
	 * Line 6:    atomtype        id
	 * Line 7:    rx   ry   rz
	 * Line 8:    vx   vy   vz      if (keytrj > 0)
	 * Line 9:    fx   fy   fz	if (keytrj > 1)
	 *   ...
	 */

	// Skip title
	if (!parser.skipLines(1)) return false;

	// Read in keytrj, imcon, and number of atoms, and initiliase arrays
	if (!parser.getArgsDelim(LineParser::Defaults)) return false;
	int keytrj = parser.argi(0);
	int imcon = parser.argi(1);
	int nAtoms = parser.argi(2);
	if (keytrj != 2)
	{
		Messenger::error("File does not contain forces (keytrj = %i).\n", keytrj);
		return false;
	}
	Messenger::print(" --> Expecting forces for %i atoms (DLPOLY keytrj=%i, imcon=%i).\n", nAtoms, keytrj, imcon);
	fx.initialise(nAtoms);
	fy.initialise(nAtoms);
	fz.initialise(nAtoms);

	// Skip cell information if given
	if (imcon > 0) parser.skipLines(3);

	// Loop over atoms
	for (int n=0; n<nAtoms; ++n)
	{
		// Skip position and velocity lines
		if (!parser.skipLines(2)) return false;
		if (!parser.getArgsDelim(LineParser::Defaults)) return false;
		fx[n] = parser.argd(0);
		fy[n] = parser.argd(1);
		fz[n] = parser.argd(2);
	}

	return true;
}