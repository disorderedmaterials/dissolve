// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lock.h"
#include "base/messenger.h"

/*
 * Lock
 */

Lock::Lock() { lockCounter_ = 0; }

Lock::operator bool() { return isLocked(); }

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
