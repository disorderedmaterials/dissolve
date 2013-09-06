/*
	*** Element Definition
	*** src/lib/base/element.cpp
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

#include "base/element.h"
#include "base/isotope.h"
#include "base/comms.h"
#include <string.h>

/*!
 * \brief Constructor
 * \details Constructor for Element. 
 */
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
// Element Information
*/

/*!
 * \brief Set element information
 */
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

/*!
 * \brief Return atomic number (Z)
 */
int Element::z()
{
	return z_;
}

/*!
 * \brief Return name of element
 */
const char *Element::name() const
{
	return name_.get();
}

/*!
 * \brief Return symbol of element
 */
const char *Element::symbol() const
{
	return symbol_.get();
}

/*!
 * \brief Return atomic radius of element
 */
double Element::atomicRadius() const
{
	return atomicRadius_;
}

/*!
 * \brief Return element colour
 */
const float *Element::colour() const
{
	return colour_;
}

/*
// Isotopes
*/

/*!
 * \brief Add new Isotope to this element
 */
Isotope *Element::addIsotope()
{
	return isotopes_.add();
}

/*!
 * \brief Return first Isotope
 */
Isotope *Element::isotopes() const
{
	return isotopes_.first();
}

/*!
 * \brief Return nth Isotope
 */
Isotope *Element::isotope(int n)
{
	return isotopes_[n];
}

/*!
 * \brief Return number of Isotopes in list
 */
int Element::nIsotopes() const
{
	return isotopes_.nItems();
}

/*!
 * \brief Return Isotope with specified A (if it exists)
 */
Isotope* Element::hasIsotope(int A) const
{
	for (Isotope* tope = isotopes_.first(); tope != NULL; tope = tope->next) if (tope->A() == A) return tope;
	return NULL;
}

/*
// Parameters
*/

/*!
 * \brief Add new Parameters to this element
 */
Parameters *Element::addParameters()
{
	return parameters_.add();
}

/*!
 * \brief Return first Parameters
 */
Parameters *Element::parameters() const
{
	return parameters_.first();
}

/*!
 * \brief Return nth Parameters
 */
Parameters *Element::parameters(int n)
{
	return parameters_[n];
}

/*!
 * \brief Return number of Parameters in list
 */
int Element::nParameters() const
{
	return parameters_.nItems();
}

/*!
 * \brief Find Parameters with name specified
 */
Parameters* Element::findParameters(const char* name) const
{
	for (Parameters* params = parameters_.first(); params != NULL; params = params->next) if (strcmp(params->name(),name) == 0) return params;
	return NULL;
}

/*!
 * \brief Return index of specified parameters
 */
int Element::indexOfParameters(Parameters* p) const
{
	return parameters_.indexOf(p);
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
bool Element::broadcast()
{
#ifdef PARALLEL
	// Send basic info
	msg.printVerbose("[MPI] Element ");
	if (!Comm.broadcast(name_)) return false;
	msg.printVerbose("%s...", name_.get());
	if (!Comm.broadcast(&z_, 1)) return false;
	msg.printVerbose("%i...", z_);
	if (!Comm.broadcast(symbol_)) return false;
	msg.printVerbose("%s...", symbol_.get());
	if (!Comm.broadcast(&atomicRadius_, 1)) return false;
	msg.printVerbose("%f...", atomicRadius_);
	if (!Comm.broadcast(colour_, 4)) return false;
	msg.printVerbose("%f %f %f %f...", colour_[0], colour_[1], colour_[2], colour_[3]);

	// Add isotopes
	bool result;
	msg.printVerbose("Isotopes...");
	BroadcastList<Isotope>(isotopes_, result);
	if (!result)
	{
		msg.print("Failed to broadcast Isotope data for element '%s'.\n", name_.get());
		return false;
	}
	
	// Add parameters
	msg.printVerbose("Parameters...");
	BroadcastList<Parameters>(parameters_, result);
	if (!result)
	{
		msg.print("Failed to broadcast Parameter data for element '%s'.\n", name_.get());
		return false;
	}
	msg.printVerbose("Success.\n");
#endif
	return true;
}
