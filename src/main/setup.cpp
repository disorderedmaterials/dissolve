/*
	*** dUQ - Setup
	*** src/main/setup.cpp
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

#include "main/duq.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "templates/genericlisthelper.h"

// Set number of test points to use when calculating Box normalisation arrays
void DUQ::setNBoxNormalisationPoints(int nPoints)
{
	nBoxNormalisationPoints_ = nPoints;
}

// Return number of test points to use when calculating Box normalisation arrays
int DUQ::nBoxNormalisationPoints() const
{
	return nBoxNormalisationPoints_;
}

// Set random seed
void DUQ::setSeed(int seed)
{
	seed_ = seed;
}

// Return random seed
int DUQ::seed() const
{
	return seed_;
}

// Set frequency with which to write various iteration dat
void DUQ::setRestartFileFrequency(int n)
{
	restartFileFrequency_ = n;
}

// Return frequency with which to write restart file
int DUQ::restartFileFrequency() const
{
	return restartFileFrequency_;
}

// Set up all simulation data, checking it as we go
bool DUQ::setUpSimulation()
{
	/*
	 * Check Species
	 */

	Messenger::print("*** Checking Species definitions...\n");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		Messenger::print("--- Species '%s'...\n", sp->name());
		if (!sp->checkSetup(atomTypes_)) return false;
	}

	/*
	 * Set up Configurations
	 */

	Messenger::print("\n");
	Messenger::print("*** Setting up Configurations...\n");
	int index = 0;
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next, ++index)
	{
		Messenger::print("*** Configuration %2i: '%s'\n", index, cfg->name());

		// If the restart file exists, we will take our coordinates from it - no need to set anything up here.
		// However, if the inputCoordinatesFile_ is set, we create our config from the Species definition and load the coordinates.
		// Otherwise, if the restart file does not exist, and the inputCoordinatesFile_ is not set, generate a random configuration.
		if (cfg->hasInputCoordinatesFile())
		{
			if (DUQSys::fileExists(cfg->inputCoordinatesFile()))
			{
				// Set up the Configuration from the Species populations, unless it has already been set up by reading the restart file
				if (cfg->nAtoms() == 0)
				{
					// No atoms, so presumably no data was read from the restart file. Set the Configuration up in the normal way (but without randomising molecules)
					if (!cfg->initialise(worldPool_, false, pairPotentialRange_, nBoxNormalisationPoints_)) return false;
				}

				Messenger::print("Loading initial coordinates from file '%s'...\n", cfg->inputCoordinatesFile());
				LineParser inputFileParser(&worldPool_);
				if (!inputFileParser.openInput(cfg->inputCoordinatesFile())) return false;
				if (!cfg->loadCoordinates(inputFileParser, cfg->inputCoordinatesFormat())) return false;
				inputFileParser.closeFiles();
			}
			else return Messenger::error("Input coordinates file '%s' specified, but it does not exist.\n", cfg->inputCoordinatesFile());
		}
		else if (cfg->nAtoms() == 0)
		{
			// Set up the Configuration from the Species populations
			if (!cfg->initialise(worldPool_, true, pairPotentialRange_, nBoxNormalisationPoints_)) return false;
		}
		else Messenger::print("Configuration loaded from the restart file.\n");

		// Update Cell contents / Atom locations
		cfg->updateCellContents();
	}

	/*
	 * Associate Modules to Data
	 */

	Messenger::print("\n");
	Messenger::print("*** Linking associated Modules to Data...\n");
	index = 0;
	for (Data* data = data_.first(); data != NULL; data = data->next, ++index)
	{
		Messenger::print("*** Data %2i: '%s'\n", index, data->name());

		if (data->hasAssociatedModuleName())
		{
			// Find the specified Module
			Module* module = ModuleList::findInstanceByUniqueName(data->associatedModuleName());
			if (!module)
			{
				Messenger::error("Failed to find Module '%s' for association with Data '%s'.\n", data->associatedModuleName(), data->name());
				return false;
			}
			data->setAssociatedModule(module);

			// Try to set-up the data here, so any normalisation etc. is performed now if it is possible to do so
			data->setUp(processingModuleData_, false);
		}
		else Messenger::print("  -> No Module associated to this Data.\n");
	}

	/*
	 * Pair Potentials
	 * We expect a PairPotential to have been defined for every combination of AtomType used in the system
	 */

	Messenger::print("\n");
	Messenger::print("*** Setting up PairPotentials...\n");
	int nMissingPots = 0;
	for (AtomType* at1 = atomTypes_.first(); at1 != NULL; at1 = at1->next)
	{
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next)
		{
			PairPotential* pot = pairPotential(at1, at2);
			if (pot == NULL)
			{
				Messenger::error("A PairPotential between AtomTypes '%s' and '%s' is required, but has not been defined.\n", at1->name(), at2->name());
				++nMissingPots;
			}

			// Setup PairPotential
			if (!pot->setUp(pairPotentialRange_, pairPotentialDelta_, pairPotentialsIncludeCoulomb_))
			{
				Messenger::error("Failed to set up PairPotential between AtomTypes '%s' and '%s'.\n", at1->name(), at2->name());
				return false;
			}

			// Retrieve additional potential from the processing module data, if present
			CharString itemName("Potential_%s-%s_Additional", pot->atomTypeNameI(), pot->atomTypeNameJ());

			if (!processingModuleData_.contains(itemName, "DUQ")) continue;
			pot->setUAdditional(GenericListHelper<XYData>::retrieve(processingModuleData_, itemName, "DUQ", XYData()));

		}
	}
	if (nMissingPots > 0) return false;

	// Create PairPotential matrix
	Messenger::print("Creating PairPotential matrix (%ix%i)...\n", atomTypes_.nItems(), atomTypes_.nItems());
	if (!potentialMap_.initialise(atomTypes_, pairPotentials_, pairPotentialRange_)) return false;

	/*
	 * Master Intramolecular Terms
	 */

	Messenger::print("\n");
	Messenger::print("*** Setting up Master Intramolecular Terms...\n");

	// All master intramolecular terms must first be initialised with the number of AtomTypes present in the system
	for (MasterIntra* b = masterBonds_.first(); b != NULL; b = b->next) b->initialiseUsageArray(atomTypes_.nItems());
	for (MasterIntra* a = masterAngles_.first(); a != NULL; a = a->next) a->initialiseUsageArray(atomTypes_.nItems());
	for (MasterIntra* t = masterTorsions_.first(); t != NULL; t = t->next) t->initialiseUsageArray(atomTypes_.nItems());

	// Loop over Configurations, flagging any initial usage of master terms in the relevant MasterIntra usage array
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next, ++index)
	{
		// Bonds
		DynamicArrayIterator<Bond> bondIterator(cfg->bonds());
		while (Bond* b = bondIterator.iterate()) if (b->speciesBond()->masterParameters()) b->speciesBond()->masterParameters()->registerUsage(b->i()->masterTypeIndex(), b->j()->masterTypeIndex());

		// Angles
		DynamicArrayIterator<Angle> angleIterator(cfg->angles());
		while (Angle* a = angleIterator.iterate()) if (a->speciesAngle()->masterParameters()) a->speciesAngle()->masterParameters()->registerUsage(a->i()->masterTypeIndex(), a->k()->masterTypeIndex());
	}

	if (nMasterBonds() > 0)
	{
		Messenger::print("\n  Bond Masters:\n");
		Messenger::print("     Name        Form            Parameters\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (MasterIntra* b = masterBonds_.first(); b != NULL; b = b->next)
		{
			CharString s("     %-10s  %-12s", b->name(), SpeciesBond::bondFunction( (SpeciesBond::BondFunction) b->form()));
			for (int n=0; n<MAXINTRAPARAMS; ++n) s.strcatf("  %12.4e", b->parameter(n));
			Messenger::print("%s\n", s.get());
		}

		Messenger::print("\n     Name        Usage Count\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (MasterIntra* b = masterBonds_.first(); b != NULL; b = b->next)
		{
			bool first = true;
			for (int n=0; n<atomTypes_.nItems(); ++n)
			{
				for (int m=n; m<atomTypes_.nItems(); ++m)
				{
					if (b->usageCount(n, m) == 0) continue;
					if (first) Messenger::print("     %-10s   %5s-%-5s   %i", b->name(), atomTypes_[n]->name(), atomTypes_[m]->name(), b->usageCount(n, m));
					else Messenger::print("                  %5s-%-5s   %i", atomTypes_[n]->name(), atomTypes_[m]->name(), b->usageCount(n, m));
					first = false;
				}
			}
			if (first) Messenger::print("      [[ No Uses ]]\n");
		}
	}

	if (nMasterAngles() > 0)
	{
		Messenger::print("\n  Angle Masters:\n");
		Messenger::print("     Name        Form            Parameters\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (MasterIntra* a = masterAngles_.first(); a != NULL; a = a->next)
		{
			CharString s("     %-10s  %-12s", a->name(), SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) a->form()));
			for (int n=0; n<MAXINTRAPARAMS; ++n) s.strcatf("  %12.4e", a->parameter(n));
			Messenger::print("%s\n", s.get());
		}

		Messenger::print("\n     Name        Usage Count\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (MasterIntra* a = masterAngles_.first(); a != NULL; a = a->next)
		{
			bool first = true;
			for (int n=0; n<atomTypes_.nItems(); ++n)
			{
				for (int m=n; m<atomTypes_.nItems(); ++m)
				{
					if (a->usageCount(n, m) == 0) continue;
					if (first) Messenger::print("     %-10s   %5s-?-%-5s   %i", a->name(), atomTypes_[n]->name(), atomTypes_[m]->name(), a->usageCount(n, m));
					else Messenger::print("                  %5s-?-%-5s   %i", atomTypes_[n]->name(), atomTypes_[m]->name(), a->usageCount(n, m));
					first = false;
				}
			}
			if (first) Messenger::print("      [[ No Uses ]]\n");
		}
	}

	if (nMasterTorsions() > 0)
	{
		Messenger::print("\n  Torsion Masters:\n");
		Messenger::print("     Name        Form            Parameters\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (MasterIntra* t = masterTorsions_.first(); t != NULL; t = t->next)
		{
			CharString s("     %-10s  %-12s", t->name(), SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) t->form()));
			for (int n=0; n<MAXINTRAPARAMS; ++n) s.strcatf("  %12.4e", t->parameter(n));
			Messenger::print("%s\n", s.get());
		}
	}

	/*
	 * Construct Pre/Post-Process Lists
	 */

	Messenger::print("*** Creating Pre/Post-Processing task list...\n");
	// Loop over configurations
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		// Loop over Modules, checking for those that have pre- or post-processing steps
		RefListIterator<Module,bool> moduleIterator(cfg->modules().modules());
		while (Module* module = moduleIterator.iterate())
		{
			// Pre-Processing
			if (module->hasPreProcessing())
			{
				// If the Module's instance type is UniqueInstance, check that it is not already in the list
				if (module->instanceType() == Module::UniqueInstance)
				{
					Module* oldModule = findPreProcessingTask(module->name());
					if (!oldModule) preProcessingTasks_.add(module);
				}
				else preProcessingTasks_.add(module);
			}

			// Post-Processing
			if (module->hasPostProcessing())
			{
				// If the Module's instance type is UniqueInstance, check that it is not already in the list
				if (module->instanceType() == Module::UniqueInstance)
				{
					Module* oldModule = findPostProcessingTask(module->name());
					if (!oldModule) postProcessingTasks_.add(module);
				}
				else postProcessingTasks_.add(module);
			}
		}
	}
	// Loop over processing modules and add pre/post tasks
	RefListIterator<Module,bool> processingIterator(processingModules_.modules());
	while (Module* module = processingIterator.iterate())
	{
		// Pre-Processing
		if (module->hasPreProcessing())
		{
			// If the Module's instance type is UniqueInstance, check that it is not already in the list
			if (module->instanceType() == Module::UniqueInstance)
			{
				Module* oldModule = findPreProcessingTask(module->name());
				if (!oldModule) preProcessingTasks_.add(module);
			}
			else preProcessingTasks_.add(module);
		}

		// Post-Processing
		if (module->hasPostProcessing())
		{
			// If the Module's instance type is UniqueInstance, check that it is not already in the list
			if (module->instanceType() == Module::UniqueInstance)
			{
				Module* oldModule = findPostProcessingTask(module->name());
				if (!oldModule) postProcessingTasks_.add(module);
			}
			else postProcessingTasks_.add(module);
		}
	}

	if (preProcessingTasks_.nItems() == 0) Messenger::print("No pre-processing tasks found.\n");
	else Messenger::print("%i pre-processing %s found.\n", preProcessingTasks_.nItems(), preProcessingTasks_.nItems() == 1 ? "task" : "tasks");
	RefListIterator<Module,bool> preProcessingIterator(preProcessingTasks_);
	while (Module* module = preProcessingIterator.iterate())
	{
		Messenger::print("    %s:\n", module->name());
		if (module->nConfigurationTargets() == 0) Messenger::print("      No Configuration targets.\n");
		else
		{
			Messenger::print("      %i Configuration %s:\n", module->nConfigurationTargets(), module->nConfigurationTargets() == 1 ? "target" : "targets");
			RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
			while (Configuration* cfg = configIterator.iterate()) Messenger::print("      --> %s\n", cfg->name());
		}
	}

	if (processingModules_.nModules() == 0) Messenger::print("No main processing Modules found.\n");
	else Messenger::print("%i main processing %s found.\n", processingModules_.nModules(), processingModules_.nModules() == 1 ? "Module" : "Modules");
	RefListIterator<Module,bool> mainProcessingIterator(processingModules_.modules());
	while (Module* module = mainProcessingIterator.iterate())
	{
		Messenger::print("    %s:\n", module->name());
		if (module->nConfigurationTargets() == 0) Messenger::print("      No Configuration targets.\n");
		else
		{
			Messenger::print("      %i Configuration %s:\n", module->nConfigurationTargets(), module->nConfigurationTargets() == 1 ? "target" : "targets");
			RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
			while (Configuration* cfg = configIterator.iterate()) Messenger::print("      --> %s\n", cfg->name());
		}
	}

	if (postProcessingTasks_.nItems() == 0) Messenger::print("No post-processing tasks found.\n");
	else Messenger::print("%i post-processing %s found.\n", postProcessingTasks_.nItems(), postProcessingTasks_.nItems() == 1 ? "task" : "tasks");
	RefListIterator<Module,bool> postProcessingIterator(postProcessingTasks_);
	while (Module* module = postProcessingIterator.iterate())
	{
		Messenger::print("    %s:\n", module->name());
		if (module->nConfigurationTargets() == 0) Messenger::print("      No Configuration targets.\n");
		else
		{
			Messenger::print("      %i Configuration %s:\n", module->nConfigurationTargets(), module->nConfigurationTargets() == 1 ? "target" : "targets");
			RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
			while (Configuration* cfg = configIterator.iterate()) Messenger::print("      --> %s\n", cfg->name());
		}
	}

	Messenger::print("*** Defined Species\n");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		Messenger::print("--- Species '%s'...\n", sp->name());
		sp->print();
	}

	return true;
}
