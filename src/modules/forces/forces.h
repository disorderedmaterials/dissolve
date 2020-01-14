/*
	*** Forces Module
	*** src/modules/forces/forces.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_MODULE_FORCES_H
#define DISSOLVE_MODULE_FORCES_H

#include "module/module.h"
#include "io/import/forces.h"

// Forward Declarations
class Molecule;
class PotentialMap;

// Forces Module
class ForcesModule : public Module
{
	/*
	 * Calculates the total forces in a system
	 */

	public:
	// Constructor
	ForcesModule();
	// Destructor
	~ForcesModule();


	/*
	 * Instances
	 */
	public:
	// Create instance of this module
	Module* createInstance() const;


	/*
	 * Definition
	 */
	public:
	// Return type of module
	const char* type() const;
	// Return category for module
	const char* category() const;
	// Return brief description of module
	const char* brief() const;
	// Return the number of Configuration targets this Module requires
	int nRequiredTargets() const;


	/*
	 * Initialisation
	 */
	protected:
	// Perform any necessary initialisation for the Module
	void initialise();

	/*
	 * Data
	 */
	private:
	// Reference forces for test
	ForceImportFileFormat referenceForces_;


	/*
	 * Processing
	 */
	private:
	// Run main processing
	bool process(Dissolve& dissolve, ProcessPool& procPool);

	public:
	// Run set-up stage
	bool setUp(Dissolve& dissolve, ProcessPool& procPool);


	/*
	 * Force Methods
	 */
	public:
	// Calculate total intramolecular forces
	static void intramolecularForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Calculate interatomic forces within the specified Configuration
	static void interatomicForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Calculate total forces within the specified Configuration
	static void totalForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Calculate total intramolecular forces acting on specific atoms
	static void intramolecularForces(ProcessPool& procPool, Configuration* cfg, const Array<int>& targetIndices, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Calculate interatomic forces on specified atoms within the specified Configuration
	static void interatomicForces(ProcessPool& procPool, Configuration* cfg, const Array<int>& targetIndices, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Calculate forces acting on specific atoms within the specified Configuration (arising from all atoms)
	static void totalForces(ProcessPool& procPool, Configuration* cfg, const Array<int>& targetIndices, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Calculate forces acting on specific Molecules within the specified Configuration (arising from all atoms)
	static void totalForces(ProcessPool& procPool, Configuration* cfg, const Array<Molecule*>& targetMolecules, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz);
};

#endif

