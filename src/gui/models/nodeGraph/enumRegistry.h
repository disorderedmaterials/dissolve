// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/enumOptionsBase.h"
#include "gui/models/nodeGraph/enumOptionsModel.h"
#include <map>
#include <memory>
#include <typeindex>

// Registry of EnumOptions
class EnumRegistry
{
    private:
    static std::map<std::type_index, std::shared_ptr<EnumOptionsModel>> options_;

    public:
    static void instantiateOptions();

    static bool hasEnumOption(std::type_index enumType);
    static std::shared_ptr<EnumOptionsModel> options(std::type_index enumType);
};
