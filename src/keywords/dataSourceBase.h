// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "classes/dataSource.h"
#include "io/fileAndFormat.h"
#include "math/dataBase.h"

// Supports up to two data sources in each keyword call
using DataPair = std::pair<DataSource, DataSource>;

class DataSourceKeywordBase : public KeywordBase
{
    public:
    DataSourceKeywordBase(std::vector<DataPair> &dataSources, std::string_view endKeyword);
    ~DataSourceKeywordBase() override = default;

    /*
     * Data
     */
    protected:
    const std::string endKeyword_;

    std::vector<DataPair> &dataSources_;

    public:
    std::vector<DataPair> &dataSources();
    std::string externalKeyword();
    std::string internalKeyword();
};