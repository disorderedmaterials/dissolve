// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "data/formfactordata.h"

FormFactorData::FormFactorData(int Z, int formalCharge) : Z_(Z), formalCharge_(formalCharge) {}

/*
 * Element State
 */

// Return element to which the data relates
int FormFactorData::Z() const { return Z_; }

// Return formal charge of element to which the data relates
int FormFactorData::formalCharge() const { return formalCharge_; }

/*
 * Form Factor Generation
 */

// Return magnitude of form factor at specified Q value
double FormFactorData::magnitude(double Q) const { return 0.0; }
