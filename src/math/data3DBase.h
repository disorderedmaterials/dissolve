// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "math/dataBase.h"
#include "templates/array3D.h"

// Data 3D Base
class Data3DBase : public DataBase
{
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
