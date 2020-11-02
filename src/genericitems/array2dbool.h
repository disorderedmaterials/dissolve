// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "genericitems/container.h"

// GenericItemContainer< Array2D<bool> >
template <> class GenericItemContainer<Array2D<bool>> : public GenericItem
{
    public:
    GenericItemContainer<Array2D<bool>>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Data
     */
    private:
    // Data item
    Array2D<bool> data_;

    public:
    // Return data item
    Array2D<bool> &data() { return data_; }

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, itemClassName()))
            return new GenericItemContainer<Array2D<bool>>(name, flags);
        return nullptr;
    }

    public:
    // Return class name contained in item
    std::string_view itemClassName() { return "Array2D<bool>"; }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser) { return write(data_, parser); }
    // Read data through specified parser
    bool read(LineParser &parser, CoreData &coreData) { return read(data_, parser); }
    // Write specified data through specified parser
    static bool write(const Array2D<bool> &thisData, LineParser &parser)
    {
        parser.writeLineF("{}  {}  {}\n", thisData.nRows(), thisData.nColumns(), DissolveSys::btoa(thisData.halved()));
        for (auto n = 0; n < thisData.linearArraySize(); ++n)
            if (!parser.writeLineF("{}\n", thisData.constLinearValue(n) ? 'T' : 'F'))
                return false;
        return true;
    }
    // Read specified data through specified parser
    static bool read(Array2D<bool> &thisData, LineParser &parser)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        int nRows = parser.argi(0), nColumns = parser.argi(1);
        thisData.initialise(nRows, nColumns, parser.argb(2));

        for (auto n = 0; n < thisData.linearArraySize(); ++n)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            thisData.linearArray()[n] = (parser.argsv(0)[0] == 'T' || parser.argsv(0)[0] == 't');
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
