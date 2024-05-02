// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "main/version.h"

namespace dissolve
{

SerialisedValue backwardsUpgrade(SerialisedValue contents);

}
