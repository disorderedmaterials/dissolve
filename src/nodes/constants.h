// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

namespace NodeConstants
{
// Processing result
enum class ProcessResult
{
    Failed,
    Unchanged,
    Success
};
// Invalid version index
static constexpr int InvalidVersion = -1;
}; // namespace NodeConstants
