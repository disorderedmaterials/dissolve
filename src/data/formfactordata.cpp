/*
	*** Form Factor Data Base Class
	*** src/data/formfactordata.h
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

#include "data/formfactordata.h"

FormFactorData::FormFactorData(int Z, int formalCharge) : Z_(Z), formalCharge_(formalCharge) {}

/*
 * Element State
 */

// Return element to which the data relates
int FormFactorData::Z() const { return Z_; }

// Return formal charge of element to which the data relates
int FormFactorData::formalCharge() const { return formalCharge_; }

/*
 * Form Factor Generation
 */

// Return magnitude of form factor at specified Q value
double FormFactorData::magnitude(double Q) const { return 0.0; }
