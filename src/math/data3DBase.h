// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/import/data3D.h"
#include "math/dataBase.h"
#include "templates/array3D.h"

// Forward declarations
class Data3DImportFileFormat;

// Data 3D Base
class Data3DBase : public DataBase
{
    /*
     * Type Definitions
     */
    public:
    using Formatter = class Data3DImportFileFormat;

    /*
     * Axis Information
     */
    public:
    // Return y axis vector
    virtual const std::vector<double> &yAxis() const = 0;
    // Return z axis vector
    virtual const std::vector<double> &zAxis() const = 0;

    /*
     * Values / Errors
     */
    public:
    // Return values array
    virtual const Array3D<double> &values() const = 0;
    // Return errors array
    virtual const Array3D<double> &errors() const = 0;
};
