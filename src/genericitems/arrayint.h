// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "genericitems/container.h"

// GenericItemContainer< Array<int> >
template <> class GenericItemContainer<Array<int>> : public GenericItem
{
    public:
    GenericItemContainer<Array<int>>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Data
     */
    private:
    // Data item
    Array<int> data_;

    public:
    // Return data item
    Array<int> &data() { return data_; }

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, itemClassName()))
            return new GenericItemContainer<Array<int>>(name, flags);
        return nullptr;
    }

    public:
    // Return class name contained in item
    std::string_view itemClassName() { return "Array<int>"; }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser)
    {
        parser.writeLineF("{}\n", data_.nItems());
        for (auto n = 0; n < data_.nItems(); ++n)
        {
            if (!parser.writeLineF("{}\n", data_.at(n)))
                return false;
        }
        return true;
    }
    // Read data through specified parser
    bool read(LineParser &parser, CoreData &coreData)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        int nItems = parser.argi(0);
        data_.createEmpty(nItems);
        for (auto n = 0; n < nItems; ++n)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            data_.add(parser.argi(0));
        }
        return true;
    }

    /*
     * Parallel Comms
     */
    public:
    // Broadcast item contents
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData) { return procPool.broadcast(data_, root); }
    // Return equality between items
    bool equality(ProcessPool &procPool) { return procPool.equality(data_); }
};
