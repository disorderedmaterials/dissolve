// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser3D.h"
#include "classes/configuration.h"
#include "math/data3D.h"

DataNormaliser3D::DataNormaliser3D(Data3D &targetData) : DataNormaliserBase<Data3D>(targetData) {}

void DataNormaliser3D::normaliseBySitePopulation(double population) { targetData_ /= population; }

void DataNormaliser3D::normaliseByGrid()
{
    // Determine bin area from first points of data
    auto xBinWidth = targetData_.xAxis().at(1) - targetData_.xAxis().at(0);
    auto yBinWidth = targetData_.yAxis().at(1) - targetData_.yAxis().at(0);
    auto zBinWidth = targetData_.zAxis().at(1) - targetData_.zAxis().at(0);
    auto binVolume = xBinWidth * yBinWidth * zBinWidth;

    targetData_ /= binVolume;
}

void DataNormaliser3D::normaliseByNumberDensity(double population, Configuration *targetConfiguration) { ; }

void DataNormaliser3D::normaliseBySphericalShell() { ; }

void DataNormaliser3D::normaliseByValue(double value, bool absolute) { ; }

void DataNormaliser3D::normaliseByExpression(std::string_view expressionString) { ; }
