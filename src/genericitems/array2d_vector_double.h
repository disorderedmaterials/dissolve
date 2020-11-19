// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "genericitems/container.h"
#include <vector>

// GenericItemContainer< Array2D< Array<double> > >
template <> class GenericItemContainer<Array2D<std::vector<double>>> : public GenericItem
{
    public:
    GenericItemContainer<Array2D<std::vector<double>>>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Data
     */
    private:
    // Data item
    Array2D<std::vector<double>> data_;

    public:
    // Return data item
    Array2D<std::vector<double>> &data() { return data_; }

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, itemClassName()))
            return new GenericItemContainer<Array2D<std::vector<double>>>(name, flags);
        return nullptr;
    }

    public:
    // Return class name contained in item
    std::string_view itemClassName() { return "Array2D<std::vector<double>>"; }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser)
    {
        parser.writeLineF("{}  {}  {}\n", data_.nRows(), data_.nColumns(), DissolveSys::btoa(data_.halved()));
        for (auto data: data_)
        {
            parser.writeLineF("{}\n", data.size());
            for (auto n : data)
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
        int nRows = parser.argi(0), nColumns = parser.argi(1);
        data_.initialise(nRows, nColumns, parser.argb(2));

        for (auto n = 0; n < data_.linearArraySize(); ++n)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            int nItems = parser.argi(0);
            auto &data = data_.linearArray()[n];
            data.clear();
            data.resize(nItems);
            for (auto n : data)
            {
                if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                    return false;
                n = parser.argd(0);
            }
        }
        return true;
    }
    // Write specified data through specified parser
    static bool write(const Array2D<std::vector<double>> &thisData, LineParser &parser)
    {
        parser.writeLineF("{}  {}  {}\n", thisData.nRows(), thisData.nColumns(), DissolveSys::btoa(thisData.halved()));
        for (const auto &arrayData : thisData)
        {
            parser.writeLineF("{}\n", arrayData.size());
            for (auto m : arrayData)
            {
                if (!parser.writeLineF("{:16.9e}\n", m))
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

        for (auto n = 0; n < thisData.linearArraySize(); ++n)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            int nItems = parser.argi(0);
            thisData.linearArray()[n].createEmpty(nItems);
            for (auto m = 0; m < nItems; ++m)
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
            for (auto n = 0; n < data_.linearArraySize(); ++n)
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
            for (auto n = 0; n < data_.linearArraySize(); ++n)
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
        for (auto n = 0; n < data_.linearArraySize(); ++n)
            if (!procPool.equality(data_.linearArray()[n]))
                return Messenger::error("Array<double> index {} is not equivalent (process {}.\n", procPool.poolRank());
#endif
        return true;
    }
};
