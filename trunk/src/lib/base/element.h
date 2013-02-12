/*
	*** Element Definition
	*** src/lib/base/element.h
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

#ifndef DUQ_ELEMENT_H
#define DUQ_ELEMENT_H

#include "templates/list.h"
#include "base/dnchar.h"
#include "base/isotope.h"
#include "base/parameters.h"

/*!
 * \brief Element Definition
 * \details Element definition, including names and representative colour for a single element.
 */
class Element
{
	public:
	// Constructor
	Element();


	/*!
	 * \name Element Information
	 */
	///@{
	private:
	// Atomic number (Z)
	int z_;
	// Element name
	Dnchar name_;
	// Element symbol
	Dnchar symbol_;
	// Rough elemental radius (for bond calculation etc.)
	double atomicRadius_;
	// Element colour
	float colour_[4];

	public:
	// Set element information
	void set(int z, const char* name, const char* symbol, double radius, double r, double g, double b, double a);
	// Return atomic number (Z)
	int z();
	// Return name of element
	const char* name() const;
	// Return symbol of element
	const char* symbol() const;
	// Return atomic radius of element
	double atomicRadius() const;
	// Return element colour
	const float* colour() const;
	///@}


	/*!
	 * \name Isotopes
	 */
	///@{
	private:
	// Isotope list
	List<Isotope> isotopes_;

	public:
	// Add new Isotope to this element
	Isotope* addIsotope();
	// Return first Isotope
	Isotope* isotopes() const;
	// Return nth Isotope
	Isotope* isotope(int n);
	// Return number of Isotopes in list
	int nIsotopes() const;
	// Return Isotope with specified A (if it exists)
	Isotope* hasIsotope(int A) const;
	///@}


	/*!
	 * \name Parameters
	 */
	///@{
	private:
	// Basic Parameters
	List<Parameters> parameters_;
	
	public:
	// Add new Parameters to this element
	Parameters* addParameters();
	// Return first Parameters
	Parameters* parameters() const;
	// Return nth Parameters
	Parameters* parameters(int n);
	// Return number of Parameters in list
	int nParameters() const;
	// Find Parameters with name specified
	Parameters* findParameters(const char* name) const;
	// Return index of specified parameters
	int indexOfParameters(Parameters* p) const;
	///@}


	/*!
	 * \name Parallel Comms
	 */
	///@{
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast();
	///@}
};

#endif
