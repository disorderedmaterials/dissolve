// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"

// Forward Declarations
class Data1D;
class Data2D;

// Error Calculation
namespace Error
{

// Error Types
enum ErrorType
{
    RMSEError,
    MAAPEError,
    MAPEError,
    PercentError,
    RFactorError,
    EuclideanError,
    nErrorCalculationTypes
};
// Return enum options for ErrorType
EnumOptions<Error::ErrorType> errorTypes();

/*
 * Data1D
 */

// Return error of specified type between supplied data
double error(ErrorType errorType, const Data1D &A, const Data1D &B, bool quiet = false);
// Return RMSE between supplied data
double rmse(const Data1D &A, const Data1D &B, bool quiet = false);
// Return MAAPE between supplied data
double maape(const Data1D &A, const Data1D &B, bool quiet = false);
// Return MAPE between supplied data
double mape(const Data1D &A, const Data1D &B, bool quiet = false);
// Return percentage error between supplied data
double percent(const Data1D &A, const Data1D &B, bool quiet = false);
// Return R-Factor (average squared error per point) between supplied data
double rFactor(const Data1D &A, const Data1D &B, bool quiet = false);
// Return Euclidean distance, normalised to mean of B, between supplied data
double euclidean(const Data1D &A, const Data1D &B, bool quiet = false);

}; // namespace Error
