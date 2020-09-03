/*
    *** Generic Item Container - Array2D<DummyClass>
    *** src/genericitems/array2ddummy.h
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

#include "base/dummyclass.h"
#include "genericitems/container.h"
#include "math/data1d.h"

// GenericItemContainer< Array2D<DummyClass> >
template <> class GenericItemContainer<Array2D<DummyClass>> : public GenericItem
{
    public:
    GenericItemContainer<Array2D<DummyClass>>(std::string_view name, int flags = 0) : GenericItem(name, flags) {}

    /*
     * Item Class
     */
    protected:
    // Create a new GenericItem containing same class as current type
    GenericItem *createItem(std::string_view className, std::string_view name, int flags = 0)
    {
        if (DissolveSys::sameString(className, "Array2D<Data1D>"))
            return new GenericItemContainer<Array2D<Data1D>>(name, flags);
        return NULL;
    }

    public:
    // Return class name contained in item
    std::string_view itemClassName() { return "Array2D<DummyClass>"; }

    /*
     * I/O
     */
    public:
    // Write data through specified parser
    bool write(LineParser &parser) { return false; }
    // Read data through specified parser
    bool read(LineParser &parser, CoreData &coreData) { return false; }

    /*
     * Parallel Comms
     */
    public:
    // Broadcast item contents
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData) { return false; }
    // Return equality between items
    bool equality(ProcessPool &procPool) { return false; }
};
