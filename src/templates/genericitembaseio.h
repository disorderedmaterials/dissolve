/*
	*** Generic Item Base I/O
	*** src/templates/genericitembaseio.h
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

#ifndef DUQ_GENERICITEMBASEIO_H
#define DUQ_GENERICITEMBASEIO_H

#include "base/messenger.h"

// Forward Declarations
/* none */

// Base IO functions for a GenericItem
// Subclass this if read/write is not needed by the class.
class GenericItemBaseIO
{
	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		Messenger::error("Tried to write() a class that doesn't support it.\n");
		return false;
	}
	// Read data through specified parser
	bool read(LineParser& parser)
	{
		Messenger::error("Tried to read() a class that doesn't support it.\n");
		return false;
	}
};

#endif
