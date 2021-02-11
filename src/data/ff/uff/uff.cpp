// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/uff/uff.h"
#include "classes/atomtype.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "data/elements.h"

/*
 * Implements "UFF, a Full Periodic Table Force Field for Molecular Mechanics and Molecular Dynamics Simulations"
 * A. K. Rappe, C. J. Casewit, K. S. Colwell, W. A. Goddard III, and W. M. Skiff
 * J. Am. Chem. Soc. 114, 10024-10039 (1992)
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
bool Forcefield_UFF::setUp()
{
    uffAtomTypes_ = {
        // 	El		FFID	Name		NETA	Description
        // 					  r	theta     x       D     zeta      Z      chi	geom	V
        // U
        {Elements::H, 1, "H_", "", "Generic hydrogen", 0.3540, 180.00, 2.8860, 0.0440, 12.000, 0.7120, 4.528, 0, 0.0, 0.0},
        {Elements::H, 2, "H_b", "-B(n=2)", "Bridging hydrogen in B-H-B", 0.4600, 83.50, 2.8860, 0.0440, 12.000, 0.7125, 4.528,
         8, 0.0, 0.0},
        {Elements::He, 3, "He4+4", "", "Helium", 0.8490, 90.00, 2.3620, 0.0560, 15.240, 0.0972, 9.66, 4, 0.0, 0.0},
        {Elements::Li, 4, "Li", "", "Lithium", 1.3360, 180.00, 2.4510, 0.0250, 12.000, 1.0255, 3.006, 0, 0.0, 2.0},
        {Elements::Be, 5, "Be3+2", "", "Beryllium", 1.0740, 109.47, 2.7450, 0.0850, 12.000, 1.5650, 4.877, 3, 0.0, 2.0},
        {Elements::B, 6, "B_3", "nbonds=4", "Boron (tetrahedral)", 0.8380, 109.47, 4.0830, 0.1800, 12.052, 1.7550, 5.11, 3, 0.0,
         2.0},
        {Elements::B, 7, "B_2", "nbonds=3", "Boron (trigonal)", 0.8280, 120.00, 4.0830, 0.1800, 12.052, 1.7550, 5.11, 2, 0.0,
         2.0},
        {Elements::C, 8, "C_3", "nbonds=4", "Carbon (tetrahedral)", 0.7570, 109.47, 3.8510, 0.1050, 12.730, 1.9120, 5.343, 3,
         2.119, 2.0},
        {Elements::C, 9, "C_R", "nbonds=3,ring()", "Carbon (resonant)", 0.7290, 120.00, 3.8510, 0.1050, 12.730, 1.9120, 5.343,
         9, 0.0, 2.0},
        {Elements::C, 10, "C_2", "nbonds=3", "Carbon (trigonal)", 0.7320, 120.00, 3.8510, 0.1050, 12.730, 1.9120, 5.343, 2, 0.0,
         2.0},
        {Elements::C, 11, "C_1", "nbonds=2", "Carbon (linear)", 0.7060, 180.00, 3.8510, 0.1050, 12.730, 1.9120, 5.343, 1, 0.0,
         2.0},
        {Elements::N, 12, "N_3", "", "Nitrogen (tetrahedral)", 0.7000, 106.70, 3.6600, 0.0690, 13.407, 2.5438, 6.899, 3, 0.450,
         0.0},
        {Elements::N, 13, "N_R", "nbonds=2,ring()", "Nitrogen (resonant)", 0.6990, 120.00, 3.6600, 0.0690, 13.407, 2.5438,
         6.899, 9, 0.0, 2.0},
        {Elements::N, 14, "N_2", "nbonds=2", "Nitrogen (trigonal)", 0.6850, 111.30, 3.6600, 0.0690, 13.407, 2.5438, 6.899, 2,
         0.0, 2.0},
        {Elements::N, 15, "N_1", "nbonds=1", "Nitrogen (linear)", 0.6560, 180.00, 3.6600, 0.0690, 13.407, 2.5438, 6.899, 1, 0.0,
         2.0},
        {Elements::O, 16, "O_3", "", "Oxygen (tetrahedral)", 0.6580, 104.51, 3.5000, 0.0600, 14.085, 2.2998, 8.741, 3, 0.018,
         2.0},
        {Elements::O, 17, "O_3_z", "", "Oxygen (in silicate)", 0.5280, 145.50, 3.5000, 0.0600, 14.085, 2.2998, 8.741, 3, 0.018,
         2.0},
        {Elements::O, 18, "O_R", "", "Oxygen (resonant)", 0.6800, 110.30, 3.5000, 0.0600, 14.085, 2.2998, 8.741, 9, 0.0, 2.0},
        {Elements::O, 19, "O_2", "", "Oxygen (trigonal)", 0.6340, 120.00, 3.5000, 0.0600, 14.085, 2.2998, 8.741, 2, 0.0, 2.0},
        {Elements::O, 20, "O_1", "", "Oxygen (linear)", 0.6390, 180.00, 3.5000, 0.0600, 14.085, 2.2998, 8.741, 1, 0.0, 2.0},
        {Elements::F, 21, "F_", "", "Fluorine", 0.6680, 180.00, 3.3640, 0.0500, 14.762, 1.7350, 10.874, 0, 0.0, 2.0},
        {Elements::Ne, 22, "Ne4+4", "", "Neon", 0.9200, 90.00, 3.2430, 0.0420, 15.440, 0.1944, 11.04, 4, 0.0, 2.0},
        {Elements::Na, 23, "Na", "", "Sodium", 1.5390, 180.00, 2.9830, 0.0300, 12.000, 1.0809, 2.843, 0, 0.0, 1.25},
        {Elements::Mg, 24, "Mg3+2", "", "Magnesium", 1.4210, 109.47, 3.0210, 0.1110, 12.000, 1.7866, 3.951, 3, 0.0, 1.25},
        {Elements::Al, 25, "Al3", "", "Aluminium", 1.2440, 109.47, 4.4990, 0.5050, 11.278, 1.7924, 4.06, 3, 0, 1.25},
        {Elements::Si, 26, "Si3", "", "Silicon", 1.1170, 109.47, 4.2950, 0.4020, 12.175, 2.3232, 4.168, 3, 1.225, 1.25},
        {Elements::P, 27, "P_3+3", "", "Phosphorus (tetrahedral, oxidation state +3)", 1.1010, 93.80, 4.1470, 0.3050, 13.072,
         2.8627, 5.463, 3, 2.4, 1.25},
        {Elements::P, 28, "P_3+5", "", "Phosphorus (tetrahedral, oxidation state +5)", 1.0560, 109.47, 4.1470, 0.3050, 13.072,
         2.8627, 5.463, 3, 2.4, 1.25},
        {Elements::P, 29, "P_3+q", "", "XXX", 1.0560, 109.47, 4.1470, 0.3050, 13.072, 2.8627, 5.463, 3, 2.4, 1.25},
        {Elements::S, 30, "S_3+2", "", "Sulfur (tetrahedral, oxidation state +2)", 1.0640, 92.10, 4.0350, 0.2740, 13.969,
         2.7032, 6.928, 3, 0.484, 1.25},
        {Elements::S, 31, "S_3+4", "", "Sulfur (tetrahedral, oxidation state +4)", 1.0490, 103.20, 4.0350, 0.2740, 13.969,
         2.7032, 6.928, 3, 0.484, 1.25},
        {Elements::S, 32, "S_3+6", "", "Sulfur (tetrahedral, oxidation state +6)", 1.0270, 109.47, 4.0350, 0.2740, 13.969,
         2.7032, 6.928, 3, 0.484, 1.25},
        {Elements::S, 33, "S_R", "", "Sulfur (resonant)", 1.0770, 92.20, 4.0350, 0.2740, 13.969, 2.7032, 6.928, 9, 0.0, 1.25},
        {Elements::S, 34, "S_2", "", "Sulfur (trigonal)", 0.8540, 120.00, 4.0350, 0.2740, 13.969, 2.7032, 6.928, 2, 0.0, 1.25},
        {Elements::Cl, 35, "Cl", "", "Chlorine", 1.0440, 180.00, 3.9470, 0.2270, 14.886, 2.3484, 8.564, 0, 0.0, 1.25},
        {Elements::Ar, 36, "Ar4+4", "", "Argon", 1.0320, 90.00, 3.8680, 0.1850, 15.763, 0.2994, 9.465, 4, 0.0, 1.25},
        {Elements::K, 37, "K_", "", "Krypton", 1.9530, 180.00, 3.8120, 0.0350, 12.000, 1.1645, 2.421, 0, 0.0, 0.7},
        {Elements::Ca, 38, "Ca6+2", "", "Calcium (octahedral, oxidation state +2)", 1.7610, 90.00, 3.3990, 0.2380, 12.000,
         2.1414, 3.231, 6, 0.0, 0.7},
        {Elements::Sc, 39, "Sc3+3", "", "Scandium (tetrahedral, oxidation state +3)", 1.5130, 109.47, 3.2950, 0.0190, 12.000,
         2.5924, 3.395, 3, 0.0, 0.7},
        {Elements::Ti, 40, "Ti3+4", "", "Titanium (tetrahedral, oxidation state +4)", 1.4120, 109.47, 3.1750, 0.0170, 12.000,
         2.6595, 3.47, 3, 0.0, 0.7},
        {Elements::Ti, 41, "Ti6+4", "", "Titanium (octahedral, oxidation state +4)", 1.4120, 90.00, 3.1750, 0.0170, 12.000,
         2.6595, 3.47, 6, 0.0, 0.7},
        {Elements::V, 42, "V_3+5", "", "Vanadium", 1.4020, 109.47, 3.1440, 0.0160, 12.000, 2.6789, 3.65, 3, 0.0, 0.7},
        {Elements::Cr, 43, "Cr6+3", "", "Chromium", 1.3450, 90.00, 3.0230, 0.0150, 12.000, 2.4631, 3.415, 6, 0.0, 0.7},
        {Elements::Mn, 44, "Mn6+2", "", "Manganese", 1.3820, 90.00, 2.9610, 0.0130, 12.000, 2.4301, 3.325, 6, 0.0, 0.7},
        {Elements::Fe, 45, "Fe3+2", "", "Iron (tetrahedral, oxidation state +2)", 1.4120, 109.47, 2.9120, 0.0130, 12.000,
         2.4301, 3.76, 3, 0.0, 0.7},
        {Elements::Fe, 46, "Fe6+2", "", "Iron (octahedral, oxidation state +2)", 1.3350, 90.00, 2.9120, 0.0130, 12.000, 2.4301,
         3.76, 6, 0.0, 0.7},
        {Elements::Co, 47, "Co6+3", "", "Cobalt (octahedral, oxidation state +3)", 1.2410, 90.00, 2.8720, 0.0140, 12.000,
         2.4301, 4.105, 6, 0.0, 0.7},
        {Elements::Ni, 48, "Ni4+2", "", "Nickel", 1.1640, 90.00, 2.8340, 0.0150, 12.000, 2.4301, 4.465, 4, 0.0, 0.7},
        {Elements::Cu, 49, "Cu3+1", "", "Copper", 1.3020, 109.47, 3.4950, 0.0050, 12.000, 1.7565, 4.2, 3, 0.0, 0.7},
        {Elements::Zn, 50, "Zn3+2", "", "Zinc", 1.1930, 109.47, 2.7630, 0.1240, 12.000, 1.3084, 5.106, 3, 0.0, 0.7},
        {Elements::Ga, 51, "Ga3+3", "", "Gallium (tetrahedral, oxidation state +3)", 1.2600, 109.47, 4.3830, 0.4150, 11.000,
         1.8206, 3.641, 3, 0.0, 0.7},
        {Elements::Ge, 52, "Ge3", "", "Germanium", 1.1970, 109.47, 4.2800, 0.3790, 12.000, 2.7888, 4.051, 3, 0.701, 0.0},
        {Elements::As, 53, "As3+3", "", "Astatine (tetrahedral, oxidation state +3)", 1.2110, 92.10, 4.2300, 0.3090, 13.000,
         2.8640, 5.188, 3, 1.5, 0.0},
        {Elements::Se, 54, "Se3+2", "", "Selenium", 1.1900, 90.60, 4.2050, 0.2910, 14.000, 2.7645, 6.428, 3, 0.335, 0.0},
        {Elements::Br, 55, "Br", "", "Bromine", 1.1920, 180.00, 4.1890, 0.2510, 15.000, 2.5186, 7.79, 0, 0.0, 0.7},
        {Elements::Kr, 56, "Kr4+4", "", "Krypton", 1.1470, 90.00, 4.1410, 0.2200, 16.000, 0.4520, 8.505, 4, 0.0, 0.7},
        {Elements::Rb, 57, "Rb", "", "Rubidium", 2.2600, 180.00, 4.1140, 0.0400, 12.000, 1.5922, 2.331, 0, 0.0, 0.2},
        {Elements::Sr, 58, "Sr6+2", "", "Strontium", 2.0520, 90.00, 3.6410, 0.2350, 12.000, 2.4486, 3.024, 6, 0.0, 0.2},
        {Elements::Y, 59, "Y_3+3", "", "Yttrium (tetrahedral, oxidation state +3)", 1.6980, 109.47, 3.3450, 0.0720, 12.000,
         3.2573, 3.83, 3, 0.0, 0.2},
        {Elements::Zr, 60, "Zr3+4", "", "Zirconium", 1.5640, 109.47, 3.1240, 0.0690, 12.000, 3.6675, 3.4, 3, 0.0, 0.2},
        {Elements::Nb, 61, "Nb3+5", "", "Niobium", 1.4730, 109.47, 3.1650, 0.0590, 12.000, 3.6179, 3.55, 3, 0.0, 0.2},
        {Elements::Mo, 62, "Mo6+6", "", "Molybdenum (octahedral)", 1.4670, 90.00, 3.0520, 0.0560, 12.000, 3.4021, 3.465, 6, 0.0,
         0.2},
        {Elements::Mo, 63, "Mo3+6", "", "Molybdenum (tetrahedral)", 1.4840, 109.47, 3.0520, 0.0560, 12.000, 3.4021, 3.465, 3,
         0.0, 0.2},
        {Elements::Tc, 64, "Tc6+5", "", "Technecium", 1.3220, 90.00, 2.9980, 0.0480, 12.000, 3.4021, 3.29, 6, 0.0, 0.2},
        {Elements::Ru, 65, "Ru6+2", "", "Rubidium", 1.4780, 90.00, 2.9630, 0.0560, 12.000, 3.4021, 3.575, 6, 0.0, 0.2},
        {Elements::Rh, 66, "Rh6+3", "", "Rhodium (octahedral, oxidation state +3)", 1.3320, 90.00, 2.9290, 0.0530, 12.000,
         3.5081, 3.975, 6, 0.0, 0.2},
        {Elements::Pd, 67, "Pd4+2", "", "Palladium", 1.3380, 90.00, 2.8990, 0.0480, 12.000, 3.2077, 4.32, 4, 0.0, 0.2},
        {Elements::Ag, 68, "Ag1+1", "", "Silver (linear, oxidation state +1)", 1.3860, 180.00, 3.1480, 0.0360, 12.000, 1.9557,
         4.436, 1, 0.0, 0.2},
        {Elements::Cd, 69, "Cd3+2", "", "Cadmium", 1.4030, 109.47, 2.8480, 0.2280, 12.000, 1.6525, 5.034, 3, 0.0, 0.2},
        {Elements::In, 70, "In3+3", "", "Indium (tetrahedral, oxidation state +3)", 1.4590, 109.47, 4.4630, 0.5990, 11.000,
         2.0704, 3.506, 3, 0.0, 0.2},
        {Elements::Sn, 71, "Sn3", "", "Tin", 1.3980, 109.47, 4.3920, 0.5670, 12.000, 2.9608, 3.987, 3, 0.199, 0.2},
        {Elements::Sb, 72, "Sb3+3", "", "Antimony (tetrahedral, oxidation state +3)", 1.4070, 91.60, 4.4200, 0.4490, 13.000,
         2.7042, 4.899, 3, 1.1, 0.2},
        {Elements::Te, 73, "Te3+2", "", "Tellurium", 1.3860, 90.25, 4.4700, 0.3980, 14.000, 2.8821, 5.816, 3, 0.3, 0.2},
        {Elements::I, 74, "I_", "", "Iodine", 1.3820, 180.00, 4.5000, 0.3390, 15.000, 2.6537, 6.822, 0, 0, 0.2},
        {Elements::Xe, 75, "Xe4+4", "", "Xenon", 1.2670, 90.00, 4.4040, 0.3320, 12.000, 0.5560, 7.595, 4, 0, 0.2},
        {Elements::Cs, 76, "Cs", "", "Caesium", 2.5700, 180.00, 4.5170, 0.0450, 12.000, 1.5728, 2.183, 0, 0.0, 0.1},
        {Elements::Ba, 77, "Ba6+2", "", "Barium", 2.2770, 90.00, 3.7030, 0.3640, 12.000, 2.7266, 2.814, 6, 0.0, 0.1},
        {Elements::La, 78, "La3+3", "", "Lanthanum (tetrahedral, oxidation state +3)", 1.9430, 109.47, 3.5220, 0.0170, 12.000,
         3.3049, 2.8355, 3, 0.0, 0.1},
        {Elements::Ce, 79, "Ce6+3", "", "Cerium (octahedral, oxidation state +3)", 1.8410, 90.00, 3.5560, 0.0130, 12.000,
         3.3049, 2.774, 6, 0.0, 0.1},
        {Elements::Pr, 80, "Pr6+3", "", "Praseodymium (octahedral, oxidation state +3)", 1.8230, 90.00, 3.6060, 0.0100, 12.000,
         3.3049, 2.858, 6, 0.0, 0.1},
        {Elements::Nd, 81, "Nd6+3", "", "Neodymium (octahedral, oxidation state +3)", 1.8160, 90.00, 3.5750, 0.0100, 12.000,
         3.3049, 2.8685, 6, 0.0, 0.1},
        {Elements::Pm, 82, "Pm6+3", "", "Prometheum (octahedral, oxidation state +3)", 1.8010, 90.00, 3.5470, 0.0090, 12.000,
         3.3049, 2.881, 6, 0.0, 0.1},
        {Elements::Sm, 83, "Sm6+3", "", "Samarium (octahedral, oxidation state +3)", 1.7800, 90.00, 3.5200, 0.0080, 12.000,
         3.3049, 2.9115, 6, 0.0, 0.1},
        {Elements::Eu, 84, "Eu6+3", "", "Europium (octahedral, oxidation state +3)", 1.7710, 90.00, 3.4930, 0.0080, 12.000,
         3.3049, 2.8785, 6, 0.0, 0.1},
        {Elements::Gd, 85, "Gd6+3", "", "Gadolinium (octahedral, oxidation state +3)", 1.7350, 90.00, 3.3680, 0.0090, 12.000,
         3.3049, 3.1665, 6, 0.0, 0.1},
        {Elements::Tb, 86, "Tb6+3", "", "Terbium (octahedral, oxidation state +3)", 1.7320, 90.00, 3.4510, 0.0070, 12.000,
         3.3049, 3.018, 6, 0.0, 0.1},
        {Elements::Dy, 87, "Dy6+3", "", "Dysprosium (octahedral, oxidation state +3)", 1.7100, 90.00, 3.4280, 0.0070, 12.000,
         3.3049, 3.0555, 6, 0.0, 0.1},
        {Elements::Ho, 88, "Ho6+3", "", "Holmium (octahedral, oxidation state +3)", 1.6960, 90.00, 3.4090, 0.0070, 12.000,
         3.4157, 3.127, 6, 0.0, 0.1},
        {Elements::Er, 89, "Er6+3", "", "Erbium (octahedral, oxidation state +3)", 1.6730, 90.00, 3.3910, 0.0070, 12.000,
         3.3049, 3.1865, 6, 0.0, 0.1},
        {Elements::Tm, 90, "Tm6+3", "", "Thulium (octahedral, oxidation state +3)", 1.6600, 90.00, 3.3740, 0.0060, 12.000,
         3.3049, 3.2514, 6, 0.0, 0.1},
        {Elements::Yb, 91, "Yb6+3", "", "Ytterbium (octahedral, oxidation state +3)", 1.6370, 90.00, 3.3550, 0.2280, 12.000,
         2.6177, 3.2889, 6, 0.0, 0.1},
        {Elements::Lu, 92, "Lu6+3", "", "Lutetium (octahedral, oxidation state +3)", 1.6710, 90.00, 3.6400, 0.0410, 12.000,
         3.2709, 2.9629, 6, 0.0, 0.1},
        {Elements::Hf, 93, "Hf3+4", "", "Hafnium (tetrahedral, oxidation state +4)", 1.6110, 109.47, 3.1410, 0.0720, 12.000,
         3.9212, 3.7, 3, 0.0, 0.1},
        {Elements::Ta, 94, "Ta3+5", "", "Tantalum (tetrahedral, oxidation state +5)", 1.5110, 109.47, 3.1700, 0.0810, 12.000,
         4.0748, 5.1, 3, 0.0, 0.1},
        {Elements::W, 95, "W_6+6", "", "Tungsten (octahedral, oxidation state +6)", 1.3920, 90.00, 3.0690, 0.0670, 12.000,
         3.6937, 4.63, 6, 0.0, 0.1},
        {Elements::W, 96, "W_3+4", "", "Tungsten (tetrahedral, oxidation state +4)", 1.5260, 109.47, 3.0690, 0.0670, 12.000,
         3.6937, 4.63, 3, 0.0, 0.1},
        {Elements::W, 97, "W_3+6", "", "Tungsten (tetrahedral, oxidation state +6)", 1.3800, 109.47, 3.0690, 0.0670, 12.000,
         3.6937, 4.63, 3, 0.0, 0.1},
        {Elements::Re, 98, "Re6+5", "", "Rhenium (octahedral, oxidation state +5)", 1.3720, 90.00, 2.9540, 0.0660, 12.000,
         3.6937, 3.96, 6, 0.0, 0.1},
        {Elements::Re, 99, "Re3+7", "", "Rhenium (tetrahedral, oxidation state +7)", 1.3140, 109.47, 2.9540, 0.0660, 12.000,
         3.6937, 3.96, 3, 0.0, 0.1},
        {Elements::Os, 100, "Os6+6", "", "Osmium (octahedral, oxidation state +6)", 1.3720, 90.00, 3.1200, 0.0370, 12.000,
         3.6937, 5.14, 6, 0.0, 0.1},
        {Elements::Ir, 101, "Ir6+3", "", "Iridium (octahedral, oxidation state +3)", 1.3710, 90.00, 2.8400, 0.0730, 12.000,
         3.7307, 5.0, 6, 0.0, 0.1},
        {Elements::Pt, 102, "Pt4+2", "", "Platinum", 1.3640, 90.00, 2.7540, 0.0800, 12.000, 3.3817, 4.79, 4, 0.0, 0.1},
        {Elements::Au, 103, "Au4+3", "", "Gold", 1.2620, 90.00, 3.2930, 0.0390, 12.000, 2.6255, 4.894, 4, 0.0, 0.1},
        {Elements::Hg, 104, "Hg1+2", "", "Mercury", 1.3400, 180.00, 2.7050, 0.3850, 12.000, 1.7497, 6.27, 1, 0.0, 0.1},
        {Elements::Tl, 105, "Tl3+3", "", "Thallium (tetrahedral, oxidation state +3)", 1.5180, 120.00, 4.3470, 0.6800, 11.000,
         2.0685, 3.2, 3, 0.0, 0.1},
        {Elements::Pb, 106, "Pb3", "", "Lead", 1.4590, 109.47, 4.2970, 0.6630, 12.000, 2.8461, 3.9, 3, 0.1, 0.1},
        {Elements::Bi, 107, "Bi3+3", "", "Bismuth (tetrahedral, oxidation state +3)", 1.5120, 90.00, 4.3700, 0.5180, 13.000,
         2.4700, 4.69, 3, 1.0, 0.1},
        {Elements::Po, 108, "Po3+2", "", "Polonium (tetrahedral, oxidation state +2)", 1.5000, 90.00, 4.7090, 0.3250, 14.000,
         2.3329, 4.21, 3, 0.3, 0.1},
        {Elements::At, 109, "At", "", "Astatine", 1.5450, 180.00, 4.7500, 0.2840, 15.000, 2.2357, 4.75, 0, 0.0, 0.1},
        {Elements::Rn, 110, "Rn4+4", "", "Radon", 1.4200, 90.00, 4.7650, 0.2480, 16.000, 0.5832, 5.37, 4, 0.0, 0.1},
        {Elements::Fr, 111, "Fr", "", "Francium", 2.8800, 180.00, 4.9000, 0.0500, 12.000, 1.8469, 2.0, 0, 0.0, 0.0},
        {Elements::Ra, 112, "Ra6+2", "", "Radium (octahedral, oxidation state +2)", 2.5120, 90.00, 3.6770, 0.4040, 12.000,
         2.9161, 2.843, 6, 0.0, 0.0},
        {Elements::Ac, 113, "Ac6+3", "", "Actinium (octahedral, oxidation state +3)", 1.9830, 90.00, 3.4780, 0.0330, 12.000,
         3.8882, 2.835, 6, 0.0, 0.0},
        {Elements::Th, 114, "Th6+4", "", "Thorium (octahedral, oxidation state +4)", 1.7210, 90.00, 3.3960, 0.0260, 12.000,
         4.2021, 3.175, 6, 0.0, 0.0},
        {Elements::Pa, 115, "Pa6+4", "", "Protactinium (octahedral, oxidation state +4)", 1.7110, 90.00, 3.4240, 0.0220, 12.000,
         3.8882, 2.985, 6, 0.0, 0.0},
        {Elements::U, 116, "U_6+4", "", "Uranium (octahedral, oxidation state +4)", 1.6840, 90.00, 3.3950, 0.0220, 12.000,
         3.8882, 3.341, 6, 0.0, 0.0},
        {Elements::Np, 117, "Np6+4", "", "Neptunium (octahedral, oxidation state +4)", 1.6660, 90.00, 3.4240, 0.0190, 12.000,
         3.8882, 3.549, 6, 0.0, 0.0},
        {Elements::Pu, 118, "Pu6+4", "", "Plutonium (octahedral, oxidation state +4)", 1.6570, 90.00, 3.4240, 0.0160, 12.000,
         3.8882, 3.243, 6, 0.0, 0.0},
        {Elements::Am, 119, "Am6+4", "", "Americium (octahedral, oxidation state +4)", 1.6600, 90.00, 3.3810, 0.0140, 12.000,
         3.8882, 2.9895, 6, 0.0, 0.0},
        {Elements::Cm, 120, "Cm6+3", "", "Curium (octahedral, oxidation state +3)", 1.8010, 90.00, 3.3260, 0.0130, 12.000,
         3.8882, 2.8315, 6, 0.0, 0.0},
        {Elements::Bk, 121, "Bk6+3", "", "Berkelium (octahedral, oxidation state +3)", 1.7610, 90.00, 3.3390, 0.0130, 12.000,
         3.8882, 3.1935, 6, 0.0, 0.0},
        {Elements::Cf, 122, "Cf6+3", "", "Californium (octahedral, oxidation state +3)", 1.7500, 90.00, 3.3130, 0.0130, 12.000,
         3.8882, 3.197, 6, 0.0, 0.0},
        {Elements::Es, 123, "Es6+3", "", "Einsteinium (octahedral, oxidation state +3)", 1.7240, 90.00, 3.2990, 0.0120, 12.000,
         3.8882, 3.333, 6, 0.0, 0.0},
        {Elements::Fm, 124, "Fm6+3", "", "Fermium (octahedral, oxidation state +3)", 1.7120, 90.00, 3.2860, 0.0120, 12.000,
         3.8882, 3.4, 6, 0.0, 0.0},
        {Elements::Md, 125, "Md6+3", "", "Mendelevium (octahedral, oxidation state +3)", 1.6890, 90.00, 3.2740, 0.0110, 12.000,
         3.8882, 3.47, 6, 0.0, 0.0},
        {Elements::No, 126, "No6+3", "", "Nobelium (octahedral, oxidation state +3)", 1.6790, 90.00, 3.2480, 0.0110, 12.000,
         3.8882, 3.475, 6, 0.0, 0.0},
        {Elements::Lr, 127, "Lr6+3", "", "Lawrencium (octahedral, oxidation state +3)", 1.6980, 90.00, 3.2360, 0.0110, 12.000,
         3.8882, 3.5, 6, 1.0, 1.0},
        {Elements::C, 200, "C_am", "nbonds=3,-O(nbonds=1),-N", "Carbon (amide)", 0.7290, 120.00, 3.8510, 0.1050, 12.730, 1.9120,
         5.343, 9, 0.0, 2.0},
        {Elements::N, 201, "N_am", "nbonds=3,-C(-O(nbonds=1))", "Nitrogen (amide)", 0.6990, 120.00, 3.6600, 0.0690, 13.407,
         2.5438, 6.899, 9, 0.0, 2.0}

    };

    // Create NETA definitions for each atom type - this is normally handled automatically, but we have custom atom types so
    // must do it ourselves
    auto nFailed = std::count_if(uffAtomTypes_.begin(), uffAtomTypes_.end(), [this](auto &atomType) {
        auto success = atomType.createNETA(this);
        if (!success)
            Messenger::print("Failed to parse NETA definition '{}' for UFF atom type '{}'.", atomType.neta().definitionString(),
                             atomType.name());
        return !success;
    });

    if (nFailed > 0)
        Messenger::error("Failed to create {} NETA {} for the forcefield '{}'.\n", nFailed,
                         nFailed == 1 ? "definition" : "definitions", name());

    return (nFailed == 0);
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_UFF::name() const { return "UFF"; }

// Return description for Forcefield
std::string_view Forcefield_UFF::description() const
{
    return "Implements 'UFF, a Full Periodic Table Force Field for Molecular Mechanics and Molecular Dynamics "
           "Simulations', A. K. Rappe, C. J. Casewit, K. S. Colwell, W. A. Goddard III, and W. "
           "M. "
           "Skiff, <i>J. Am. Chem. Soc.</i> <b>114</b>, 10024-10039 (1992).<br/>Notes:<ul><li>Any inconsistencies between "
           "the forcefield as implemented here and the original work are the sole "
           "responsibility of TGAY</li><li>Generator data 8 (THyb) are used to quickly determine the method of torsional "
           "parameter generation.</li><li>Torsional parameters U(i) are assigned to "
           "the "
           "second through sixth periods, following M. G. Martin's implementation in MCCCS Towhee.</li><li>Other "
           "modifications from the original paper are made following the MCCCS Towhee "
           "implementation.</li></ul>";
}

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType Forcefield_UFF::shortRangeType() const { return Forcefield::LennardJonesType; }

/*
 * Atom Type Data
 */

