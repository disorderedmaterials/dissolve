/*
	*** dUQ - Setup
	*** src/lib/main/setup.cpp
	Copyright T. Youngs 2012-2013

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
#include "main/flags.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/molecule.h"
#include "base/comms.h"

/*
// Setup
*/

/*!
 * \brief Clear all data
 */
void DUQ::clearModel()
{
	// Configuration
	configuration_.clear();

	// AtomType index and PotentialMap
	typeIndex_.clear();
	potentialMap_.clear();
}

/*!
 * \brief Load Box normalisation array
 */
bool DUQ::loadBoxNormalisationFile(const char* fileName)
{
	if (!boxNormalisation_.load(fileName))
	{
		msg.error("Failed to load Box normalisation data.\n");
		return false;
	}
	
	boxNormalisationFileName_ = fileName;
	msg.print("--> Successfully loaded box normalisation data from file '%s'.\n", boxNormalisationFileName_.get());
	boxNormalisation_.interpolate();

	return true;
}

/*!
 * \brief Setup pairpotentials
 */
bool DUQ::setupPotentials()
{
	// We will construct a complete list of all AtomTypes used over all Samples.
	// So, loop over Samples and go through Isotopologues in each mixture
	typeIndex_.clear();
	for (Molecule* mol = configuration_.molecules(); mol != NULL; mol = mol->next)
	{
		for (SpeciesAtom* i = mol->species()->atoms(); i != NULL; i = i->next) typeIndex_.add(i->atomType(), PeriodicTable::element(i->element()).isotopes(), 1);
	}
	msg.print("--> %i unique AtomTypes (disregarding isotopic substitutions) used in system:\n", typeIndex_.nItems());

	// Calculate fractional populations
	typeIndex_.finalise();
	
	// Print AtomType information (excluding isotope information)
	typeIndex_.print();

	// Assign indices to atom types
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next) at->setIndex(typeIndex_.indexOf(at));

	// Create PairPotential matrix
	msg.print("--> Creating matrix (%ix%i)...\n", typeIndex_.nItems(), typeIndex_.nItems());
	if (!potentialMap_.initialise(typeIndex_, pairPotentials_, pairPotentialRange_)) return false;
	
	return true;
}

/*!
 * \brief Print Stetup
 * \details Prints all variables controlling the simulation
 */
bool DUQ::printSetup()
{
	msg.print("\n");
	msg.print("Simulation Setup:\n");
	msg.print("%-40s = %9.3e\n", "Q-Dependent Broadening FWHM", qDependentFWHM_);
	msg.print("%-40s = %9.3e\n", "Q-Independent Broadening FWHM", qIndependentFWHM_);
	msg.print("%-40s = %9i\n", "Random Seed", seed_);
	msg.print("%-40s = %9.3e (%s)\n", "Simulation Temperature", temperature_, "K");
	msg.print("%-40s = %9.3e (%s)\n", "RDF Bin Width", rdfBinWidth_, "Angstroms");
	msg.print("%-40s = %9.3e (%s)\n", "RDF Range", rdfRange_, "Angstroms");
	msg.print("%-40s = %s\n", "RDF Calculation Method", rdfMethod(rdfMethod_));
	msg.print("%-40s = %9i\n", "Simplex NCycles", simplexNCycles_);
	msg.print("%-40s = %9i\n", "Simplex NMoves", simplexNMoves_);
	msg.print("%-40s = %9.3e (%s)\n", "Simplex Temperature", simplexTemperature_, "barns/sr/atom");
	msg.print("%-40s = %9.3e (%s)\n", "Simplex Tolerance", simplexTolerance_, "barns/sr/atom");

	return true;
}

/*!
 * \brief Check current setup
 * \details Checks all defined objects (Species, Grains, Samples etc.) for consistency, omissions, and glaring errors
 * which will prevent (or badly damage) the calculation about to begin. It returns false in the event of encountering
 * one or more severe errors which prevent the calculation from running.
 */
