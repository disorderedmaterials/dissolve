// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "data/ff/pcl2019/anions.h"

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_PCL2019_Anions::setUp()
{
    // Create atom types
    // tetrafluoroborate anion
    addAtomType(ELEMENT_B, 1, "B", "", "", 0.96, "B");
    addAtomType(ELEMENT_F, 2, "FB", "", "", -0.49, "F");
    // hexafluorophosphate JCSPerkin2 (1999) 2365
    addAtomType(ELEMENT_P, 3, "P", "", "", 1.34, "P");
    addAtomType(ELEMENT_F, 4, "FP", "", "", -0.39, "F");
    // chloride JPCB 108 (2004) 2038
    addAtomType(ELEMENT_CL, 5, "Cl", "", "", -1.0, "Cl");
    // bromide JPCB 110 (2006) 19586
    addAtomType(ELEMENT_BR, 6, "Br", "", "", -1.0, "Br");
    // bistriflamide JPCB 108 (2004) 16893, PCCP 43 (2017) 29617
    addAtomType(ELEMENT_C, 7, "CBT", "", "", 0.35, "CF");
    addAtomType(ELEMENT_S, 8, "SBT", "", "", 1.02, "SB");
    addAtomType(ELEMENT_N, 9, "NBT", "", "", -0.66, "NB");
    addAtomType(ELEMENT_O, 10, "OBT", "", "", -0.53, "OB");
    addAtomType(ELEMENT_F, 11, "F1", "", "", -0.16, "FB");
    // triflate PCCP 43 (2017) 29617
    addAtomType(ELEMENT_O, 12, "OTF", "", "", -0.63, "OB");
    // longer perfluoroalkanesulfonylamides
    addAtomType(ELEMENT_C, 13, "C1F", "", "", 0.19, "CF");
    addAtomType(ELEMENT_C, 14, "CEF", "", "", 0.36, "CF");
    // bis(fluorosulfonyl)amide PCCP 43 (2017) 29617
    addAtomType(ELEMENT_F, 15, "FSI", "", "", -0.13, "FB");
    // dicyanamide JPCB 110 (2006) 19586
    addAtomType(ELEMENT_N, 16, "N3A", "", "", -0.76, "N3");
    addAtomType(ELEMENT_C, 17, "CZA", "", "", 0.64, "CZ");
    addAtomType(ELEMENT_N, 18, "NZA", "", "", -0.76, "NZ");
    // acetate OPLS-AA
    addAtomType(ELEMENT_O, 19, "O2", "", "", -0.80, "O2");
    addAtomType(ELEMENT_C, 20, "CO2", "", "", 0.70, "CO");
    addAtomType(ELEMENT_C, 21, "CTA", "", "", -0.28, "CT");
    // trifluorocetate PCCP 17 (2015) 22321
    addAtomType(ELEMENT_O, 22, "O2F", "", "", -0.75, "OC_a");
    addAtomType(ELEMENT_C, 23, "CFA", "", "", 0.40, "CF");
    addAtomType(ELEMENT_F, 24, "FFA", "", "", -0.20, "F");
    // alkylsulfates JPCB 112 (2008) 5039
    addAtomType(ELEMENT_S, 25, "SO", "", "", 1.18, "SO");
    addAtomType(ELEMENT_O, 26, "OS4", "", "", -0.65, "OS");
    addAtomType(ELEMENT_O, 27, "OC4", "", "", -0.45, "OC");
    addAtomType(ELEMENT_C, 28, "CS4", "", "", 0.22, "CT");
    addAtomType(ELEMENT_H, 29, "HS4", "", "", 0.00, "HC");
    // alkylsulfonates JPCB 112 (2008) 5039
    addAtomType(ELEMENT_O, 30, "OS3", "", "", -0.68, "OS");
    addAtomType(ELEMENT_C, 31, "CS3", "", "", -0.14, "CT");
    addAtomType(ELEMENT_H, 32, "HS3", "", "", 0.00, "HC");
    // thiocyanate JCP 128 (2008) 154504
    addAtomType(ELEMENT_S, 33, "SK", "", "", -0.72, "SK");
    addAtomType(ELEMENT_C, 34, "CK", "", "", 0.44, "CK");
    addAtomType(ELEMENT_N, 35, "NK", "", "", -0.72, "NK");
    // tricyanomethanide OPLS nitriles, our charges
    addAtomType(ELEMENT_C, 36, "C3A", "", "", -0.910, "C3A");
    addAtomType(ELEMENT_C, 37, "CN", "", "", 0.655, "CN");
    addAtomType(ELEMENT_N, 38, "NC", "", "", -0.685, "NC");
    // tosylate OPLS toluene, SO3- JPCB 112 (2008) 5039, our charges
    addAtomType(ELEMENT_C, 39, "CAPS", "", "", -0.140, "CA");
    addAtomType(ELEMENT_C, 40, "CAMS", "", "", 0.000, "CA");
    addAtomType(ELEMENT_C, 41, "CAOS", "", "", -0.270, "CA");
    addAtomType(ELEMENT_C, 42, "CATS", "", "", 0.157, "CA");
    addAtomType(ELEMENT_C, 43, "CTTS", "", "", -0.197, "CT");
    addAtomType(ELEMENT_H, 44, "HATS", "", "", 0.100, "HA");
    addAtomType(ELEMENT_H, 45, "HTS", "", "", 0.060, "HC");

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_PCL2019_Anions::name() const { return "PCL2019/Anions"; }

// Return description for Forcefield
std::string_view Forcefield_PCL2019_Anions::description() const
{
    static std::string desc = fmt::format("Ionic liquid anions forcefield of Padua & Canongia Lopes (and contributors), 19 June 2019 version, "
                           "covering .<br/><br/>References: {}",
                           publicationReferences());
    return desc;
}
