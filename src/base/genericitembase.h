/*
	*** Generic Item Base
	*** src/templates/genericitembase.h
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

#ifndef DUQ_GENERICITEMBASE_H
#define DUQ_GENERICITEMBASE_H

#include "base/messenger.h"

// Forward Declarations
/* none */

// Base function templates for a Class that is to be used as a GenericItem
class GenericItemBase
{
	/*
	 * Item Class
	 */
	public:
	// Return class name
	static const char* itemClassName();


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	virtual bool write(LineParser& parser);
	// Read data through specified parser
	virtual bool read(LineParser& parser);
};

#endif
