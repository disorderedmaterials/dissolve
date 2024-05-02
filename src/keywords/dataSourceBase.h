// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "classes/dataSource.h"
#include "io/fileAndFormat.h"
#include "keywords/base.h"
#include "math/dataBase.h"

// Base keyword for data source
class DataSourceKeywordBase : public KeywordBase
{

    public:
    DataSourceKeywordBase();
    ~DataSourceKeywordBase() override = default;
};