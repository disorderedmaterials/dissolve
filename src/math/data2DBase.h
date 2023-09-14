// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/import/data2D.h"
#include "math/dataBase.h"
#include "templates/array2D.h"

// Data 2D Base
class Data2DBase : public DataBase
{

    /*
     * Type Definitions
     */
    public:
    using Formatter = class Data2DImportFileFormat;

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
