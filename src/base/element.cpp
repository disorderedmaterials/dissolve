/*
	*** Element Definition
	*** src/base/element.cpp
	Copyright T. Youngs 2012-2018

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

#include "base/element.h"
#include "base/isotope.h"
#include "base/processpool.h"
#include "templates/broadcastlist.h"
#include <string.h>

// Constructor
Element::Element()
{
	z_ = 0;
	atomicRadius_ = 0.0;
	colour_[0] = 0.0f;
	colour_[1] = 0.0f;
	colour_[2] = 0.0f;
	colour_[3] = 1.0f;
}

/*
 * Element Information
 */

// Set element information
void Element::set(int z, const char* name, const char* symbol, double radius, double r, double g, double b, double a)
{
	z_ = z;
	name_ = name;
	symbol_ = symbol;
	atomicRadius_ = radius;
	colour_[0] = (float) r;
	colour_[1] = (float) g;
	colour_[2] = (float) b;
	colour_[3] = (float) a;
}

// Return atomic number (Z)
int Element::z()
{
	return z_;
}

// Return name of element
const char* Element::name() const
{
	return name_.get();
}

// Return symbol of element
const char* Element::symbol() const
{
	return symbol_.get();
}

// Return atomic radius of element
double Element::atomicRadius() const
{
	return atomicRadius_;
}

// Return element colour
const float *Element::colour() const
{
	return colour_;
}

/*
 * Isotopes
 */

// Add new Isotope to this Element
Isotope* Element::addIsotope()
{
	Isotope* newIsotope = new Isotope();
	newIsotope->setElement(this);
	isotopes_.own(newIsotope);
	return newIsotope;
}

// Return Isotopes for Element
const List<Isotope>& Element::isotopes() const
{
	return isotopes_;
}

// Return nth Isotope
Isotope* Element::isotope(int n)
{
	return isotopes_[n];
}

// Return number of Isotopes in list
int Element::nIsotopes() const
{
	return isotopes_.nItems();
}

// Return Isotope with specified A (if it exists)
Isotope* Element::hasIsotope(int A) const
{
	for (Isotope* tope = isotopes_.first(); tope != NULL; tope = tope->next) if (tope->A() == A) return tope;
	return NULL;
}

// Return index of specified Isotope (or -1 if it does not exist)
int Element::isotopeIndex(Isotope* isotope)
{
	int index = 0;
	for (Isotope* tope = isotopes_.first(); tope != NULL; tope = tope->next, ++index) if (tope == isotope) return index;
	return -1;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Element::broadcast(ProcessPool& procPool)
{
#ifdef PARALLEL
	// Send basic info
	if (!procPool.broadcast(name_)) return false;
	if (!procPool.broadcast(&z_, 1)) return false;
	if (!procPool.broadcast(symbol_)) return false;
	if (!procPool.broadcast(&atomicRadius_, 1)) return false;
	if (!procPool.broadcast(colour_, 4)) return false;

	// Add isotopes
	BroadcastList<Isotope> isotopeBroadcaster(procPool, 0, isotopes_);
	if (isotopeBroadcaster.failed())
	{
		Messenger::print("Failed to broadcast Isotope data for element '%s'.\n", name_.get());
		return false;
	}
#endif
	return true;
}
