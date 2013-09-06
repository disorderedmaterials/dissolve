/*
	*** CheckPoint Data
	*** src/lib/templates/checkpoint.h
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

#ifndef DUQ_CHECKPOINT_H
#define DUQ_CHECKPOINT_H

#include "base/constants.h"
#include "base/mutex.h"
#include "templates/list.h"
#include <QtCore/QMutexLocker>

/*!
 * \brief CheckPointData2D
 * \details Template class to store pointer to a source object D, an updateable copy of D (for export),
 * and a Mutex to allow threaded access to the copy.
 */
template <class D> class CheckPoint : public ListItem< CheckPoint<D> >
{
	public:
	// Constructor
	CheckPoint<D>() : ListItem< CheckPoint<D> >()
	{
		source_ = NULL;
		nUpdates_ = 0;
		staticData_ = false;
	};


	/*!
	 * \name Data Source, Copy, and Mutex
	 */
	///@{
	private:
	// Pointer to source data
	D* source_;
	// Local copy of source data
	D copy_;
	// Mutex for threaded access
	dUQMutex mutex_;
	// Number of times the data has been updated
	int nUpdates_;
	// Whether the data should be continuously updated (true) or is static (false)
	bool staticData_;
	// Group name (if any) for this data
	Dnchar groupName_;
	
	public:
	// Set source data
	void setSource(D* source, const char* groupName = NULL, bool staticData = false)
	{
		source_ = source;
		staticData_ = staticData;
		groupName_ = groupName;
	}

	// Set source data (from reference)
	void setSource(D& source, const char* groupName = NULL, bool staticData = false)
	{
		setSource(&source, groupName, staticData);
	}

	// Update local copy
	void update()
	{
		// Only update staticData_ if this is the first time
		if (staticData_ && (nUpdates_ > 0)) return;

		// Lock the mutex, copy data, and release mutex
		mutex_.lock();
		copy_ = (*source_);
		mutex_.unlock();
		++nUpdates_;
	}

	// Check out copy
	D& checkOut()
	{
		mutex_.lock();
		return copy_;
	}

	// Check copy back in
	void checkIn()
	{
		mutex_.unlock();
	}

	// Return group name (if any) for this data
	const char* groupName()
	{
		return groupName_.get();
	}
	///@}
};

#endif
