/*
    *** Waasmaier & Kirfel X-Ray Form Factors
    *** src/data/formfactors_wk1995.h
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
            Messenger::error("Form factor data set type {} not recognised.\n");
    }

    return {};
}

// Return form factor data from specified dataset for given element and formal charge (if it exists)
OptionalReferenceWrapper<const FormFactorData> formFactorData(XRayFormFactorData dataSet, Element *el, int formalCharge)
{
    return formFactorData(dataSet, el->Z(), formalCharge);
}

} // namespace XRayFormFactors
