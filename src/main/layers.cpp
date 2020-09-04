/*
    *** Dissolve - Layers
    *** src/main/layers.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main/dissolve.h"

/*
 * Public Functions
 */

// Add new processing layer
ModuleLayer *Dissolve::addProcessingLayer() { return processingLayers_.add(); }

// Remove specified processing layer
void Dissolve::removeProcessingLayer(ModuleLayer *layer)
{
    if (!layer)
        return;

    // Remove any references to the Modules in the layer before we delete them
    ListIterator<Module> moduleIterator(layer->modules());
    while (Module *module = moduleIterator.iterate())
        removeReferencesTo(module);

    // Delete the module instances themselves
    moduleIterator.restart();
    while (Module *module = moduleIterator.iterate())
        moduleInstances_.remove(module);

    // Now safe to remove the layer
    processingLayers_.remove(layer);
}

// Find named processing layer
ModuleLayer *Dissolve::findProcessingLayer(std::string_view name) const
{
    ListIterator<ModuleLayer> layerIterator(processingLayers_);
    while (ModuleLayer *layer = layerIterator.iterate())
        if (DissolveSys::sameString(layer->name(), name))
            return layer;

    return nullptr;
}

// Own the specified processing layer
bool Dissolve::ownProcessingLayer(ModuleLayer *layer)
{
    // Sanity check - do we already own this Configuration?
    if (processingLayers_.contains(layer))
        return Messenger::error("Already own ModuleLayer '{}', so nothing to do.\n", layer->name());

    processingLayers_.own(layer);

    return true;
}

// Return number of defined processing layers
int Dissolve::nProcessingLayers() const { return processingLayers_.nItems(); }

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
List<ModuleLayer> &Dissolve::processingLayers() { return processingLayers_; }

// Return data associated with processing Modules
GenericList &Dissolve::processingModuleData() { return processingModuleData_; }

// Create and add a named Module to the named layer (creating it if necessary), with optional Configuration target
Module *Dissolve::createModuleInLayer(std::string_view moduleType, std::string_view layerName, Configuration *cfg)
{
    // First, attempt to create a new Module with the specified name
    Module *module = createModuleInstance(moduleType);
    if (!module)
        return nullptr;

    // Find / create the specified layer
    ModuleLayer *layer = findProcessingLayer(layerName);
    if (!layer)
    {
        layer = addProcessingLayer();
        layer->setName(layerName);
    }

    // Add the new Module to the layer
    layer->own(module);

    // Set Configuration target in the Module if specified
    if (cfg)
        module->addTargetConfiguration(cfg);

    return module;
}
