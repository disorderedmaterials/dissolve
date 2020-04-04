/*
	*** Waasmaier & Kirfel X-Ray Form Factors
	*** src/data/formfactors_wk1995.h
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

#ifndef DISSOLVE_DATA_FORMFACTORS_WK1995_H
#define DISSOLVE_DATA_FORMFACTORS_WK1995_H

#include "data/formfactor.h"

// Waasmaier & Kirfel '95 Form Factor Data
class FormFactorData_WK1995 : public FormFactorData
{
	public:
	// Constructor
	FormFactorData_WK1995(int z = 0, int formalCharge = 0, double a1 = 0.0, double b1 = 0.0, double a2 = 0.0, double b2 = 0.0, double a3 = 0.0, double b3 = 0.0, double a4 = 0.0, double b4 = 0.0, double a5 = 0.0, double b5 = 0.0, double c = 0.0);
	// Assignment Operator
	FormFactorData_WK1995& operator=(const FormFactorData_WK1995& source);


	/*
	 * Form Factor Data
	 */
	private:
	// Function parameters
	double a_[5], b_[5], c_;

	public:
	// Return index of isotope in it's Element parent's list
	int index() const;


	/*
	 * Form Factor Generation
	 */
	public:
	// Return magnitude of form factor at specified Q value
	double magnitude(double Q = 0.0) const;
};

#endif
