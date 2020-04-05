/*
	*** XRay Weights Container
	*** src/classes/xrayweights.h
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

#ifndef DISSOLVE_XRAYWEIGHTS_H
#define DISSOLVE_XRAYWEIGHTS_H

#include "classes/atomtypelist.h"
#include "data/formfactors.h"
#include "genericitems/base.h"
#include "templates/array2d.h"
#include "templates/list.h"
#include <functional>
#include <vector>

// Forward Declarations
class SpeciesInfo;

// XRay Weights Container
class XRayWeights : public GenericItemBase
{
      public:
	// Constructor
	XRayWeights();
	// Copy Constructor
	XRayWeights(const XRayWeights &source);
	// Assignment operator
	void operator=(const XRayWeights &source);

	/*
	 * Source AtomTypes
	 */
      private:
	// X-Ray form factors to use
	XRayFormFactors::XRayFormFactorData formFactors_;
	// Type list derived from supplied Species
	AtomTypeList atomTypes_;
	// Form factor data for atom types
	std::vector<std::reference_wrapper<const FormFactorData>> formFactorData_;
	// Whether the structure is valid (i.e. has been finalised)
	bool valid_;

      private:
	// Retrieve form factor data for the current atom types
	bool getFormFactors();

      public:
	// Clear contents
	void clear();
	// Set-up from supplied SpeciesInfo list
	bool setUp(List<SpeciesInfo> &speciesInfoList, XRayFormFactors::XRayFormFactorData formFactors);
	// Return AtomTypeList
	AtomTypeList &atomTypes();
	// Return number of used AtomTypes
	int nUsedTypes() const;
	// Print atomtype information
	void print() const;

	/*
	 * Data
	 */
      private:
	// Concentration product matrix (ci * cj)
	Array2D<double> concentrationProducts_;
	// Pre-factors matrix (ci * cj * [2-dij])
	Array2D<double> preFactors_;

      private:
	// Calculate  matrices based on current AtomType information
	void calculateMatrices();

      public:
	// Return concentration product for types i and j
	double concentrationProduct(int i, int j) const;
	// Return form factor product for types i and j at specified Q value
	double formFactorProduct(int i, int j, double Q) const;
	// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) at specified Q value
	double weight(int i, int j, double Q) const;
	// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) over supplied Q values
	Array<double> weight(int i, int j, const Array<double> Q) const;
	// Return whether the structure is valid (i.e. has been finalised)
	bool isValid() const;

	/*
	 * GenericItemBase Implementations
	 */
      public:
	// Return class name
	static const char *itemClassName();
	// Read data through specified LineParser
	bool read(LineParser &parser, const CoreData &coreData);
	// Write data through specified LineParser
	bool write(LineParser &parser);

	/*
	 * Parallel Comms
	 */
      public:
	// Broadcast item contents
	bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
	// Check item equality
	bool equality(ProcessPool &procPool);
};

#endif
