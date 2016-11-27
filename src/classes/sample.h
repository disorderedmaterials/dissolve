/*
	*** Sample Definition
	*** src/classes/sample.h
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

#ifndef DUQ_SAMPLE_H
#define DUQ_SAMPLE_H

#include "classes/isotopologuemix.h"
#include "classes/atomtypelist.h"
#include "classes/histogram.h"
#include "modules/modulelist.h"
#include "base/dnchar.h"
#include "templates/reflist.h"

// Forward Declarations
class Species;

// Sample Definition
class Sample : public ModuleList, public ListItem<Sample>
{
	public:
	// Constructor
	Sample();
	// Copy Constructor
	Sample(const Sample& source);
	// Assignment Operator
	void operator=(const Sample& source);


	/*
	 * Basic Information
	 */
	private:
	// Descriptive name
	Dnchar name_;
	
	public:
	// Set name of Sample
	void setName(const char* name);
	// Return name of Sample
	const char* name() const;
	
	
	/*
	 * Isotopologue Definition
	 */
	private:
	// List of IsotopologueMix-tures for Species in this Sample
	List<IsotopologueMix> isotopologueMixtures_;
	// Type list for this Sample
	AtomTypeList atomTypes_;

	public:
	// Update IsotopologueMix data
	void updateIsotopologueMixtures(const List<Species>& species);
	// Return whether the Sample contains a mixtures definition for the provided Species
	IsotopologueMix* hasSpeciesIsotopologueMixture(Species* sp) const;
	// Add Isotopologue to mixture
	bool addIsotopologueToMixture(Species* sp, Isotopologue* iso, double relPop);
	// Return first IsotopologueMix
	IsotopologueMix* isotopologueMixtures() const;
	// Return nth IsotopologueMix
	IsotopologueMix* isotopologueMixture(int n);
	// Assign default (first) Isotopologues for all Species
	void assignDefaultIsotopes();

	public:
	// Create type list
	bool createTypeList(const List<Species>& allSpecies, const List<AtomType>& masterIndex);
	// Return AtomTypeList
	AtomTypeList& atomTypes();
	// Return number of used AtomTypes
	int nUsedTypes();


	/*
	 * Reference Data
	 */
	private:
	// Whether reference data exists
	bool hasReferenceData_;
	// Filename of reference data (if present)
	Dnchar referenceDataFileName_;
	// Reference data
	Data2D referenceData_;
	// Calculated data
	Data2D calculatedData_;

	public:
	// Return whether reference data exists
	bool hasReferenceData();
	// Load reference data
	bool loadReferenceData(const char* filename);
	// Return reference data filename (if any)
	Dnchar& referenceDataFileName();
	// Return reference data
	Data2D& referenceData();
	// Return calculated data
	Data2D& calculatedData();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, const List<Species>& species);
};

#endif
