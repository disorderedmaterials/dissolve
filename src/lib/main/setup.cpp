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
bool DUQ::loadBoxNormalisationFile(const char *fileName)
{
	if (!boxNormalisation_.load(fileName))
	{
		msg.error("Failed to load Box normalisation data.\n");
		return FALSE;
	}
	
	boxNormalisationFileName_ = fileName;
	msg.print("--> Successfully loaded box normalisation data from file '%s'.\n", boxNormalisationFileName_.get());
	boxNormalisation_.interpolate();

	return TRUE;
}

/*!
 * \brief Create AtomType index and PairPotential map
 */
bool DUQ::setupIndexAndMaps()
{
	// We will construct a complete list of all AtomTypes used over all Samples.
	// So, loop over Samples and go through Isotopologues in each mixture
	typeIndex_.clear();
	for (Molecule* mol = configuration_.molecules(); mol != NULL; mol = mol->next)
	{
		for (Atom* i = mol->species()->atoms(); i != NULL; i = i->next) typeIndex_.add(i->atomType(), PeriodicTable::element(i->element()).isotopes(), 1);
	}
	msg.print("--> %i unique AtomTypes (disregarding isotopic substitutions) used in system:\n", typeIndex_.nItems());

	// Calculate fractional populations
	typeIndex_.finalise();
	
	// Print AtomType information (excluding isotope information)
	typeIndex_.print();

	// Create PairPotential matrix
	msg.print("--> Creating matrix (%ix%i)...\n", typeIndex_.nItems(), typeIndex_.nItems());
	return potentialMap_.initialise(typeIndex_, pairPotentials_, configuration_.nAtoms(), configuration_.molecules(), pairPotentialRange_);
}

/*!
 * \brief Set up index lists for Species
 */
bool DUQ::setupIntramolecular()
{
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		msg.print("--> Species: '%s'\n", sp->name());
		if (!sp->calculateIndexLists()) return FALSE;
	}
	return TRUE;
}

/*!
 * \brief Print Stetup
 * \details Prints all variables controlling the simulation
 */
bool DUQ::finaliseAndPrintSetup()
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

	return TRUE;
}

/*!
 * \brief Check current setup
 * \details Checks all defined objects (Species, Grains, Samples etc.) for consistency, ommissions, and glaring errors
 * which will prevent (or badly damage) the calculation about to begin. It returns FALSE in the event of encountering
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
 * \brief Setup Simulation
 * \details This sets up all data necessary to perform a simulation, based on the current loaded configuration. The steps are as follows:
 * 1) Intramolecular terms are set up for each Species
 * 2) A set of empty Molecules is created through repeated calls to Configuration::addMolecule(Species*)
 * 3) A periodic Box and Cell definition are created to hold the Molecules at the desired density with Configuration::setupBox()
 * 4) Master Atom and Grain arrays are created, linked together, and copies of randomised Species coordinates made with Configuration::instantiate()
 * 5) Process limits are determined
 * 6) An AtomType index and PairPotential map is created with setupIndexAndMaps()
 * 7) Partial RDF and S(Q) matrices are created
 * 8) Samples are setup
 * 9) Calculation variables are checked and printed out
 */
bool DUQ::setupSimulation()
{
	msg.print("\n");
	msg.print("Setting up simulation...\n");

	if (multiplier_ < 1)
	{
		msg.error("System multiplier is zero or negative (%i).\n", multiplier_);
		return FALSE;
	}

	clearModel();

	// Create intramolecular index lists
	msg.print("\n");
	msg.print("Calculating index lists for defined intramolecular interactions...\n");
	if (!setupIntramolecular()) return FALSE;
	
	// Loop over components, creating space for Molecules as we go (all processes)
	msg.print("\n");
	msg.print("Adding Molecules...\n");
	int count;
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		// For safety, only one process will determine the number of molecules of each component
		if (Comm.master()) count = sp->relativePopulation() * multiplier_;
		if (!Comm.broadcast(&count, 1)) return FALSE;
		
		for (int n = 0; n < count; ++n) configuration_.addMolecule(sp);
	}

	// Create a Box (all processes)
	msg.print("\n");
	msg.print("Setting up Box...\n");
	if (!configuration_.setupBox(pairPotentialRange_, relativeBoxLengths_, boxAngles_, atomicDensity(), nonPeriodic_))
	{
		msg.error("Failed to set-up Box/Cells for Configuration.\n");
		return FALSE;
	}

	// Create Atom and Grain arrays, and Molecule copies (Master only, broadcast to slaves)
	msg.print("\n");
	msg.print("Instantiating Atoms, Grains, and Molecules...\n");
	if (!configuration_.instantiate())
	{
		msg.error("Failed to instantiate Molecules.\n");
		return FALSE;
	}
	
	// Create random configuration / load initial coordinates from file
	if (randomConfiguration_)
	{
		if (!MPIRunMaster(configuration_.randomise())) return FALSE;
	}
	else if (!MPIRunMaster(configuration_.loadInitialCoordinates(initialCoordinatesFile_.get()))) return FALSE;
	if (!configuration_.broadcastCoordinates()) return FALSE;
	updateGrains(configuration_);

	// Assign Atom/Grain limits to processes
	msg.print("\n");
	msg.print("Setting process limits...\n");
	if (!Comm.calculateLimits(configuration_.nAtoms(), configuration_.nGrains())) return FALSE;

	// Construct AtomType index and array
	msg.print("\n");
	msg.print("Constructing AtomType index, map, and PairPotential array...\n");
	if (!setupIndexAndMaps())
	{
		msg.error("Failed to create AtomType index, map, and PairPotentail and array.\n");
		return FALSE;
	}

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
			return FALSE;
		}
		else if (rdfRange_ > maxR)
		{
			msg.error("Requested RDF range is greater then the maximum possible extent for the Box.\n");
			return FALSE;
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
			if (!configuration_.box()->calculateRDFNormalisation(boxNormalisation_, rdfRange_, rdfBinWidth_, boxNormalisationPoints_)) return FALSE;
		}
	}
	msg.print("\n");
	msg.print("--> Constructing RDF/F(Q) lists and matrices...\n");
	if (!setupPartials())
	{
		msg.error("Failed to create RDF/F(Q) lists/matrices.\n");
		return FALSE;
	}

	// Prepare Samples
	msg.print("\n");
	msg.print("Preparing Samples...\n");
	if (!setupSamples())
	{
		msg.error("Failed to setup Samples.\n");
		return FALSE;
	}

	// Setup/create simulation variables
	msg.print("\n");
	msg.print("Setting simulation variables...\n");
	if (!finaliseAndPrintSetup()) return FALSE;
	
	msg.print("\n");
	msg.print("Simulation setup is complete.\n");

	return TRUE;
}

/*!
 * \brief Return random seed
 * \details The default value (-1) indicates that a new random seed (based on the system time) should be chosen.
 */
int DUQ::seed()
{
	return seed_;
}
