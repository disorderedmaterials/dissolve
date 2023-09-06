// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "math/dataBase.h"
#include <vector>

// Data 1D Base
class Data1DBase : public DataBase
{
    /*
     * Values / Errors
     */
    public:
    // Return values Array
    virtual const std::vector<double> &values() const = 0;
    // Return errors Array
    virtual const std::vector<double> &errors() const = 0;
};
