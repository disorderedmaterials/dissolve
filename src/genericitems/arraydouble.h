// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "genericitems/container.h"

// GenericItemContainer< Array<double> >
template <> class GenericItemContainer<Array<double>> : public GenericItem
{
    public:
    GenericItemContainer<Array<double>>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Data
     */
    private:
    // Data item
    Array<double> data_;

    public:
    // Return data item
    Array<double> &data() { return data_; }

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, itemClassName()))
            return new GenericItemContainer<Array<double>>(name, flags);
        return nullptr;
    }

    public:
    // Return class name contained in item
    std::string_view itemClassName() { return "Array<double>"; }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser) { return write(data_, parser); }
    // Read data through specified parser
    bool read(LineParser &parser, CoreData &coreData) { return read(data_, parser); }
    // Write specified data through specified parser
    static bool write(const Array<double> &thisData, LineParser &parser)
    {
        parser.writeLineF("{}\n", thisData.nItems());
        for (int n = 0; n < thisData.nItems(); ++n)
        {
            if (!parser.writeLineF("{:16.9e}\n", thisData.constAt(n)))
                return false;
        }
        return true;
    }
    // Read specified data through specified parser
    static bool read(Array<double> &thisData, LineParser &parser)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        int nItems = parser.argi(0);
        thisData.createEmpty(nItems);
        for (int n = 0; n < nItems; ++n)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            thisData.add(parser.argd(0));
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
