/*
	*** Dissolve - Setup
	*** src/main/setup.cpp
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

#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "templates/genericlisthelper.h"

// Set up all simulation data, checking it as we go
bool Dissolve::setUpSimulation()
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
			if (DissolveSys::fileExists(cfg->inputCoordinatesFile()))
			{
				// Set up the Configuration from the Species populations, unless it has already been set up by reading the restart file
				if (cfg->nAtoms() == 0)
				{
					// No atoms, so presumably no data was read from the restart file. Set the Configuration up in the normal way (but without randomising molecules)
					if (!cfg->initialise(worldPool(), false, pairPotentialRange_, nBoxNormalisationPoints_)) return false;
				}

				Messenger::print("Loading initial coordinates from file '%s'...\n", cfg->inputCoordinatesFile());
				LineParser inputFileParser(&worldPool());
				if (!inputFileParser.openInput(cfg->inputCoordinatesFile())) return false;
				if (!cfg->loadCoordinates(inputFileParser, cfg->inputCoordinatesFormat())) return false;
				inputFileParser.closeFiles();
			}
			else return Messenger::error("Input coordinates file '%s' specified, but it does not exist.\n", cfg->inputCoordinatesFile());
		}
		else if (cfg->nAtoms() == 0)
		{
			// Set up the Configuration from the Species populations
			if (!cfg->initialise(worldPool(), true, pairPotentialRange_, nBoxNormalisationPoints_)) return false;
		}
		else Messenger::print("Configuration loaded from the restart file.\n");

		// Update Cell contents / Atom locations
		cfg->updateCellContents();
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

			if (!processingModuleData_.contains(itemName, "Dissolve")) continue;
			pot->setUAdditional(GenericListHelper<Data1D>::retrieve(processingModuleData_, itemName, "Dissolve", Data1D()));

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

	if (mainProcessingModules_.nModules() == 0) Messenger::print("No main processing Modules found.\n");
	else Messenger::print("%i main processing %s found.\n", mainProcessingModules_.nModules(), mainProcessingModules_.nModules() == 1 ? "Module" : "Modules");
	ListIterator<ModuleReference> mainProcessingIterator(mainProcessingModules_.modules());
	while (ModuleReference* modRef = mainProcessingIterator.iterate())
	{
		Module* module = modRef->module();

		Messenger::print("    %s:\n", module->type());
		if (module->nConfigurationTargets() == 0) Messenger::print("      No Configuration targets.\n");
		else
		{
			Messenger::print("      %i Configuration %s:\n", module->nConfigurationTargets(), module->nConfigurationTargets() == 1 ? "target" : "targets");
			RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
			while (Configuration* cfg = configIterator.iterate()) Messenger::print("      --> %s\n", cfg->name());
		}
	}

	/*
	 * Perform Set-Up Steps in Modules
	 */

	Messenger::print("*** Performing Module set-up...\n");

	// Loop over configurations
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		// Loop over Modules
		ListIterator<ModuleReference> moduleIterator(cfg->modules().modules());
		while (ModuleReference* modRef = moduleIterator.iterate())
		{
			Module* module = modRef->module();

			if (!module->setUp(*this, worldPool())) return false;
		}
	}

	// Loop over processing modules 
	ListIterator<ModuleReference> processingIterator(mainProcessingModules_.modules());
	while (ModuleReference* modRef = processingIterator.iterate())
	{
		Module* module = modRef->module();

		if (!module->setUp(*this, worldPool())) return false;
	}

	/*
	 * Print Defined Species
	 */

	Messenger::print("*** Defined Species\n");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		Messenger::print("--- Species '%s'...\n", sp->name());
		sp->print();
	}

	return true;
}
