// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "modules/siteRDF/siteRDF.h"

// Return whether specified coordination number range is enabled
bool SiteRDFModule::isRangeEnabled(int id) const { return rangeEnabled_[id]; }
