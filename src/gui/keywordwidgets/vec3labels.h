// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/vec3labels.h"
#include <QLabel>

// Forward Declarations
/* none */

// Vec3 Widget Labels
class Vec3WidgetLabels
{
    public:
    // Set/hide/show label according to labelling type
    static void set(QLabel *label, Vec3Labels::LabelType labelType, int index);
};
