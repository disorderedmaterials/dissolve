// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "genericitems/container.h"
#include "templates/broadcastarray.h"

// GenericItemContainer< Array<T> >
template <class T> class GenericItemContainer<Array<T>> : public GenericItem
{
    public:
    GenericItemContainer<Array<T>>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Data
     */
    private:
    // Data item
    Array<T> data_;

    public:
    // Return data item
    Array<T> &data() { return data_; }
    const Array<T> &data() const { return data_; }

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, itemClassName()))
            return new GenericItemContainer<Array<T>>(name, flags);
        return nullptr;
    }

    public:
    // Return class name contained in item
    std::string_view itemClassName()
    {
        static std::string className = fmt::format("Array<{}>", T::itemClassName());
        return className;
    }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser)
    {
        parser.writeLineF("{}\n", data_.nItems());
        T *array = data_.array();
        for (auto n = 0; n < data_.nItems(); ++n)
        {
            if (!array[n].write(parser))
                return false;
        }
        return true;
    }
    // Read data through specified parser
    bool read(LineParser &parser, const CoreData &coreData)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        int nItems = parser.argi(0);
        data_.initialise(nItems);

        for (auto n = 0; n < nItems; ++n)
            if (!data_[n].read(parser, coreData))
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
        bool success;
        BroadcastArray<T>(procPool, root, data_, coreData, success);
        return success;
    }
    // Return equality between items
    bool equality(ProcessPool &procPool) { return false; }
};
