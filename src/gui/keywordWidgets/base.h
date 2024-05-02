// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/signals.h"
#include "templates/flags.h"

// Forward Declarations
class CoreData;
class GenericList;

// Keyword Widget Base Class
class KeywordWidgetBase
{
    public:
    KeywordWidgetBase(const CoreData &coreData);

    /*
     * CoreData Reference
     */
    protected:
    // CoreData source
    const CoreData &coreData_;

    /*
     * Update
     */
    protected:
    // Whether the widget is currently refreshing
    bool refreshing_;

    public:
    // Update value displayed in widget
    virtual void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags = {}) = 0;
};
