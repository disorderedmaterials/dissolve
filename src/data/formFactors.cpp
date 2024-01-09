// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/formFactors.h"
#include "data/formFactors_dummy.h"
#include <functional>
#include <optional>

namespace XRayFormFactors
{

// Return EnumOptions for ConfigurationDisplayStyle
EnumOptions<XRayFormFactors::XRayFormFactorData> xRayFormFactorData()
{

    return EnumOptions<XRayFormFactors::XRayFormFactorData>(
        "XRayFormFactors", {{XRayFormFactors::NoFormFactorData, "None"}, {XRayFormFactors::WaasmaierKirfel1995, "WK1995"}});
}

// Return form factor data from specified dataset for given element and formal charge (if it exists)
OptionalReferenceWrapper<const FormFactorData> formFactorData(XRayFormFactorData dataSet, Elements::Element Z, int formalCharge)
{
    switch (dataSet)
    {
        case (XRayFormFactors::WaasmaierKirfel1995):
            return wk1995Data(Z, formalCharge);
        default:
            Messenger::error("Form factor data set type {} not recognised.\n", dataSet);
    }

    return {};
}

} // namespace XRayFormFactors
