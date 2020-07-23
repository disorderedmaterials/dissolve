/*
    *** Lock and Locker
    *** src/base/lock.h
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

#pragma once

// Forward Declarations
/* none */

class Lock
{
    public:
    Lock();
    ~Lock();
    // Declare Locker to be our friend
    friend class Locker;

    private:
    // Lock counter
    int lockCounter_;

    private:
    // Increase lock count
    void addLockLevel();
    // Decrease lock count
    void removeLockLevel();

    public:
    // Return whether we are currently locked
    bool isLocked() const;
};

// Locker
class Locker
{
    public:
    Locker(Lock &lock);
    ~Locker();

    private:
    // Target Lock
    Lock &lock_;
    // Whether our lock has already been released
    bool unlocked_;

    public:
    // Manually release the lock
    void unlock();
};
