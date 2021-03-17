// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/base.h"

KeywordWidgetBase::KeywordWidgetBase(const CoreData &coreData) : coreData_(coreData) { refreshing_ = false; }
