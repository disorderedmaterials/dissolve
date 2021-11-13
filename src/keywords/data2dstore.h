// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class Data2DStore;

// Keyword with a store of Data2D
class Data2DStoreKeyword : public KeywordBase
{
    public:
    explicit Data2DStoreKeyword(Data2DStore &data);
    ~Data2DStoreKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    Data2DStore &data_;

    public:
    // Return reference to data
    Data2DStore &data();
    const Data2DStore &data() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    int maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
