// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"

// Broadcast Vector
template <class T> class BroadcastVector
{
    /*
     * Constructor-only template class which iterates over a supplied sstd::vector, broadcasting the object from master
     * to slave processes. The items must provide the 'broadcast()' virtual.
     */
    private:
    // Result of broadcast
    bool result_;

    public:
    BroadcastVector(ProcessPool &procPool, int root, std::vector<T> &items, const CoreData &coreData)
    {
        result_ = false;
        int count;
        if (procPool.isMaster())
        {
            // Broadcast number of items in list, then list items...
            count = items.size();
            if (!procPool.broadcast(count, root))
                return;
            for (auto item : items)
                if (!item.broadcast(procPool, root, coreData))
                    return;
        }
        else
        {
            // Get number of list items to expect
            if (!procPool.broadcast(count, root))
                return;

            // Clear list and reconstruct
            items.clear();
            for (int n = 0; n < count; ++n)
            {
                // Slaves must create a suitable structure first, and then join the broadcast
                items.emplace_back();
                if (!items.back().broadcast(procPool, root, coreData))
                    return;
            }
        }

        // All OK - success!
        result_ = true;
    }
    // Return whether broadcast was successful
    bool success() { return result_; }
    // Return whether broadcast failed
    bool failed() { return (!result_); }
};