// Return UFF atom type with name specified
OptionalReferenceWrapper<const UFFAtomType> Forcefield_UFF::uffAtomTypeByName(std::string_view name) const
{
    auto it = std::find_if(uffAtomTypes_.begin(), uffAtomTypes_.end(),
                           [&](auto &data) { return DissolveSys::sameString(data.name(), name); });
    if (it == uffAtomTypes_.end())
        return {};
    return *it;
}

// Return first UFF atom type for specified element
OptionalReferenceWrapper<const UFFAtomType> Forcefield_UFF::uffAtomTypeForElement(int el) const
{
    auto it = std::find_if(uffAtomTypes_.begin(), uffAtomTypes_.end(), [&](auto &data) { return data.Z() == el; });
    if (it == uffAtomTypes_.end())
        return {};
    return *it;
}

// Determine and return atom type for specified SpeciesAtom
OptionalReferenceWrapper<const UFFAtomType> Forcefield_UFF::determineUFFAtomType(SpeciesAtom *i) const
{
    // Go through AtomTypes defined for the target's element, and check NETA scores
    auto bestScore = -1;
    OptionalReferenceWrapper<const UFFAtomType> bestType;
    for (const auto &type : uffAtomTypes_)
    {
        // Is this type of the correct base element?
        if (type.Z() != i->Z())
            continue;

        // Get the scoring for this type
        auto score = type.neta().score(i);
        Messenger::printVerbose("  -- score for type index {} ({}) is {}.\n", type.index(), type.name(), score);
        if (score > bestScore)
        {
            bestScore = score;
            bestType = type;
        }
    }

    if (bestScore == -1)
        Messenger::printVerbose("  -- no suitable type found.");
    else
        Messenger::printVerbose("  Best type for atom {} is {} ({}) with a score of {}.\n", i->userIndex(),
                                bestType->get().index(), bestType->get().name(), bestScore);

    return bestType;
}

