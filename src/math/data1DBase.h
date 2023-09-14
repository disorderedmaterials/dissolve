// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/import/data1D.h"
#include "math/dataBase.h"
#include <vector>

// Forward Declarations
class Data1DImportFileFormat;

// Data 1D Base
class Data1DBase : public DataBase
{
    /*
     * Type Definitions
     */
    public:
    using Formatter = class Data1DImportFileFormat;

    /*
     * Values / Errors
     */
    public:
    // Return values Array
    virtual const std::vector<double> &values() const = 0;
    // Return errors Array
    virtual const std::vector<double> &errors() const = 0;
};
