// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/scaleChargesDialogModel.h"

ScaleChargesDialogModel::ScaleChargesDialogModel() {}

ScaleChargesDialogModel::~ScaleChargesDialogModel() {}

double ScaleChargesDialogModel::value() const { return currentValue_; }

void ScaleChargesDialogModel::updateValue(double newVal) { currentValue_ = newVal; }

void ScaleChargesDialogModel::setOption(Option option) { scale_type_ = option; }

ScaleChargesDialogModel::Option ScaleChargesDialogModel::getOption() { return scale_type_; }

bool ScaleChargesDialogModel::scaleCharges(Species *species)
{
    auto scaleFactor = value();
    if (scale_type_ != Scale)
    {
        auto scaleTarget = scaleFactor;
        if (scaleTarget == 0.0)
        {
            return false;
        }

        double sum = 0.0;
        for (auto &atom : species->atoms())
            sum += atom.charge();
        scaleFactor = scaleTarget / sum;
    }
    for (auto &atom : species->atoms())
        atom.setCharge(atom.charge() * scaleFactor);

    return true;
}
