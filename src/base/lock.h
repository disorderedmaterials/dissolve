// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

class Lock
{
    public:
    Lock();
    ~Lock() = default;
    operator bool();
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
