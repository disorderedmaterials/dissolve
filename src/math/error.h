/*
	*** Error Calculation
	*** src/math/error.h
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_ERROR_H
#define DISSOLVE_ERROR_H

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
	enum ErrorType { RMSEError, MAAPEError, MAPEError, PercentError, RFactorError, nErrorCalculationTypes };
	// Return enum options for ErrorType
	static EnumOptions<Error::ErrorType> errorTypes();


	/*
	 * Data1D
	 */
	public:
	// Return erorr of specified type between supplied data
	static double error(ErrorType errorType, const Data1D& A, const Data1D& B, bool quiet = false);
	// Return RMSE between supplied data
	static double rmse(const Data1D& A, const Data1D& B, bool quiet = false);
	// Return MAAPE between supplied data
	static double maape(const Data1D& A, const Data1D& B, bool quiet = false);
	// Return MAPE between supplied data
	static double mape(const Data1D& A, const Data1D& B, bool quiet = false);
	// Return percentage error between supplied data
	static double percent(const Data1D& A, const Data1D& B, bool quiet = false);
	// Return R-Factor (average squared error per point) between supplied data
	static double rFactor(const Data1D& A, const Data1D& B, bool quiet = false);
};

#endif
