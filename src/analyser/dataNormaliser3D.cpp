// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser3D.h"
#include "classes/configuration.h"
#include "math/data3D.h"

DataNormaliser3D::DataNormaliser3D(Data3D &targetData) : targetData_(targetData) {}

void DataNormaliser3D::normaliseBySitePopulation(double population) { targetData_ /= population; }

void DataNormaliser3D::normaliseByGrid() {}