bool DUQ::checkSetup()
{
	int nErrors = 0;
	msg.print("\n");
	msg.print("*** Checking System Setup ***\n");

	/* Check each defined Species */
	msg.print("\n");
	msg.print("Checking Species...\n");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		msg.print("--- Species '%s'...\n", sp->name());
		nErrors += sp->checkSetup(atomTypes_);
	}

	/* Check System Composition */
	msg.print("\n");
	msg.print("Checking System composition...\n");
	if (multiplier_ < 1)
	{
		msg.error("System multiplier is zero or negative (%i).\n", multiplier_);
		++nErrors;
	}
	else for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		if ((sp->relativePopulation()*multiplier_) < 1)
		{
			msg.error("Relative population for Species '%s' is too low (%e) to provide any Molecules in the system.\n", sp->name(), sp->relativePopulation());
			++nErrors;
		}
	}
	
	/* Check Sample Definitions */
	/* At the same time we will build up a list of used AtomTypes */
	msg.print("\n");
	msg.print("Checking Sample definitions....\n");
	RefList<AtomType,int> typeCount(atomTypes_, 0);
	if (samples_.nItems() == 0)
	{
		msg.error("No Samples defined.\n");
		++nErrors;
	}
	else for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		msg.print("--- Sample '%s'...\n", sam->name());
		for (IsotopologueMix* mix = sam->isotopologueMixtures(); mix != NULL; mix = mix->next)
		{
			// Loop over Isotopologues in this mixture
			for (RefListItem<Isotopologue,double>* ri = mix->isotopologues(); ri != NULL; ri = ri->next)
			{
				// Check populations for each tope in the mixture
				if ((ri->data*multiplier_) < 1)
				{
					msg.error("Relative population for Isotopologue '%s' in Species '%s' is too low (%e) to provide any Molecules in the system.\n", ri->item->name(), mix->species()->name(), ri->data);
					++nErrors;
				}
				
				// Go through AtomTypes in Isotopologue, building up typeCount
				for (RefListItem<AtomType,Isotope*>* rj = ri->item->isotopes(); rj != NULL; rj = rj->next)
				{
					RefListItem<AtomType,int>* rk = typeCount.contains(rj->item);
					++rk->data;
				}
			}
		}
	}

	/* AtomTypes */
	/* Use AtomType count RefList constructed above */
	msg.print("\n");
	msg.print("Checking AtomType usage....\n");
	for (RefListItem<AtomType,int>* ri = typeCount.first(); ri != NULL; ri = ri->next)
	{
		// Warn about unused AtomTypes
		if (ri->data == 0)
		{
			msg.error("AtomType '%s' is defined but is not present in the system as it stands.\n", ri->item->name());
			++nErrors;
		}

	}

	/* Pair Potentials */
	/* We expect a PairPotential to have been defined for every combination of AtomType used in the system */
	msg.print("\n");
	msg.print("Checking PairPotential definitions....\n");
	for (RefListItem<AtomType,int>* ri = typeCount.first(); ri != NULL; ri = ri->next)
	{
		for (RefListItem<AtomType,int>* rj = ri; rj != NULL; rj = rj->next)
		{
			PairPotential* pot = hasPairPotential(ri->item, rj->item);
			if (pot == NULL)
			{
				msg.error("A PairPotential between AtomTypes '%s' and '%s' is required, but has not been defined.\n", ri->item->name(), rj->item->name());
				++nErrors;
			}
			else if ((ri->data == 0) || (rj->data == 0))
			{
				msg.error("A PairPotential between AtomTypes '%s' and '%s' has been defined, but is not required.\n", ri->item->name(), rj->item->name());
				++nErrors;
			}
		}
	}

	msg.print("\n");
	if (nErrors == 0) msg.print("No errors found with system setup.\n");
	else msg.print("%i error(s) found in system setup.\n", nErrors);

	return (nErrors == 0);
}

/*!
 * \brief Setup atomic configuration
 * \details Create or load the atomic configuration to simulate
 */
bool DUQ::setupConfiguration()
{
	// First stage is always to create 'empty' molecules in the main Configuration
	msg.print("--> Creating space for molecules...\n");
	int count;
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		// For safety, only one process will determine the number of molecules of each component
		if (Comm.master()) count = sp->relativePopulation() * multiplier_;
		if (!Comm.broadcast(&count, 1)) return false;
		
		for (int n = 0; n < count; ++n) configuration_.addMolecule(sp);
	}

	// Create a Box (all processes)
	msg.print("--> Creating periodic box...\n");
	if (!configuration_.setupBox(pairPotentialRange_, relativeBoxLengths_, boxAngles_, atomicDensity(), cellDensityMultiplier_, nonPeriodic_))
	{
		msg.error("Failed to set-up Box/Cells for Configuration.\n");
		return false;
	}

	// Now, we need some coordinates - either we are creating a random configuration of molecules, or we are loading in a set of coordinates from a file
	Species sourceCoordinates;
	if (!randomConfiguration_)
	{
		// Construct a temporary Species to load the source coordinates into
		if (!MPIRunMaster(sourceCoordinates.load(initialCoordinatesFile_.get()))) return false;
		sourceCoordinates.broadcast(atomTypes_);

		// Species now contains some number of atoms - does it match the number in the configuration's molecules?
		if (configuration_.nAtoms() != sourceCoordinates.nAtoms())
		{
			msg.error("Number of atoms in initial coordinates file (%i) does not match that in configuration (%i).\n", sourceCoordinates.nAtoms(), configuration_.nAtoms());
			return false;
		}
	}

	// Create Atom and Grain arrays, and Molecule copies
	msg.print("\n");
	msg.print("Setting up molecules, atoms, and grains...\n");
	if (!configuration_.setupMolecules(sourceCoordinates))
	{
		msg.error("Failed to setup molecules.\n");
		return false;
	}

	if (!configuration_.updateAtomsInCells()) return false;
	updateGrains(configuration_);
}

/*!
 * \brief Setup Simulation
 * \details This sets up all data necessary to perform a simulation, based on the current loaded configuration.
 */
