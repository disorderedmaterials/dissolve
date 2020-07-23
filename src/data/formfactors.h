/*
    *** X-Ray Form Factors
    *** src/data/formfactors.h
    Copyright T. Youngs 2012-2020

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

#pragma once

#include "base/enumoptions.h"
#include "data/elements.h"
#include "data/formfactordata.h"
#include "templates/optionalref.h"
#include <tuple>

// X-Ray Form Factors
namespace XRayFormFactors
{
// X-Ray Form Factor Data Sets
enum XRayFormFactorData
{
    NoFormFactorData,
    WaasmaierKirfel1995, /* Waasmaier & Kirfel 1995 */
    nXRayFormFactorData
};

// Return EnumOptions for XRayFormFactorData
EnumOptions<XRayFormFactorData> xRayFormFactorData();

// Return form factor data from specified dataset for given element and formal charge (if it exists)
OptionalReferenceWrapper<const FormFactorData> formFactorData(XRayFormFactorData dataSet, int Z, int formalCharge = 0);

// Return form factor data from specified dataset for given element and formal charge (if it exists)
OptionalReferenceWrapper<const FormFactorData> formFactorData(XRayFormFactorData dataSet, Element *el, int formalCharge = 0);

// Return Waasmaier & Kirfel (1995) form factor data for given element and formal charge (if it exists)
OptionalReferenceWrapper<const FormFactorData> wk1995Data(int Z, int formalCharge = 0);
}; // namespace XRayFormFactors
