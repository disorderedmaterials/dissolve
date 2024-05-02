// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/formFactors_dummy.h"
#include <algorithm>
#include <functional>
#include <vector>

FormFactorData_Dummy::FormFactorData_Dummy() : FormFactorData() {}

FormFactorData_Dummy &FormFactorData_Dummy::operator=(const FormFactorData_Dummy &source)
{
    formalCharge_ = source.formalCharge_;

    return (*this);
}

// Return magnitude of form factor at specified Q value
double FormFactorData_Dummy::magnitude(double Q) const { return 0.0; }
