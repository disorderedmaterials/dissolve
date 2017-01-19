/*
	*** Export Module
	*** src/modules/export.cpp
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

#include "modules/export.h"
#include "main/duq.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/sysfunc.h"

// Static Members
List<Module> Export::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
Export::Export() : Module()
{
	// Add to instances list and set unique name for this instance
	instances_.own(this);
	uniqueName_.sprintf("%s%02i", name(), instances_.nItems()-1);

	// Setup variables / control parameters
	options_.add("SaveTrajectory", false);
	options_.add("TrajectoryFile", "trajectory.xyz");
}

// Destructor
Export::~Export()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& Export::instances()
{
	return instances_;
}

// Create instance of this module
Module* Export::createInstance()
{
	return new Export;
}

/*
 * Definition
 */

// Return name of module
const char* Export::name()
{
	return "Export";
}

// Return brief description of module
const char* Export::brief()
{
	return "Export data from one or more Configurations";
}

// Return instance type for module
Module::InstanceType Export::instanceType()
{
	return Module::MultipleInstance;
}

// Whether the Module has a pre-processing stage
bool Export::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool Export::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool Export::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* Export::dependentModules()
{
	return "";
}

// Setup supplied dependent module (only if it has been auto-added)
bool Export::setupDependentModule(Module* depMod)
{
	return true;
}

// Parse keyword line, returning 1 on success, 0 for not recognised, and -1 for failed
int Export::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	return 0;
}

/*
 * Targets
 */

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int Export::nTargetableConfigurations()
{
	return -1;
}

/*
 * Method
 */

// Perform setup tasks for module
bool Export::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool Export::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool Export::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	* Export data from the target Configuration(s)
	 */

	// Loop over target Configurations
	for (RefListItem<Configuration,bool>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next)
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item;

		// Retrieve control parameters from Configuration
		bool saveTrajectory = GenericListHelper<bool>::retrieve(cfg->moduleData(), "SaveTrajectory", uniqueName(), options_.valueAsBool("SaveTrajectory"));

		/*
		 * Save XYZ Trajectory
		 */
		if (saveTrajectory)
		{
			const char* trajectoryFile = GenericListHelper<const char*>::retrieve(cfg->moduleData(), "TrajectoryFile", uniqueName(), options_.valueAsString("TrajectoryFile"));
			Messenger::print("Export: Appending to trajectory output file '%s'...\n", trajectoryFile);

			// Only the pool master saves the data
			if (procPool.isMaster())
			{
				// Open the file
				LineParser parser;
				if (!parser.appendOutput(trajectoryFile))
				{
					parser.closeFiles();
					procPool.stop();
					return false;
				}
				else if (!writeConfigurationXYZ(parser, cfg, cfg->name()))
				{
					Messenger::print("Export: Failed to append to trajectory file.\n");
					parser.closeFiles();
					procPool.stop();
					return false;
				}

				procPool.proceed();
			}
			else if (!procPool.decision()) return false;

			Messenger::print("Export: Finished appending trajectory file.\n");
		}
	}

	return true;
}

// Execute post-processing stage
bool Export::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

/*
 * Static Functions
 */

// Write Configuration as XYZ
bool Export::writeConfigurationXYZ(LineParser& parser, Configuration* cfg, const char* header)
{
	// Write number of atoms and title
	if (!parser.writeLineF("%i\n", cfg->nAtoms())) return false;
	if (!parser.writeLineF("%s\n", header)) return false;
	
	// Write Atoms
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		Atom* i = cfg->atom(n);
		if (!parser.writeLineF("%-3s   %10.4f  %10.4f  %10.4f\n", PeriodicTable::element(i->element()).symbol(), i->r().x, i->r().y, i->r().z)) return false;
	}

	return true;
}

// Write Configuration as CONFIG
bool Export::writeConfigurationDLPOLY(LineParser& parser, Configuration* cfg, const char* header)
{
	// Write title
	parser.writeLineF("%s\n", header);

	// Write keytrj and imcon
	if (cfg->box()->type() == Box::NonPeriodicBox) parser.writeLineF("%10i%10i\n", 0, 0);
	else if (cfg->box()->type() == Box::CubicBox) parser.writeLineF("%10i%10i\n", 0, 1);
	else if (cfg->box()->type() == Box::OrthorhombicBox) parser.writeLineF("%10i%10i\n", 0, 2);
	else parser.writeLineF("%10i%10i\n", 0, 3);
	
	// Write Cell
	if (cfg->box()->type() != Box::NonPeriodicBox)
	{
		Matrix3 axes = cfg->box()->axes();
		parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[0], axes[1], axes[2]);
		parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[3], axes[4], axes[5]);
		parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[6], axes[7], axes[8]);
	}

	// Write Atoms
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		Atom* i = cfg->atom(n);
		parser.writeLineF("%-6s%10i%20.10f\n%20.12f%20.12f%20.12f\n", cfg->type(i->localTypeIndex())->name(), n+1, PeriodicTable::element(i->element()).isotope(0)->atomicWeight(), i->r().x, i->r().y, i->r().z);
	}

	Messenger::print("Export: Finished writing model CONFIG file.\n");

	return true;
}
