// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/dissolve.h"
#include "module/context.h"
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
        coreData_.removeReferencesTo(module.get());

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

// Return list of processing layers
std::vector<std::unique_ptr<ModuleLayer>> &Dissolve::processingLayers() { return processingLayers_; }

// Run the set-up stages of all modules in all layers
bool Dissolve::setUpProcessingLayerModules()
{
    auto setUpResult = true;

    ModuleContext context(worldPool(), coreData(), pairPotentialRange(), pairPotentialDelta(), potentialMap());

    for (auto &layer : processingLayers_)
        if (!layer->setUpAll(context))
            setUpResult = false;
    return setUpResult;
}

// Return data associated with processing Modules
GenericList &Dissolve::processingModuleData() { return processingModuleData_; }
