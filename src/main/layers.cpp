// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"

/*
 * Public Functions
 */

// Add new processing layer
ModuleLayer *Dissolve::addProcessingLayer() { return processingLayers_.emplace_back(std::make_unique<ModuleLayer>()).get(); }

// Remove specified processing layer
void Dissolve::removeProcessingLayer(ModuleLayer *layer)
{
    if (!layer)
        return;

    // Remove any references to the Modules in the layer before we delete it
    for (auto &module : layer->modules())
        removeReferencesTo(module.get());

    // Now safe to remove the layer
    processingLayers_.erase(
        std::find_if(processingLayers_.begin(), processingLayers_.end(), [layer](const auto &l) { return l.get() == layer; }));
}

// Find named processing layer
ModuleLayer *Dissolve::findProcessingLayer(std::string_view name) const
{
    auto it = std::find_if(processingLayers_.begin(), processingLayers_.end(),
                           [name](auto &layer) { return DissolveSys::sameString(layer->name(), name); });
    if (it == processingLayers_.end())
        return nullptr;
    return it->get();
}

// Own the specified processing layer
bool Dissolve::ownProcessingLayer(ModuleLayer *layer)
{
    // Sanity check - do we already own this Configuration?
    auto it = std::find_if(processingLayers_.begin(), processingLayers_.end(), [layer](auto &l) { return l.get() == layer; });
    if (it != processingLayers_.end())
        return Messenger::error("Already own ModuleLayer '{}', so nothing to do.\n", layer->name());

    processingLayers_.emplace_back(layer);

    return true;
}

// Return number of defined processing layers
int Dissolve::nProcessingLayers() const { return processingLayers_.size(); }

// Generate unique processing layer name with base name provided
std::string Dissolve::uniqueProcessingLayerName(std::string_view base) const
{
    std::string_view baseName = base.empty() ? "Unnamed" : base;
    std::string uniqueName{baseName};

    // Find an unused name starting with the baseName provided
    auto suffix = 0;
    while (findProcessingLayer(uniqueName))
        uniqueName = fmt::format("{}{}", baseName, ++suffix);

    return uniqueName;
}

// Return list of processing layers
std::vector<std::unique_ptr<ModuleLayer>> &Dissolve::processingLayers() { return processingLayers_; }

// Return data associated with processing Modules
GenericList &Dissolve::processingModuleData() { return processingModuleData_; }
