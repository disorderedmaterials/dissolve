// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "math/range.h"
#include "templates/optionalRef.h"

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
    ASEError,
    RFactorError,
    EuclideanError,
    nErrorCalculationTypes
};
// Error information struct
struct ErrorReport
{
    ErrorType errorType;
    double error;
    double firstX;
    double lastX;
    int nPointsConsidered;
};
// Return enum options for ErrorType
EnumOptions<Error::ErrorType> errorTypes();
// Return error report formatted to string
std::string errorReportString(const ErrorReport &errorReport);

/*
 * Data1D
 */

// Return error of specified type between supplied data over all points or the range given
ErrorReport error(ErrorType errorType, const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range = std::nullopt);
// Return error of specified type between supplied double vectors
ErrorReport error(ErrorType errorType, const std::vector<double> &vecA, const std::vector<double> &vecB);
// Return RMSE between supplied data
ErrorReport rmse(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range = std::nullopt);
// Return MAAPE between supplied data
ErrorReport maape(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range = std::nullopt);
// Return MAPE between supplied data
ErrorReport mape(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range = std::nullopt);
// Return percentage error between supplied data
ErrorReport percent(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range = std::nullopt);
// Return absolute squared error between supplied data
ErrorReport ase(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range = std::nullopt);
// Return R-Factor (average squared error per point) between supplied data
ErrorReport rFactor(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range = std::nullopt);
// Return Euclidean distance, normalised to mean of B, between supplied data
ErrorReport euclidean(const Data1D &A, const Data1D &B, OptionalReferenceWrapper<Range> range = std::nullopt);

}; // namespace Error
