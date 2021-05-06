// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"
#include "math/function1d.h"

// Keyword with Function1D Data
class Function1DKeyword : public KeywordData<Functions::Function1DWrapper>
{
    public:
    Function1DKeyword(Functions::Function1DWrapper value = Functions::Function1DWrapper(Functions::Function1D::None),
                      int functionProperties = FunctionProperties::None);
    ~Function1DKeyword() = default;

    /*
     * Data
     */
    private:
    // Requested function properties
    int functionProperties_;

    public:
    // Return requested function properties
    int functionProperties() const;

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
