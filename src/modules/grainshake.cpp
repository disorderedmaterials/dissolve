/*
	*** Dissolve Methods - GrainShake
	*** src/main/methods_grainshake.cpp
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
#include "modules/grainshake.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/changestore.h"
#include "classes/energykernel.h"
#include "base/processpool.h"
#include "base/timer.h"
#include "math/matrix4.h"

/*
 * Constructor / Destructor
 */

// Constructor
GrainShakeModule::GrainShakeModule() : Module()
{
	// Set up variables / control parameters
	setVariable("cutoffDistance", -1.0);
	setVariable("nShakesPerGrain", 1);
	setVariable("targetAcceptanceRate", 0.33);
	setVariable("translationStepSize", 0.05);
	setVariable("rotationStepSize", 2.0);
}

// Destructor
GrainShakeModule::~GrainShakeModule()
{
}

// Create instance of this module
Module* GrainShakeModule::createInstance()
{
	return new GrainShakeModule;
}

/*
 * Definition
 */

// Return type of module
const char* GrainShakeModule::type()
{
	return "GrainShake";
}

// Return brief description of module
const char* GrainShakeModule::brief()
{
	return "Perform Monte Carlo on all grains";
}

// Return type of module
Module::ModuleType GrainShakeModule::type()
{
	return Module::EvolutionModule;
}

/*
 * Processing
 */

// Run main processing

