/*
	*** Enum Helper Templates
	*** src/templates/enumhelpers.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_ENUMHELPERS_H
#define DUQ_ENUMHELPERS_H

// Enum Broadcast Vessel
template <class E> class EnumCast
{
	/*
	 * Template-only class that takes reference to an enum and allows the integer conversion to be passed by reference to the
	 * broadcast routines. Before destruction, the integerValue_ that was subject to broadcast is cast back into the original enum ref.
	 */
	public:
	// Constructor
	EnumCast(E& originalEnum) : originalEnum_(originalEnum)
	{
		integerValue_ = originalEnum_;
	}
	// Destructor
	~EnumCast()
	{
		// Cast integer variable back into enum
		originalEnum_ = (E) integerValue_;
	}
	// Conversion Operator (to int&)
	operator int&()
	{
		return integerValue_;
	}

	private:
	// Original enum object
	E& originalEnum_;
	// Integer conversion of enum 
	int integerValue_;
};

// Enum Container
template <class E> class EnumContainer
{
	/*
	 * Template-only class that stores an enum for inclusion in a list or similar.
	 */
	public:
	// Constructor
	EnumContainer(E value = 0) : value_(value)
	{
	}
	// Conversion operator
	operator E()
	{
		return value_;
	}
	// Assignment Operator
	void operator=(const E value)
	{
		value_ = value;
	}

	private:
	// Original enum
	E value_;

	public:
	// Return value
	E& value()
	{
		return value_;
	}
};

#endif
