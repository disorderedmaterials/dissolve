// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/typeDefs.h"

class Configuration;
class Data3D;

class DataNormaliser3D
{
    public:
    DataNormaliser3D(Data3D &targetData);

    /*
     * Target
     */
    private:
    // Target data to normalise
    Data3D &targetData_;

    /*
     * Normalisation functions
     */
    public:
    void normaliseBySitePopulation(double population);
    void normaliseByGrid();
};