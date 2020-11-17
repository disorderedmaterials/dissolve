// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "genericitems/container.h"
#include "templates/broadcastarray.h"
#include <vector>

// GenericItemContainer< std::vector<double> >
template <> class GenericItemContainer<std::vector<double>> : public GenericItem
{
    public:
    GenericItemContainer<std::vector<double>>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Data
     */
    private:
    // Data item
    std::vector<double> data_;

    public:
    // Return data item
    std::vector<double> &data() { return data_; }

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, itemClassName()))
            return new GenericItemContainer<std::vector<double>>(name, flags);
        return nullptr;
    }

    public:
    // Return class name contained in item
    std::string_view itemClassName() { return "std::vector<double>"; }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser)
    {
        parser.writeLineF("{}\n", data_.size());
        for (auto n : data_)
        {
            if (!parser.writeLineF("{:16.9e}\n", n))
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
        data_.clear();
        data_.resize(nItems);

        for (auto n : data_)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            n = parser.argd(0);
        }
        return true;
    }

    /*
     * Parallel Comms
     */
    public:
    // Broadcast item contents
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
    {
        bool result = false;
        int count;
        if (procPool.isMaster())
        {
            // Broadcast number of items in list, then list items...
            count = data_.size();
            if (!procPool.broadcast(count, root))
                return false;
            for (auto n : data_)
                procPool.broadcast(n, root);
        }
        else
        {
            // Get number of list items to expect
            if (!procPool.broadcast(count, root))
                return false;

            // Clear list and reconstruct
            data_.clear();
            data_.resize(count);
            for (auto n : data_)
                procPool.broadcast(n, root);
        }

        // All OK - success!
        result = true;
    }
    // Return equality between items
    bool equality(ProcessPool &procPool) { return false; }
};
