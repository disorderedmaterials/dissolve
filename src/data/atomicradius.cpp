/*
    *** Atomic Radius Data
    *** src/data/atomicradius.cpp
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

#include "data/atomicradius.h"
#include "base/messenger.h"
#include <stddef.h>

/*
 * Atomic Radius Data
 */

AtomicRadiusData::AtomicRadiusData(int z, double radius) : ElementReference(z) { radius_ = radius; }

// Return radius of element
double AtomicRadiusData::radius() const { return radius_; }

/*
 * Atomic Radius Helper Class
 */

// Return radius data for specified Z
const AtomicRadiusData &AtomicRadius::radiusData(int Z)
{
    /*
     * Atomic Radius Data
     *
     * Source: "Covalent radii revisited",
     *      B. Cordero, V. Gómez, A. E. Platero-Prats, M. Revés, J. Echeverría, E. Cremades, F. Barragán and S. Alvarez
     *      Dalton Trans., 2008 (DOI: https://dx.doi.org/10.1039/b801115j)
     *      Notes: High-Spin radii taken for 1st transition elements, sp3 value taken for Carbon. Bk and beyond set to 1.50.
     */

    static AtomicRadiusData radiusData[] = {
        {ELEMENT_XX, 0.00}, {ELEMENT_H, 0.31},  {ELEMENT_HE, 0.28}, {ELEMENT_LI, 1.28}, {ELEMENT_BE, 0.96}, {ELEMENT_B, 0.84},
        {ELEMENT_C, 0.76},  {ELEMENT_N, 0.71},  {ELEMENT_O, 0.66},  {ELEMENT_F, 0.57},  {ELEMENT_NE, 0.58}, {ELEMENT_NA, 1.66},
        {ELEMENT_MG, 1.41}, {ELEMENT_AL, 1.21}, {ELEMENT_SI, 1.11}, {ELEMENT_P, 1.07},  {ELEMENT_S, 1.05},  {ELEMENT_CL, 1.02},
        {ELEMENT_AR, 1.06}, {ELEMENT_K, 2.03},  {ELEMENT_CA, 1.76}, {ELEMENT_SC, 1.70}, {ELEMENT_TI, 1.60}, {ELEMENT_V, 1.53},
        {ELEMENT_CR, 1.39}, {ELEMENT_MN, 1.61}, {ELEMENT_FE, 1.52}, {ELEMENT_CO, 1.50}, {ELEMENT_NI, 1.24}, {ELEMENT_CU, 1.32},
        {ELEMENT_ZN, 1.22}, {ELEMENT_GA, 1.22}, {ELEMENT_GE, 1.20}, {ELEMENT_AS, 1.19}, {ELEMENT_SE, 1.20}, {ELEMENT_BR, 1.20},
        {ELEMENT_KR, 1.16}, {ELEMENT_RB, 2.20}, {ELEMENT_SR, 1.95}, {ELEMENT_Y, 1.90},  {ELEMENT_ZR, 1.75}, {ELEMENT_NB, 1.64},
        {ELEMENT_MO, 1.54}, {ELEMENT_TC, 1.47}, {ELEMENT_RU, 1.46}, {ELEMENT_RH, 1.42}, {ELEMENT_PD, 1.39}, {ELEMENT_AG, 1.45},
        {ELEMENT_CD, 1.44}, {ELEMENT_IN, 1.42}, {ELEMENT_SN, 1.39}, {ELEMENT_SB, 1.39}, {ELEMENT_TE, 1.38}, {ELEMENT_I, 1.39},
        {ELEMENT_XE, 1.40}, {ELEMENT_CS, 2.44}, {ELEMENT_BA, 2.15}, {ELEMENT_LA, 2.07}, {ELEMENT_CE, 2.04}, {ELEMENT_ND, 2.01},
        {ELEMENT_PM, 1.99}, {ELEMENT_SM, 1.98}, {ELEMENT_EU, 1.98}, {ELEMENT_GD, 1.96}, {ELEMENT_TB, 1.94}, {ELEMENT_DY, 1.92},
        {ELEMENT_HO, 1.92}, {ELEMENT_ER, 1.89}, {ELEMENT_TM, 1.90}, {ELEMENT_YB, 1.87}, {ELEMENT_LU, 1.87}, {ELEMENT_HF, 1.75},
        {ELEMENT_TA, 1.70}, {ELEMENT_W, 1.62},  {ELEMENT_RE, 1.51}, {ELEMENT_OS, 1.44}, {ELEMENT_IR, 1.41}, {ELEMENT_PT, 1.36},
        {ELEMENT_AU, 1.36}, {ELEMENT_HG, 1.32}, {ELEMENT_TL, 1.45}, {ELEMENT_PB, 1.46}, {ELEMENT_BI, 1.48}, {ELEMENT_PO, 1.40},
        {ELEMENT_AT, 1.50}, {ELEMENT_RN, 1.50}, {ELEMENT_FR, 2.60}, {ELEMENT_RA, 2.21}, {ELEMENT_AC, 2.15}, {ELEMENT_TH, 2.06},
        {ELEMENT_U, 1.96},  {ELEMENT_NP, 1.90}, {ELEMENT_PU, 1.87}, {ELEMENT_AM, 1.80}, {ELEMENT_CM, 1.69}, {ELEMENT_BK, 1.50},
        {ELEMENT_CF, 1.50}, {ELEMENT_ES, 1.50}, {ELEMENT_FM, 1.50}, {ELEMENT_MD, 1.50}, {ELEMENT_NO, 1.50}, {ELEMENT_LR, 1.50},
        {ELEMENT_DB, 1.50}, {ELEMENT_SG, 1.50}, {ELEMENT_BH, 1.50}, {ELEMENT_HS, 1.50}, {ELEMENT_MT, 1.50}, {ELEMENT_RG, 1.50},
        {ELEMENT_CN, 1.50}, {ELEMENT_NH, 1.50}, {ELEMENT_FL, 1.50}, {ELEMENT_MC, 1.50}, {ELEMENT_LV, 1.50}, {ELEMENT_TS, 1.50},
        {ELEMENT_OG, 1.50}};

    if ((Z < 0) || (Z > nElements()))
    {
        Messenger::error("AtomicRadius::radius() - Element with Z={} is out of range!\n", Z);
        return radiusData[0];
    }

    return radiusData[Z];
}

// Return atomic radius for specified Element
double AtomicRadius::radius(Element *element)
{
#ifdef CHECKS
    if (element == nullptr)
    {
        Messenger::error("AtomicRadius::radius() - Element is NULL.\n");
        return 0.0;
    }
#endif

    return radiusData(element->Z()).radius();
}

// Return atomic radius for specified Z
double AtomicRadius::radius(int Z) { return radiusData(Z).radius(); }
