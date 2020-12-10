// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "data/formfactors.h"
#include "data/formfactors_dummy.h"
#include <functional>
#include <optional>

namespace XRayFormFactors
{

// Return EnumOptions for ConfigurationDisplayStyle
EnumOptions<XRayFormFactors::XRayFormFactorData> xRayFormFactorData()
{
    static EnumOptionsList XRayFormFactorDataSets = EnumOptionsList()
                                                    << EnumOption(XRayFormFactors::NoFormFactorData, "None")
                                                    << EnumOption(XRayFormFactors::WaasmaierKirfel1995, "WK1995");

    static EnumOptions<XRayFormFactors::XRayFormFactorData> options("XRayFormFactors", XRayFormFactorDataSets);

    return options;
}

// Return form factor data from specified dataset for given element and formal charge (if it exists)
OptionalReferenceWrapper<const FormFactorData> formFactorData(XRayFormFactorData dataSet, int Z, int formalCharge)
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

// Return form factor data from specified dataset for given element and formal charge (if it exists)
OptionalReferenceWrapper<const FormFactorData> formFactorData(XRayFormFactorData dataSet, Element *el, int formalCharge)
{
    return formFactorData(dataSet, el->Z(), formalCharge);
}

} // namespace XRayFormFactors
