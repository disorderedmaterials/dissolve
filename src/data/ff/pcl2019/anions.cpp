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
    addAtomType(ELEMENT_B, 1, "B", "-F(n=4)", "Tetrafluoroborate boron", 0.96, "B");
    addAtomType(ELEMENT_F, 2, "FB", "-&1", "Tetrafluoroborate fluorine", -0.49, "F");
    // hexafluorophosphate JCSPerkin2 (1999) 2365
    addAtomType(ELEMENT_P, 3, "P", "-F(n=6)", "Hexafluorophosphate phosphorus", 1.34, "P");
    addAtomType(ELEMENT_F, 4, "FP", "-&3", "Hexafluorophosphate fluorine", -0.39, "F");
    // chloride JPCB 108 (2004) 2038
    addAtomType(ELEMENT_CL, 5, "Cl", "nbonds=0", "Chloride", -1.0, "Cl");
    // bromide JPCB 110 (2006) 19586
    addAtomType(ELEMENT_BR, 6, "Br", "nbonds=0", "Bromide", -1.0, "Br");
    // bistriflamide JPCB 108 (2004) 16893, PCCP 43 (2017) 29617
    addAtomType(ELEMENT_C, 7, "CBT", "-F(n=3),-S(-O(n>=2)", "Fluorosulphonyl carbon", 0.35, "CF");
    addAtomType(ELEMENT_S, 8, "SBT", "nbonds=4,-O(n>=2),-[N,O],-[C,F]", "Fluorosulphonyl sulphur", 1.02, "SB");
    addAtomType(ELEMENT_N, 9, "NBT", "nbonds=2,-&8(n=2)", "Bridging nitrogen in bis-fluorosulphonyls", -0.66, "NB");
    addAtomType(ELEMENT_O, 10, "OBT", "nbonds=1,-S(-O(n=2))", "Sulphonyl oxygen", -0.53, "OB");
    addAtomType(ELEMENT_F, 11, "F1", "nbonds=1,-C(-S)", "Fluorine on carbon next to sulphonyl / sulphonate", -0.16, "FB");
    // triflate PCCP 43 (2017) 29617
    addAtomType(ELEMENT_O, 12, "OTF", "nbonds=1,-S(-O(n=3))", "Sulphonate oxygen", -0.63, "OB");
    // longer perfluoroalkanesulphonylamides
    addAtomType(ELEMENT_C, 13, "C1F", "nbonds=4,-F(n=2),-S,-C", "First fluorinated carbon in ethylsulphonyl chain", 0.19, "CF");
    addAtomType(ELEMENT_C, 14, "CEF", "nbonds=4,-F(n=3),-C(-S)", "Terminal fluorinated carbon in ethylsulphonyl chain", 0.36,
                "CF");
    // bis(fluorosulphonyl)amide PCCP 43 (2017) 29617
    addAtomType(ELEMENT_F, 15, "FSI", "", "", -0.13, "FB");
    // dicyanamide JPCB 110 (2006) 19586
    addAtomType(ELEMENT_N, 16, "N3A", "-C(nbonds=2,-N(nbonds=1),n=2)", "Dicyanamide nitrogen", -0.76, "N3");
    addAtomType(ELEMENT_C, 17, "CZA", "-&16", "Dicyanamide carbon", 0.64, "CZ");
    addAtomType(ELEMENT_N, 18, "NZA", "-&17", "Dicyanamide nitrogen", -0.76, "NZ");
    // acetate OPLS-AA
    addAtomType(ELEMENT_C, 19, "CO2", "nbonds=3,-O(n=2),-C(nbonds=4)", "Acetate carboxyl carbon", 0.70, "CO");
    addAtomType(ELEMENT_O, 20, "O2", "-&19,-C(nh=3)", "Acetate oxygen", -0.80, "O2");
    addAtomType(ELEMENT_C, 21, "CTA", "nh=3,-&19", "Acetate methyl carbon", -0.28, "CT");
    // trifluorocetate PCCP 17 (2015) 22321
    addAtomType(ELEMENT_O, 22, "O2F", "-&19,-C(-F(n=3))", "Trifluoroacetate carboxyl carbon", -0.75, "OC_a");
    addAtomType(ELEMENT_C, 23, "CFA", "-&19,-F(n=3)", "Trifluoroacetate fluoromethyl carbon", 0.40, "CF");
    addAtomType(ELEMENT_F, 24, "FFA", "-&23", "Trifluoroacetate fluorine", -0.20, "F");
    // alkylsulphates JPCB 112 (2008) 5039
    addAtomType(ELEMENT_S, 25, "SO", "-O(n=3),-[C,O]", "Alkylsulphate sulphur", 1.18, "SO");
    addAtomType(ELEMENT_O, 26, "OS4", "-&25(-O(-C))", "Alkylsulphate oxygen", -0.65, "OS");
    addAtomType(ELEMENT_O, 27, "OC4", "-C(nh=3),-S(-O(n=4))", "Alkylsulphate bridging oxygen", -0.45, "OC");
    addAtomType(ELEMENT_C, 28, "CS4", "nh=3,-O(-&25)", "Alkylsulphate methyl carbon", 0.22, "CT");
    addAtomType(ELEMENT_H, 29, "HS4", "-&28", "Alkylsulphate methyl hydrogen", 0.00, "HC");
    // alkylsulphonates JPCB 112 (2008) 5039
    addAtomType(ELEMENT_O, 30, "OS3", "-&25(-C)", "Alkylsulphonate oxygen", -0.68, "OS");
    addAtomType(ELEMENT_C, 31, "CS3", "nh=3,-&25", "Alkylsulphonate methyl carbon", -0.14, "CT");
    addAtomType(ELEMENT_H, 32, "HS3", "-&31", "Alkylsulphonate methyl hydrogen", 0.00, "HC");
    // thiocyanate JCP 128 (2008) 154504
    addAtomType(ELEMENT_S, 33, "SK", "nbonds=1,-C(nbonds=2,-N)", "Thiocyanate sulphur", -0.72, "SK");
    addAtomType(ELEMENT_C, 34, "CK", "-&33", "Thiocyanate carbon", 0.44, "CK");
    addAtomType(ELEMENT_N, 35, "NK", "-&34", "Thiocyanate nitrogen", -0.72, "NK");
    // tricyanomethanide OPLS nitriles, our charges
    addAtomType(ELEMENT_C, 36, "C3A", "-C(nbonds=2,-N,n=3)", "Tricyanomethanide central carbon", -0.910, "C3A");
    addAtomType(ELEMENT_C, 37, "CN", "-&36", "Tricyanomethanide cyano carbon", 0.655, "CN");
    addAtomType(ELEMENT_N, 38, "NC", "-&37", "Tricyanomethanide cyano nitrogen", -0.685, "NC");
    // tosylate OPLS toluene, SO3- JPCB 112 (2008) 5039, our charges
    addAtomType(ELEMENT_C, 39, "CAPS", "-&25,ring(size=6,C(n=6)),nbonds=3", "Tosylate para carbon", -0.140, "CA");
    addAtomType(ELEMENT_C, 40, "CAMS", "-&39,ring(size=6,C(n=6)),nbonds=3,nh=1", "Tosylate meta carbon", 0.000, "CA");
    addAtomType(ELEMENT_C, 41, "CAOS", "-&40,ring(size=6,C(n=6)),nbonds=3,nh=1", "Tosylate ortho carbon", -0.270, "CA");
    addAtomType(ELEMENT_C, 42, "CATS", "-&41(n=2),ring(size=6,C(n=6)),nbonds=3,nh=0", "Tosylate ring carbon", 0.157, "CA");
    addAtomType(ELEMENT_C, 43, "CTTS", "-&42,nh=3", "Tosylate methyl carbon", -0.197, "CT");
    addAtomType(ELEMENT_H, 44, "HATS", "-[&40,&41]", "Tosylate ring hydrogen", 0.100, "HA");
    addAtomType(ELEMENT_H, 45, "HTS", "-&43", "Tosylate methyl carbon", 0.060, "HC");

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
    static std::string desc =
        fmt::format("Ionic liquid anions forcefield of Padua & Canongia Lopes (and contributors), 19 June 2019 version, "
                    "covering .<br/><br/>References: {}",
                    publicationReferences());
    return desc;
}