// Determine and return atom type for specified SpeciesAtom
OptionalReferenceWrapper<const ForcefieldAtomType> Forcefield_UFF::determineAtomType(SpeciesAtom *i) const
{
    Messenger::printVerbose("Determining atom type for atom {} ({})\n", i->userIndex(), Elements::symbol(i->Z()));

    auto uffType = determineUFFAtomType(i);
    if (!uffType)
        return std::nullopt;

    return OptionalReferenceWrapper<const ForcefieldAtomType>(*uffType);
}

/*
 * Term Assignment
 */

// Return bond order correction for specified atom type pair
double Forcefield_UFF::bondOrderCorrection(const UFFAtomType &i, const UFFAtomType &j) const
{
    // Bond-order correction = -lambda * (ri + rj) * ln(n)  (eq 3)
    const auto lambda = 0.1332;

    // Determine bond order, recognising special case for the C-N amide bond
    auto bondOrder = 1.0;
    if ((i.name() == "C_am" && j.name() == "N_am") || (i.name() == "N_am" && j.name() == "C_am"))
        bondOrder = 1.41;
    else if (i.name().back() == 'R' && j.name().back() == 'R')
        bondOrder = 1.5;
    else if (i.name().back() == '2' && j.name().back() == '2')
        bondOrder = 2.0;
    else if (i.name().back() == '1' && j.name().back() == '1')
        bondOrder = 3.0;

    return -lambda * (i.r() + j.r()) * log(bondOrder);
}