bool GrainShakeModule::execute(Dissolve& dissolve)
{
	/*
	* Perform a Grain shake
	* 
	* This is a parallel routine, operating in process groups.
	 */

	// Get target Configuration
	Configuration* cfg = targetConfigurations_.firstItem();

	// Retrieve control parameters from Configuration
	const double cutoffDistance = variableAsDouble("cutoffDistance") < 0.0 ? dissolve.pairPotentialRange() : variableAsDouble("cutoffDistance");
	const int nShakesPerGrain = variableAsInt("nShakesPerGrain");
	const double targetAcceptanceRate = variableAsDouble("targetAcceptanceRate");
	double rotationStepSize = variableAsDouble("rotationStepSize");
	double translationStepSize = variableAsDouble("translationStepSize");
	const double termScale = 1.0;
	const double rRT = 1.0/(.008314472*cfg->temperature());
	const double cutoffSq = cutoffDistance*cutoffDistance;

	// Print argument/parameter summary
	Messenger::print("GrainShake: Cutoff distance is %f\n", cutoffDistance);
	Messenger::print("GrainShake: Performing %i shake(s) per Grain\n", nShakesPerGrain);
	Messenger::print("GrainShake: Translation step is %f Angstroms, rotation step is %f degrees, target acceptance rate is %f.\n", translationStepSize, rotationStepSize, targetAcceptanceRate);

	// Initialise the Cell distributor
	const bool willBeModified = true, allowRepeats = false;
	cfg->initialiseCellDistribution();

	// Create a local ChangeStore and EnergyKernel
	ChangeStore changeStore;
	EnergyKernel kernel(cfg, dissolve.potentialMap());

	// Initialise the random number buffer
	Comm.initialiseRandomBuffer(ProcessPool::Group);

	// Enter calculation loop until no more Cells are available
	int cellId, n, shake, m;
	Matrix4 transform;
	int nTries = 0, nAccepted = 0;
	bool accept;
	double delta, currentEnergy, newEnergy, totalDelta = 0.0;
	Cell* cell;
	Atom* i;
	Grain* grainI;
	Vec3<double> centre;

	Timer timer;
	Comm.resetAccumulatedTime();
	while (cellId = cfg->nextAvailableCell(willBeModified, allowRepeats), cellId != Cell::AllCellsComplete)
	{
		// Check for valid cell
		if (cellId == Cell::NoCellsAvailable)
		{
			// No valid cell, but still need to enter into change distribution with other processes
			changeStore.distributeAndApply(cfg);
			cfg->finishedWithCell(willBeModified, cellId);
			continue;
		}
		cell = cfg->cell(cellId);
		Messenger::printVerbose("Cell %i now the target, containing %i Grains interacting with %i neighbours.\n", cellId, cell->nGrains(), cell->nTotalCellNeighbours());

		/*
		 * Calculation Begin
		 */

		nTries += cell->nGrains() * nShakesPerGrain;
		for (n=0; n<cell->nGrains(); ++n)
		{
			// Get current Grain and  
			grainI = cell->grain(n);
			currentEnergy = kernel.energy(grainI, false, ProcessPool::Group);
			currentEnergy += kernel.fullIntraEnergy(grainI, termScale);

			// Set current Grain as target in ChangeStore
			changeStore.add(grainI);

			// Loop over number of shakes per Grain
			for (shake=0; shake<nShakesPerGrain; ++shake)
			{
				// Create a random transformation matrix
				transform.createRotationXY(Comm.randomPlusMinusOne()*rotationStepSize, Comm.randomPlusMinusOne()*rotationStepSize);
				transform.setTranslation(Comm.randomPlusMinusOne()*translationStepSize, Comm.randomPlusMinusOne()*translationStepSize, Comm.randomPlusMinusOne()*translationStepSize);
				
				// Generate new Atom coordinates - must store the original Grain centre, since it will change as soon
				// as an Atom coordinate gets modified.
				centre = grainI->centre();
				for (m=0; m<grainI->nAtoms(); ++m)
				{
					i = grainI->atom(m);
					i->setCoordinates(transform * (i->r() - centre) + centre);
				}

				// Calculate new energy
				newEnergy = kernel.energy(grainI, false, ProcessPool::Group);
				newEnergy += kernel.fullIntraEnergy(grainI, termScale);

				// Trial the transformed Grain position
				delta = newEnergy - currentEnergy;
				accept = delta < 0 ? true : (Comm.random() < exp(-delta*rRT));

				if (accept)
				{
// 					Messenger::print("Accepts move with delta %f\n", delta);
					// Accept new (current) position of target Grain
					changeStore.updateAll();
					currentEnergy = newEnergy;
					totalDelta += delta;
					++nAccepted;
				}
				else changeStore.revertAll();
			}
			
			// Store modifications to Atom positions ready for broadcast later
			changeStore.storeAndReset();
		}

		/*
		 * Calculation End
		 */

		// Distribute coordinate changes to all processes
		changeStore.distributeAndApply(cfg);
		changeStore.reset();

		// Must unlock the Cell when we are done with it!
		cfg->finishedWithCell(willBeModified, cellId);
	}
	timer.stop();

	// Grains have moved, so refold and update locations
	cfg->updateGrains();

	// Collect statistics from process group leaders
	if (!Comm.allSum(&nAccepted, 1, ProcessPool::Leaders)) return false;
	if (!Comm.allSum(&nTries, 1, ProcessPool::Leaders)) return false;
	if (!Comm.allSum(&totalDelta, 1, ProcessPool::Leaders)) return false;
	if (Comm.groupLeader())
	{
		double rate = double(nAccepted)/nTries;

		Messenger::print("GrainShake: Overall acceptance rate was %4.2f (%i of %i attempted moves) (%s work, %s comms)\n", rate, nAccepted, nTries, timer.totalTimeString(), Comm.accumulatedTimeString());
		Messenger::print("GrainShake: Total energy delta was %10.4e kJ/mol.\n", totalDelta);

		// Adjust step size
		translationStepSize *= rate/targetAcceptanceRate;
		rotationStepSize *= rate/targetAcceptanceRate;

// 		if (translationStepSize < 0.05) translationStepSize = 0.05;
// 		else if (translationStepSize > maxTranslationStep_) translationStepSize = maxTranslationStep_;
// 		if (rotationStepSize < 3.0) rotationStepSize = 3.0;
	}

	// Store updated parameter values
	if (!procPool.broadcast(&translationStepSize, 1, 0, ProcessPool::Group)) return false;
	if (!procPool.broadcast(&rotationStepSize, 1, 0, ProcessPool::Group)) return false;
// 	rotationStepSizeParam->setValue(rotationStepSize); TODO
// 	translationStepSizeParam->setValue(translationStepSize);
	Messenger::print("GrainShake: Updated translation step is %f Angstroms, rotation step is %f degrees.\n", translationStepSize, rotationStepSize);

	// Update total energy
	cfg->registerEnergyChange(totalDelta);
	cfg->accumulateEnergyChange();

	// Increase contents version in Configuration
	if (nAccepted > 0) cfg->incrementCoordinateIndex();

	return true;
}
