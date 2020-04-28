/*
	*** Form Factor Base Class
	*** src/data/formfactor.h
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

#ifndef DISSOLVE_DATA_FORMFACTORDATA_H
#define DISSOLVE_DATA_FORMFACTORDATA_H

// Form Factor Base Class
class FormFactorData
{
      public:
	// Constructor
	FormFactorData(int Z = 0, int formalCharge = 0);

	/*
	 * Element State
	 */
      protected:
	// Element to which the data relates
	int Z_;
	// Formal charge of element to which the data relates
	int formalCharge_;

      public:
	// Return element to which the data relates
	int Z() const;
	// Return formal charge of element to which the data relates
	int formalCharge() const;

	/*
	 * Form Factor Generation
	 */
      public:
	// Return magnitude of form factor at specified Q value
	virtual double magnitude(double Q = 0.0) const;
};

#endif
