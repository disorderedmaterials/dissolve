/*
    *** Generic Item Container - Array2D< Array<double> >
    *** src/genericitems/array2darraydouble.h
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

// GenericItemContainer< Array2D< Array<double> > >
template <> class GenericItemContainer<Array2D<Array<double>>> : public GenericItem
{
    public:
    GenericItemContainer<Array2D<Array<double>>>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Data
     */
    private:
    // Data item
    Array2D<Array<double>> data_;

    public:
    // Return data item
    Array2D<Array<double>> &data() { return data_; }

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, itemClassName()))
            return new GenericItemContainer<Array2D<Array<double>>>(name, flags);
        return nullptr;
    }

    public:
    // Return class name contained in item
    std::string_view itemClassName() { return "Array2D<Array<double>>"; }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser) { return write(data_, parser); }
    // Read data through specified parser
    bool read(LineParser &parser, CoreData &coreData) { return read(data_, parser); }
    // Write specified data through specified parser
    static bool write(const Array2D<Array<double>> &thisData, LineParser &parser)
    {
        parser.writeLineF("{}  {}  {}\n", thisData.nRows(), thisData.nColumns(), DissolveSys::btoa(thisData.halved()));
        for (int n = 0; n < thisData.linearArraySize(); ++n)
        {
            const Array<double> &arrayData = thisData.constLinearValue(n);

            parser.writeLineF("{}\n", arrayData.nItems());
            for (int m = 0; m < arrayData.nItems(); ++m)
            {
                if (!parser.writeLineF("{:16.9e}\n", arrayData.constAt(m)))
                    return false;
            }
        }
        return true;
    }
    // Read specified data through specified parser
    static bool read(Array2D<Array<double>> &thisData, LineParser &parser)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        int nRows = parser.argi(0), nColumns = parser.argi(1);
        thisData.initialise(nRows, nColumns, parser.argb(2));

        for (int n = 0; n < thisData.linearArraySize(); ++n)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            int nItems = parser.argi(0);
            thisData.linearArray()[n].createEmpty(nItems);
            for (int m = 0; m < nItems; ++m)
            {
                if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                    return false;
                thisData.linearArray()[n].add(parser.argd(0));
            }
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
#ifdef PARALLEL
        int nRows, nColumns;
        bool half;
        if (root == procPool.poolRank())
        {
            // Broadcast array size first...
            nRows = data_.nRows();
            if (!procPool.broadcast(nRows, root))
            {
                Messenger::print("Failed to broadcast Array2D< Array<double> > nRows from root rank {}.\n", root);
                return false;
            }
            nColumns = data_.nColumns();
            if (!procPool.broadcast(nColumns, root))
            {
                Messenger::print("Failed to broadcast Array2D< Array<double> > nColmnns from root rank {}.\n", root);
                return false;
            }
            half = data_.halved();
            if (!procPool.broadcast(half, root))
            {
                Messenger::print("Failed to broadcast Array2D< Array<double> > half-diagonal status from root "
                                 "rank {}.\n",
                                 root);
                return false;
            }

            // Now broadcast Array elements
            for (int n = 0; n < data_.linearArraySize(); ++n)
            {
                if (!procPool.broadcast(data_.linearArray()[n], root))
                    return false;
            }
        }
        else
        {
            // Slaves receive the size, and then create and receive the array
            if (!procPool.broadcast(nRows, root))
            {
                Messenger::print("Slave {} (world rank {}) failed to receive Array2D< Array<double> > nRows "
                                 "from root rank {}.\n",
                                 procPool.poolRank(), procPool.worldRank(), root);
                return false;
            }
            if (!procPool.broadcast(nColumns, root))
            {
                Messenger::print("Slave {} (world rank {}) failed to receive Array2D< Array<double> > nRows "
                                 "from root rank {}.\n",
                                 procPool.poolRank(), procPool.worldRank(), root);
                return false;
            }
            if (!procPool.broadcast(half, root))
            {
                Messenger::print("Slave {} (world rank {}) failed to receive Array2D< Array<double> > halved "
                                 "status from root rank {}.\n",
                                 procPool.poolRank(), procPool.worldRank(), root);
                return false;
            }

            // Resize and receive array
            data_.initialise(nRows, nColumns, half);
            for (int n = 0; n < data_.linearArraySize(); ++n)
            {
                if (!procPool.broadcast(data_.linearArray()[n], root))
                    return false;
            }
        }
#endif
        return true;
    }
    // Return equality between items
    bool equality(ProcessPool &procPool)
    {
#ifdef PARALLEL
        // Verify array size and state first
        if (!procPool.equality(data_.nRows()))
            return Messenger::error("Array2D<double> nRows are not equal (process {} has {}).\n", procPool.poolRank(),
                                    data_.nRows());
        if (!procPool.equality(data_.nColumns()))
            return Messenger::error("Array2D<double> nColumns are not equal (process {} has {}).\n", procPool.poolRank(),
                                    data_.nColumns());
        if (!procPool.equality(data_.halved()))
            return Messenger::error("Array2D<double> half-status are not equivalent (process {} has {}).\n",
                                    procPool.poolRank(), data_.halved());

        // Keep it simple (and slow) and check/send one object at a time
        for (int n = 0; n < data_.linearArraySize(); ++n)
            if (!procPool.equality(data_.linearArray()[n]))
                return Messenger::error("Array<double> index {} is not equivalent (process {}.\n", procPool.poolRank());
#endif
        return true;
    }
};
