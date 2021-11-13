// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "math/function1d.h"

// Keyword with Function1D Data
class Function1DKeyword : public KeywordBase
{
    public:
    Function1DKeyword(Functions::Function1DWrapper &data, int functionProperties = FunctionProperties::None);
    ~Function1DKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    Functions::Function1DWrapper &data_;
    // Requested function properties
    int functionProperties_;

    public:
    // Return reference to data
    Functions::Function1DWrapper &data();
    const Functions::Function1DWrapper &data() const;
    // Return requested function properties
    int functionProperties() const;

    /*
     * Arguments
     */
    public:
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
