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
#include "base/sysfunc.h"

// Dump full system setup
void DUQ::dumpSystemSetup(bool includeData)
{
	// Print the full system setup (in input file format)/
	// Can optionally include data (i.e. datasets, coordinates, box normalisation etc.) for extra checks

	// Write title comment
	Messenger::print("\n\n# SYSTEM DUMP\n\n");
	
	// Write AtomTypes block
	Messenger::print("# AtomType Definitions\n");
	Messenger::print("%s\n", InputBlocks::inputBlock(InputBlocks::AtomTypesBlock));
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next)
	{
		Messenger::print("  %s  '%s'  %3s  '%s'\n", AtomTypesBlock::keyword(AtomTypesBlock::AtomTypeKeyword), at->name(), periodicTable.element(at->element()).symbol(), at->parameters()->name());
	}
	Messenger::print("%s\n\n", AtomTypesBlock::keyword(AtomTypesBlock::EndAtomTypesKeyword));

	// Write Species data
	Messenger::print("# Species Definitions\n");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		Messenger::print("%s '%s'\n", InputBlocks::inputBlock(InputBlocks::SpeciesBlock), sp->name());
		
		// Atoms
		Messenger::print("  # Atoms\n");
		int count = 0;
		for (SpeciesAtom* i = sp->atoms(); i != NULL; i = i->next)
		{
			++count;
			Messenger::print("  %s  %3i  %3s  %8.3f %8.3f %8.3f %8.3f '%s'\n", SpeciesBlock::keyword(SpeciesBlock::AtomKeyword), count, periodicTable.element(i->element()).symbol(), i->r().x, i->r().y, i->r().z, i->charge(), i->atomType() == NULL ? "???" : i->atomType()->name());
		}
		
		// Bonds
		Messenger::print("\n  # Bonds\n");
		for (SpeciesBond* b = sp->bonds(); b != NULL; b = b->next)
		{
			Messenger::print("  %s  %3i  %3i  %8.3f %8.3f\n", SpeciesBlock::keyword(SpeciesBlock::BondKeyword), b->indexI()+1, b->indexJ()+1, b->equilibrium(), b->forceConstant());
		}
		
		// Angles
		Messenger::print("\n  # Angles\n");
		for (SpeciesAngle* a = sp->angles(); a != NULL; a = a->next)
		{
			Messenger::print("  %s  %3i  %3i  %3i  %8.3f %8.3f\n", SpeciesBlock::keyword(SpeciesBlock::AngleKeyword), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->equilibrium(), a->forceConstant());
		}
		
		// Grains
		Messenger::print("\n  # Grain Definitions\n");
		for (SpeciesGrain* sg = sp->grains(); sg != NULL; sg = sg->next)
		{
			Messenger::print("  %s  '%s'", SpeciesBlock::keyword(SpeciesBlock::GrainKeyword), sg->name());
			for (RefListItem<SpeciesAtom,int>* ri = sg->atoms(); ri != NULL; ri = ri->next) Messenger::print("  %i", ri->item->userIndex());
			Messenger::print("\n");
		}

		// Isotopologues
		Messenger::print("\n  # Isotopologues\n");
		for (Isotopologue* iso = sp->isotopologues(); iso != NULL; iso = iso->next)
		{
			Messenger::print("  %s  '%s'", SpeciesBlock::keyword(SpeciesBlock::IsotopologueKeyword), iso->name());
			for (RefListItem<AtomType,Isotope*>* ri = iso->isotopes(); ri != NULL; ri = ri->next)
			{
				Messenger::print("  %s=%i", ri->item->name(), ri->data->A());
			}
			Messenger::print("\n");
		}
		
		// Done with this species
		Messenger::print("%s\n\n", SpeciesBlock::keyword(SpeciesBlock::EndSpeciesKeyword));
	}

	// Write Configurations
	Messenger::print("# Configurations\n");
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		Messenger::print("%s  '%s'\n", InputBlocks::inputBlock(InputBlocks::ConfigurationBlock), cfg->name());
		Messenger::print("  %s  %i\n", ConfigurationBlock::keyword(ConfigurationBlock::MultiplierKeyword), cfg->multiplier());
		Messenger::print("  %s  %f  %s\n", ConfigurationBlock::keyword(ConfigurationBlock::DensityKeyword), cfg->density(), cfg->densityIsAtomic() ? "atoms/A3" : "g/cm3");
		Messenger::print("  %s  %f  %f  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::CellLengthsKeyword), cfg->relativeBoxLengths().x, cfg->relativeBoxLengths().y, cfg->relativeBoxLengths().z);
		Messenger::print("  %s  %f  %f  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::CellAnglesKeyword), cfg->boxAngles().x, cfg->boxAngles().y, cfg->boxAngles().z);
		if (cfg->nonPeriodic()) Messenger::print("  %s\n", ConfigurationBlock::keyword(ConfigurationBlock::NonPeriodicKeyword));
		if (!DUQSys::isEmpty(cfg->inputCoordinatesFile())) Messenger::print("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::InputCoordinatesKeyword), cfg->inputCoordinatesFile());
		if (!DUQSys::isEmpty(cfg->outputCoordinatesFile())) Messenger::print("  %s  '%s'  %i\n", ConfigurationBlock::keyword(ConfigurationBlock::OutputCoordinatesKeyword), cfg->outputCoordinatesFile(), cfg->coordinatesOutputFrequency());
		if (cfg->useOutputCoordinatesAsInput()) Messenger::print("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::UseOutputAsInputKeyword), DUQSys::btoa(true));

		// Species
		Messenger::print("\n  # Species\n");
		RefListIterator<Species,double> speciesIterator(cfg->usedSpecies());
		while (Species* sp = speciesIterator.iterate()) Messenger::print("  %s  '%s'  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesAddKeyword), sp->name(), speciesIterator.currentData());

		// Modules
		Messenger::print("\n  # Modules\n");
		if (cfg->nModules() == 0) Messenger::print("  # -- None\n");
		RefListIterator<Module,bool> moduleIterator(cfg->modules());
		while (Module* module = moduleIterator.iterate())
		{
			Messenger::print("  %s  %s  # %s\n", ConfigurationBlock::keyword(ConfigurationBlock::ModuleKeyword), module->name(), module->uniqueName());

			// For each Module, print all available options
			for (PlainValue* option = module->options().values(); option != NULL; option = option->next)
			{
				if (option->type() == PlainValue::StringType) Messenger::print("    %s  '%s'\n", option->name(), option->asString());
				else Messenger::print("    %s  %s\n", option->name(), option->asString());
			}

			Messenger::print("  %s\n", ModuleBlock::keyword(ModuleBlock::EndModuleKeyword));
		}

		Messenger::print("\n");
		Messenger::print("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::BoxNormalisationFileKeyword), cfg->boxNormalisationFileName());
		Messenger::print("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::BraggMaximumQKeyword), cfg->braggMaximumQ());
		Messenger::print("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::RDFBinWidthKeyword), cfg->rdfBinWidth());
		Messenger::print("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::RDFRangeKeyword), cfg->rdfRange());
		Messenger::print("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::TemperatureKeyword), cfg->temperature());

		Messenger::print("%s\n\n", ConfigurationBlock::keyword(ConfigurationBlock::EndConfigurationKeyword));
	}

	// Write Sample blocks
	Messenger::print("# Samples\n");
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		// Reference data present?
		if (!sam->referenceDataFileName().isEmpty())
		{
			Messenger::print("  %s '%s'\n", SampleBlock::keyword(SampleBlock::ReferenceDataKeyword), sam->referenceDataFileName().get());
		}

		// Modules
		Messenger::print("\n  # Modules\n");
		if (sam->nModules() == 0) Messenger::print("  # -- None\n");
		RefListIterator<Module,bool> moduleIterator(sam->modules());
		while (Module* module = moduleIterator.iterate())
		{
			Messenger::print("  %s  %s  # %s\n", SampleBlock::keyword(SampleBlock::ModuleKeyword), module->name(), module->uniqueName());

			// For each Module, print all available options
			for (PlainValue* option = module->options().values(); option != NULL; option = option->next)
			{
				if (option->type() == PlainValue::StringType) Messenger::print("    %s  '%s'\n", option->name(), option->asString());
				else Messenger::print("    %s  %s\n", option->name(), option->asString());
			}

			// Not in a Configuration block, so print associated Configuration information
			//Messenger::print("%s  '%s'\n", InputBlocks::inputBlock(Keywords::SampleBlock), sam->name()); 
			//for (IsotopologueMix* iso = sam->isotopologueMixtures(); iso != NULL; iso = iso->next)
			//{
			//	double sum = iso->totalRelative();
			//	for (RefListItem<Isotopologue,double>* ri = iso->isotopologues(); ri != NULL; ri = ri->next)
			//	{
			//		Messenger::print("  %s  '%s'  '%s'  %f\n", Keywords::sampleKeyword(Keywords::IsotopologueSampleKeyword), iso->species()->name(), ri->item->name(), ri->data);
			//	}
			//}

			Messenger::print("  %s\n", ModuleBlock::keyword(ModuleBlock::EndModuleKeyword));
		}

		Messenger::print("%s\n\n", SampleBlock::keyword(SampleBlock::EndSampleKeyword));
	}

	// Write PairPotentials block
	Messenger::print("# Pair Potentials\n");
	Messenger::print("%s\n", InputBlocks::inputBlock(InputBlocks::PairPotentialsBlock));
	Messenger::print("  %s  %f\n", PairPotentialsBlock::keyword(PairPotentialsBlock::RangeKeyword), pairPotentialRange_);
	Messenger::print("  %s  %f\n", PairPotentialsBlock::keyword(PairPotentialsBlock::DeltaKeyword), pairPotentialDelta_);
	Messenger::print("  %s  %f\n", PairPotentialsBlock::keyword(PairPotentialsBlock::TruncationWidthKeyword), pairPotentialTruncationWidth_);
	for (PairPotential* pot = pairPotentials_.first(); pot != NULL; pot = pot->next)
	{
		if (pot->type() == PairPotential::CoulombType) Messenger::print("  %s  '%s'  '%s'  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->chargeI(), pot->chargeJ());
		else if (pot->type() == PairPotential::DispersionType) Messenger::print("  %s  '%s'  '%s'  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->sigmaIJ(), pot->epsilonIJ());
		else Messenger::print("  %s  '%s'  '%s'  %f  %f  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->sigmaIJ(), pot->epsilonIJ(), pot->chargeI(), pot->chargeJ());
	}
	Messenger::print("%s\n\n", PairPotentialsBlock::keyword(PairPotentialsBlock::EndPairPotentialsKeyword));

	// Write Simulation block
	Messenger::print("# Simulation\n");
	Messenger::print("%s\n", InputBlocks::inputBlock(InputBlocks::SimulationBlock));
	Messenger::print("  %s  %i\n", SimulationBlock::keyword(SimulationBlock::BoxNormalisationPointsKeyword), boxNormalisationPoints_);
	Messenger::print("  %s  %i\n", SimulationBlock::keyword(SimulationBlock::SeedKeyword), seed_);
	Messenger::print("  %s  %s\n", SimulationBlock::keyword(SimulationBlock::WindowFunctionKeyword), Data2D::windowFunction(windowFunction_));
	Messenger::print("%s\n\n", SimulationBlock::keyword(SimulationBlock::EndSimulationKeyword));

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
