// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "data/ff/pcl2019/cations.h"

/*
 * Set Up
 */

//  Set up / create all forcefield terms
bool Forcefield_PCL2019_Cations::setUp()
{
    // Create atom types
    // dialkylimidazolium JPCB 108 (2004) 2038
    addAtomType(ELEMENT_C, 1, "C1", "", "", -0.17, "CT");
    // C1A - only for polarisable FF
    addAtomType(ELEMENT_C, 2, "C1A", "", "", -0.17, "CT");
    addAtomType(ELEMENT_C, 3, "C2", "", "", 0.01, "CT");
    addAtomType(ELEMENT_C, 4, "CE", "", "", -0.05, "CT");
    addAtomType(ELEMENT_C, 5, "CS", "", "", -0.12, "CT");
    addAtomType(ELEMENT_C, 6, "CT", "", "", -0.18, "CT");
    addAtomType(ELEMENT_C, 7, "CR", "", "", -0.11, "CR");
    addAtomType(ELEMENT_C, 8, "CW", "", "", -0.13, "CW");
    addAtomType(ELEMENT_H, 9, "HCR", "", "", 0.21, "HA");
    addAtomType(ELEMENT_H, 10, "HCW", "", "", 0.21, "HA");
    addAtomType(ELEMENT_H, 11, "HC", "", "", 0.06, "HC");
    addAtomType(ELEMENT_H, 12, "H1", "", "", 0.13, "HC");
    addAtomType(ELEMENT_N, 13, "NA", "", "", 0.15, "NA");
    // dialkylmethylmidazolium JPCB 112 (2008) 5039
    addAtomType(ELEMENT_C, 14, "CRM", "", "", 0.19, "CR");
    addAtomType(ELEMENT_N, 15, "NAM", "", "", 0.04, "NA");
    addAtomType(ELEMENT_C, 16, "CCR", "", "", -0.26, "CT");
    // aromatic OPLS-AA
    addAtomType(ELEMENT_C, 17, "CA", "", "", -0.115, "CA");
    addAtomType(ELEMENT_H, 18, "HA", "", "", 0.115, "HA");
    // benzylimidazolium JPCB 117 (2013) 7416 (OPLS-AA benzyl ok)
    addAtomType(ELEMENT_C, 19, "C1T", "", "", 0.039, "CT");
    // toluene JACS 118 (1996) 11217; JCC 25 (2004) 1322
    addAtomType(ELEMENT_C, 20, "CAP", "", "", -0.154, "CA");
    addAtomType(ELEMENT_C, 21, "CAM", "", "", -0.145, "CA");
    addAtomType(ELEMENT_C, 22, "CAO", "", "", -0.147, "CA");
    addAtomType(ELEMENT_C, 23, "CAT", "", "", -0.081, "CA");
    // CTT   CT  ELEMENT_C  -0.197  lj    3.50   0.27614
    addAtomType(ELEMENT_H, 24, "HAT", "", "", 0.148, "HA");
    addAtomType(ELEMENT_H, 25, "HT", "", "", 0.092, "HC");
    // pyridinium JPCB 110 (2006) 19586
    addAtomType(ELEMENT_N, 26, "NAP", "", "", 0.15, "NA");
    addAtomType(ELEMENT_C, 27, "CAPO", "", "", 0.00, "CA");
    addAtomType(ELEMENT_C, 28, "CAPM", "", "", -0.07, "CA");
    addAtomType(ELEMENT_C, 29, "CAPP", "", "", 0.02, "CA");
    addAtomType(ELEMENT_H, 30, "HAP", "", "", 0.15, "HA");
    // isoquinolinium
    addAtomType(ELEMENT_N, 31, "NAQ", "", "", 0.305, "NA");
    addAtomType(ELEMENT_C, 32, "CAQ", "", "", 0.185, "CA");
    // imidazolium fluoroalkyl JPCB 114 (2010) 3592
    // fluoroalkyl OPLS-AA JPCA 105 (2001) 4118 
    addAtomType(ELEMENT_C, 33, "C1H", "", "", -0.05, "CT");
    addAtomType(ELEMENT_C, 34, "CFH", "", "", 0.12, "CF");
    addAtomType(ELEMENT_C, 35, "CTF", "", "", 0.36, "CF");
    addAtomType(ELEMENT_C, 36, "CSF", "", "", 0.24, "CF");
    addAtomType(ELEMENT_F, 37, "F", "", "", -0.12, "F");
    // ammonium, pyrrolidinium JPCB 108 (2004) 16893
    addAtomType(ELEMENT_N, 38, "N4", "", "", 0.12, "NT");
    addAtomType(ELEMENT_N, 39, "N3", "", "", 0.03, "NT");
    addAtomType(ELEMENT_H, 40, "H3", "", "", 0.31, "HN");
    // alcohols OPLS-AA JACS 118 (1996) 11225; JPC 100 (1996) 18010
    addAtomType(ELEMENT_C, 41, "CTO", "", "", 0.145, "CT");  
    addAtomType(ELEMENT_H, 42, "H1O", "", "", 0.040, "HC");
    addAtomType(ELEMENT_O, 43, "OH", "", "", -0.683, "OH");
    addAtomType(ELEMENT_H, 44, "HO", "", "", 0.418, "HO");
    // alkoxy imidazolium JPCB 114 (2010) 3595
    addAtomType(ELEMENT_C, 45, "C2O", "", "", 0.275, "CT");
    // cholinium same as OPLS-AA alcohols or ammonium (Ferid, Agilio)
    addAtomType(ELEMENT_C, 46, "COL", "", "", 0.275, "CT");
    // guanidinium
    addAtomType(ELEMENT_N, 47, "NG", "", "", -0.94, "NG");
    addAtomType(ELEMENT_C, 48, "CG", "", "", 1.06, "CG");
    addAtomType(ELEMENT_H, 49, "HG", "", "", 0.46, "HG");
    // phosphonium OPLS-AA, JPCB 110 (2006) 19586
    addAtomType(ELEMENT_P, 50, "P3", "", "", 0.68, "PT");
    addAtomType(ELEMENT_C, 51, "C1P", "", "", -0.31, "CT");
    // lithium cation Aqvist
    addAtomType(ELEMENT_LI, 52, "Li", "", "", 1.00, "Li");
    // sodium cation
    addAtomType(ELEMENT_NA, 53, "Na", "", "", 1.00, "Na");

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_PCL2019_Cations::name() const { return "PCL2019/Cations"; }

// Return description for Forcefield
std::string_view Forcefield_PCL2019_Cations::description() const
{
    static std::string desc = fmt::format("Ionic liquid cations forcefield of Padua & Canongia Lopes (and contributors), 19 June 2019 "
                           "version, covering .<br/><br/>References: {}",
                           publicationReferences());
    return desc;
}
