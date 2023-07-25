// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/import/data3D.h"
#include "math/data3D.h"
#include "templates/optionalRef.h"
#include <list>

// Data3D Store
class Data3DStore : public Serialisable<const CoreData &>
{
    public:
    Data3DStore() = default;
    ~Data3DStore() = default;

    /*
     * Data
     */
    private:
    // Vector of contained data
    std::vector<std::shared_ptr<std::pair<Data3D, Data3DImportFileFormat>>> data_;

    public:
    // Add named data reference to store, reading file and format from specified parser / starting argument
    bool addData(std::string_view dataName, LineParser &parser, int startArg, std::string_view endKeyword,
                 const CoreData &coreData);
    // Check to see if the named data is present in the store
    bool containsData(std::string_view name) const;
    // Return named data
    OptionalReferenceWrapper<const Data3D> data(std::string_view name) const;
    // Return vector of all data
    const std::vector<std::shared_ptr<std::pair<Data3D, Data3DImportFileFormat>>> &data() const;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
