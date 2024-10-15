// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/isotopes.h"
#include <algorithm>
#include <fmt/format.h>
#include <stdexcept>

namespace Sears91
{
/*
 * Neutron Scattering Lengths and Cross Sections
 *
 * Varley F. Sears
 * AECL Research, Chalk River Laboratories
 * Chalk River, Ontario, Canada K0J 1J0
 * 1991 November 11
 *
 * DRAFT
 *
 *	Symbol  Unit    Quantity
 *	--------------------------------------------------------------
 *	Z		atomic number
 *	A		mass number
 *	I(p)	spin (parity) of the nuclear ground state
 *	Atwt	Atomic weight (C=12)
 *	bc	fm	bound coherent scattering length
 *	bi	fm	bound incoherent scattering length
 *	sc	barn[1]	bound coherent scattering cross section
 *	si	barn	bound incoherent scattering cross section
 *	ss	barn	total bound scattering cross section
 *	sa	barn	absorption cross section for 2200 m/s neutrons[2]
 *
 *	[1] 1 barn = 100 fm**2
 *	[2] E = 25.30 meV, k = 3.494 Angstroms**-1, lambda = 1.798 Angstroms
 */

static std::vector<IsotopeData> sears91Data_ = {
    {Isotope::Unknown, Elements::Unknown, 0, "", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {Isotope::H_Natural, Elements::H, 0, "", 1.00797, -3.739, 0, 1.7568, 80.26, 82.02, 0.3326},
    {Isotope::H_1, Elements::H, 1, "1/2(+)", 1.00783, -3.7406, 25.274, 1.7583, 80.27, 82.03, 0.3326},
    {Isotope::H_2, Elements::H, 2, "1(+)", 2.014, 6.671, 4.04, 5.592, 2.05, 7.64, 0.000519},
    {Isotope::H_3, Elements::H, 3, "1/2(+)", 3.01605, 4.792, -1.04, 2.89, 0.14, 3.03, 0},
    {Isotope::He_Natural, Elements::He, 0, "", 4.0026, 3.26, 0, 1.34, 0, 1.34, 0.00747},
    {Isotope::He_3, Elements::He, 3, "1/2(+)", 3.01603, 5.74, -2.5, 4.42, 1.6, 6, 5333},
    {Isotope::He_4, Elements::He, 4, "0(+)", 4.0026, 3.26, 0, 1.34, 0, 1.34, 0},
    {Isotope::Li_Natural, Elements::Li, 0, "", 6.9409, -1.9, 0, 0.454, 0.92, 1.37, 70.5},
    {Isotope::Li_6, Elements::Li, 6, "1(+)", 6.01512, 2, -1.89, 0.51, 0.46, 0.97, 940},
    {Isotope::Li_7, Elements::Li, 7, "3/2(-)", 7.016, -2.22, -2.49, 0.619, 0.78, 1.4, 0.0454},
    {Isotope::Be_Natural, Elements::Be, 0, "3/2(-)", 9.01218, 7.79, 0.12, 7.63, 0.0018, 7.63, 0.0076},
    {Isotope::B_Natural, Elements::B, 0, "", 10.81003, 5.3, 0, 3.54, 1.7, 5.24, 767},
    {Isotope::B_10, Elements::B, 10, "3(+)", 10.01294, -0.1, -4.7, 0.144, 3, 3.1, 3835},
    {Isotope::B_11, Elements::B, 11, "3/2(-)", 11.00931, 6.65, -1.3, 5.56, 0.21, 5.77, 0.0055},
    {Isotope::C_Natural, Elements::C, 0, "", 12.0111, 6.646, 0, 5.55, 0.001, 5.551, 0.0035},
    {Isotope::C_12, Elements::C, 12, "0(+)", 12, 6.6511, 0, 5.559, 0, 5.559, 0.00353},
    {Isotope::C_13, Elements::C, 13, "1/2(-)", 13.00336, 6.19, -0.52, 4.81, 0.034, 4.84, 0.00137},
    {Isotope::N_Natural, Elements::N, 0, "", 14.0067, 9.36, 0, 11.01, 0.5, 11.51, 1.9},
    {Isotope::N_14, Elements::N, 14, "1(+)", 14.00307, 9.37, 2, 11.03, 0.5, 11.53, 1.91},
    {Isotope::N_15, Elements::N, 15, "1/2(-)", 15.00011, 6.44, -0.02, 5.21, 0.0001, 5.21, 2.4E-05},
    {Isotope::O_Natural, Elements::O, 0, "", 15.9994, 5.803, 0, 4.232, 0, 4.232, 0.00019},
    {Isotope::O_16, Elements::O, 16, "0(+)", 15.99492, 5.803, 0, 4.232, 0, 4.232, 0.0001},
    {Isotope::O_17, Elements::O, 17, "5/2(+)", 16.99913, 5.78, 0.18, 4.2, 0.004, 4.2, 0.236},
    {Isotope::O_18, Elements::O, 18, "0(+)", 17.99916, 5.84, 0, 4.29, 0, 4.29, 0.00016},
    {Isotope::F_Natural, Elements::F, 0, "1/2(+)", 18.9984, 5.654, -0.082, 4.017, 0.0008, 4.018, 0.0096},
    {Isotope::Ne_Natural, Elements::Ne, 0, "", 20.179, 4.566, 0, 2.62, 0.008, 2.628, 0.039},
    {Isotope::Ne_20, Elements::Ne, 20, "0(+)", 19.99244, 4.631, 0, 2.695, 0, 2.695, 0.036},
    {Isotope::Ne_21, Elements::Ne, 21, "3/2(+)", 20.99384, 6.66, 0, 5.6, 0.05, 5.7, 0.67},
    {Isotope::Ne_22, Elements::Ne, 22, "0(+)", 21.99138, 3.87, 0, 1.88, 0, 1.88, 0.046},
    {Isotope::Na_Natural, Elements::Na, 0, "3/2(+)", 22.98977, 3.58, 3.65, 1.611, 1.674, 3.285, 0.53},
    {Isotope::Mg_Natural, Elements::Mg, 0, "", 24.305, 5.375, 0, 3.631, 0.08, 3.71, 0.063},
    {Isotope::Mg_24, Elements::Mg, 24, "0(+)", 23.98504, 5.66, 0, 4.03, 0, 4.03, 0.05},
    {Isotope::Mg_25, Elements::Mg, 25, "5/2(+)", 24.98584, 3.62, 1.48, 1.65, 0.28, 1.93, 0.19},
    {Isotope::Mg_26, Elements::Mg, 26, "0(+)", 25.98259, 4.89, 0, 3, 0, 3, 0.0382},
    {Isotope::Al_Natural, Elements::Al, 0, "5/2(+)", 26.98154, 3.449, 0.256, 1.495, 0.0082, 1.503, 0.231},
    {Isotope::Si_Natural, Elements::Si, 0, "", 28.0855, 4.1534, 0, 2.1678, 0.003, 2.171, 0.171},
    {Isotope::Si_28, Elements::Si, 28, "0(+)", 27.97693, 4.111, 0, 2.124, 0, 2.124, 0.177},
    {Isotope::Si_29, Elements::Si, 29, "1/2(+)", 28.9765, 4.7, 0.09, 2.78, 0.001, 2.78, 0.101},
    {Isotope::Si_30, Elements::Si, 30, "0(+)", 29.97377, 4.58, 0, 2.64, 0, 2.64, 0.107},
    {Isotope::P_Natural, Elements::P, 0, "1/2(+)", 30.97376, 5.13, 0.2, 3.307, 0.005, 3.312, 0.172},
    {Isotope::S_Natural, Elements::S, 0, "", 32.066, 2.847, 0, 1.0186, 0.007, 1.026, 0.53},
    {Isotope::S_32, Elements::S, 32, "0(+)", 31.97207, 2.804, 0, 0.988, 0, 0.988, 0.54},
    {Isotope::S_33, Elements::S, 33, "3/2(+)", 32.97146, 4.74, 1.5, 2.8, 0.3, 3.1, 0.54},
    {Isotope::S_34, Elements::S, 34, "0(+)", 33.96787, 3.48, 0, 1.52, 0, 1.52, 0.227},
    {Isotope::S_36, Elements::S, 36, "0(+)", 35.96708, 3, 0, 1.1, 0, 1.1, 0.15},
    {Isotope::Cl_Natural, Elements::Cl, 0, "", 35.453, 9.577, 0, 11.526, 5.3, 16.8, 33.5},
    {Isotope::Cl_35, Elements::Cl, 35, "3/2(+)", 34.96885, 11.65, 6.1, 17.06, 4.7, 21.8, 44.1},
    {Isotope::Cl_37, Elements::Cl, 37, "3/2(+)", 36.9659, 3.08, 0.1, 1.19, 0.001, 1.19, 0.433},
    {Isotope::Ar_Natural, Elements::Ar, 0, "", 39.948, 1.909, 0, 0.458, 0.225, 0.683, 0.675},
    {Isotope::Ar_36, Elements::Ar, 36, "0(+)", 35.96755, 24.9, 0, 77.9, 0, 77.9, 5.2},
    {Isotope::Ar_38, Elements::Ar, 38, "0(+)", 37.96273, 3.5, 0, 1.5, 0, 1.5, 0.8},
    {Isotope::Ar_40, Elements::Ar, 40, "0(+)", 39.96238, 1.83, 0, 0.421, 0, 0.421, 0.66},
    {Isotope::K_Natural, Elements::K, 0, "", 39.0983, 3.67, 0, 1.69, 0.27, 1.96, 2.1},
    {Isotope::K_39, Elements::K, 39, "3/2(+)", 38.96371, 3.74, 1.4, 1.76, 0.25, 2.01, 2.1},
    {Isotope::K_40, Elements::K, 40, "4(-)", 39.974, 3, 0, 1.1, 0.5, 1.6, 35},
    {Isotope::K_41, Elements::K, 41, "3/2(+)", 40.96183, 2.69, 1.5, 0.91, 0.3, 1.2, 1.46},
    {Isotope::Ca_Natural, Elements::Ca, 0, "", 40.078, 4.76, 0, 2.85, 0.04, 2.89, 0.43},
    {Isotope::Ca_40, Elements::Ca, 40, "0(+)", 39.96259, 4.85, 0, 2.96, 0, 2.96, 0.41},
    {Isotope::Ca_42, Elements::Ca, 42, "0(+)", 41.95862, 3.15, 0, 1.25, 0, 1.25, 0.68},
    {Isotope::Ca_43, Elements::Ca, 43, "7/2(-)", 42.95877, 0.2, 0, 0.005, 0.5, 0.5, 6.2},
    {Isotope::Ca_44, Elements::Ca, 44, "0(+)", 43.95548, 1.8, 0, 0.41, 0, 0.41, 0.88},
    {Isotope::Ca_46, Elements::Ca, 46, "0(+)", 45.95369, 2.55, 0, 0.82, 0, 0.82, 0.74},
    {Isotope::Ca_48, Elements::Ca, 48, "0(+)", 47.95253, 1.5, 0, 0.28, 0, 0.28, 1.09},
    {Isotope::Sc_Natural, Elements::Sc, 0, "7/2(-)", 44.95591, 12.29, -6, 19, 4.5, 23.5, 27.5},
    {Isotope::Ti_Natural, Elements::Ti, 0, "", 47.88, -3.438, 0, 1.485, 2.87, 4.35, 6.09},
    {Isotope::Ti_46, Elements::Ti, 46, "0(+)", 45.95263, 4.93, 0, 3.05, 0, 3.05, 0.59},
    {Isotope::Ti_47, Elements::Ti, 47, "5/2(-)", 46.95176, 3.63, -3.5, 1.66, 1.5, 3.2, 1.7},
    {Isotope::Ti_48, Elements::Ti, 48, "0(+)", 47.94795, -6.08, 0, 4.65, 0, 4.65, 7.84},
    {Isotope::Ti_49, Elements::Ti, 49, "7/2(-)", 48.94787, 1.04, 5.1, 0.14, 3.3, 3.4, 2.2},
    {Isotope::Ti_50, Elements::Ti, 50, "0(+)", 49.94479, 6.18, 0, 4.8, 0, 4.8, 0.179},
    {Isotope::V_Natural, Elements::V, 0, "", 50.9415, -0.3824, 0, 0.0184, 5.08, 5.1, 5.08},
    {Isotope::V_50, Elements::V, 50, "6(+)", 49.94716, 7.6, 0, 7.3, 0.5, 7.8, 60},
    {Isotope::V_51, Elements::V, 51, "7/2(-)", 50.94396, -0.402, 6.35, 0.0203, 5.07, 5.09, 4.9},
    {Isotope::Cr_Natural, Elements::Cr, 0, "", 51.996, 3.635, 0, 1.66, 1.83, 3.49, 3.05},
    {Isotope::Cr_50, Elements::Cr, 50, "0(+)", 49.94605, -4.5, 0, 2.54, 0, 2.54, 15.8},
    {Isotope::Cr_52, Elements::Cr, 52, "0(+)", 51.94051, 4.92, 0, 3.042, 0, 3.042, 0.76},
    {Isotope::Cr_53, Elements::Cr, 53, "3/2(-)", 52.94065, -4.2, 6.87, 2.22, 5.93, 8.15, 18.1},
    {Isotope::Cr_54, Elements::Cr, 54, "0(+)", 53.93888, 4.55, 0, 2.6, 0, 2.6, 0.36},
    {Isotope::Mn_Natural, Elements::Mn, 0, "5/2(-)", 54.938, -3.73, 1.79, 1.75, 0.4, 2.15, 13.3},
    {Isotope::Fe_Natural, Elements::Fe, 0, "", 55.847, 9.54, 0, 11.44, 0.38, 11.82, 2.56},
    {Isotope::Fe_54, Elements::Fe, 54, "0(+)", 53.93961, 4.2, 0, 2.2, 0, 2.2, 2.25},
    {Isotope::Fe_56, Elements::Fe, 56, "0(+)", 55.93494, 10.03, 0, 12.64, 0, 12.64, 2.59},
    {Isotope::Fe_57, Elements::Fe, 57, "1/2(-)", 56.9354, 2.3, 0, 0.66, 0.3, 1, 2.48},
    {Isotope::Fe_58, Elements::Fe, 58, "0(+)", 57.93328, 15, 0, 28, 0, 28, 1.28},
    {Isotope::Co_Natural, Elements::Co, 0, "7/2(-)", 58.9332, 2.78, -6.2, 0.97, 4.8, 5.8, 37.18},
    {Isotope::Ni_Natural, Elements::Ni, 0, "", 58.67, 10.3, 0, 13.3, 5.2, 18.5, 4.49},
    {Isotope::Ni_58, Elements::Ni, 58, "0(+)", 57.93535, 14.4, 0, 26.1, 0, 26.1, 4.6},
    {Isotope::Ni_60, Elements::Ni, 60, "0(+)", 59.93079, 2.8, 0, 0.99, 0, 0.99, 2.9},
    {Isotope::Ni_61, Elements::Ni, 61, "3/2(-)", 60.93106, 7.6, 3.9, 7.26, 1.9, 9.2, 2.5},
    {Isotope::Ni_62, Elements::Ni, 62, "0(+)", 61.92835, -8.7, 0, 9.5, 0, 9.5, 14.5},
    {Isotope::Ni_64, Elements::Ni, 64, "0(+)", 63.92797, -0.38, 0, 0.018, 0, 0.018, 1.52},
    {Isotope::Cu_Natural, Elements::Cu, 0, "", 63.546, 7.718, 0, 7.485, 0.55, 8.03, 3.78},
    {Isotope::Cu_63, Elements::Cu, 63, "3/2(-)", 62.9296, 6.43, 0.22, 5.2, 0.006, 5.2, 4.5},
    {Isotope::Cu_65, Elements::Cu, 65, "3/2(-)", 64.92779, 10.61, 1.79, 14.1, 0.4, 14.5, 2.17},
    {Isotope::Zn_Natural, Elements::Zn, 0, "", 65.39, 5.68, 0, 4.054, 0.077, 4.131, 1.11},
    {Isotope::Zn_64, Elements::Zn, 64, "0(+)", 63.92915, 5.22, 0, 3.42, 0, 3.42, 0.93},
    {Isotope::Zn_66, Elements::Zn, 66, "0(+)", 65.92603, 5.97, 0, 4.48, 0, 4.48, 0.62},
    {Isotope::Zn_67, Elements::Zn, 67, "5/2(-)", 66.92713, 7.56, -1.5, 7.18, 0.28, 7.46, 6.8},
    {Isotope::Zn_68, Elements::Zn, 68, "0(+)", 67.92485, 6.03, 0, 4.57, 0, 4.57, 1.1},
    {Isotope::Zn_70, Elements::Zn, 70, "0(+)", 69.92533, 6, 0, 4.5, 0, 4.5, 0.092},
    {Isotope::Ga_Natural, Elements::Ga, 0, "", 69.723, 7.288, 0, 6.675, 0.16, 6.83, 2.75},
    {Isotope::Ga_69, Elements::Ga, 69, "3/2(-)", 68.92558, 7.88, -0.85, 7.8, 0.091, 7.89, 2.18},
    {Isotope::Ga_71, Elements::Ga, 71, "3/2(-)", 70.9247, 6.4, -0.82, 5.15, 0.084, 5.23, 3.61},
    {Isotope::Ge_Natural, Elements::Ge, 0, "", 72.59, 8.185, 0, 8.42, 0.18, 8.6, 2.2},
    {Isotope::Ge_70, Elements::Ge, 70, "0(+)", 69.92425, 10, 0, 12.6, 0, 12.6, 3},
    {Isotope::Ge_72, Elements::Ge, 72, "0(+)", 71.92208, 8.51, 0, 9.1, 0, 9.1, 0.8},
    {Isotope::Ge_73, Elements::Ge, 73, "9/2(+)", 72.92346, 5.02, 3.4, 3.17, 1.5, 4.7, 15.1},
    {Isotope::Ge_74, Elements::Ge, 74, "0(+)", 73.92118, 7.58, 0, 7.2, 0, 7.2, 0.4},
    {Isotope::Ge_76, Elements::Ge, 76, "0(+)", 75.9214, 8.2, 0, 8, 0, 8, 0.16},
    {Isotope::As_Natural, Elements::As, 0, "3/2(-)", 74.92159, 6.58, -0.69, 5.44, 0.06, 5.5, 4.5},
    {Isotope::Se_Natural, Elements::Se, 0, "", 78.96, 7.97, 0, 7.98, 0.32, 8.3, 11.7},
    {Isotope::Se_74, Elements::Se, 74, "0(+)", 73.92248, 0.8, 0, 0.1, 0, 0.1, 51.8},
    {Isotope::Se_76, Elements::Se, 76, "0(+)", 75.91921, 12.2, 0, 18.7, 0, 18.7, 85},
    {Isotope::Se_77, Elements::Se, 77, "1/2(-)", 76.91991, 8.25, 0, 8.6, 0.05, 8.65, 42},
    {Isotope::Se_78, Elements::Se, 78, "0(+)", 77.9173, 8.24, 0, 8.5, 0, 8.5, 0.43},
    {Isotope::Se_80, Elements::Se, 80, "0(+)", 79.91652, 7.48, 0, 7.03, 0, 7.03, 0.61},
    {Isotope::Se_82, Elements::Se, 82, "0(+)", 81.9167, 6.34, 0, 5.05, 0, 5.05, 0.044},
    {Isotope::Br_Natural, Elements::Br, 0, "", 79.904, 6.795, 0, 5.8, 0.1, 5.9, 6.9},
    {Isotope::Br_79, Elements::Br, 79, "3/2(-)", 78.91834, 6.8, -1.1, 5.81, 0.15, 5.96, 11},
    {Isotope::Br_81, Elements::Br, 81, "3/2(-)", 80.91629, 6.79, 0.6, 5.79, 0.05, 5.84, 2.7},
    {Isotope::Kr_Natural, Elements::Kr, 0, "", 83.8, 7.81, 0, 7.67, 0.01, 7.68, 25},
    {Isotope::Kr_78, Elements::Kr, 78, "0(+)", 77.9204, 0, 0, 0, 0, 0, 6.4},
    {Isotope::Kr_80, Elements::Kr, 80, "0(+)", 79.91638, 0, 0, 0, 0, 0, 11.8},
    {Isotope::Kr_82, Elements::Kr, 82, "0(+)", 81.91348, 0, 0, 0, 0, 0, 29},
    {Isotope::Kr_83, Elements::Kr, 83, "9/2(+)", 82.91414, 0, 0, 0, 0, 0, 185},
    {Isotope::Kr_84, Elements::Kr, 84, "0(+)", 83.91151, 0, 0, 0, 0, 0, 0.113},
    {Isotope::Kr_86, Elements::Kr, 86, "0(+)", 85.91062, 0, 0, 0, 0, 0, 0.003},
    {Isotope::Rb_Natural, Elements::Rb, 0, "", 85.4678, 7.09, 0, 6.32, 0.5, 6.8, 0.38},
    {Isotope::Rb_85, Elements::Rb, 85, "5/2(-)", 84.91179, 7.03, 0, 6.2, 0.5, 6.7, 0.48},
    {Isotope::Rb_87, Elements::Rb, 87, "3/2(-)", 86.90919, 7.23, 0, 6.6, 0.5, 7.1, 0.12},
    {Isotope::Sr_Natural, Elements::Sr, 0, "", 87.62, 7.02, 0, 6.19, 0.06, 6.25, 1.28},
    {Isotope::Sr_84, Elements::Sr, 84, "0(+)", 83.91343, 7, 0, 6, 0, 6, 0.87},
    {Isotope::Sr_86, Elements::Sr, 86, "0(+)", 85.90927, 5.67, 0, 4.04, 0, 4.04, 1.04},
    {Isotope::Sr_87, Elements::Sr, 87, "9/2(+)", 86.90888, 7.4, 0, 6.88, 0.5, 7.4, 16},
    {Isotope::Sr_88, Elements::Sr, 88, "0(+)", 87.90562, 7.15, 0, 6.42, 0, 6.42, 0.058},
    {Isotope::Y_Natural, Elements::Y, 0, "1/2(-)", 88.90585, 7.75, 1.1, 7.55, 0.15, 7.7, 1.28},
    {Isotope::Zr_Natural, Elements::Zr, 0, "", 91.224, 7.16, 0, 6.44, 0.02, 6.46, 0.185},
    {Isotope::Zr_90, Elements::Zr, 90, "0(+)", 89.9047, 6.4, 0, 5.1, 0, 5.1, 0.011},
    {Isotope::Zr_91, Elements::Zr, 91, "5/2(+)", 90.90564, 8.7, -1.08, 9.5, 0.15, 9.7, 1.17},
    {Isotope::Zr_92, Elements::Zr, 92, "0(+)", 91.90504, 7.4, 0, 6.9, 0, 6.9, 0.22},
    {Isotope::Zr_94, Elements::Zr, 94, "0(+)", 93.90631, 8.2, 0, 8.4, 0, 8.4, 0.0499},
    {Isotope::Zr_96, Elements::Zr, 96, "0(+)", 95.90828, 5.5, 0, 3.8, 0, 3.8, 0.0229},
    {Isotope::Nb_Natural, Elements::Nb, 0, "9/2(+)", 92.90638, 7.054, -0.139, 6.253, 0.0024, 6.255, 1.15},
    {Isotope::Mo_Natural, Elements::Mo, 0, "", 95.94, 6.715, 0, 5.67, 0.04, 5.71, 2.48},
    {Isotope::Mo_92, Elements::Mo, 92, "0(+)", 91.90681, 6.91, 0, 6, 0, 6, 0.019},
    {Isotope::Mo_94, Elements::Mo, 94, "0(+)", 93.90509, 6.8, 0, 5.81, 0, 5.81, 0.015},
    {Isotope::Mo_95, Elements::Mo, 95, "5/2(+)", 94.90584, 6.91, 0, 6, 0.5, 6.5, 13.1},
    {Isotope::Mo_96, Elements::Mo, 96, "0(+)", 95.90468, 6.2, 0, 4.83, 0, 4.83, 0.5},
    {Isotope::Mo_97, Elements::Mo, 97, "5/2(+)", 96.90602, 7.24, 0, 6.59, 0.5, 7.1, 2.5},
    {Isotope::Mo_98, Elements::Mo, 98, "0(+)", 97.90541, 6.58, 0, 5.44, 0, 5.44, 0.127},
    {Isotope::Mo_100, Elements::Mo, 100, "0(+)", 99.90748, 6.73, 0, 5.69, 0, 5.69, 0.4},
    {Isotope::Tc_Natural, Elements::Tc, 0, "9/2(+)", 98.90625, 6.8, 0, 5.8, 0.5, 6.3, 20},
    {Isotope::Ru_Natural, Elements::Ru, 0, "", 101.07, 7.21, 0, 6.5, 0.1, 6.6, 2.56},
    {Isotope::Ru_96, Elements::Ru, 96, "0(+)", 95.9076, 0, 0, 0, 0, 0, 0.28},
    {Isotope::Ru_98, Elements::Ru, 98, "0(+)", 97.90529, 0, 0, 0, 0, 0, 0},
    {Isotope::Ru_99, Elements::Ru, 99, "5/2(+)", 98.90594, 0, 0, 0, 0, 0, 6.9},
    {Isotope::Ru_100, Elements::Ru, 100, "0(+)", 99.90422, 0, 0, 0, 0, 0, 4.8},
    {Isotope::Ru_101, Elements::Ru, 101, "5/2(+)", 100.90558, 0, 0, 0, 0, 0, 3.3},
    {Isotope::Ru_102, Elements::Ru, 102, "0(+)", 101.90435, 0, 0, 0, 0, 0, 1.17},
    {Isotope::Ru_104, Elements::Ru, 104, "0(+)", 103.90542, 0, 0, 0, 0, 0, 0.31},
    {Isotope::Rh_Natural, Elements::Rh, 0, "1/2(-)", 102.9055, 5.88, 0, 4.34, 0.3, 4.6, 144.8},
    {Isotope::Pd_Natural, Elements::Pd, 0, "", 106.42, 5.91, 0, 4.39, 0.093, 4.48, 6.9},
    {Isotope::Pd_102, Elements::Pd, 102, "0(+)", 101.90563, 7.7, 0, 7.5, 0, 7.5, 3.4},
    {Isotope::Pd_104, Elements::Pd, 104, "0(+)", 103.90403, 7.7, 0, 7.5, 0, 7.5, 0.6},
    {Isotope::Pd_105, Elements::Pd, 105, "5/2(+)", 104.90508, 5.5, -2.6, 3.8, 0.8, 4.6, 20},
    {Isotope::Pd_106, Elements::Pd, 106, "0(+)", 105.90348, 6.4, 0, 5.1, 0, 5.1, 0.304},
    {Isotope::Pd_108, Elements::Pd, 108, "0(+)", 107.9039, 4.1, 0, 2.1, 0, 2.1, 8.5},
    {Isotope::Pd_110, Elements::Pd, 110, "0(+)", 109.90517, 7.7, 0, 7.5, 0, 7.5, 0.226},
    {Isotope::Ag_Natural, Elements::Ag, 0, "", 107.8682, 5.922, 0, 4.407, 0.58, 4.99, 63.3},
    {Isotope::Ag_107, Elements::Ag, 107, "1/2(-)", 106.90509, 7.555, 1, 7.17, 0.13, 7.3, 37.6},
    {Isotope::Ag_109, Elements::Ag, 109, "1/2(-)", 108.90476, 4.165, -1.6, 2.18, 0.32, 2.5, 91},
    {Isotope::Cd_Natural, Elements::Cd, 0, "", 112.41, 5.1, 0, 3.3, 2.4, 5.7, 2520},
    {Isotope::Cd_106, Elements::Cd, 106, "0(+)", 105.90646, 0, 0, 0, 0, 0, 1},
    {Isotope::Cd_108, Elements::Cd, 108, "0(+)", 107.90418, 0, 0, 0, 0, 0, 1.1},
    {Isotope::Cd_110, Elements::Cd, 110, "0(+)", 109.90301, 0, 0, 0, 0, 0, 11},
    {Isotope::Cd_111, Elements::Cd, 111, "1/2(+)", 110.90418, 0, 0, 0, 0, 5, 24},
    {Isotope::Cd_112, Elements::Cd, 112, "0(+)", 111.90276, 7.4, 0, 6.9, 0, 6.9, 2.2},
    {Isotope::Cd_113, Elements::Cd, 113, "1/2(+)", 112.9044, -8, 0, 12.1, 0, 0, 20600},
    {Isotope::Cd_114, Elements::Cd, 114, "0(+)", 113.90336, 6.4, 0, 5.1, 0, 5.1, 0.34},
    {Isotope::Cd_116, Elements::Cd, 116, "0(+)", 115.90475, 7.1, 0, 6.3, 0, 6.3, 0.075},
    {Isotope::In_Natural, Elements::In, 0, "", 114.82, 4.065, 0, 2.08, 0.54, 2.62, 193.8},
    {Isotope::In_113, Elements::In, 113, "9/2(+)", 112.90406, 5.39, 0, 3.65, 0, 3.65, 12},
    {Isotope::In_115, Elements::In, 115, "9/2(+)", 114.90388, 4.01, -2.1, 2.02, 0.55, 2.57, 202},
    {Isotope::Sn_Natural, Elements::Sn, 0, "", 118.71, 6.225, 0, 4.87, 0.022, 4.892, 0.626},
    {Isotope::Sn_112, Elements::Sn, 112, "0(+)", 111.90483, 6, 0, 4.5, 0, 4.5, 1},
    {Isotope::Sn_114, Elements::Sn, 114, "0(+)", 113.90278, 6.2, 0, 4.8, 0, 4.8, 0.114},
    {Isotope::Sn_115, Elements::Sn, 115, "1/2(+)", 114.90335, 6, 0, 4.5, 0.3, 4.8, 30},
    {Isotope::Sn_116, Elements::Sn, 116, "0(+)", 115.90175, 6, 0, 4.5, 0, 4.5, 0.14},
    {Isotope::Sn_117, Elements::Sn, 117, "1/2(+)", 116.90296, 6.6, 0, 5.5, 0.3, 5.8, 2.3},
    {Isotope::Sn_118, Elements::Sn, 118, "0(+)", 117.90161, 6, 0, 4.5, 0, 4.5, 0.22},
    {Isotope::Sn_119, Elements::Sn, 119, "1/2(+)", 118.90331, 6.2, 0, 4.8, 0.3, 5.1, 2.2},
    {Isotope::Sn_120, Elements::Sn, 120, "0(+)", 119.9022, 6.6, 0, 5.5, 0, 5.5, 0.14},
    {Isotope::Sn_122, Elements::Sn, 122, "0(+)", 121.90344, 5.7, 0, 4.1, 0, 4.1, 0.18},
    {Isotope::Sn_124, Elements::Sn, 124, "0(+)", 123.90527, 6.1, 0, 4.7, 0, 4.7, 0.133},
    {Isotope::Sb_Natural, Elements::Sb, 0, "", 121.75, 5.57, 0, 3.9, 0, 3.9, 4.91},
    {Isotope::Sb_121, Elements::Sb, 121, "5/2(+)", 120.90382, 5.71, -0.05, 4.1, 0.0003, 4.1, 5.75},
    {Isotope::Sb_123, Elements::Sb, 123, "7/2(+)", 122.90422, 5.38, -0.1, 3.64, 0.001, 3.64, 3.8},
    {Isotope::Te_Natural, Elements::Te, 0, "", 127.6, 5.8, 0, 4.23, 0.09, 4.32, 4.7},
    {Isotope::Te_120, Elements::Te, 120, "0(+)", 119.90405, 5.3, 0, 3.5, 0, 3.5, 2.3},
    {Isotope::Te_122, Elements::Te, 122, "0(+)", 121.90305, 3.8, 0, 1.8, 0, 1.8, 3.4},
    {Isotope::Te_123, Elements::Te, 123, "1/2(+)", 122.90427, -0.05, -2.04, 0.002, 0.52, 0.52, 418},
    {Isotope::Te_124, Elements::Te, 124, "0(+)", 123.90282, 7.96, 0, 8, 0, 8, 6.8},
    {Isotope::Te_125, Elements::Te, 125, "1/2(+)", 124.90443, 5.02, -0.26, 3.17, 0.008, 3.18, 1.55},
    {Isotope::Te_126, Elements::Te, 126, "0(+)", 125.90331, 5.56, 0, 3.88, 0, 3.88, 1.04},
    {Isotope::Te_128, Elements::Te, 128, "0(+)", 127.90446, 5.89, 0, 4.36, 0, 4.36, 0.215},
    {Isotope::Te_130, Elements::Te, 130, "0(+)", 129.90623, 6.02, 0, 4.55, 0, 4.55, 0.29},
    {Isotope::I_Natural, Elements::I, 0, "5/2(+)", 126.90447, 5.28, 1.58, 3.5, 0.31, 3.81, 6.15},
    {Isotope::Xe_Natural, Elements::Xe, 0, "", 131.29, 4.92, 0, 3.04, 0, 0, 23.9},
    {Isotope::Xe_124, Elements::Xe, 124, "0(+)", 123.90589, 0, 0, 0, 0, 0, 165},
    {Isotope::Xe_126, Elements::Xe, 126, "0(+)", 125.90428, 0, 0, 0, 0, 0, 3.5},
    {Isotope::Xe_128, Elements::Xe, 128, "0(+)", 127.90353, 0, 0, 0, 0, 0, 8},
    {Isotope::Xe_129, Elements::Xe, 129, "1/2(+)", 128.90478, 0, 0, 0, 0, 0, 21},
    {Isotope::Xe_130, Elements::Xe, 130, "0(+)", 129.90351, 0, 0, 0, 0, 0, 26},
    {Isotope::Xe_131, Elements::Xe, 131, "3/2(+)", 130.90507, 0, 0, 0, 0, 0, 85},
    {Isotope::Xe_132, Elements::Xe, 132, "0(+)", 131.90414, 0, 0, 0, 0, 0, 0.45},
    {Isotope::Xe_134, Elements::Xe, 134, "0(+)", 133.9054, 0, 0, 0, 0, 0, 0.265},
    {Isotope::Xe_136, Elements::Xe, 136, "0(+)", 135.90721, 0, 0, 0, 0, 0, 0.26},
    {Isotope::Cs_Natural, Elements::Cs, 0, "7/2(+)", 132.90543, 5.42, 1.29, 3.69, 0.21, 3.9, 29},
    {Isotope::Ba_Natural, Elements::Ba, 0, "", 137.33, 5.07, 0, 3.23, 0.15, 3.38, 1.1},
    {Isotope::Ba_130, Elements::Ba, 130, "0(+)", 129.90628, -3.6, 0, 1.6, 0, 1.6, 30},
    {Isotope::Ba_132, Elements::Ba, 132, "0(+)", 131.90504, 7.8, 0, 7.6, 0, 7.6, 7},
    {Isotope::Ba_134, Elements::Ba, 134, "0(+)", 133.90449, 5.7, 0, 4.08, 0, 4.08, 2},
    {Isotope::Ba_135, Elements::Ba, 135, "3/2(+)", 134.90567, 4.67, 0, 2.74, 0.5, 3.2, 5.8},
    {Isotope::Ba_136, Elements::Ba, 136, "0(+)", 135.90455, 4.91, 0, 3.03, 0, 3.03, 0.68},
    {Isotope::Ba_137, Elements::Ba, 137, "3/2(+)", 136.90581, 6.83, 0, 5.86, 0.5, 6.4, 3.6},
    {Isotope::Ba_138, Elements::Ba, 138, "0(+)", 137.90523, 4.84, 0, 2.94, 0, 2.94, 0.27},
    {Isotope::La_Natural, Elements::La, 0, "", 138.9055, 8.24, 0, 8.53, 1.13, 9.66, 8.97},
    {Isotope::La_138, Elements::La, 138, "5(+)", 137.90711, 8, 0, 8, 0.5, 8.5, 57},
    {Isotope::La_139, Elements::La, 139, "7/2(+)", 138.90635, 8.24, 3, 8.53, 1.13, 9.66, 8.93},
    {Isotope::Ce_Natural, Elements::Ce, 0, "", 140.12, 4.84, 0, 2.94, 0, 2.94, 0.63},
    {Isotope::Ce_136, Elements::Ce, 136, "0(+)", 135.90714, 5.8, 0, 4.23, 0, 4.23, 7.3},
    {Isotope::Ce_138, Elements::Ce, 138, "0(+)", 137.90599, 6.7, 0, 5.64, 0, 5.64, 1.1},
    {Isotope::Ce_140, Elements::Ce, 140, "0(+)", 139.90543, 4.84, 0, 2.94, 0, 2.94, 0.57},
    {Isotope::Ce_142, Elements::Ce, 142, "0(+)", 141.90924, 4.75, 0, 2.84, 0, 2.84, 0.95},
    {Isotope::Pr_Natural, Elements::Pr, 0, "5/2(+)", 140.90765, 4.45, -0.54, 2.49, 0.037, 2.53, 11.5},
    {Isotope::Nd_Natural, Elements::Nd, 0, "", 144.24, 7.69, 0, 7.43, 9.2, 16.6, 50.5},
    {Isotope::Nd_142, Elements::Nd, 142, "0(+)", 141.90772, 7.7, 0, 7.5, 0, 7.5, 18.7},
    {Isotope::Nd_143, Elements::Nd, 143, "7/2(-)", 142.90981, 14, 21, 25, 55, 80, 334},
    {Isotope::Nd_144, Elements::Nd, 144, "0(+)", 143.91008, 2.8, 0, 1, 0, 1, 3.6},
    {Isotope::Nd_145, Elements::Nd, 145, "7/2(-)", 144.91257, 14, 0, 25, 5, 30, 42},
    {Isotope::Nd_146, Elements::Nd, 146, "0(+)", 145.91311, 8.7, 0, 9.5, 0, 9.5, 1.4},
    {Isotope::Nd_148, Elements::Nd, 148, "0(+)", 147.91689, 5.7, 0, 4.1, 0, 4.1, 2.5},
    {Isotope::Nd_150, Elements::Nd, 150, "0(+)", 149.92089, 5.3, 0, 3.5, 0, 3.5, 1.2},
    {Isotope::Pm_Natural, Elements::Pm, 0, "7/2(+)", 146.91514, 12.6, 3.2, 20, 1.3, 21.3, 168.4},
    {Isotope::Sm_Natural, Elements::Sm, 0, "", 150.36, 0.8, 0, 0.422, 39, 39, 5922},
    {Isotope::Sm_144, Elements::Sm, 144, "0(+)", 143.912, -3, 0, 1, 0, 1, 0.7},
    {Isotope::Sm_147, Elements::Sm, 147, "7/2(-)", 146.9149, 14, 11, 25, 14, 39, 57},
    {Isotope::Sm_148, Elements::Sm, 148, "0(+)", 147.91482, -3, 0, 1, 0, 1, 2.4},
    {Isotope::Sm_149, Elements::Sm, 149, "7/2(-)", 148.91718, -19.2, 31.4, 63.5, 137, 200, 42080},
    {Isotope::Sm_150, Elements::Sm, 150, "0(+)", 149.91727, 14, 0, 25, 0, 25, 104},
    {Isotope::Sm_152, Elements::Sm, 152, "0(+)", 151.91973, -5, 0, 3.1, 0, 3.1, 206},
    {Isotope::Sm_154, Elements::Sm, 154, "0(+)", 153.92221, 9.3, 0, 11, 0, 11, 8.4},
    {Isotope::Eu_Natural, Elements::Eu, 0, "", 151.96, 7.22, 0, 6.75, 2.5, 9.2, 4530},
    {Isotope::Eu_151, Elements::Eu, 151, "5/2(+)", 150.91985, 6.13, 4.5, 5.5, 3.1, 8.6, 9100},
    {Isotope::Eu_153, Elements::Eu, 153, "5/2(+)", 152.92123, 8.22, 0, 8.5, 1.3, 9.8, 312},
    {Isotope::Gd_Natural, Elements::Gd, 0, "", 157.25, 6.5, 0, 29.3, 151, 180, 49700},
    {Isotope::Gd_152, Elements::Gd, 152, "0(+)", 151.91979, 10, 0, 13, 0, 13, 735},
    {Isotope::Gd_154, Elements::Gd, 154, "0(+)", 153.92086, 10, 0, 13, 0, 13, 85},
    {Isotope::Gd_155, Elements::Gd, 155, "3/2(-)", 154.92262, 6, 5, 40.8, 25, 66, 61100},
    {Isotope::Gd_156, Elements::Gd, 156, "0(+", 155.92212, 6.3, 0, 5, 0, 5, 1.5},
    {Isotope::Gd_157, Elements::Gd, 157, "3/2(-)", 156.93396, -1.14, 5, 650, 394, 1044, 259000},
    {Isotope::Gd_158, Elements::Gd, 158, "0(+)", 157.9241, 9, 0, 10, 0, 10, 2.2},
    {Isotope::Gd_160, Elements::Gd, 160, "0(+)", 159.92705, 9.15, 0, 10.52, 0, 10.52, 0.77},
    {Isotope::Tb_Natural, Elements::Tb, 0, "3/2(+)", 158.92534, 7.38, -0.17, 6.84, 0.004, 6.84, 23.4},
    {Isotope::Dy_Natural, Elements::Dy, 0, "", 162.5, 16.9, 0, 35.9, 54.4, 90.3, 994},
    {Isotope::Dy_156, Elements::Dy, 156, "0(+)", 155.92528, 6.1, 0, 4.7, 0, 4.7, 33},
    {Isotope::Dy_158, Elements::Dy, 158, "0(+)", 157.9244, 6, 0, 5, 0, 5, 43},
    {Isotope::Dy_160, Elements::Dy, 160, "0(+)", 159.92519, 6.7, 0, 5.6, 0, 5.6, 56},
    {Isotope::Dy_161, Elements::Dy, 161, "5/2(+)", 160.92693, 10.3, 0, 13.3, 3, 16, 600},
    {Isotope::Dy_162, Elements::Dy, 162, "0(+)", 161.9268, -1.4, 0, 0.25, 0, 0.25, 194},
    {Isotope::Dy_163, Elements::Dy, 163, "5/2(-)", 162.92873, 5, 1.3, 3.1, 0.21, 3.3, 124},
    {Isotope::Dy_164, Elements::Dy, 164, "0(+)", 163.92917, 49.4, 0, 307, 0, 307, 2840},
    {Isotope::Ho_Natural, Elements::Ho, 0, "7/2(-)", 164.93032, 8.01, -1.7, 8.06, 0.36, 8.42, 64.7},
    {Isotope::Er_Natural, Elements::Er, 0, "", 167.26, 8.16, 0, 8.37, 0.8, 9.2, 159},
    {Isotope::Er_162, Elements::Er, 162, "0(+)", 161.92878, 7, 0, 6, 0, 6, 19},
    {Isotope::Er_164, Elements::Er, 164, "0(+)", 163.9292, 8.2, 0, 8.4, 0, 8.4, 13},
    {Isotope::Er_166, Elements::Er, 166, "0(+)", 165.93029, 10.3, 0, 13.3, 0, 13.3, 19.6},
    {Isotope::Er_167, Elements::Er, 167, "7/2(+)", 166.93205, 3.9, 1, 1.91, 0.13, 2.04, 659},
    {Isotope::Er_168, Elements::Er, 168, "0(+)", 167.93237, 8.6, 0, 9.3, 0, 9.3, 2.74},
    {Isotope::Er_170, Elements::Er, 170, "0(+)", 169.93546, 9.3, 0, 10.9, 0, 10.9, 5.8},
    {Isotope::Tm_Natural, Elements::Tm, 0, "1/2(+)", 168.93421, 7.07, 0.9, 6.28, 0.1, 6.38, 100},
    {Isotope::Yb_Natural, Elements::Yb, 0, "", 173.04, 12.43, 0, 19.42, 4, 23.4, 34.8},
    {Isotope::Yb_168, Elements::Yb, 168, "0(+)", 167.93389, -4.07, 0, 2.13, 0, 2.13, 2230},
    {Isotope::Yb_170, Elements::Yb, 170, "0(+)", 169.93476, 6.77, 0, 5.8, 0, 5.8, 11.4},
    {Isotope::Yb_171, Elements::Yb, 171, "1/2(-)", 170.93632, 9.66, -5.59, 11.7, 3.9, 15.6, 48.6},
    {Isotope::Yb_172, Elements::Yb, 172, "0(+)", 171.93638, 9.43, 0, 11.2, 0, 11.2, 0.8},
    {Isotope::Yb_173, Elements::Yb, 173, "5/2(-)", 172.93821, 9.56, -5.3, 11.5, 3.5, 15, 17.1},
    {Isotope::Yb_174, Elements::Yb, 174, "0(+)", 173.93886, 19.3, 0, 46.8, 0, 46.8, 69.4},
    {Isotope::Yb_176, Elements::Yb, 176, "0(+)", 175.94256, 8.72, 0, 9.6, 0, 9.6, 2.85},
    {Isotope::Lu_Natural, Elements::Lu, 0, "", 174.967, 7.21, 0, 6.53, 0.7, 7.2, 74},
    {Isotope::Lu_175, Elements::Lu, 175, "7/2(+)", 174.94077, 7.24, 0, 6.59, 0.6, 7.2, 21},
    {Isotope::Lu_176, Elements::Lu, 176, "7(-)", 175.94268, 6.1, 0, 4.7, 1.2, 5.9, 2065},
    {Isotope::Hf_Natural, Elements::Hf, 0, "", 178.49, 7.77, 0, 7.6, 2.6, 10.2, 104.1},
    {Isotope::Hf_174, Elements::Hf, 174, "0(+)", 173.94004, 10.9, 0, 15, 0, 15, 561},
    {Isotope::Hf_176, Elements::Hf, 176, "0(+)", 175.94141, 6.61, 0, 5.5, 0, 5.5, 23.5},
    {Isotope::Hf_177, Elements::Hf, 177, "7/2(-)", 176.94322, 0.8, 0.9, 0.1, 0.1, 0.2, 373},
    {Isotope::Hf_178, Elements::Hf, 178, "0(+)", 177.9437, 5.9, 0, 4.4, 0, 4.4, 84},
    {Isotope::Hf_179, Elements::Hf, 179, "9/2(+)", 178.94581, 7.46, 0, 7, 0.14, 7.1, 41},
    {Isotope::Hf_180, Elements::Hf, 180, "0(+)", 179.94655, 13.2, 0, 21.9, 0, 21.9, 13.04},
    {Isotope::Ta_Natural, Elements::Ta, 0, "", 180.9479, 6.91, 0, 6, 0.01, 6.01, 20.6},
    {Isotope::Ta_180, Elements::Ta, 180, "9(-)", 179.94746, 7, 0, 6.2, 0.5, 7, 563},
    {Isotope::Ta_181, Elements::Ta, 181, "7/2(+)", 180.94799, 6.91, -0.29, 6, 0.011, 6.01, 20.5},
    {Isotope::W_Natural, Elements::W, 0, "", 183.85, 4.86, 0, 2.97, 1.63, 4.6, 18.3},
    {Isotope::W_180, Elements::W, 180, "0(+)", 179.9467, 5, 0, 3, 0, 3, 30},
    {Isotope::W_182, Elements::W, 182, "0(+)", 181.9482, 6.97, 0, 6.1, 0, 6.1, 20.7},
    {Isotope::W_183, Elements::W, 183, "1/2(-)", 182.95022, 6.53, 0, 5.36, 0.3, 5.7, 10.1},
    {Isotope::W_184, Elements::W, 184, "0(+)", 183.95093, 7.48, 0, 7.03, 0, 7.03, 1.7},
    {Isotope::W_186, Elements::W, 186, "0(+)", 185.95436, -0.72, 0, 0.065, 0, 0.065, 37.9},
    {Isotope::Re_Natural, Elements::Re, 0, "", 186.207, 9.2, 0, 10.6, 0.9, 11.5, 89.7},
    {Isotope::Re_185, Elements::Re, 185, "5/2(+)", 184.95295, 9, 2, 10.2, 0.5, 10.7, 112},
    {Isotope::Re_187, Elements::Re, 187, "5/2(+)", 186.95574, 9.3, 2.8, 10.9, 1, 11.9, 76},
    {Isotope::Os_Natural, Elements::Os, 0, "", 190.2, 10.7, 0, 14.4, 0.3, 14.7, 16},
    {Isotope::Os_184, Elements::Os, 184, "0(+)", 183.95249, 10, 0, 13, 0, 13, 3000},
    {Isotope::Os_186, Elements::Os, 186, "0(+)", 185.95383, 11.6, 0, 17, 0, 17, 80},
    {Isotope::Os_187, Elements::Os, 187, "1/2(-)", 186.95574, 10, 0, 13, 0.3, 13, 320},
    {Isotope::Os_188, Elements::Os, 188, "0(+)", 187.95586, 7.6, 0, 7.3, 0, 7.3, 4.7},
    {Isotope::Os_189, Elements::Os, 189, "3/2(-)", 188.95814, 10.7, 0, 14.4, 0.5, 14.9, 25},
    {Isotope::Os_190, Elements::Os, 190, "0(+)", 189.95844, 11, 0, 15.2, 0, 15.2, 13.1},
    {Isotope::Os_192, Elements::Os, 192, "0(+)", 191.96147, 11.5, 0, 16.6, 0, 16.6, 2},
    {Isotope::Ir_Natural, Elements::Ir, 0, "", 192.22, 10.6, 0, 14.1, 0, 14, 425},
    {Isotope::Ir_191, Elements::Ir, 191, "3/2(+)", 190.96058, 0, 0, 0, 0, 0, 954},
    {Isotope::Ir_193, Elements::Ir, 193, "3/2(+)", 192.96292, 0, 0, 0, 0, 0, 111},
    {Isotope::Pt_Natural, Elements::Pt, 0, "", 195.08, 9.6, 0, 11.58, 0.13, 11.71, 10.3},
    {Isotope::Pt_190, Elements::Pt, 190, "0(+)", 189.95992, 9, 0, 10, 0, 10, 152},
    {Isotope::Pt_192, Elements::Pt, 192, "0(+)", 191.96102, 9.9, 0, 12.3, 0, 12.3, 10},
    {Isotope::Pt_194, Elements::Pt, 194, "0(+)", 193.96266, 10.55, 0, 14, 0, 14, 1.44},
    {Isotope::Pt_195, Elements::Pt, 195, "1/2(-)", 194.96477, 8.83, -1, 9.8, 0.13, 9.9, 27.5},
    {Isotope::Pt_196, Elements::Pt, 196, "0(+)", 195.96493, 9.89, 0, 12.3, 0, 12.3, 0.72},
    {Isotope::Pt_198, Elements::Pt, 198, "0(+)", 197.96787, 7.8, 0, 7.6, 0, 7.6, 3.66},
    {Isotope::Au_Natural, Elements::Au, 0, "3/2(+)", 196.96654, 7.63, -1.84, 7.32, 0.43, 7.75, 98.65},
    {Isotope::Hg_Natural, Elements::Hg, 0, "", 200.59, 12.692, 0, 20.24, 6.6, 26.8, 372.3},
    {Isotope::Hg_196, Elements::Hg, 196, "0(+)", 195.96581, 30.3, 0, 115, 0, 115, 3080},
    {Isotope::Hg_198, Elements::Hg, 198, "0(+)", 197.96674, 0, 0, 0, 0, 0, 2},
    {Isotope::Hg_199, Elements::Hg, 199, "1/2(-)", 198.96825, 16.9, 15.5, 36, 30, 66, 2150},
    {Isotope::Hg_200, Elements::Hg, 200, "0(+)", 199.9683, 0, 0, 0, 0, 0, 60},
    {Isotope::Hg_201, Elements::Hg, 201, "3/2(-)", 200.97028, 0, 0, 0, 0, 0, 7.8},
    {Isotope::Hg_202, Elements::Hg, 202, "0(+)", 201.97062, 0, 0, 0, 0, 0, 4.89},
    {Isotope::Hg_204, Elements::Hg, 204, "0(+)", 203.97347, 0, 0, 0, 0, 0, 0.43},
    {Isotope::Tl_Natural, Elements::Tl, 0, "", 204.383, 8.776, 0, 9.678, 0.21, 9.89, 3.43},
    {Isotope::Tl_203, Elements::Tl, 203, "1/2(+)", 202.97232, 6.99, 1.06, 6.14, 0.14, 6.28, 11.4},
    {Isotope::Tl_205, Elements::Tl, 205, "1/2(+)", 204.9744, 9.52, -0.242, 11.39, 0.007, 11.4, 0.104},
    {Isotope::Pb_Natural, Elements::Pb, 0, "", 207.2, 9.405, 0, 11.115, 0.003, 11.118, 0.171},
    {Isotope::Pb_204, Elements::Pb, 204, "0(+)", 203.97302, 10.6, 0, 14, 0, 14, 0.65},
    {Isotope::Pb_206, Elements::Pb, 206, "0(+)", 205.97444, 9.23, 0, 10.71, 0, 10.71, 0.03},
    {Isotope::Pb_207, Elements::Pb, 207, "1/2(-)", 206.97587, 9.28, 0.14, 10.82, 0.002, 10.82, 0.699},
    {Isotope::Pb_208, Elements::Pb, 208, "0(+)", 207.97663, 9.5, 0, 11.34, 0, 11.34, 0.00048},
    {Isotope::Bi_Natural, Elements::Bi, 0, "9/2(-)", 208.98037, 8.532, 0.259, 9.148, 0.0084, 9.156, 0.0338},
    {Isotope::Ra_Natural, Elements::Ra, 0, "0(+)", 226.0254, 10, 0, 13, 0, 13, 12.8},
    {Isotope::Th_Natural, Elements::Th, 0, "0(+)", 232.03805, 10.52, 0, 13.91, 0, 13.91, 7.37},
    {Isotope::Pa_Natural, Elements::Pa, 0, "3/2(-)", 231.03588, 9.1, 0, 10.4, 0.1, 10.5, 200.6},
    {Isotope::U_Natural, Elements::U, 0, "", 238.0289, 8.417, 0, 8.903, 0.005, 8.908, 7.57},
    {Isotope::U_233, Elements::U, 233, "5/2(+)", 233.03963, 10.1, 0, 12.8, 0.1, 12.9, 574},
    {Isotope::U_234, Elements::U, 234, "0(+)", 234.04095, 12.4, 0, 19.3, 0, 19.3, 100.1},
    {Isotope::U_235, Elements::U, 235, "7/2(-)", 235.04392, 10.47, 0, 13.78, 0.2, 14, 680.9},
    {Isotope::U_238, Elements::U, 238, "0(+)", 238.05078, 8.402, 0, 8.871, 0, 8.871, 2.68},
    {Isotope::Np_Natural, Elements::Np, 0, "5/2(+)", 237.04817, 10.55, 0, 14, 0, 14.5, 175.9},
    {Isotope::Pu_Natural, Elements::Pu, 0, "0(+)", 238.04955, 14.1, 0, 25, 0, 25, 558},
    {Isotope::Pu_239, Elements::Pu, 239, "1/2(+)", 239.05216, 7.7, 0, 7.5, 0.2, 7.7, 1017},
    {Isotope::Pu_240, Elements::Pu, 240, "0(+)", 240.05381, 3.5, 0, 1.54, 0, 1.54, 289.6},
    {Isotope::Pu_242, Elements::Pu, 242, "0(+)", 242.05874, 8.1, 0, 8.2, 0, 8.2, 18.5},
    {Isotope::Am_Natural, Elements::Am, 0, "5/2(-)", 243.06138, 8.3, 0, 8.7, 0.3, 9, 75},
    {Isotope::Am_244, Elements::Am, 244, "0(+)", 244.06275, 9.5, 0, 11.3, 0, 11.3, 16.2},
    {Isotope::Am_246, Elements::Am, 246, "0(+)", 246.06722, 9.3, 0, 10.9, 0, 10.9, 1.36},
    {Isotope::Am_248, Elements::Am, 248, "0(+)", 248.07234, 7.7, 0, 7.5, 0, 7.5, 3},
    {Isotope::Phantom_Natural, Elements::Phantom, 0, "", 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};

/*
 * IsotopeData
 */

IsotopeData::IsotopeData(Sears91::Isotope tope, Elements::Element Z, int A, std::string_view spin, double mass, double bc,
                         double bi, double sc, double si, double totalxs, double absxs)
    : isotope_(tope), Z_(Z), A_(A), mass_(mass), spin_(spin), boundCoherent_(bc), boundIncoherent_(bi), boundCoherentXS_(sc),
      boundIncoherentXS_(si), totalXS_(totalxs), absorptionXS_(absxs)
{
}

// Return isotope
Sears91::Isotope IsotopeData::isotope() const { return isotope_; }

// Return element (Z) of Isotope
Elements::Element IsotopeData::Z() const { return Z_; }

// Return mass number (A) of Isotope
int IsotopeData::A() const { return A_; }

// Return isotope mass (given C = 12)
double IsotopeData::mass() const { return mass_; }

// Return bound coherent scattering length (fm)
double IsotopeData::boundCoherent() const { return boundCoherent_; }

// Return bound incoherent scattering length (fm)
double IsotopeData::boundIncoherent() const { return boundIncoherent_; }

// Return bound coherent scattering cross section (barn)
double IsotopeData::boundCoherentXS() const { return boundCoherentXS_; }

// Return bound incoherent scattering cross section (barn)
double IsotopeData::boundIncoherentXS() const { return boundIncoherentXS_; }

// Return total bound scattering cross section (barn)
double IsotopeData::totalXS() const { return totalXS_; }

// Return absorption cross section for thermal (2200 m/s) neutron (barn)
double IsotopeData::absorptionXS() const { return absorptionXS_; }

// Return whether Isotope with specified A exists for the given element
bool hasIsotope(Elements::Element Z, int A)
{
    return std::find_if(sears91Data_.begin(), sears91Data_.end(),
                        [Z, A](const auto &topeData) { return topeData.Z() == Z && topeData.A() == A; }) != sears91Data_.end();
}

// Return all isotope enums corresponding to element
std::vector<Isotope> isotopes(Elements::Element Z)
{
    std::vector<Isotope> topes;
    for (const auto &tope : sears91Data_)
        if (tope.Z() == Z)
            topes.push_back(tope.isotope());
    return topes;
}

// Return isotope enum corresponding element and A (if it exists)
Isotope isotope(Elements::Element Z, int A)
{
    auto it = std::find_if(sears91Data_.begin(), sears91Data_.end(),
                           [Z, A](const auto &topeData) { return topeData.Z() == Z && topeData.A() == A; });
    if (it == sears91Data_.end())
        throw(std::runtime_error(fmt::format("No isotope with A = {} available for element {}.\n", A, Elements::symbol(Z))));

    return it->isotope();
}

// Return natural isotope for element (if it exists)
Isotope naturalIsotope(Elements::Element Z)
{
    auto it = std::find_if(sears91Data_.begin(), sears91Data_.end(),
                           [Z](const auto &topeData) { return topeData.Z() == Z && topeData.A() == 0; });
    if (it == sears91Data_.end())
        throw(
            std::runtime_error(fmt::format("No natural isotope information available for element {}.\n", Elements::symbol(Z))));

    return it->isotope();
}

// Return element (Z) of specified isotope
Elements::Element Z(Isotope tope) { return sears91Data_[tope].Z(); }

// Return mass number (A) of specified isotope
int A(Isotope tope) { return sears91Data_[tope].A(); }

// Return mass (given C = 12) of specified isotope
double mass(Isotope tope) { return sears91Data_[tope].mass(); }

// Return bound coherent scattering length (fm) of specified isotope
double boundCoherent(Isotope tope) { return sears91Data_[tope].boundCoherent(); }

// Return bound incoherent scattering length (fm) of specified isotope
double boundIncoherent(Isotope tope) { return sears91Data_[tope].boundIncoherent(); }

// Return bound coherent scattering cross section (barn) of specified isotope
double boundCoherentXS(Isotope tope) { return sears91Data_[tope].boundCoherentXS(); }

// Return bound incoherent scattering cross section (barn) of specified isotope
double boundIncoherentXS(Isotope tope) { return sears91Data_[tope].boundIncoherentXS(); }

// Return total bound scattering cross section (barn) of specified isotope
double totalXS(Isotope tope) { return sears91Data_[tope].totalXS(); }

// Return absorption cross section for thermal (2200 m/s) neutron (barn) of specified isotope
double absorptionXS(Isotope tope) { return sears91Data_[tope].absorptionXS(); }

}; // namespace Sears91
