/*
	*** SpeciesIntra Definition
	*** src/classes/speciesintra.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_SPECIESINTRA_H
#define DUQ_SPECIESINTRA_H

#include "base/charstring.h"
#include "templates/array.h"

#define MAXINTRAPARAMS 4

// Forward Declarations
class Species;
class MasterIntra;

/*
 * SpeciesIntra Definition
 */
class SpeciesIntra
{
	public:
	// Constructor
	SpeciesIntra();
	// Destructor
	~SpeciesIntra();


	/*
	 * Basic Data
	 */
	protected:
	// Parent Species
	Species* parent_;

	public:
	// Set parent Species
	void setParent(Species* parent);
	// Return parent Species
	Species* parent() const;


	/*
	 * Interaction Parameters
	 */
	protected:
	// Linked master from which parameters should be taken (if relevant)
	MasterIntra* masterParameters_;
	// Index of functional form of interaction
	int form_;
	// Parameters for interaction
	double parameters_[MAXINTRAPARAMS];

	public:
	// Set linked master from which parameters should be taken
	void setMasterParameters(MasterIntra* master);
	// Return linked master from which parameters should be taken
	MasterIntra* masterParameters();
	// Set functional form index of interaction
	void setForm(int form);
	// Return functional form index of interaction
	int form() const;
	// Set nth parameter
	void setParameter(int id, double value);
	// Return nth parameter
	double parameter(int id) const;
	// Return array of parameters
	const double* parameters() const;
	// Return parameters as Array<double>
	Array<double> parametersAsArray() const;
	// Set parameters from Array<double>
	void setParametersFromArray(Array<double> params);
};

#endif
