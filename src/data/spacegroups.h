// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "math/matrix4.h"
#include "sginfo/sginfo.h"
#include <string>

class SpaceGroup
{
    public:
    SpaceGroup();
    ~SpaceGroup();

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
    // Return number of defined space groups
    static int nSpaceGroups();
    // Return whether the space group is valid
    bool isValid() const;
    // Return space group name
    std::string_view name() const;
    // Return space group name for International Tables index provided
    static std::string_view name(int id);
    // Return formatted space group name
    std::string_view formattedName() const;
    // Return formatted space group name for International Tables index provided
    static std::string_view formattedName(int id);
    // Return space group index in International Tables
    int internationalTableIndex() const;
    // Return vector of symmetry group operators for the space group
    std::vector<Matrix4> symmetryOperators() const;
};
