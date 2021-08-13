// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "sginfo/sginfo.h"
#include <string>

class Spacegroup
{
    public:
    Spacegroup();
    ~Spacegroup();

    /*
     * Data
     */
    private:
    // Name / index information
    const T_TabSgName *sgNameId_;
    // SGInfo information
    T_SgInfo sgInfo_;

    public:
    // Initialise for specified Hermann-Mauguin group
    bool initialise(std::string hmName);
    // Return whether the Spacegroup is valid
    bool isValid() const;
    // Return Spacegroup name
    std::string_view name() const;
    // Return spacegroup name for International Tables index provided
    static std::string_view name(int id);
    // Return formatted Spacegroup name
    std::string_view formattedName() const;
    // Return formatted spacegroup name for International Tables index provided
    static std::string_view formattedNname(int id);
    // Return Spacegroup index in International Tables
    int internationalTableIndex() const;
};
