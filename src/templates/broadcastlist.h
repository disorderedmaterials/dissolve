// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"

// BroadcastList
template <class T> class BroadcastList
{
    /*
     * Constructor-only template class which iterates over a supplied list, broadcasting the object from master
     * to slave processes. The List must contain items which provide the 'broadcast()' virtual.
     */
    private:
    // Result of broadcast
    bool result_;

    public:
    BroadcastList(ProcessPool &procPool, int root, List<T> &items, const CoreData &coreData)
    {
        result_ = false;
        int count;
        if (procPool.isMaster())
        {
            // Broadcast number of items in list, then list items...
            count = items.nItems();
            if (!procPool.broadcast(count, root))
                return;
            ListIterator<T> itemIterator(items);
            while (T *item = itemIterator.iterate())
                if (!item->broadcast(procPool, root, coreData))
                    return;
        }
        else
        {
            // Get number of list items to expect
            if (!procPool.broadcast(count, root))
                return;

            // Clear list and reconstruct
            items.clear();
            for (auto n = 0; n < count; ++n)
            {
                // Slaves must create a suitable structure first, and then join the broadcast
                T *item = items.add();
                if (!item->broadcast(procPool, root, coreData))
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
