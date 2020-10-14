// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"

// Forward Declarations
class CoreData;
class ProcessPool;

// Base function templates for a Class that is to be used as a GenericItem
class GenericItemBase
{
    /*
     * Item Class
     */
    public:
    // Return class name
    static std::string_view itemClassName();

    /*
     * I/O
     */
    public:
    // Read data through specified parser
    virtual bool read(LineParser &parser, CoreData &coreData);
    // Write data through specified parser
    virtual bool write(LineParser &parser);

    /*
     * Parallel Comms
     */
    public:
    // Broadcast item contents
    virtual bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Return equality between items
    virtual bool equality(ProcessPool &procPool);
};
