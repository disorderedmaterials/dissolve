/*
	*** dUQ Dump Functions
	*** src/main/dump.cpp
	Copyright T. Youngs 2012-2016

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
#include "main/keywords.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/box.h"

// Dump full system setup
void DUQ::dumpSystemSetup(bool includeData)
{
	// Print the full system setup (in input file format)/
	// Can optionally include data (i.e. datasets, coordinates, box normalisation etc.) for extra checks

	// Write title comment
	Messenger::print("\n\n# SYSTEM DUMP\n\n");
	
	// Write AtomTypes block
	Messenger::print("# AtomType Definitions\n");
	Messenger::print("%s\n", Keywords::inputBlock(Keywords::AtomTypesBlock));
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next)
	{
		Messenger::print("  %s  '%s'  %3s  '%s'\n", Keywords::atomTypesKeyword(Keywords::AtomTypeKeyword), at->name(), periodicTable.element(at->element()).symbol(), at->parameters()->name());
	}
	Messenger::print("%s\n\n", Keywords::atomTypesKeyword(Keywords::EndAtomTypesKeyword));

	// Write Species data
	Messenger::print("# Species Definitions\n");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		Messenger::print("%s '%s'\n", Keywords::inputBlock(Keywords::SpeciesBlock), sp->name());
		
		// Atoms
		Messenger::print("  # Atoms\n");
		int count = 0;
		for (SpeciesAtom* i = sp->atoms(); i != NULL; i = i->next)
		{
			++count;
			Messenger::print("  %s  %3i  %3s  %8.3f %8.3f %8.3f %8.3f '%s'\n", Keywords::speciesKeyword(Keywords::AtomKeyword), count, periodicTable.element(i->element()).symbol(), i->r().x, i->r().y, i->r().z, i->charge(), i->atomType() == NULL ? "???" : i->atomType()->name());
		}
		
		// Bonds
		Messenger::print("\n  # Bonds\n");
		for (SpeciesBond* b = sp->bonds(); b != NULL; b = b->next)
		{
			Messenger::print("  %s  %3i  %3i  %8.3f %8.3f\n", Keywords::speciesKeyword(Keywords::BondKeyword), b->indexI()+1, b->indexJ()+1, b->equilibrium(), b->forceConstant());
		}
		
		// Angles
		Messenger::print("\n  # Angles\n");
		for (SpeciesAngle* a = sp->angles(); a != NULL; a = a->next)
		{
			Messenger::print("  %s  %3i  %3i  %3i  %8.3f %8.3f\n", Keywords::speciesKeyword(Keywords::AngleKeyword), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->equilibrium(), a->forceConstant());
		}
		
		// Grains
		Messenger::print("\n  # Grain Definitions\n");
		for (SpeciesGrain* sg = sp->grains(); sg != NULL; sg = sg->next)
		{
			Messenger::print("  %s  '%s'", Keywords::speciesKeyword(Keywords::GrainKeyword), sg->name());
			for (RefListItem<SpeciesAtom,int>* ri = sg->atoms(); ri != NULL; ri = ri->next) Messenger::print("  %i", ri->item->userIndex());
			Messenger::print("\n");
		}

		// Isotopologues
		Messenger::print("\n  # Isotopologues\n");
		for (Isotopologue* iso = sp->isotopologues(); iso != NULL; iso = iso->next)
		{
			Messenger::print("  %s  '%s'", Keywords::speciesKeyword(Keywords::IsotopologueKeyword), iso->name());
			for (RefListItem<AtomType,Isotope*>* ri = iso->isotopes(); ri != NULL; ri = ri->next)
			{
				Messenger::print("  %s=%i", ri->item->name(), ri->data->A());
			}
			Messenger::print("\n");
		}
		
		// Done with this species
		Messenger::print("%s\n\n", Keywords::speciesKeyword(Keywords::EndSpeciesKeyword));
	}

	// Write Sample blocks
	Messenger::print("# Samples\n");
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		Messenger::print("%s  '%s'\n", Keywords::inputBlock(Keywords::SampleBlock), sam->name()); 
		for (IsotopologueMix* iso = sam->isotopologueMixtures(); iso != NULL; iso = iso->next)
		{
			double sum = iso->totalRelative();
			for (RefListItem<Isotopologue,double>* ri = iso->isotopologues(); ri != NULL; ri = ri->next)
			{
				Messenger::print("  %s  '%s'  '%s'  %f\n", Keywords::sampleKeyword(Keywords::IsotopologueSampleKeyword), iso->species()->name(), ri->item->name(), ri->data);
			}
		}
		// Reference data present?
		if (!sam->referenceDataFileName().isEmpty())
		{
			Messenger::print("  %s '%s'\n", Keywords::sampleKeyword(Keywords::ReferenceDataKeyword), sam->referenceDataFileName().get());
		}
		Messenger::print("%s\n\n", Keywords::sampleKeyword(Keywords::EndSampleKeyword));
	}

	// Write Configurations
	Messenger::print("# Configurations\n");
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		Messenger::print("%s  '%s'\n", Keywords::inputBlock(Keywords::ConfigurationBlock), cfg->name());
		Messenger::print("  %s  %i\n", Keywords::configurationKeyword(Keywords::MultiplierKeyword), cfg->multiplier());
		Messenger::print("  %s  %f  %s\n", Keywords::configurationKeyword(Keywords::DensityKeyword), cfg->density(), cfg->densityIsAtomic() ? "atoms/A3" : "g/cm3");
		Messenger::print("  %s  %f  %f  %f\n", Keywords::configurationKeyword(Keywords::CellLengthsKeyword), cfg->relativeBoxLengths().x, cfg->relativeBoxLengths().y, cfg->relativeBoxLengths().z);
		Messenger::print("  %s  %f  %f  %f\n", Keywords::configurationKeyword(Keywords::CellAnglesKeyword), cfg->boxAngles().x, cfg->boxAngles().y, cfg->boxAngles().z);
		if (cfg->nonPeriodic()) Messenger::print("  %s\n", Keywords::configurationKeyword(Keywords::NonPeriodicKeyword));

		// Species
		Messenger::print("\n  # Species\n");
		for (RefListItem<Species,double>* ri = cfg->usedSpecies(); ri != NULL; ri = ri->next)
		{
			Species* sp = ri->item;
			Messenger::print("  %s  '%s'  %f\n", Keywords::configurationKeyword(Keywords::SpeciesAddKeyword), sp->name(), ri->data);
		}

		// Modules
		Messenger::print("\n  # Modules\n");
		if (cfg->nModules() == 0) Messenger::print("  # -- None\n");
		for (int n=0; n<Module::nModuleTypes; ++n)
		{
			Module::ModuleType mt = (Module::ModuleType) n;
			
			for (RefListItem<Module,bool>* ri = cfg->modules(mt); ri != NULL; ri = ri->next)
			{
				Module* module = ri->item;

				Messenger::print("  %s  %s\n", Keywords::configurationKeyword(Keywords::ModuleAddKeyword), module->name());

				// For each Module, print all available variables
				for (Variable* var = module->variables(); var != NULL; var = var->next)
				{
					if (var->type() == Variable::StringType) Messenger::print("    %s  %s  '%s'\n", Keywords::moduleKeyword(Keywords::SetModuleVariableKeyword), var->name(), var->asChar());
					else Messenger::print("    %s  %s  %s\n", Keywords::moduleKeyword(Keywords::SetModuleVariableKeyword), var->name(), var->asChar());
				}

				Messenger::print("  %s\n", Keywords::moduleKeyword(Keywords::EndModuleKeyword));
			}
		}

		Messenger::print("\n");
		Messenger::print("  %s  '%s'\n", Keywords::configurationKeyword(Keywords::BoxNormalisationFileKeyword), cfg->boxNormalisationFileName());
		Messenger::print("  %s  %f\n", Keywords::configurationKeyword(Keywords::BraggMaximumQKeyword), cfg->braggMaximumQ());
		Messenger::print("  %s  %f\n", Keywords::configurationKeyword(Keywords::RDFBinWidthKeyword), cfg->rdfBinWidth());
		Messenger::print("  %s  %f\n", Keywords::configurationKeyword(Keywords::RDFRangeKeyword), cfg->rdfRange());
		Messenger::print("  %s  %i\n", Keywords::configurationKeyword(Keywords::RDFSmoothingKeyword), cfg->rdfSmoothing());
		Messenger::print("  %s  %f\n", Keywords::configurationKeyword(Keywords::TemperatureKeyword), cfg->temperature());

		Messenger::print("%s\n\n", Keywords::configurationKeyword(Keywords::EndConfigurationKeyword));
	}

	// Write PairPotentials block
	Messenger::print("# Pair Potentials\n");
	Messenger::print("%s\n", Keywords::inputBlock(Keywords::PairPotentialsBlock));
	Messenger::print("  %s  %f\n", Keywords::pairPotentialsKeyword(Keywords::RangeKeyword), pairPotentialRange_);
	Messenger::print("  %s  %f\n", Keywords::pairPotentialsKeyword(Keywords::DeltaKeyword), pairPotentialDelta_);
	Messenger::print("  %s  %f\n", Keywords::pairPotentialsKeyword(Keywords::TruncationWidthKeyword), pairPotentialTruncationWidth_);
	for (PairPotential* pot = pairPotentials_.first(); pot != NULL; pot = pot->next)
	{
		if (pot->type() == PairPotential::CoulombType) Messenger::print("  %s  '%s'  '%s'  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->chargeI(), pot->chargeJ());
		else if (pot->type() == PairPotential::DispersionType) Messenger::print("  %s  '%s'  '%s'  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->sigmaIJ(), pot->epsilonIJ());
		else Messenger::print("  %s  '%s'  '%s'  %f  %f  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->sigmaIJ(), pot->epsilonIJ(), pot->chargeI(), pot->chargeJ());
	}
	Messenger::print("%s\n\n", Keywords::pairPotentialsKeyword(Keywords::EndPairPotentialsKeyword));

	// Write Simulation block
	Messenger::print("# Simulation\n");
	Messenger::print("%s\n", Keywords::inputBlock(Keywords::SimulationBlock));
	Messenger::print("  %s  %i\n", Keywords::simulationKeyword(Keywords::BoxNormalisationPointsKeyword), boxNormalisationPoints_);
	Messenger::print("  %s  %i\n", Keywords::simulationKeyword(Keywords::SeedKeyword), seed_);
	Messenger::print("  %s  %s\n", Keywords::simulationKeyword(Keywords::WindowFunctionKeyword), Data2D::windowFunction(windowFunction_));
	Messenger::print("%s\n\n", Keywords::simulationKeyword(Keywords::EndSimulationKeyword));

	// Additional data?
	if (!includeData) return;

	Messenger::print("\n");

	// Reference Data
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		if (!sam->hasReferenceData()) continue;
		Messenger::print("\n# Reference data for Sample '%s':\n", sam->name());
		sam->referenceData().dump();
	}

	// Configurations
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		Messenger::print("\n# Configuration: '%s'\n", cfg->name());
		Messenger::print("# Atomic coordinates\n");
		Atom* atoms = cfg->atoms();
		for (int n=0; n<cfg->nAtoms(); ++n)
		{
			Messenger::print("%10i  %5s  %10.4e  %10.4e  %10.4e  %i  %i\n", n+1, PeriodicTable::element(atoms[n].element()).symbol(), atoms[n].r().x, atoms[n].r().y, atoms[n].r().z, atoms[n].localTypeIndex(), atoms[n].globalTypeIndex());
		}

		Messenger::print("# Box normalisation\n");
		Data2D boxNorm = cfg->boxNormalisation();
		boxNorm.dump();
	}

	// PairPotentials
	for (PairPotential* pp = pairPotentials_.first(); pp != NULL; pp = pp->next)
	{
		Messenger::print("\n# PairPotential between atomtypes %s and %s:\n", pp->atomTypeNameI(), pp->atomTypeNameJ());
		Messenger::print("# Original:\n");
		pp->originalU().dump();
		Messenger::print("# Current:\n");
		pp->u().dump();
		Messenger::print("# Current (Derivative):\n");
		pp->dU().dump();
	}
}
