// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "genericitems/list.h"

// Register all broadcasters
void GenericList::registerBroadcasters()
{
    // POD - int
    registerBroadcaster<int>([](std::any &a, ProcessPool &procPool, const int root, const CoreData &coreData) {
        return procPool.broadcast(std::any_cast<int &>(a), root);
    });
}
