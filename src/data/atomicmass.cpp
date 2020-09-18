/*
    *** Atomic Mass Data
    *** src/data/atomicmass.cpp
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

#include "data/atomicmass.h"
#include "base/messenger.h"
#include <stddef.h>

/*
 * Atomic Mass Data
 */

AtomicMassData::AtomicMassData(int z, double massMin, double massMax) : ElementReference(z)
{
    if (massMax < 0.0)
        mass_ = massMin;
    else
        mass_ = (massMax + massMin) * 0.5;
}

// Return mass of element
double AtomicMassData::mass() const { return mass_; }

/*
 * Atomic Mass Helper Class
 */

// Return mass data for specified Z
const AtomicMassData &AtomicMass::massData(int Z)
{
    /*
     * Atomic Mass Data
     *
     * Source: CIAAW. Atomic weights of the elements 2017. Available online at www.ciaaw.org
     * (http://www.ciaaw.org/atomic-weights.htm)
     */

    static AtomicMassData massData[] = {
        {ELEMENT_XX, 0.0},
        {ELEMENT_H, 1.00798175}, // Uncertainty = (1), VSMOW
        {ELEMENT_HE, 4.002602},  // Uncertainty = (2)
        {ELEMENT_LI, 6.938, 6.997},
        {ELEMENT_BE, 9.0121831}, // Uncertainty = (5)
        {ELEMENT_B, 10.806, 10.821},
        {ELEMENT_C, 12.0096, 12.0116},
        {ELEMENT_N, 14.00643, 14.00728},
        {ELEMENT_O, 15.99903, 15.99977},
        {ELEMENT_F, 18.998403163}, // Uncertainty = (6)
        {ELEMENT_NE, 20.1797},     // Uncertainty = (6)
        {ELEMENT_NA, 22.98976928}, // Uncertainty = (2)
        {ELEMENT_MG, 24.304, 24.307},
        {ELEMENT_AL, 26.9815384}, // Uncertainty = (3)
        {ELEMENT_SI, 28.084, 28.086},
        {ELEMENT_P, 30.973761998}, // Uncertainty = (5)
        {ELEMENT_S, 32.059, 32.076},
        {ELEMENT_CL, 35.446, 35.457},
        {ELEMENT_AR, 39.792, 39.963},
        {ELEMENT_K, 39.0983},    // Uncertainty = (1)
        {ELEMENT_CA, 40.078},    // Uncertainty = (4)
        {ELEMENT_SC, 44.955908}, // Uncertainty = (5)
        {ELEMENT_TI, 47.867},    // Uncertainty = (1)
        {ELEMENT_V, 50.9415},    // Uncertainty = (1)
        {ELEMENT_CR, 51.9961},   // Uncertainty = (6)
        {ELEMENT_MN, 54.938043}, // Uncertainty = (2)
        {ELEMENT_FE, 55.845},    // Uncertainty = (2)
        {ELEMENT_CO, 58.933194}, // Uncertainty = (3)
        {ELEMENT_NI, 58.6934},   // Uncertainty = (4)
        {ELEMENT_CU, 63.546},    // Uncertainty = (3)
        {ELEMENT_ZN, 65.38},     // Uncertainty = (2)
        {ELEMENT_GA, 69.723},    // Uncertainty = (1)
        {ELEMENT_GE, 72.630},    // Uncertainty = (8)
        {ELEMENT_AS, 74.921595}, // Uncertainty = (6)
        {ELEMENT_SE, 78.971},    // Uncertainty = (8)
        {ELEMENT_BR, 79.901, 79.907},
        {ELEMENT_KR, 83.798},       // Uncertainty = (2)
        {ELEMENT_RB, 85.4678},      // Uncertainty = (3)
        {ELEMENT_SR, 87.62},        // Uncertainty = (1)
        {ELEMENT_Y, 88.90584},      // Uncertainty = (1)
        {ELEMENT_ZR, 91.224},       // Uncertainty = (2)
        {ELEMENT_NB, 92.90637},     // Uncertainty = (1)
        {ELEMENT_MO, 95.95},        // Uncertainty = (1)
        {ELEMENT_TC, 98.0},         // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_RU, 101.07},       // Uncertainty = (2)
        {ELEMENT_RH, 102.90549},    // Uncertainty = (2)
        {ELEMENT_PD, 106.42},       // Uncertainty = (1)
        {ELEMENT_AG, 107.8682},     // Uncertainty = (2)
        {ELEMENT_CD, 112.414},      // Uncertainty = (4)
        {ELEMENT_IN, 114.818},      // Uncertainty = (1)
        {ELEMENT_SN, 118.710},      // Uncertainty = (7)
        {ELEMENT_SB, 121.760},      // Uncertainty = (1)
        {ELEMENT_TE, 127.60},       // Uncertainty = (3)
        {ELEMENT_I, 126.90447},     // Uncertainty = (3)
        {ELEMENT_XE, 131.293},      // Uncertainty = (6)
        {ELEMENT_CS, 132.90545196}, // Uncertainty = (6)
        {ELEMENT_BA, 137.327},      // Uncertainty = (7)
        {ELEMENT_LA, 138.90547},    // Uncertainty = (7)
        {ELEMENT_CE, 140.116},      // Uncertainty = (1)
        {ELEMENT_PR, 140.90766},    // Uncertainty = (1)
        {ELEMENT_ND, 144.242},      // Uncertainty = (3)
        {ELEMENT_PM, 145.0},        // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_SM, 150.36},       // Uncertainty = (2)
        {ELEMENT_EU, 151.964},      // Uncertainty = (1)
        {ELEMENT_GD, 157.25},       // Uncertainty = (3)
        {ELEMENT_TB, 158.925354},   // Uncertainty = (8)
        {ELEMENT_DY, 162.500},      // Uncertainty = (1)
        {ELEMENT_HO, 164.930328},   // Uncertainty = (7)
        {ELEMENT_ER, 167.259},      // Uncertainty = (3)
        {ELEMENT_TM, 168.934218},   // Uncertainty = (6)
        {ELEMENT_YB, 173.045},      // Uncertainty = (10)
        {ELEMENT_LU, 174.9668},     // Uncertainty = (1)
        {ELEMENT_HF, 178.49},       // Uncertainty = (2)
        {ELEMENT_TA, 180.94788},    // Uncertainty = (2)
        {ELEMENT_W, 183.84},        // Uncertainty = (1)
        {ELEMENT_RE, 186.207},      // Uncertainty = (1)
        {ELEMENT_OS, 190.23},       // Uncertainty = (3)
        {ELEMENT_IR, 192.217},      // Uncertainty = (2)
        {ELEMENT_PT, 195.084},      // Uncertainty = (9)
        {ELEMENT_AU, 196.966570},   // Uncertainty = (4)
        {ELEMENT_HG, 200.592},      // Uncertainty = (3)
        {ELEMENT_TL, 204.382, 204.385},
        {ELEMENT_PB, 207.2},     // Uncertainty = (1)
        {ELEMENT_BI, 208.98040}, // Uncertainty = (1)
        {ELEMENT_PO, 209.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_AT, 210.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_RN, 222.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_FR, 223.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_RA, 226.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_AC, 227.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_TH, 232.0377},  // Uncertainty = (4)
        {ELEMENT_PA, 231.03588}, // Uncertainty = (1)
        {ELEMENT_U, 238.02891},  // Uncertainty = (3)
        {ELEMENT_NP, 237.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_PU, 244.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_AM, 243.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_CM, 247.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_BK, 247.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_CF, 251.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_ES, 252.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_FM, 257.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_MD, 258.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_NO, 259.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_LR, 262.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_RF, 267.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_DB, 268.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_SG, 269.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_BH, 270.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_HS, 269.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_MT, 278.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_DS, 281.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_RG, 280.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_CN, 285.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_NH, 286.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_FL, 289.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_MC, 289.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_LV, 293.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_TS, 294.0},     // Assumed mass (http://www.rsc.org/periodic-table)
        {ELEMENT_OG, 294.0}      // Assumed mass (http://www.rsc.org/periodic-table)
    };

    if ((Z < 0) || (Z > nElements()))
    {
        Messenger::error("AtomicMass::mass() - Element with Z={} is out of range!\n", Z);
        return massData[0];
    }

    return massData[Z];
}

// Return atomic mass for specified Element
double AtomicMass::mass(Element *element)
{
#ifdef CHECKS
    if (element == nullptr)
    {
        Messenger::error("AtomicMass::mass() - Element is NULL.\n");
        return 0.0;
    }
#endif

    return massData(element->Z()).mass();
}

// Return atomic mass for specified Z
double AtomicMass::mass(int Z) { return massData(Z).mass(); }

// Return reduced mass for specified pair of Z's
double AtomicMass::reducedMass(int Z1, int Z2)
{
    return (massData(Z1).mass() * massData(Z2).mass()) / (massData(Z1).mass() + massData(Z2).mass());
}

// Return reduced mass for specified pair of Elements
double AtomicMass::reducedMass(Element *e1, Element *e2)
{
#ifdef CHECKS
    if (e1 == nullptr)
    {
        Messenger::error("AtomicMass::reducedMass() - Element 1 is NULL.\n");
        return 1.0;
    }
    if (e2 == nullptr)
    {
        Messenger::error("AtomicMass::reducedMass() - Element 2 is NULL.\n");
        return 1.0;
    }
#endif
    return (massData(e1->Z()).mass() * massData(e2->Z()).mass()) / (massData(e1->Z()).mass() + massData(e2->Z()).mass());
}
