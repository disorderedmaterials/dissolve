// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "genericitems/container.h"

// GenericItemContainer< Vec3<int> >
template <> class GenericItemContainer<Vec3<int>> : public GenericItem
{
    public:
    GenericItemContainer<Vec3<int>>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Data
     */
    private:
    // Data item
    Vec3<int> data_;

    public:
    // Return data item
    Vec3<int> &data() { return data_; }
    const Vec3<int> &data() const { return data_; }

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, itemClassName()))
            return new GenericItemContainer<Vec3<int>>(name, flags);
        return nullptr;
    }

    public:
    // Return class name contained in item
    std::string_view itemClassName() { return "Vec3<int>"; }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser) { return parser.writeLineF("{}  {}  {}\n", data_.x, data_.y, data_.z); }
    // Read data through specified parser
    bool read(LineParser &parser, CoreData &coreData)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        data_ = parser.arg3i(0);
        return true;
    }

    /*
     * Parallel Comms
     */
    public:
    // Broadcast item contents
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData) { return procPool.broadcast(data_, root); }
    // Check item equality
    bool equality(ProcessPool &procPool)
    {
        if (!procPool.equality(data_.x))
            return false;
        if (!procPool.equality(data_.y))
            return false;
        if (!procPool.equality(data_.z))
            return false;
        return true;
    }
};
