// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/dissolve.h"

/*
 * Public Functions
 */

// Add new processing layer
ModuleLayer *Dissolve::addProcessingLayer()
{
    return coreData_.addProcessingLayer();
}
// Remove specified processing layer
void Dissolve::removeProcessingLayer(ModuleLayer *layer)
{
    if (!layer)
        return;

    // Remove any references to the Modules in the layer before we delete it
    for (auto &module : layer->modules())
        removeReferencesTo(module.get());

    // Now safe to remove the layer
    coreData_.processingLayers().erase(
        std::find_if(coreData_.processingLayers().begin(), coreData_.processingLayers().end(), [layer](const auto &l) { return l.get() == layer; }));
}

// Find named processing layer
ModuleLayer *Dissolve::findProcessingLayer(std::string_view name) const
{
    return coreData_.findProcessingLayer(name);
}

// Own the specified processing layer
bool Dissolve::ownProcessingLayer(ModuleLayer *layer)
{
    return coreData_.ownProcessingLayer(layer);
}

// Return number of defined processing layers
int Dissolve::nProcessingLayers() const { return coreData_.nProcessingLayers(); }

// Return list of processing layers
std::vector<std::unique_ptr<ModuleLayer>> &Dissolve::processingLayers() { return coreData_.processingLayers(); }

// Run the set-up stages of all modules in all layers
bool Dissolve::setUpProcessingLayerModules()
{
    auto setUpResult = true;
    for (auto &layer : coreData_.processingLayers())
        if (!layer->setUpAll(*this, worldPool()))
            setUpResult = false;
    return setUpResult;
}

// Return data associated with processing Modules
GenericList &Dissolve::processingModuleData() { return coreData_.processingModuleData(); }
