// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"
#include "math/windowfunction.h"

// Forward Declarations
/* none */

// Keyword with WindowFunction Data
class WindowFunctionKeyword : public KeywordData<WindowFunction>
{
    public:
    WindowFunctionKeyword(WindowFunction value);
    ~WindowFunctionKeyword();

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const;
    // Return maximum number of arguments accepted
    int maxArguments() const;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, CoreData &coreData);
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix);
};
