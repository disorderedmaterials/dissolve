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
	// Data
	T1 a;
	T2 b;
};

#endif
