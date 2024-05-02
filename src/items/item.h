// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <any>

// Generic Item Definitions
namespace GenericItem
{
// Item Data Typedef
using Type = std::tuple<std::any, std::string, int, int>;
// Item Data Tuple Indices
enum ItemData
{
    AnyObject,
    ClassName,
    Version,
    Flags
};
// Item Flags
enum ItemFlag
{
    NoFlags = 0,                  /* No flags */
    InRestartFileFlag = 1,        /* The item should be written to the restart file */
    IsReferencePointDataFlag = 2, /* The item was loaded as reference point data */
    ProtectedFlag = 4             /* The item will not be clear()'d unless forced */
};
// Retrieval Status
enum class ItemStatus
{
    Existing,
    Created
};
}; // namespace GenericItem
