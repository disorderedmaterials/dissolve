/*
	*** Structure Factor Module
	*** src/modules/fq.cpp
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
#include "modules/fq.h"
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
StructureFactor::StructureFactor() : Module()
{
	// Setup variables / control parameters
	setVariable("QDelta", 0.05);
	setVariable("QMax", -1.0);
	setVariable("Bragg", "off");
	setVariable("BraggQDepBroadening", 0.0063);
	setVariable("BraggQIndepBroadening", 0.0);
	setVariable("QDepBroadening", 0.02); // FWHM of Gaussian for Q-dependent instrument broadening function (if required)
	setVariable("QIndepepBroadening", 0.0); // FWHM of Gaussian for Q-independent instrument broadening function (if required)
	setVariable("NormalisedToAvSq", false);
	setVariable("NormalisedToSqAv", false);
}	
	
// Destructor
StructureFactor::~StructureFactor()
{
}

// Create instance of this module
Module* StructureFactor::createInstance()
{
	return new StructureFactor;
}

/*
 * Definition
 */

// Return name of module
const char* StructureFactor::name()
{
	return "StructureFactor";
}

// Return brief description of module
const char* StructureFactor::brief()
{
	return "Calculate total and partial structure factors";
}

// Return type of module
Module::ModuleType StructureFactor::type()
{
	return Module::CalculationModule;
}

// Number of Configurations that this module requires to run
int StructureFactor::nConfigurationsRequired()
{
	return 1;
}

// Return instance type for module
Module::InstanceType StructureFactor::instanceType()
{
	return Module::MultipleInstance;
}

/*
 * Method
 */

// Execute Method
bool StructureFactor::execute(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Calculate weighted or unweighted structure factors for the Sample or Configuration
	 * 
	 * This is a serial routine, with each process constructing its own copy of the data.
	 * Partial calculation routines called by this routine are parallel.
	 */

	// Get target Configuration
	Configuration* cfg = targetConfigurations_.firstItem();

	// Retrieve control parameters from Configuration
	const double cutoffDistance = variableAsDouble("cutoffDistance") < 0.0 ? duq.pairPotentialRange() : variableAsDouble("cutoffDistance");
	const int nShakesPerAtom = variableAsInt("nShakesPerAtom");
	const double targetAcceptanceRate = variableAsDouble("targetAcceptanceRate");
	double stepSize = variableAsDouble("stepSize");
	const double termScale = 1.0;
	const double rRT = 1.0/(.008314472*cfg->temperature());

	// Print argument/parameter summary
	Messenger::print("StructureFactor: Cutoff distance is %f\n", cutoffDistance);
	Messenger::print("StructureFactor: Performing %i shake(s) per Atom\n", nShakesPerAtom);
	Messenger::print("StructureFactor: Translation step is %f Angstroms, target acceptance rate is %f.\n", stepSize, targetAcceptanceRate);

	// If there is a Sample target, then we calculate the weighted structure factors for it (using the supplied Configurations)
	// Otherwise takt eh Configuration target and calculate unweighted structure factors for it.

// From Sample::finaliseReferenceData().	
// 		// Is data normalised?
// 	if (referenceDataNormalisation_ == Sample::AverageSquaredNormalisation)
// 	{
// 		Messenger::print("--> Removing normalisation (multiplying by <b>**2 = %f).\n", boundCoherentAverageSquared_);
// 		referenceData_.arrayY() *= boundCoherentAverageSquared_;
// 	}
// 	else if (referenceDataNormalisation_ == Sample::SquaredAverageNormalisation)
// 	{
// 		Messenger::print("--> Removing normalisation (multiplying by <b**2> = %f).\n", boundCoherentSquaredAverage_);
// 		referenceData_.arrayY() *= boundCoherentSquaredAverage_;
// 	}

}