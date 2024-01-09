// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "classes/dataSource.h"
#include "io/fileAndFormat.h"
#include "math/dataBase.h"

// Forward Declarations
class DataSource;

// Base keyword for data source
class DataSourceKeywordBase : public KeywordBase
{
    // Typedef
    public:
    using DataPair = std::pair<DataSource, DataSource>;

    public:
    DataSourceKeywordBase(std::vector<DataPair> &dataSources, std::string_view endKeyword);
    ~DataSourceKeywordBase() override = default;

    /*
     * Data
     */
    protected:
    // End keyword
    const std::string endKeyword_;
    // Reference to module data
    std::vector<DataPair> &dataSources_;

    public:
    // Returns reference to module data
    std::vector<DataPair> &dataSources();
};