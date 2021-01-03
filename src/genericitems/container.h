// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"
#include "genericitems/item.h"

// GenericItemContainer Template Class
template <class T> class GenericItemContainer : public GenericItem
{
    public:
    GenericItemContainer<T>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Data
     */
    private:
    // Data item
    T data_;

    public:
    // Return data item
    T &data() { return data_; }

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, itemClassName()))
            return new GenericItemContainer<T>(name, flags);
        return nullptr;
    }

    public:
    // Return class name contained in item
    virtual std::string_view itemClassName() { return T::itemClassName(); }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser) { return data_.write(parser); }
    // Read data through specified parser
    bool read(LineParser &parser, CoreData &coreData) { return data_.read(parser, coreData); }

    /*
     * Parallel Comms
     */
    public:
    // Broadcast item contents
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
    {
        return data_.broadcast(procPool, root, coreData);
    }
    // Check for equality
    bool equality(ProcessPool &procPool) { return data_.equality(procPool); }
};
