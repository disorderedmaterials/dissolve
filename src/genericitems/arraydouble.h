/*
    *** Generic Item Container - Array<double>
    *** src/genericitems/arraydouble.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
