/*
    *** Lock and Locker
    *** src/base/lock.cpp
    Copyright T. Youngs 2019-2020

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

#include "base/lock.h"
#include "base/messenger.h"

/*
 * Lock
 */

Lock::Lock() { lockCounter_ = 0; }

Lock::~Lock() {}

// Increase lock count
void Lock::addLockLevel() { ++lockCounter_; }

// Decrease lock count
void Lock::removeLockLevel()
{
    --lockCounter_;
    if (lockCounter_ < 0)
        Messenger::error("Warning - Lock count has been forced into negative numbers!\n");
}

// Return whether we are currently locked
bool Lock::isLocked() const { return (lockCounter_ > 0); }

/*
 * Locker
 */

Locker::Locker(Lock &lock) : lock_(lock)
{
    lock.addLockLevel();

    unlocked_ = false;
}

Locker::~Locker()
{
    if (!unlocked_)
        lock_.removeLockLevel();
}

// Manually release the lock
void Locker::unlock()
{
    if (!unlocked_)
        lock_.removeLockLevel();

    unlocked_ = true;
}
