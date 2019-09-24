/*
	*** Dissolve - Setup
	*** src/main/setup.cpp
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

#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "genericitems/listhelper.h"

// Set up all simulation data, checking it as we go
bool Dissolve::setUpSimulation()
{
	/*
	 * Check Species
	 */

	Messenger::print("*** Checking Species definitions...\n");
	for (Species* sp = species().first(); sp != NULL; sp = sp->next())
	{
		Messenger::print("--- Species '%s'...\n", sp->name());
		if (!sp->checkSetUp(coreData_.atomTypes())) return false;
	}

	/*
	 * Set up Configurations
	 */

	Messenger::print("\n");
	Messenger::print("*** Setting up Configurations...\n");
	int index = 0;
	for (Configuration* cfg = configurations().first(); cfg != NULL; cfg = cfg->next(), ++index)
	{
		Messenger::print("*** Configuration %2i: '%s'\n", index, cfg->name());

		/*
		 * If an input coordinates file has been specified and exists then this overrides our generator *and* any
		 * coordinates that may be present in the restart file. If no restart file data was read in (nAtoms will currently
		 * be zero) then we will need to run the generator() anyway on order to set up the species / atoms.
		 */
		if (cfg->inputCoordinates().hasValidFileAndFormat())
		{
			if (DissolveSys::fileExists(cfg->inputCoordinates()))
			{
				// Set up the Configuration from the Species populations, unless it has already been set up by reading the restart file
				if (cfg->nAtoms() == 0)
				{
					// No atoms (presumably no data was read from the restart file) so run the generator
					if (!cfg->generate(worldPool(), pairPotentialRange_)) return false;
				}

				Messenger::print("Loading initial coordinates from file '%s'...\n", cfg->inputCoordinates().filename());
				LineParser inputFileParser(&worldPool());
				if (!inputFileParser.openInput(cfg->inputCoordinates())) return false;
				if (!cfg->loadCoordinates(inputFileParser, cfg->inputCoordinates().coordinateFormat())) return false;
				inputFileParser.closeFiles();
			}
			else return Messenger::error("Input coordinates file '%s' specified, but it does not exist.\n", cfg->inputCoordinates().filename());
		}
		else if (cfg->nAtoms() == 0)
		{
			// Set up the Configuration from its generator
			if (!cfg->generate(worldPool(), pairPotentialRange_)) return false;
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
	for (AtomType* at1 = coreData_.atomTypes().first(); at1 != NULL; at1 = at1->next())
	{
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next())
		{
			PairPotential* pot = pairPotential(at1, at2);
			if (pot == NULL)
			{
				Messenger::error("A PairPotential between AtomTypes '%s' and '%s' is required, but has not been defined.\n", at1->name(), at2->name());
				++nMissingPots;
				continue;
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
	Messenger::print("Creating PairPotential matrix (%ix%i)...\n", coreData_.nAtomTypes(), coreData_.nAtomTypes());
	if (!potentialMap_.initialise(coreData_.atomTypes(), pairPotentials_, pairPotentialRange_)) return false;

	/*
	 * Master Intramolecular Terms
	 */

	Messenger::print("\n");
	Messenger::print("*** Setting up Master Intramolecular Terms...\n");

	// All master intramolecular terms must first be initialised with the number of AtomTypes present in the system
	// TODO This could be removed and made a special check function of the GUI / CLI
	for (MasterIntra* b = coreData_.masterBonds().first(); b != NULL; b = b->next()) b->initialiseUsageArray(coreData_.nAtomTypes());
	for (MasterIntra* a = coreData_.masterAngles().first(); a != NULL; a = a->next()) a->initialiseUsageArray(coreData_.nAtomTypes());
	for (MasterIntra* t = coreData_.masterTorsions().first(); t != NULL; t = t->next()) t->initialiseUsageArray(coreData_.nAtomTypes());

	// Loop over Configurations, flagging any initial usage of master terms in the relevant MasterIntra usage array
	for (Configuration* cfg = configurations().first(); cfg != NULL; cfg = cfg->next(), ++index)
	{
		// Bonds
		DynamicArrayIterator<Bond> bondIterator(cfg->bonds());
		while (Bond* b = bondIterator.iterate()) if (b->speciesBond()->masterParameters()) b->speciesBond()->masterParameters()->registerUsage(b->i()->masterTypeIndex(), b->j()->masterTypeIndex());

		// Angles
		DynamicArrayIterator<Angle> angleIterator(cfg->angles());
		while (Angle* a = angleIterator.iterate()) if (a->speciesAngle()->masterParameters()) a->speciesAngle()->masterParameters()->registerUsage(a->i()->masterTypeIndex(), a->k()->masterTypeIndex());

		// Torsions
		DynamicArrayIterator<Torsion> torsionIterator(cfg->torsions());
		while (Torsion* t = torsionIterator.iterate()) if (t->speciesTorsion()->masterParameters()) t->speciesTorsion()->masterParameters()->registerUsage(t->i()->masterTypeIndex(), t->l()->masterTypeIndex());
	}

	if (coreData_.nMasterBonds() > 0)
	{
		Messenger::print("\n  Bond Masters:\n");
		Messenger::print("     Name        Form            Parameters\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (MasterIntra* b = coreData_.masterBonds().first(); b != NULL; b = b->next())
		{
			CharString s("     %-10s  %-12s", b->name(), SpeciesBond::bondFunction( (SpeciesBond::BondFunction) b->form()));
			for (int n=0; n<MAXINTRAPARAMS; ++n) s.strcatf("  %12.4e", b->parameter(n));
			Messenger::print("%s\n", s.get());
		}

		Messenger::print("\n     Name        Usage Count\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (MasterIntra* b = coreData_.masterBonds().first(); b != NULL; b = b->next())
		{
			bool first = true;
			for (int n=0; n<nAtomTypes(); ++n)
			{
				for (int m=n; m<nAtomTypes(); ++m)
				{
					if (b->usageCount(n, m) == 0) continue;
					if (first) Messenger::print("     %-10s   %5s-%-5s   %i", b->name(), atomType(n)->name(), atomType(m)->name(), b->usageCount(n, m));
					else Messenger::print("                  %5s-%-5s   %i", atomType(n)->name(), atomType(m)->name(), b->usageCount(n, m));
					first = false;
				}
			}
			if (first) Messenger::print("      [[ No Uses ]]\n");
		}
	}

	if (coreData_.nMasterAngles() > 0)
	{
		Messenger::print("\n  Angle Masters:\n");
		Messenger::print("     Name        Form            Parameters\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (MasterIntra* a = coreData_.masterAngles().first(); a != NULL; a = a->next())
		{
			CharString s("     %-10s  %-12s", a->name(), SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) a->form()));
			for (int n=0; n<MAXINTRAPARAMS; ++n) s.strcatf("  %12.4e", a->parameter(n));
			Messenger::print("%s\n", s.get());
		}

		Messenger::print("\n     Name        Usage Count\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (MasterIntra* a = coreData_.masterAngles().first(); a != NULL; a = a->next())
		{
			bool first = true;
			for (int n=0; n<nAtomTypes(); ++n)
			{
				for (int m=n; m<nAtomTypes(); ++m)
				{
					if (a->usageCount(n, m) == 0) continue;
					if (first) Messenger::print("     %-10s   %5s-?-%-5s   %i", a->name(), atomType(n)->name(), atomType(m)->name(), a->usageCount(n, m));
					else Messenger::print("                  %5s-?-%-5s   %i", atomType(n)->name(), atomType(m)->name(), a->usageCount(n, m));
					first = false;
				}
			}
			if (first) Messenger::print("      [[ No Uses ]]\n");
		}
	}

	if (coreData_.nMasterTorsions() > 0)
	{
		Messenger::print("\n  Torsion Masters:\n");
		Messenger::print("     Name        Form            Parameters\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (MasterIntra* t = coreData_.masterTorsions().first(); t != NULL; t = t->next())
		{
			CharString s("     %-10s  %-12s", t->name(), SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) t->form()));
			for (int n=0; n<MAXINTRAPARAMS; ++n) s.strcatf("  %12.4e", t->parameter(n));
			Messenger::print("%s\n", s.get());
		}

		Messenger::print("\n     Name        Usage Count\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		for (MasterIntra* t = coreData_.masterTorsions().first(); t != NULL; t = t->next())
		{
			bool first = true;
			for (int n=0; n<nAtomTypes(); ++n)
			{
				for (int m=n; m<nAtomTypes(); ++m)
				{
					if (t->usageCount(n, m) == 0) continue;
					if (first) Messenger::print("     %-10s   %5s-?-?-%-5s   %i", t->name(), atomType(n)->name(), atomType(m)->name(), t->usageCount(n, m));
					else Messenger::print("                  %5s-?-?-%-5s   %i", atomType(n)->name(), atomType(m)->name(), t->usageCount(n, m));
					first = false;
				}
			}
			if (first) Messenger::print("      [[ No Uses ]]\n");
		}
	}

	ListIterator<ModuleLayer> processingLayerIterator(processingLayers_);
	while (ModuleLayer* layer = processingLayerIterator.iterate())
	{
		Messenger::print("Processing layer '%s':\n\n", layer->name());
	
		if (layer->nModules() == 0) Messenger::print("No Modules found.\n");
		else Messenger::print("%i main processing %s found.\n", layer->nModules(), layer->nModules() == 1 ? "Module" : "Modules");
		ListIterator<Module> processingIterator(layer->modules());
		while (Module* module = processingIterator.iterate())
		{
			Messenger::print("    %s:\n", module->type());
			if (module->nTargetConfigurations() == 0) Messenger::print("      No Configuration targets.\n");
			else
			{
				Messenger::print("      %i Configuration %s:\n", module->nTargetConfigurations(), module->nTargetConfigurations() == 1 ? "target" : "targets");
				RefListIterator<Configuration> configIterator(module->targetConfigurations());
				while (Configuration* cfg = configIterator.iterate()) Messenger::print("      --> %s\n", cfg->name());
			}
		}
	}


	/*
	 * Perform Set-Up Steps in Modules
	 */

	Messenger::print("*** Performing Module set-up...\n");

	// Loop over all used modules (in Configurations and ModuleLayers)
	RefListIterator<Module> moduleIterator(moduleInstances_);
	while (Module* module = moduleIterator.iterate())
	{
		if (!module->setUp(*this, worldPool())) return false;
	}


	/*
	 * Print Defined Species
	 */

	Messenger::print("*** Defined Species\n");
	for (Species* sp = species().first(); sp != NULL; sp = sp->next())
	{
		Messenger::print("--- Species '%s'...\n", sp->name());
		sp->print();
	}

	return true;
}
