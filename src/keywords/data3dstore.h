// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class Data3DStore;

// Keyword managing a store of Data3D
class Data3DStoreKeyword : public KeywordBase
{
    public:
    explicit Data3DStoreKeyword(Data3DStore &data);
    ~Data3DStoreKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    Data3DStore &data_;

    public:
    // Return reference to data
    Data3DStore &data();
    const Data3DStore &data() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
