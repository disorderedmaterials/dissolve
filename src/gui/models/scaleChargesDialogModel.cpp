// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/scaleChargesDialogModel.h"

ScaleChargesDialogModel::ScaleChargesDialogModel() {}

ScaleChargesDialogModel::~ScaleChargesDialogModel() {}

double ScaleChargesDialogModel::value() const { return currentValue_; }

void ScaleChargesDialogModel::updateValue(double newVal) { currentValue_ = newVal; }

void ScaleChargesDialogModel::setOption(int option)
{
    switch (option)
    {
        case 1:
            scale_ = true;
            scaleTo_ = false;
            break;
        case 2:
            scale_ = false;
            scaleTo_ = true;
            break;
        default:
            break;
    }
}
