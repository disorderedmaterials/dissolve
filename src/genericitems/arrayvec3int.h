/*
    *** Generic Item Container - Array< Vec3<int> >
    *** src/genericitems/arrayvec3int.h
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

// GenericItemContainer< Array< Vec3<int> > >
template <> class GenericItemContainer<Array<Vec3<int>>> : public GenericItem
{
    public:
    GenericItemContainer<Array<Vec3<int>>>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Data
     */
    private:
    // Data item
    Array<Vec3<int>> data_;

    public:
    // Return data item
    Array<Vec3<int>> &data() { return data_; }

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, itemClassName()))
            return new GenericItemContainer<Array<Vec3<int>>>(name, flags);
        return nullptr;
    }

    public:
    // Return class name contained in item
    std::string_view itemClassName() { return "Array<Vec3<int>>"; }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser)
    {
        parser.writeLineF("{}\n", data_.nItems());
        Vec3<int> *array = data_.array();
        for (int n = 0; n < data_.nItems(); ++n)
        {
            if (!parser.writeLineF("{} {} {}\n", array[n].x, array[n].y, array[n].z))
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
        for (int n = 0; n < nItems; ++n)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            data_.add(parser.arg3i(0));
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
    bool equality(ProcessPool &procPool)
    {
        // Verify array size first
        if (!procPool.equality(data_.nItems()))
            return false;
        // Keep it simple (and slow) and check/send one value at a time
        for (int n = 0; n < data_.nItems(); ++n)
            if (!procPool.equality(data_.constAt(n)))
                return false;
        return true;
    }
};
