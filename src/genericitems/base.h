/*
    *** Generic Item Base
    *** src/genericitems/base.h
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

#include "base/messenger.h"

// Forward Declarations
class CoreData;
class ProcessPool;

// Base function templates for a Class that is to be used as a GenericItem
class GenericItemBase
{
    /*
     * Item Class
     */
    public:
    // Return class name
    static std::string_view itemClassName();

    /*
     * I/O
     */
    public:
    // Read data through specified parser
    virtual bool read(LineParser &parser, CoreData &coreData);
    // Write data through specified parser
    virtual bool write(LineParser &parser);

    /*
     * Parallel Comms
     */
    public:
    // Broadcast item contents
    virtual bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Return equality between items
    virtual bool equality(ProcessPool &procPool);
};
