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

#ifndef DISSOLVE_DATA_FORMFACTORS_WK1995_H
#define DISSOLVE_DATA_FORMFACTORS_WK1995_H

#include "data/formfactordata.h"
#include <vector>

// Waasmaier & Kirfel '95 Form Factor Data
class FormFactorData_WK1995 : public FormFactorData
{
    public:
    FormFactorData_WK1995(int z = 0, int formalCharge = 0, std::vector<double> a = {}, std::vector<double> b = {},
                          double c = 0.0);
    FormFactorData_WK1995 &operator=(const FormFactorData_WK1995 &source);

    /*
     * Form Factor Data
     */
    private:
    // Exponential pre-factors
    std::vector<double> a_;
    // Exponential factors
    std::vector<double> b_;
    // Constant offset
    double c_;

    public:
    // Return index of isotope in it's Element parent's list
    int index() const;

    /*
     * Form Factor Generation
     */
    public:
    // Return magnitude of form factor at specified Q value
    double magnitude(double Q = 0.0) const;
};

#endif
