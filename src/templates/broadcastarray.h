// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"

// BroadcastArray
template <class T> class BroadcastArray
{
    /*
     * Constructor-only template class which iterates over a supplied Array, broadcasting the object from master
     * to slave processes. The Array items must be trivially copyable (via an assignment operator) and broadcastable.
     */
    public:
    BroadcastArray(ProcessPool &procPool, int root, Array<T> &array, const CoreData &coreData, bool &result)
    {
        result = false;
        int count;
        if (procPool.isMaster())
        {
            // Broadcast number of items in list, then list items...
            count = array.nItems();
            if (!procPool.broadcast(count, root))
                return;
            for (auto n = 0; n < count; ++n)
                array[n].broadcast(procPool, root, coreData);
        }
        else
        {
            // Get number of list items to expect
            if (!procPool.broadcast(count, root))
                return;

            // Clear list and reconstruct
            array.initialise(count);
            for (auto n = 0; n < count; ++n)
                array[n].broadcast(procPool, root, coreData);
        }

        // All OK - success!
        result = true;
    };
};
