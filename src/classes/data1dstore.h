// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/import/data1d.h"
#include "math/data1d.h"
#include "templates/list.h"
#include "templates/refdatalist.h"

// Forward Declarations
/* none */

// Data1D Store
class Data1DStore
{
    public:
    Data1DStore() = default;
    ~Data1DStore() = default;

    /*
     * Data
     */
    private:
    // List of contained data
    List<Data1D> data_;
    // References for Data1D and associated file/format
    RefDataList<Data1D, Data1DImportFileFormat> dataReferences_;

    public:
    // Add named data reference to store, reading file and format from specified parser / starting argument
    bool addData(std::string_view dataName, LineParser &parser, int startArg, std::string_view endKeyword,
                 const CoreData &coreData);
    // Check to see if the named data is present in the store
    bool containsData(std::string_view name) const;
    // Return named data
    const Data1D &data(std::string_view name) const;
    // Return list of all data
    const List<Data1D> &data() const;
    // Return list of all data references
    const RefDataList<Data1D, Data1DImportFileFormat> &dataReferences() const;
};
