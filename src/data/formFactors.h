// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "data/elements.h"
#include "data/formFactorData.h"
#include "templates/optionalRef.h"
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
OptionalReferenceWrapper<const FormFactorData> formFactorData(XRayFormFactorData dataSet, Elements::Element Z,
                                                              int formalCharge = 0);

// Return Waasmaier & Kirfel (1995) form factor data for given element and formal charge (if it exists)
OptionalReferenceWrapper<const FormFactorData> wk1995Data(Elements::Element Z, int formalCharge = 0);
}; // namespace XRayFormFactors
