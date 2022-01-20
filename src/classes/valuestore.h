// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "io/import/values.h"
#include "templates/optionalref.h"
#include <list>
#include <vector>

// Value Store
class ValueStore
{
    public:
    ValueStore() = default;
    ~ValueStore() = default;

    /*
     * Data
     */
    private:
    // Value of contained data
    std::list<std::tuple<std::string, std::vector<double>, ValueImportFileFormat>> data_;

    public:
    // Add named data reference to store, reading file and format from specified parser / starting argument
    bool addData(std::string_view dataName, LineParser &parser, int startArg, std::string_view endKeyword,
                 const CoreData &coreData);
    // Check to see if the named data is present in the store
    bool containsData(std::string_view name) const;
    // Return named data
    OptionalReferenceWrapper<const std::vector<double>> data(std::string_view name) const;
    // Return vector of all data
    const std::list<std::tuple<std::string, std::vector<double>, ValueImportFileFormat>> &data() const;
};