bool DUQ::setupSimulation()
{
	msg.print("\n");
	msg.print("Setting up simulation...\n");

	if (multiplier_ < 1)
	{
		msg.error("System multiplier is zero or negative (%i).\n", multiplier_);
		return false;
	}

	clearModel();

	// Setup / load / create simulation system
	msg.print("\n");
	msg.print("Setting up atomic configuration...\n");
	setupConfiguration();

	// Setup PairPotentials array and assign atomtype indices to atoms
	msg.print("\n");
	msg.print("Setting up atomtypes and pair potentials...\n");
	if (!setupPotentials())
	{
		msg.error("Failed to create AtomType index, map, and PairPotential and array.\n");
		return false;
	}

	// Setup parallel comms / limits etc.
	msg.print("\n");
	msg.print("Setting up parallel comms...\n");

	// -- Assign Atom/Grain limits to processes
	if (!Comm.calculateLimits(configuration_.nAtoms(), configuration_.nGrains())) return false;

	// Construct RDF/S(Q) arrays
	// -- Determine maximal extent of RDF (from origin to centre of box)
	msg.print("\n");
	msg.print("Setting up RDF/S(Q) data...\n");
	Vec3<double> half = configuration_.box()->axes() * Vec3<double>(0.5,0.5,0.5);
	double maxR = half.magnitude(), inscribedSphereRadius = configuration_.box()->inscribedSphereRadius();
	msg.print("--> Maximal extent for RDFs is %f Angstrom (half cell diagonal distance).\n", maxR);
	msg.print("--> Inscribed sphere radius (maximum RDF range avoiding periodic images) is %f Angstroms.\n", inscribedSphereRadius);
	if (requestedRDFRange_ < -1.5)
	{
		msg.print("--> Using maximal non-minimum image range for RDF/S(Q).\n");
		rdfRange_ = inscribedSphereRadius;
	}
	else if (requestedRDFRange_ < -0.5)
	{
		msg.print("--> Using 90%% of maximal extent for RDF/S(Q).\n");
		rdfRange_ = 0.90*maxR;
	}
	else
	{
		msg.print("--> Specific RDF range supplied (%f Angstroms).\n", requestedRDFRange_);
		rdfRange_ = requestedRDFRange_;
		if (rdfRange_ < 0.0)
		{
			msg.error("Negative RDF range requested.\n");
			return false;
		}
		else if (rdfRange_ > maxR)
		{
			msg.error("Requested RDF range is greater then the maximum possible extent for the Box.\n");
			return false;
		}
		else if (rdfRange_ > (0.90*maxR)) msg.warn("Requested RDF range is greater than 90%% of the maximum possible extent for the Box. FT may be suspect!\n");
	}
	// 'Snap' rdfRange_ to nearest bin width...
	rdfRange_ = int(rdfRange_/rdfBinWidth_) * rdfBinWidth_;
	msg.print("--> RDF range (snapped to bin width) is %f Angstroms.\n", rdfRange_);

	// Does a box normalisation already exist (i.e. has been loaded)
	if (boxNormalisation_.nPoints() > 1) msg.print("--> Box normalisation array has already been loaded - no need to calculate.\n");
	else
	{
		// Only calculate if RDF range is greater than the inscribed sphere radius
		if (rdfRange_ <= inscribedSphereRadius)
		{
			msg.print("--> No need to calculate Box normalisation array since rdfRange is within periodic range.\n");
			boxNormalisation_.clear();
			double x = rdfBinWidth_*0.5;
			while (x < rdfRange_)
			{
				boxNormalisation_.addPoint(x, 1.0);
				x += rdfBinWidth_;
			}
			boxNormalisation_.interpolate();
		}
		else
		{
			msg.print("--> Calculating box normalisation array for RDFs...\n");
			if (!configuration_.box()->calculateRDFNormalisation(boxNormalisation_, rdfRange_, rdfBinWidth_, boxNormalisationPoints_)) return false;
		}
	}
	msg.print("\n");
	msg.print("--> Constructing RDF/F(Q) lists and matrices...\n");
	if (!setupPartials())
	{
		msg.error("Failed to create RDF/F(Q) lists/matrices.\n");
		return false;
	}

	// Prepare Samples
	msg.print("\n");
	msg.print("Preparing Samples...\n");
	if (!setupSamples())
	{
		msg.error("Failed to setup Samples.\n");
		return false;
	}

	// Setup/create simulation variables
	msg.print("\n");
	msg.print("Simulation variables:\n");
	if (!printSetup()) return false;
	
	setupCheckPointData();
	
	msg.print("\n");
	msg.print("Simulation setup is complete.\n");

	return true;
}

/*!
 * \brief Return random seed
 * \details The default value (-1) indicates that a new random seed (based on the system time) should be chosen.
 */
int DUQ::seed()
{
	return seed_;
}

/*!
 * \brief Return configuration pointer
 * \details Returns a pointer to the current configuration
 */
Configuration* DUQ::configuration()
{
	return &configuration_;
}