// Return electronegativity correction for specified atom type pair
double Forcefield_UFF::electronegativityCorrection(const UFFAtomType &i, const UFFAtomType &j) const
{
    // Electronegativity correction : ri*rj * (sqrt(Xi)-sqrt(Xj))**2 / (Xi*ri + Xj*rj)    (eq 4)
    const auto chi = sqrt(i.chi()) - sqrt(j.chi());
    return i.r() * j.r() * chi * chi / (i.chi() * i.r() + j.chi() * j.r());
}

// Generate bond parameters for the supplied UFF atom types
bool Forcefield_UFF::generateBondTerm(const Species *sp, SpeciesBond &bond, const UFFAtomType &i, const UFFAtomType &j) const
{
    const auto rBO = bondOrderCorrection(i, j);
    const auto rEN = electronegativityCorrection(i, j);

    // rij : Equilibrium distance : = ri + rj + rBO - rEN  (eq 2)
    // Note: In the original paper  rij = ri + rj + rBO + rEN, but Marcus Martin (MCCCS Towhee) notes that the last term
    // should be subtracted, following conversation with Rappe.
    const auto rij = i.r() + j.r() + rBO - rEN;

    // k : Force constant : = 664.12 * (Zi * Zj) / rij**3     (note 664.12 in kcal)
    const auto k = 664.12 * (i.effectiveCharge() * j.effectiveCharge()) / (rij * rij * rij);

    // Set the parameters and form of the new bond term
    // Functional form is Harmonic : U = 0.5 * k * (r - eq)**2
    // Convert force constant from kcal to kJ
    bond.setForm(SpeciesBond::HarmonicForm);
    bond.setParameters({k * 4.184, rij});

    return true;
}

