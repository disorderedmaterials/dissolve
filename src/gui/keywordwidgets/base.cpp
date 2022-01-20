// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/keywordwidgets/base.h"

KeywordWidgetBase::KeywordWidgetBase(const CoreData &coreData) : coreData_(coreData) { refreshing_ = false; }
