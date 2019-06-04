/*
	*** Generic Item Container - Array2D<double>
	*** src/genericitems/array2ddouble.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_GENERICITEMCONTAINER_ARRAY2DDOUBLE_H
#define DISSOLVE_GENERICITEMCONTAINER_ARRAY2DDOUBLE_H

#include "genericitems/container.h"

// GenericItemContainer< Array2D<double> >
template <> class GenericItemContainer< Array2D<double> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array2D<double> >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
	}


	/*
	 * Data
	 */
	private:
	// Data item
	Array2D<double> data_;

	public:
	// Return data item
	Array2D<double>& data()
	{
		return data_;
	}


	/*
	 * Item Class
	 */
	protected:
	// Create a new GenericItem containing same class as current type
	GenericItem* createItem(const char* className, const char* name, int flags = 0)
	{
		if (DissolveSys::sameString(className, itemClassName())) return new GenericItemContainer< Array2D<double> >(name, flags);
		return NULL;
	}

	public:
	// Return class name contained in item
	const char* itemClassName()
	{
		return "Array2D<double>";
	}


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		return write(data_, parser);
	}
	// Read data through specified parser
	bool read(LineParser& parser, const CoreData& coreData)
	{
		return read(data_, parser);
	}
	// Write specified data through specified parser
	static bool write(const Array2D<double>& thisData, LineParser& parser)
	{
		parser.writeLineF("%i  %i  %s\n", thisData.nRows(), thisData.nColumns(), DissolveSys::btoa(thisData.halved()));
		for (int n=0; n<thisData.linearArraySize(); ++n) if (!parser.writeLineF("%16.9e\n", thisData.constLinearValue(n))) return false;
		return true;
	}
	// Read specified data through specified parser
	static bool read(Array2D<double>& thisData, LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		int nRows = parser.argi(0), nColumns = parser.argi(1);
		thisData.initialise(nRows, nColumns, parser.argb(2));

		for (int n=0; n<thisData.linearArraySize(); ++n)
		{
			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
			thisData.linearArray()[n] = parser.argd(0);
		}
		return true;
	}


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contents
	bool broadcast(ProcessPool& procPool, const int root, const CoreData& coreData)
	{
		return procPool.broadcast(data_, root);
	}
	// Return equality between items
	bool equality(ProcessPool& procPool)
	{
		return procPool.equality(data_);
	}
};

#endif
