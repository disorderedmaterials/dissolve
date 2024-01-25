// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/scaleChargesDialogModel.h"
#include <QMessageBox>

ScaleChargesDialogModel::ScaleChargesDialogModel() {}

ScaleChargesDialogModel::~ScaleChargesDialogModel() {}

double ScaleChargesDialogModel::value() const { return currentValue_; }

void ScaleChargesDialogModel::updateValue(double newVal) { currentValue_ = newVal; }

void ScaleChargesDialogModel::setOption(Option option) { scaleType_ = option; }

ScaleChargesDialogModel::Option ScaleChargesDialogModel::getOption() { return scaleType_; }

void ScaleChargesDialogModel::scaleCharges(Species *species, DissolveWindow *dissolveWindow, bool showDialogOnError = true)
{
    auto scaleFactor = value();
    if (scaleType_ != Scale)
    {
        auto scaleTarget = scaleFactor;
        if (if (fabs(scaleTarget) < 1.0e-6))
        {
            if (showDialogOnError) {
                QMessageBox::warning(dissolveWindow, "Scale atom charges", "Cannot scale atom charges so they sum to 0.",
                                    QMessageBox::StandardButton::Ok);
                }
        }

        auto sum = 0.0;
        for (auto &atom : species->atoms())
            sum += atom.charge();
        scaleFactor = scaleTarget / sum;
    }
    for (auto &atom : species->atoms())
        atom.setCharge(atom.charge() * scaleFactor);
}
