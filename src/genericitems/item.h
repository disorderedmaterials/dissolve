/*
    *** Generic Item
    *** src/genericitems/item.h
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

#include "base/processpool.h"
#include "base/sysfunc.h"

// Forward Declarations
class CoreData;

/*
 * GenericItem - Base class for all GenericItemContainers
 */
class GenericItem : public ListItem<GenericItem>
{
    public:
    GenericItem(std::string_view name, int flags);
    virtual ~GenericItem();
    // Item Flags
    enum ItemFlag
    {
        NoFlag = 0,                   /* No flag */
        InRestartFileFlag = 1,        /* The item should be written to the restart file */
        IsReferencePointDataFlag = 2, /* The item was loaded as reference point data */
        ProtectedFlag = 4             /* The item will not be clear()'d unless forced */
    };

    /*
     * Item Class
     */
    protected:
    // List of all available class names (as GenericItems)
    static List<GenericItem> itemClasses_;

    protected:
    // Create a new GenericItem containing same class as current type
    virtual GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0) = 0;

    public:
    // Return class name contained in item
    virtual std::string_view itemClassName() = 0;
    // Add class to list of representative itemClasses_
    static void addItemClass(GenericItem *item);
    // Return new, empty GenericItem containing the class specified
    static GenericItem *newItem(std::string_view className, std::string_view name, int flags = 0);

    /*
     * Item Contents
     */
    private:
    // Name associated to the item
    std::string name_;
    // Version of the item
    int version_;
    // Flags
    int flags_;

    public:
    // Set item name
    void setName(std::string_view name);
    // Return item name
    std::string_view name() const;
    // Return version of the item
    int version() const;
    // Set version of the item
    void setVersion(int version);
    // Bump the version of the item
    void bumpVersion();
    // Set flags for item, overwriting any existing flags
    void setFlags(int flags);
    // Add (set) flag for item
    void addFlag(ItemFlag flag);
    // Remove (uneset) flag for item
    void removeFlag(ItemFlag flag);
    // Return flags
    int flags() const;
    // Return whether specified flag is set
    bool hasFlag(GenericItem::ItemFlag flag) const;

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    virtual bool write(LineParser &parser) = 0;
    // Read data through specified parser
    virtual bool read(LineParser &parser, CoreData &coreData) = 0;

    /*
     * Parallel Comms
     */
    public:
    // Broadcast item contents
    virtual bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData) = 0;
    // Return equality between items
    virtual bool equality(ProcessPool &procPool) = 0;
};
