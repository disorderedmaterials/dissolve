/*
    *** Generic Item Container - Array2D<T>
    *** src/genericitems/array2d.h
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

// GenericItemContainer< Array2D<T> >
template <class T> class GenericItemContainer<Array2D<T>> : public GenericItem
{
    public:
    GenericItemContainer<Array2D<T>>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Data
     */
    private:
    // Data item
    Array2D<T> data_;

    public:
    // Return data item
    Array2D<T> &data() { return data_; }

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, itemClassName()))
            return new GenericItemContainer<Array2D<T>>(name, flags);
        return nullptr;
    }

    public:
    // Return class name contained in item
    std::string_view itemClassName()
    {
        static std::string className = fmt::format("Array2D<{}>", T::itemClassName());
        return className;
    }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser)
    {
        parser.writeLineF("{}  {}  {}\n", data_.nRows(), data_.nColumns(), DissolveSys::btoa(data_.halved()));
        for (int n = 0; n < data_.linearArraySize(); ++n)
            if (!data_.linearValue(n).write(parser))
                return false;
        return true;
    }
    // Read data through specified parser
    bool read(LineParser &parser, CoreData &coreData)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        int nRows = parser.argi(0), nColumns = parser.argi(1);
        data_.initialise(nRows, nColumns, parser.argb(2));

        for (int n = 0; n < data_.linearArraySize(); ++n)
            if (!data_.linearArray()[n].read(parser, coreData))
                return false;
        return true;
    }

    /*
     * Parallel Comms
     */
    public:
    // Broadcast item contents
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
    {
#ifdef PARALLEL
        int nRows, nColumns;
        bool half;
        if (procPool.poolRank() == root)
        {
            // Broadcast array size first...
            nRows = data_.nRows();
            if (!procPool.broadcast(nRows, root))
                return false;
            nColumns = data_.nColumns();
            if (!procPool.broadcast(nColumns, root))
                return false;
            half = data_.halved();
            if (!procPool.broadcast(half, root))
                return false;

            // Now broadcast Array data
            if ((nRows * nColumns) > 0)
            {
                for (int n = 0; n < data_.linearArraySize(); ++n)
                    if (!data_.linearArray()[n].broadcast(procPool, root, coreData))
                        return false;
            }
        }
        else
        {
            // Slaves receive the size, and then create and receive the array
            if (!procPool.broadcast(nRows, root))
                return false;
            if (!procPool.broadcast(nColumns, root))
                return false;
            if (!procPool.broadcast(half, root))
                return false;

            // Resize and receive array
            data_.initialise(nRows, nColumns, half);
            if ((nRows * nColumns) > 0)
            {
                for (int n = 0; n < data_.linearArraySize(); ++n)
                    if (!data_.linearArray()[n].broadcast(procPool, root, coreData))
                        return false;
            }
            else
                data_.clear();
        }
#endif
        return true;
    }
    // Return equality between items
    bool equality(ProcessPool &procPool)
    {
        // Verify array size and state first
        if (!procPool.equality(data_.nRows()))
            return Messenger::error("Array2D<double> nRows are not equal.\n");
        if (!procPool.equality(data_.nColumns()))
            return Messenger::error("Array2D<double> nColumns are not equal.\n");
        if (!procPool.equality(data_.halved()))
            return Messenger::error("Array2D<double> half-status are not equivalent.\n");
        // Keep it simple (and slow) and check/send one value at a time
        for (int n = 0; n < data_.linearArraySize(); ++n)
            if (!data_.linearArray()[n].equality(procPool))
                return false;
        return true;
    }
};
