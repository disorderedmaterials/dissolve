/*
	*** Pair Class
	*** src/lib/templates/pair.h
	Copyright T. Youngs 2012-2014

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

#ifndef DUQ_PAIR_H
#define DUQ_PAIR_H

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
