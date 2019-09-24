/*
	*** Pair Class
	*** src/templates/pair.h
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

#ifndef DISSOLVE_PAIR_H
#define DISSOLVE_PAIR_H

#include "templates/list.h"

// Pair
template <class T1, class T2> class Pair : public ListItem< Pair<T1,T2> >
{
	public:
	// Constructors
	Pair()
	{
		a_ = T1();
		aSet_ = false;
		b_ = T2();
		bSet_ = false;
	}
	Pair(T1 a)
	{
		setA(a);
		b_ = T2();
		bSet_ = false;
	}
	Pair(T1 a, T2 b)
	{
		set(a,b);
	}
	// Destructor
	~Pair()
	{
	}
	// Copy constructor
	Pair(const Pair<T1,T2>& source)
	{
		(*this) = source;
	}
	// Assignment operator
	void operator=(const Pair<T1,T2>& source)
	{
		a_ = source.a_;
		aSet_ = source.aSet_;
		b_ = source.b_;
		bSet_ = source.bSet_;
	}


	/*
	 * Data
	 */
	private:
	// Object A
	T1 a_;
	// Whether object A has been set
	bool aSet_;
	// Object B
	T2 b_;
	// Whether object B has been set
	bool bSet_;

	public:
	// Set object A
	void setA(T1 newA)
	{
		a_ = newA;
		aSet_ = true;
	}
	// Return object A
	const T1& a() const
	{
		return a_;
	}
	// Return whether object A has been set
	bool isASet() const
	{
		return aSet_;
	}
	// Set object B
	void setB(T2 newB)
	{
		b_ = newB;
		bSet_ = true;
	}
	// Return object B
	const T2& b() const
	{
		return b_;
	}
	// Return whether object B has been set
	bool isBSet() const
	{
		return bSet_;
	}
	// Set both objects
	void set(T1 newA, T2 newB)
	{
		setA(newA);
		setB(newB);
	}
};

#endif
