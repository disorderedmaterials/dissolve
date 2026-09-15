// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "enumRegistry.h"
#include "data/structureFactors.h"
#include "gui2/models/nodeGraph/enumOptionsModel.h"
#include "math/windowFunction.h"
#include "nodes/gr.h"
#include "nodes/md.h"

std::map<std::type_index, std::shared_ptr<EnumOptionsModel>> EnumRegistry::options_;

bool EnumRegistry::hasEnumOption(std::type_index enumType)
{
    instantiateOptions();
    return options_.contains(enumType);
}

std::shared_ptr<EnumOptionsModel> EnumRegistry::options(std::type_index enumType)
{
    instantiateOptions();
    return options_[enumType];
}

void EnumRegistry::instantiateOptions()
{
    if (!options_.empty())
        return;

    auto wrap = [](auto &&x) -> std::shared_ptr<EnumOptionsModel>
    {
        auto result = std::make_shared<EnumOptionsModel>();
        result->setData(std::make_shared<std::remove_reference_t<decltype(x)>>(x));
        return result;
    };

    options_ = {{typeid(StructureFactors::NormalisationType), wrap(StructureFactors::normalisationTypes())},
                {typeid(GRNode::PartialsMethod), wrap(GRNode::partialsMethods())},
                {typeid(MDNode::TimestepType), wrap(MDNode::timestepType())},
                {typeid(WindowFunction::Form), wrap(WindowFunction::forms())}};
}
