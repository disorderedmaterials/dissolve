// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "gui/models/nodeGraph/generatorGraphModel.h"

template <GeneratorGraphModel::PropertyIndex T> QVariant unlockKeyword(const KeywordStore &keywords);

template <GeneratorGraphModel::PropertyIndex T> bool updateKeyword(KeywordStore &keywords, QVariant value);
