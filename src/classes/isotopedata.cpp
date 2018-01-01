/*
	*** Isotope Data
	*** src/classes/isotopedata.cpp
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

#include "classes/isotopedata.h"
#include "base/isotope.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/ptable.h"
#include "base/processpool.h"

// Constructor
IsotopeData::IsotopeData() : MPIListItem<IsotopeData>()
{
	isotope_ = NULL;
	population_ = 0.0;
	fraction_ = 0.0;
}

// Copy Constructor
IsotopeData::IsotopeData(const IsotopeData& source)
{
	(*this) = source;
}

// Assignment Operator
void IsotopeData::operator=(const IsotopeData& source)
{
	isotope_ = source.isotope_;
	population_ = source.population_;
	fraction_ = source.fraction_;
}

/*
 * Properties
 */

// Initialise
bool IsotopeData::initialise(Isotope* isotope)
{
	isotope_ = isotope;
	if (isotope_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Isotope pointer passed to IsotopeData::initialise().\n");
		return false;
	}

	population_ = 0;
	fraction_ = 0.0;

	return true;
}

// Add to population of Isotope
void IsotopeData::add(int nAdd)
{
	population_ += nAdd;
}

// Finalise, calculating local fractional population (e.g. within an AtomTypeData)
void IsotopeData::finalise(int totalAtoms)
{
	fraction_ = double(population_) / double(totalAtoms);
}

// Zero populations
void IsotopeData::zeroPopulation()
{
	population_ = 0;
	fraction_ = 0.0;
}

// Return reference Isotope
Isotope* IsotopeData::isotope() const
{
	return isotope_;
}

// Return total population over all isotopes
int IsotopeData::population() const
{
	return population_;
}

// Return fractional population
double IsotopeData::fraction() const
{
	return fraction_;
}

/*
 * I/O
 */

// Write data through specified LineParser
bool IsotopeData::write(LineParser& parser)
{
	return parser.writeLineF("%i %i %i %s\n", isotope_->element()->z(), isotope_->A(), population_, fraction_);
}

// Read data through specified LineParser
bool IsotopeData::read(LineParser& parser)
{
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	isotope_ = PeriodicTable::element(parser.argi(0)).hasIsotope(parser.argi(1));
	population_ = parser.argi(2);
	fraction_ = parser.argd(3);
	return true;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool IsotopeData::broadcast(ProcessPool& procPool, int root)
{
#ifdef PARALLEL
	// For isotope_, need to broadcast element Z and isotope A
	int Z, A;
	if (procPool.poolRank() == root)
	{
		Z = isotope_->element()->z();
		A = isotope_->A();
	}
	procPool.broadcast(Z, root);
	procPool.broadcast(A, root);
	isotope_ = PeriodicTable::element(Z).hasIsotope(A);

	procPool.broadcast(population_, root);
	procPool.broadcast(fraction_, root);
#endif
	return true;
}
