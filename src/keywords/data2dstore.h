// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/data2dstore.h"
#include "keywords/data.h"

// Forward Declarations
/* none */

// Keyword with a store of Data2D
class Data2DStoreKeyword : public KeywordData<Data2DStore &>
{
    public:
    Data2DStoreKeyword(Data2DStore &dataStore);
    ~Data2DStoreKeyword();

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    int maxArguments() const override;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
