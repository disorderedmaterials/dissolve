// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/typeDefs.h"

class Configuration;
class Data1D;

class DataNormaliser1D
{
    public:
    DataNormaliser1D(Data1D &targetData);

    /*
     * Target
     */
    private:
    // Target data to normalise
    Data1D &targetData_;

    /*
     * Normalisation functions
     */
    public:
    void normaliseByNumberDensity(double population, Configuration *targetConfiguration);
    void normaliseBySitePopulation(double population);
    void normaliseBySphericalShell();
};