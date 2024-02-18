// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser3D.h"
#include "math/data3D.h"

DataNormaliser3D::DataNormaliser3D(Data3D &targetData) : DataNormaliserBase<Data3D>(targetData) {}

void DataNormaliser3D::normaliseByGrid()
{
    // Determine bin area from first points of data
    auto xBinWidth = targetData_.xAxis().at(1) - targetData_.xAxis().at(0);
    auto yBinWidth = targetData_.yAxis().at(1) - targetData_.yAxis().at(0);
    auto zBinWidth = targetData_.zAxis().at(1) - targetData_.zAxis().at(0);
    auto binVolume = xBinWidth * yBinWidth * zBinWidth;

    targetData_ /= binVolume;
}

void DataNormaliser3D::normaliseBySphericalShell()
{
    Messenger::warn("Spherical shell normalisation not implemented for 3D data.");
}

void DataNormaliser3D::normaliseByExpression(std::string_view expressionString)
{
    Messenger::warn("Expression normalisation not implemented for 3D data.");
}
