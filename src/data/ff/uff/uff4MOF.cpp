// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/uff/uff4MOF.h"
#include "classes/atomType.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "data/elements.h"

/*
 * Implements "Extension of the Universal Force Field to Metal-Organic Frameworks"
 * M. A. Addicoat, N. Vankova, I. F. Akter, T. Heine
 * J. Chem. Theory Comput. 2014, 10, 2, 880–891
 *
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of TGAY.
 * Generator data 8 (THyb) are used to quickly determine the method of torsional parameter generation.
 * Torsional parameters U(i) are assigned to the second through sixth periods, following M. G. Martin's implementation in MCCCS
 * Towhee. Other modifications from the original paper are made following the MCCCS Towhee implementation. All tabulated
 * energetic values are in kcal, and are converted to kJ as and when required.
 */

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_UFF4MOF::setUp()
{
    // Create the basic UFF types
    createAtomTypes();

    // Create the UFF4MOF types
    addUFFAtomType(Elements::O, 202, "O_f_3", "geometry=tet, -Zn, nbonds=4", "Oxygen (tetrahedral) in framework materials",
                   0.634, 109.47, 3.5, 0.06, 14.085, 2.3, 8.741, 0.018, 2.0);
    addUFFAtomType(Elements::O, 203, "O_z_2", "geometry=tp, nbonds=3, -[Al, Sc, V, Mn, Fe, Cr]",
                   "Oxygen (trigonal planar) in SBU", 0.528, 120.0, 3.5, 0.06, 14.085, 2.3, 8.741, 0.0, 2.0);
    addUFFAtomType(Elements::Al, 204, "Al6+3", "geometry=oct, nbonds=6, -&203(n=4), -&223",
                   "Aluminium (octahedral, oxidation state = +3)", 1.22, 90.0, 4.499, 0.505, 11.278, 1.792, 11.278, 0.0, 1.25);
    addUFFAtomType(Elements::Sc, 205, "Sc6+3", "geometry=oct, nbonds=6, -&203(n=4), -&223",
                   "Scandium (octahedral, oxidation state = +3)", 1.44, 90.0, 3.295, 0.019, 12.0, 2.595, 3.395, 0.0, 0.7);
    addUFFAtomType(Elements::Ti, 206, "Ti4+2", "nbonds>=5, -&223(n=4), -Ti(root)", "Titanium (oct/sqbp, oxidation state = +2)",
                   1.38, 90.0, 3.175, 0.017, 12.0, 2.659, 3.47, 0.0, 0.7);
    addUFFAtomType(Elements::V, 207, "V_4+2", "nbonds>=5, -&223(n=4), -V(root)", "Vanadium (oct/sqbp, oxidation state = +2)",
                   1.18, 90.0, 3.144, 0.016, 12.0, 2.679, 3.65, 0.0, 0.7);
    addUFFAtomType(Elements::V, 208, "V_6+3", "geometry=oct, nbonds=6,  -&203(n=4), -&223",
                   "Vanadium (octahedral, oxidation state = +3)", 1.30, 90.0, 3.144, 0.016, 12.0, 2.679, 3.65, 0.0, 0.7);
    addUFFAtomType(Elements::Cr, 209, "Cr4+2", "nbonds>=5, -&223(n=4), -Cr(root)", "Chromium (oct/sqbp, oxidation state = +2)",
                   1.10, 90.0, 3.023, 0.015, 12.0, 2.463, 3.415, 0.0, 0.7);
    addUFFAtomType(Elements::Cr, 210, "Cr6f3", "geometry=oct, nbonds=6, -&203(n=4), -&223",
                   "Chromium (octahedral, oxidation state = +3)", 1.28, 90.0, 3.023, 0.015, 12.0, 2.463, 3.415, 0.0, 0.7);
    addUFFAtomType(Elements::Mn, 211, "Mn6+3", "geometry=oct, nbonds=6, -&203(n=4), -&223",
                   "Manganese (octahedral, oxidation state = +3)", 1.34, 90.0, 2.961, 0.013, 12.0, 2.43, 3.325, 0.0, 0.7);
    addUFFAtomType(Elements::Mn, 212, "Mn4+2", "nbonds>=5, -&223(n=4), -Mn(root)", "Manganese (oct/sqbp, oxidation state = +2)",
                   1.26, 90.0, 2.961, 0.013, 12.0, 2.43, 3.325, 0.0, 0.7);
    addUFFAtomType(Elements::Fe, 213, "Fe6+3", "geometry=oct, nbonds=6, -&203(n=4), -&223",
                   "Iron (octahedral, oxidation state = +3)", 1.32, 90.0, 2.912, 0.013, 12.0, 2.43, 3.76, 0.0, 0.7);
    addUFFAtomType(Elements::Fe, 214, "Fe4+2", "nbonds>=5, nbonds=6, -&223(n=4), -Fe(root)",
                   "Iron (oct/sqbp, oxidation state = +2)", 1.10, 90.0, 2.912, 0.013, 12.0, 2.43, 3.76, 0.0, 0.7);
    addUFFAtomType(Elements::Co, 215, "Co3+2", "geometry=tet, nbonds=4, -&13(n=3), -&35(n=1)",
                   "Cobalt (tetrahedral, oxidation state = +2)", 1.24, 109.47, 2.872, 0.014, 12.0, 1.308, 4.105, 0.0, 0.7);
    addUFFAtomType(Elements::Co, 216, "Co4+2", "nbonds>=5, -&223(n=4), -Ti(root)", "Cobalt (oct/sqbp, oxidation state = +3)",
                   1.16, 90.0, 2.872, 0.014, 12.0, 1.308, 4.105, 0.0, 0.7);
    addUFFAtomType(Elements::Co, 217, "Co4+2", "geometry=oct, nbonds=6, -&223(n=6)",
                   "Cobalt (octahedral, oxidation state = +3) in MFU-4 SBU", 1.16, 90.0, 2.872, 0.014, 12.0, 1.308, 4.105, 0.0,
                   0.7);
    addUFFAtomType(Elements::Cu, 218, "Cu4+2", "nbonds>=5, -&223(n=4), -Cu(root)", "Copper (oct/sqbp, oxidation state = +2)",
                   1.28, 90.0, 3.495, 0.005, 12.0, 2.43, 4.2, 0.0, 0.7);
    addUFFAtomType(Elements::Zn, 219, "Zn4+2", "nbonds>=5, -&223(n=4), -Zn(root)", "Zinc (oct/sqbp, oxidation state = +2)",
                   1.34, 90.0, 2.763, 0.124, 12.0, 1.308, 5.106, 0.0, 0.7);
    addUFFAtomType(Elements::Zn, 220, "Zn4+2", "geometry=oct, nbonds=6, -&223(n=6)",
                   "Zinc (octahedral, oxidation state = +2) in MFU-4 SBU", 1.34, 90.0, 2.763, 0.124, 12.0, 1.308, 5.106, 0.0,
                   0.7);
    addUFFAtomType(Elements::Zn, 221, "Zn3f2", "geometry=tet, nbonds=4, -&202, -&223",
                   "Zinc (tetrahedral, oxidation state = +2) in Zn4_O", 1.24, 109.47, 2.763, 0.124, 12.0, 1.308, 5.106, 0.0,
                   0.7);
    addUFFAtomType(Elements::Zn, 222, "Zn3f2", "geometry=tet, nbonds=4, -&13(n=3), -&35(n=1)",
                   "Zinc (tetrahedral, oxidation state = +2)  in MFU-4 SBU", 1.24, 109.47, 2.763, 0.124, 12.0, 1.308, 5.106,
                   0.0, 0.7);
    addUFFAtomType(Elements::O, 223, "O_m_2", "nbonds=2, -[Zn, Al, Sc, Ti, V, Cr, Mn, Fe, Co, Cu], -&9",
                   "Carboxylic Oxygen (trigonal) bonded to a metal within MOF SBU", 0.6340, 120.00, 3.5000, 0.0600, 14.085,
                   2.2998, 8.741, 0.0, 2.0);

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_UFF4MOF::name() const { return "UFF4MOF"; }

// Return description for Forcefield
std::string_view Forcefield_UFF4MOF::description() const
{
    return "Implements Extension of the Universal Force Field to Metal-Organic Frameworks "
           "M. A. Addicoat, N. Vankova, I. F. Akter, T. Heine"
           "<i>J. Chem. Theory Comput. </i> <b>114</b>, 880/891 (2014).<br/>Notes:<ul><li>Any inconsistencies between "
           "the forcefield as implemented here and the original work are the sole "
           "responsibility of TGAY</li><li>Generator data 8 (THyb) are used to quickly determine the method of torsional "
           "parameter generation.</li><li>Torsional parameters U(i) are assigned to "
           "the "
           "second through sixth periods, following M. G. Martin's implementation in MCCCS Towhee.</li><li>Other "
           "modifications from the original paper are made following the MCCCS Towhee "
           "implementation.</li></ul>";
}
