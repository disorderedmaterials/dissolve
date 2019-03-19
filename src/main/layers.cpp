/*
	*** Dissolve - Layers
	*** src/main/layers.cpp
	Copyright T. Youngs 2012-2019

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
ModuleLayer* Dissolve::addProcessingLayer()
{
	return processingLayers_.add();
}

// Find named processing layer
ModuleLayer* Dissolve::findProcessingLayer(const char* name) const
{
	ListIterator<ModuleLayer> layerIterator(processingLayers_);
	while (ModuleLayer* layer = layerIterator.iterate()) if (DissolveSys::sameString(layer->name(), name)) return layer;

	return NULL;
}

// Own the specified processing layer
bool Dissolve::ownProcessingLayer(ModuleLayer* layer)
{
	// Sanity check - do we already own this Configuration?
	if (processingLayers_.contains(layer)) return Messenger::error("Already own ModuleLayer '%s', so nothing to do.\n", layer->name());

	processingLayers_.own(layer);

	setUp_ = false;

	return true;
}

// Return number of defined processing layers
int Dissolve::nProcessingLayers() const
{
	return processingLayers_.nItems();
}

// Generate unique processing layer name with base name provided
const char* Dissolve::uniqueProcessingLayerName(const char* base) const
{
	static CharString uniqueName;
	CharString baseName = base;
	uniqueName = baseName;
	int suffix = 0;

	// Must always have a baseName
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find an unused name starting with the baseName provided
	while (findProcessingLayer(uniqueName))
	{
		// Increase suffix value and regenerate uniqueName from baseName
		++suffix;
		uniqueName.sprintf("%s%i", baseName.get(), suffix);
	}

	return uniqueName;
}

// Return list of processing layers
List<ModuleLayer>& Dissolve::processingLayers()
{
	return processingLayers_;
}

// Return data associated with processing Modules
GenericList& Dissolve::processingModuleData()
{
	return processingModuleData_;
}

// Create and add a named Module to the named layer (creating it if necessary), with optional Configuration target
Module* Dissolve::createModuleInLayer(const char* moduleType, const char* layerName, Configuration* cfg)
{
	// First, attempt to create a new Module with the specified name
	Module* module = createModuleInstance(moduleType);
	if (!module) return NULL;

	// Find / create the specified layer
	ModuleLayer* layer = findProcessingLayer(layerName);
	if (!layer)
	{
		layer = addProcessingLayer();
		layer->setName(layerName);
	}

	// Add the new Module to the layer
	layer->add(module);

	// Set Configuration target in the Module if specified
	if (cfg) module->addTargetConfiguration(cfg);

	return module;
}
