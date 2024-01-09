// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/base.h"

KeywordWidgetBase::KeywordWidgetBase(const CoreData &coreData) : coreData_(coreData), refreshing_(false) {}
