// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

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

template <> struct std::formatter<XRayFormFactors::XRayFormFactorData> : std::formatter<std::string>
{
    auto format(XRayFormFactors::XRayFormFactorData x, format_context &ctx) const
    {
        // We need to cast the form factor data to an int, but the
        // value needs be in a variable so it can be called as a
        // reference.
        int refable = x;
        return formatter<string>::format(std::vformat("{}", std::make_format_args(refable)), ctx);
    }
};
