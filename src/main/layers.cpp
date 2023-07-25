// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/dissolve.h"

/*
 * Public Functions
 */

// Run the set-up stages of all modules in all layers
bool Dissolve::setUpProcessingLayerModules()
{
    auto setUpResult = true;
    for (auto &layer : processingLayers_)
        if (!layer->setUpAll(*this, worldPool()))
            setUpResult = false;
    return setUpResult;
}

// Return data associated with processing Modules
GenericList &Dissolve::processingModuleData() { return processingModuleData_; }