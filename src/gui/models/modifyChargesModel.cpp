// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/modifyChargesModel.h"
#include <QMessageBox>

#include <QDebug>

ModifyChargesModel::ModifyChargesModel() {}

ModifyChargesModel::~ModifyChargesModel() {}

double ModifyChargesModel::scaleValue() const { return currentScaleValue_; }

double ModifyChargesModel::smoothValue() const { return currentSmoothValue_; }

int ModifyChargesModel::sigFigValue() const { return currentSigFigValue_; }

void ModifyChargesModel::updateScaleValue(double newValue) { currentScaleValue_ = newValue; }

void ModifyChargesModel::updateSmoothValue(double newValue) { currentSmoothValue_ = newValue; }

void ModifyChargesModel::updateSigFigValue(int newValue) { currentSigFigValue_ = newValue; }

void ModifyChargesModel::setScaleType(ScaleType option) { scaleType_ = option; }

ModifyChargesModel::ScaleType ModifyChargesModel::getScaleType() { return scaleType_; }

void ModifyChargesModel::setModifier(ModifyChargesModel::ModifyBy option) { modify_ = option; }

ModifyChargesModel::ModifyBy ModifyChargesModel::getModifier() { return modify_; }

bool ModifyChargesModel::scale(Species *species, bool showDialogOnError = true)
{
    auto scaleFactor = scaleValue();
    if (scaleType_ != Scale)
    {
        auto scaleTarget = scaleFactor;
        if (fabs(scaleTarget) < 1.0e-6)
        {
            if (showDialogOnError)
            {
                QMessageBox::warning(nullptr, "Scale atom charges", "Cannot scale atom charges so they sum to 0.",
                                     QMessageBox::StandardButton::Ok);
            }
            return false;
        }

        auto sum = 0.0;
        for (auto &atom : species->atoms())
            sum += atom.charge();
        scaleFactor = scaleTarget / sum;
    }
    for (auto &atom : species->atoms())
        atom.setCharge(atom.charge() * scaleFactor);

    return true;
}

void ModifyChargesModel::smooth(Species *species) 
{
    auto targetSum = smoothValue();
    auto sum = species->totalCharge(false), shiftVal = 0.0;
    shiftVal = (sum - targetSum) / species->nAtoms();
    for (auto &atom : species->atoms())
        atom.setCharge(atom.charge() - shiftVal);
}

void ModifyChargesModel::reduceSignificantFigures(Species *species) 
{
    auto significantFigures = sigFigValue();
    for (auto &atom : species->atoms())
        atom.setCharge(std::round(atom.charge() * std::pow(10, significantFigures)) / std::pow(10, significantFigures));
}