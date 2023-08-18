// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"
#include "keywords/base.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "templates/optionalRef.h"
#include <optional>
#include <vector>

// Foward declaration
class DataSource;

// Keyword managing data sources
template <typename T, typename F> class DataSourceKeyword : public KeywordBase
{
    public:
    enum class SourceType
    {
        Internal,
        External
    };

    explicit DataSourceKeyword(F addData, std::string_view endKeyword);
    ~DataSourceKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data adding function in module
    F addData_;
    // Format object for the data
    FileAndFormat format_;

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
    // Has not changed from initial value
    bool isDefault() const override;
};
