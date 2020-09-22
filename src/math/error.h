// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"

// Forward Declarations
class Data1D;
class Data2D;

// Error Calculation
class Error
{
    /*
     * Error Type
     */
    public:
    // Error Types
    enum ErrorType
    {
        RMSEError,
        MAAPEError,
        MAPEError,
        PercentError,
        RFactorError,
        nErrorCalculationTypes
    };
    // Return enum options for ErrorType
    static EnumOptions<Error::ErrorType> errorTypes();

    /*
     * Data1D
     */
    public:
    // Return erorr of specified type between supplied data
    static double error(ErrorType errorType, const Data1D &A, const Data1D &B, bool quiet = false);
    // Return RMSE between supplied data
    static double rmse(const Data1D &A, const Data1D &B, bool quiet = false);
    // Return MAAPE between supplied data
    static double maape(const Data1D &A, const Data1D &B, bool quiet = false);
    // Return MAPE between supplied data
    static double mape(const Data1D &A, const Data1D &B, bool quiet = false);
    // Return percentage error between supplied data
    static double percent(const Data1D &A, const Data1D &B, bool quiet = false);
    // Return R-Factor (average squared error per point) between supplied data
    static double rFactor(const Data1D &A, const Data1D &B, bool quiet = false);
};