// Generate angle parameters for the supplied UFF atom types
bool Forcefield_UFF::generateAngleTerm(const Species *sp, SpeciesAngle &angle, const UFFAtomType &i, const UFFAtomType &j,
                                       const UFFAtomType &k) const
{
    // rBO : Bond-order correction = -0.1332 * (ri + rj) * ln(n)  (eq 3)
    // We need the bond orders of the involved bonds...
    const auto &ijRef = sp->getBond(angle.i(), angle.j());
    if (!ijRef)
        return Messenger::error("Can't locate bond i-j for bond order retrieval.\n");
    const auto &jkRef = sp->getBond(angle.j(), angle.k());
    if (!jkRef)
        return Messenger::error("Can't locate bond j-k for bond order retrieval.\n");

    const SpeciesBond &ij = *ijRef;
    const SpeciesBond &jk = *jkRef;
    const auto sumrij = i.r() + j.r();
    const auto sumrjk = j.r() + k.r();
    const auto rBOij = -0.1332 * sumrij * log(ij.bondOrder());
    const auto rBOjk = -0.1332 * sumrjk * log(jk.bondOrder());

    // rEN : Electronegativity correction : ri*rj * (sqrt(Xi)-sqrt(Xj))**2 / (Xi*ri + Xj*rj)    (eq 4)
    const auto chiij = sqrt(i.chi()) - sqrt(j.chi());
    const auto rENij = i.r() * j.r() * chiij * chiij / (i.chi() * i.r() + j.chi() * j.r());
    const auto chijk = sqrt(j.chi()) - sqrt(k.chi());
    const auto rENjk = j.r() * k.r() * chijk * chijk / (j.chi() * j.r() + k.chi() * k.r());

    // rij : Equilibrium distance : = ri + rj + rBO - rEN  (eq 2)
    // Note: In the original paper  rij = ri + rj + rBO + rEN, but Marcus Martin (MCCCS Towhee) notes that the last term
    // should be subtracted
    double rij = sumrij + rBOij - rENij;
    double rjk = sumrjk + rBOjk - rENjk;

    // Get theta for the central atom
    const auto theta = j.theta();
    const auto cosTheta = cos(theta);

    // Determine rik2 and rik5 values
    // rik2 = rij**2 + rjk**2 - 2 * rij * rjk * cos(theta)
    const auto rik2 = rij * rij + rjk * rjk - 2.0 * rij * rjk * cosTheta;
    const auto rik5 = rik2 * rik2 * sqrt(rik2);
    const auto forcek = 664.12 * 4.184 * (i.effectiveCharge() * k.effectiveCharge() / rik5) *
                        (3.0 * rij * rjk * (1.0 - cosTheta * cosTheta) - rik2 * cosTheta);

    // To determine angle form and necessary coefficients, use 'geom' integer data (which represents the third letter of the
    // atom name. This idea is shamelessly stolen from MCCCS Towhee!
    auto n = 0.0;
    const auto geom = j.geom();

    if (geom == 0)
        Messenger::error("Unable to generate angle function around central atom '{}'.\n", j.name());
    else if (geom == 1)
        n = 1;
    else if (geom == 2)
        n = 3;
    else if ((geom == 3) && (j.theta() < 90.1))
        n = 2;
    else if ((geom == 4) || (geom == 6))
        n = 4;
    else
    {
        // General nonlinear case:  U(theta) = forcek * (C0 + C1 * cos(theta) + C2 * cos(2*theta))
        const auto c2 = 1.0 / (4.0 * sin(theta) * sin(theta));
        const auto c1 = -4.0 * c2 * cosTheta;
        const auto c0 = c2 * (2.0 * cosTheta * cosTheta + 1.0);

        angle.setForm(SpeciesAngle::Cos2Form);
        angle.setParameters({forcek, c0, c1, c2});

        return true;
    }

    // Setup terms for the specific case (n != 0)
    angle.setForm(SpeciesAngle::CosineForm);
    angle.setParameters({forcek / (n * n), n, 0.0, -1.0});

    return true;
}

