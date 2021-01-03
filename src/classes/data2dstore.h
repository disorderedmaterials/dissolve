// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/import/data2d.h"
#include "math/data2d.h"
#include "templates/list.h"
#include "templates/refdatalist.h"

// Forward Declarations
/* none */

// Data2D Store
class Data2DStore
{
    public:
    Data2DStore();
    ~Data2DStore();

    /*
     * Data
     */
    private:
    // List of contained data
    List<Data2D> data_;
    // References for Data2D and associated file/format
    RefDataList<Data2D, Data2DImportFileFormat> dataReferences_;

    public:
    // Add named data reference to store, reading file and format from specified parser / starting argument
    bool addData(std::string_view dataName, LineParser &parser, int startArg, std::string_view endKeyword, CoreData &coreData);
    // Check to see if the named data is present in the store
    bool containsData(std::string_view name) const;
    // Return named data
    const Data2D &data(std::string_view name) const;
    // Return list of all data
    const List<Data2D> &data() const;
    // Return list of all data references
    const RefDataList<Data2D, Data2DImportFileFormat> &dataReferences() const;
};
