// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataOperator3D.h"
#include "math/data3D.h"

DataOperator3D::DataOperator3D(Data3D &targetData) : DataOperatorBase<Data3D, NormalisationFunction3D>(targetData) {}

void DataOperator3D::normalise(NormalisationFunction3D normalisationFunction)
{
    Messenger::warn("Normalisation function not implemented for 3D data.");
}

void DataOperator3D::normaliseByGrid()
{
    // Determine bin area from first points of data
    auto xBinWidth = targetData_.xAxis().at(1) - targetData_.xAxis().at(0);
    auto yBinWidth = targetData_.yAxis().at(1) - targetData_.yAxis().at(0);
    auto zBinWidth = targetData_.zAxis().at(1) - targetData_.zAxis().at(0);
    auto binVolume = xBinWidth * yBinWidth * zBinWidth;

    targetData_ /= binVolume;
}

void DataOperator3D::normaliseBySphericalShell()
{
    Messenger::warn("Spherical shell normalisation not implemented for 3D data.");
}

void DataOperator3D::normaliseTo(double value, bool absolute)
{
    Messenger::warn("Value normalisation not implemented for 3D data.");
}
