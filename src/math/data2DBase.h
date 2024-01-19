// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/dataBase.h"
#include "templates/array2D.h"

// Forward declarations
class Data2DImportFileFormat;

// Data 2D Base
class Data2DBase : public DataBase
{
    /*
     * Type Definitions
     */
    public:
    using Formatter = Data2DImportFileFormat;

    /*
     * Axis Information
     */
    public:
    // Return y axis vector
    virtual const std::vector<double> &yAxis() const = 0;

    /*
     * Values / Errors
     */
    public:
    // Return values array
    virtual const Array2D<double> &values() const = 0;
    // Return errors array
    virtual const Array2D<double> &errors() const = 0;
};