// Generate torsion parameters for the supplied UFF atom types
bool Forcefield_UFF::generateTorsionTerm(const Species *sp, SpeciesTorsion &torsionTerm, const UFFAtomType &i,
                                         const UFFAtomType &j, const UFFAtomType &k, const UFFAtomType &l) const
{
    /*
     * There are seven cases to consider, listed in decreasing complexity:
     *  a) j and k are both group 16 (old group 6) atoms, and both are sp3 centres
     *  b) j or k is a group 16 atom, while the other is an sp2 or resonant centre
     *  c) j or k is an sp3 atom, while the other is an sp2/resonant centre bound to another sp2/resonant centre
     *  d) j and k are both sp3 centres
     *  e) j and k are both sp2 centres
     *  f) j is sp2 and k is sp3 (or vice versa)
     *  g) everything else (no torsional barrier)
     */

    // 	# The original formula in the paper is given as a sum over cosine terms, but this reduces to: 0.5 * V * (1 -
    // cos(n*eq) * cos(n*phi)) 	# Aten's single cosine potential has the form: forcek * (1 + s*cos(period*phi - eq)) #
    // Therefore: forcek = 0.5 * V 	#		  s = -cos(n*eq) 	#	     period = n 	#
    // eq = 0.0

    const auto groupJ = Elements::group(j.Z());
    const auto groupK = Elements::group(k.Z());

    const auto geomI = i.geom() == 9 ? 2 : i.geom();
    const auto geomJ = j.geom() == 9 ? 2 : j.geom();
    const auto geomK = k.geom() == 9 ? 2 : k.geom();
    const auto geomL = l.geom() == 9 ? 2 : l.geom();
    double V, n, phi0;

    // Selection begins
    if ((groupJ == 16) && (groupK == 16) && (geomJ == 3) && (geomK == 3))
    {
        // Case a) j and k are both group 16 (old group 6) atoms, and both are sp3 centres

        // V value is 2.0 kcal for oxygen, 6.8 kcal otherwise
        double vj = j.Z() == Elements::O ? 2.0 : 6.8;
        double vk = k.Z() == Elements::O ? 2.0 : 6.8;
        V = sqrt(vj * vk);
        n = 2.0;
        phi0 = 90.0;
    }
    else if (((groupJ == 16) && (geomJ == 3) && (geomK == 2)) || ((groupK == 16) && (geomK == 3) && (geomJ == 2)))
    {
        // Case b) j or k is a group 16 atom, while the other is an sp2 or resonant centre
        // Use eq 17, but since the bond order is 1 (single bond) ln term in eq 17 is zero...
        V = 5.0 * sqrt(j.U() * k.U());
        n = 2.0;
        phi0 = 90.0;
    }
    else if ((geomJ == 3) && (geomK == 3))
    {
        // Case d) j and k are both sp3 centres
        V = sqrt(j.V() * k.V());
        n = 3.0;
        phi0 = 180.0;
    }
    else if ((geomJ == 2) && (geomK == 2))
    {
        // Case e) j and k are both sp2 centres
        // Force constant is adjusted based on current bond order
        auto jkRef = sp->getBond(torsionTerm.j(), torsionTerm.k());
        if (jkRef)
        {
            const SpeciesBond &jk = *jkRef;
            V = 5.0 * sqrt(j.U() * k.U()) * (1.0 + 4.18 * log(jk.bondOrder()));
        }
        else
        {
            Messenger::error("Can't generate correct force constant for torsion, since the SpeciesBond jk is NULL.\n");
            V = 5.0 * sqrt(j.U() * k.U());
        }
        n = 2.0;
        phi0 = 180.0;
    }
    else if (((geomJ == 3) && (geomK == 2)) || ((geomK == 3) && (geomJ == 2)))
    {
        // Case f) j is sp2 and k is sp3 (or vice versa)
        V = 1.0;
        n = 6.0;
        phi0 = 0.0;
    }
    else if (((geomJ == 3) && (geomK == 2) && (geomL == 2)) || ((geomK == 3) && (geomJ == 2) && (geomI == 2)))
    {
        // Case c) j or k is an sp3 atom, while the other is an sp2/resonant centre bound to another sp2/resonant centre
        V = 2.0;
        n = 3.0;
        phi0 = 180.0;
    }
    else
    {
        // Case g) everything else
        // Everything else....
        V = 0.0;
        n = 1.0;
        phi0 = 0.0;
    }

    // Convert V from kcal to kJ
    V *= 4.184;

    // Store the generated parameters
    torsionTerm.setForm(SpeciesTorsion::UFFCosineForm);
    torsionTerm.setParameters({V, n, phi0});

    return true;
}

