/*
	*** Simple Flag
	*** src/lib/base/flag.h
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

#ifndef DUQ_FLAG_H
#define DUQ_FLAG_H

/*!
 * \brief Flag
 * \details Simple class to allow 
 */
class Flag
{
	public:
	// Constructor
	Flag();


	/*!
	 * \name Flag and Access
	 */
	///@{
	private:
	// Flag
	bool flag_;
	
	public:
	// Set flag to TRUE
	void setTrue();
	// Set flag to FALSE
	void setFalse();
	// Return flag
	bool flag();
	///@}
};

// External Declarations
extern Flag dUQFlag;

#define SET_MODIFIED dUQFlag.setTrue();
#define CLEAR_MODIFIED dUQFlag.setFalse();
#define IS_MODIFIED dUQFlag.flag()

#endif
