// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/data3dstore.h"
#include "keywords/data.h"

// Forward Declarations
/* none */

// Keyword with a store of Data3D
class Data3DStoreKeyword : public KeywordData<Data3DStore &>
{
    public:
    Data3DStoreKeyword(Data3DStore &dataStore);
    ~Data3DStoreKeyword();

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
