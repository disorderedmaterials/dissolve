// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <string_view>

// Spacegroup Name Data
class SpacegroupNameData
{
    public:
    SpacegroupNameData(std::string_view name, std::string_view formattedName);

    private:
    // Name of the spacegroup
    std::string_view name_;
    // Formatted name of the spacegroup
    std::string_view formattedName_;

    public:
    // Return the name of the spacegroup
    std::string_view name() const;
    // Return the formatted name of the spacegroup
    std::string_view formattedName() const;
};

// Spacegroups
class Spacegroups
{
    private:
    // Return SpacegroupNameData with index specified
    static const SpacegroupNameData &spacegroupNameData(int id);
};
