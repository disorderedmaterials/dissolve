// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/import/dataBase.h"
#include "keywords/base.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/dataBase.h"
#include "templates/optionalRef.h"
#include <optional>
#include <vector>

// Foward declaration
class DataSource;

// Keyword managing data sources
// Template arguements: data class (Data1D, Data2D ...), add data function type
template <typename T, typename F> class DataSourceKeyword : public KeywordBase
{
    public:
    explicit DataSourceKeyword(F addData, std::string_view endKeyword);
    ~DataSourceKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data adding function in module
    const F addData_;
    // Format object for the data
    std::unique_ptr<DataImportFileFormat> format_;
    // Vector of vectors of pairs containing data source type and reference to data
    std::vector<std::vector<std::pair<std::string_view, std::reference_wrapper<T>>>> data_;

    /*
     * Arguments
     */
    private:
    // End keyword for the dataSource keyword
    const std::string endKeyword_;
    // Start keyword to specify an internal data source
    const std::string internalKwd_{"Internal"};
    // Start keyword to specify an external data source
    const std::string externalKwd_{"External"};
    // End keyword to specify an external data source
    const std::string externalEndKwd_{"EndExternal"};

    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
