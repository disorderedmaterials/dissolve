/*
	*** Mutex Routines
	*** src/base/mutex.cpp
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

#include "base/mutex.h"
#include <stdio.h>

#ifdef WITHGUI

#include <QtCore/QMutex>

// Constructor (GUI version)
dUQMutex::dUQMutex()
{
	mutex_ = new QMutex();
}

// Destructor (GUI version)
dUQMutex::~dUQMutex()
{
	delete mutex_;
}

// Lock mutex
void dUQMutex::lock()
{
	mutex_->lock();
}

// Unlock mutex
void dUQMutex::unlock()
{
	mutex_->unlock();
}

#else

// Constructor (non-GUI version)
dUQMutex::dUQMutex()
{
}

// Destructor (GUI version)
dUQMutex::~dUQMutex()
{
}

// Lock mutex
void dUQMutex::lock()
{
}

// Unlock mutex
void dUQMutex::unlock()
{
}

#endif
