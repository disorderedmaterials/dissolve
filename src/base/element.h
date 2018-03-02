/*
	*** Element Definition
	*** src/base/element.h
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

#ifndef DUQ_ELEMENT_H
#define DUQ_ELEMENT_H

#include "templates/list.h"
#include "base/charstring.h"
#include "base/isotope.h"
#include "base/parameters.h"

// Element Definition
class Element
{
	public:
	// Constructor
	Element();


	/*
	 * Element Information
	 */
	private:
	// Atomic number (Z)
	int z_;
	// Element name
	CharString name_;
	// Element symbol
	CharString symbol_;
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


	/*
	 * Isotopes
	 */
	private:
	// Isotope list
	List<Isotope> isotopes_;

	public:
	// Add new Isotope to this element
	Isotope* addIsotope();
	// Return Isotopes for Element
	const List<Isotope>& isotopes() const;
	// Return nth Isotope
	Isotope* isotope(int n);
	// Return number of Isotopes in list
	int nIsotopes() const;
	// Return Isotope with specified A (if it exists)
	Isotope* hasIsotope(int A) const;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool);
};

#endif
