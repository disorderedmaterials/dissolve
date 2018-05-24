/*
	*** Linked List Item Class
	*** src/templates/listiitem.h
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

#ifndef DISSOLVE_LISTITEM_H
#define DISSOLVE_LISTITEM_H

#include <stdlib.h>     // To get NULL

// Forward Declarations
/* none */

/*
 * ListItem Class
 * Basic class providing linked list pointers. Any class which is required to be contained in a linked list must
 * subclass ListItem or MPIListItem
 */
template <class T> class ListItem
{
	public:
	// Constructor
	ListItem<T>()
	{
		prev = NULL;
		next = NULL;
	}
	// List pointers
	T *prev, *next;
};

#endif