// Assign intramolecular parameters to the supplied Species
bool Forcefield_UFF::assignIntramolecular(Species *sp, int flags) const
{
    auto determineTypes = flags & Forcefield::DetermineTypesFlag;
    auto selectionOnly = flags & Forcefield::SelectionOnlyFlag;

    // Generate bond terms
    for (auto &bond : sp->bonds())
    {
        auto *i = bond.i();
        auto *j = bond.j();

        if (selectionOnly && (!bond.isSelected()))
            continue;

        auto typeI = determineTypes ? determineUFFAtomType(i) : uffAtomTypeByName(i->atomType()->name());
        auto typeJ = determineTypes ? determineUFFAtomType(j) : uffAtomTypeByName(j->atomType()->name());

        if (!typeI || !typeJ || !generateBondTerm(sp, bond, *typeI, *typeJ))
            return Messenger::error("Failed to create parameters for bond {}-{}.\n", i->userIndex(), j->userIndex());
    }

    // Generate angle terms
    for (auto &angle : sp->angles())
    {
        auto *i = angle.i();
        auto *j = angle.j();
        auto *k = angle.k();

        if (selectionOnly && (!angle.isSelected()))
            continue;

        auto typeI = determineTypes ? determineUFFAtomType(i) : uffAtomTypeByName(i->atomType()->name());
        auto typeJ = determineTypes ? determineUFFAtomType(j) : uffAtomTypeByName(j->atomType()->name());
        auto typeK = determineTypes ? determineUFFAtomType(k) : uffAtomTypeByName(k->atomType()->name());

        if (!typeI || !typeJ || !typeK || !generateAngleTerm(sp, angle, *typeI, *typeJ, *typeK))
            return Messenger::error("Failed to create parameters for angle {}-{}-{}.\n", i->userIndex(), j->userIndex(),
                                    k->userIndex());
    }

    // Generate torsion terms
    for (auto &torsion : sp->torsions())
    {
        SpeciesAtom *i = torsion.i();
        SpeciesAtom *j = torsion.j();
        SpeciesAtom *k = torsion.k();
        SpeciesAtom *l = torsion.l();

        if (selectionOnly && (!torsion.isSelected()))
            continue;

        auto typeI = determineTypes ? determineUFFAtomType(i) : uffAtomTypeByName(i->atomType()->name());
        auto typeJ = determineTypes ? determineUFFAtomType(j) : uffAtomTypeByName(j->atomType()->name());
        auto typeK = determineTypes ? determineUFFAtomType(k) : uffAtomTypeByName(k->atomType()->name());
        auto typeL = determineTypes ? determineUFFAtomType(l) : uffAtomTypeByName(l->atomType()->name());

        if (!typeI || !typeJ || !typeK || !typeL || !generateTorsionTerm(sp, torsion, *typeI, *typeJ, *typeK, *typeL))
            return Messenger::error("Failed to create parameters for torsion {}-{}-{}-{}.\n", i->userIndex(), j->userIndex(),
                                    k->userIndex(), l->userIndex());
    }

    return true;
}

// Perform some test calculations
void Forcefield_UFF::test() const {}
