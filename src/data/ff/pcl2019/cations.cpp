// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "data/ff/pcl2019/cations.h"

/*
 * Set Up
 */

//  Set up / create all forcefield terms
bool Forcefield_PCL2019_Cations::setUp()
{
    /*
     * Implementation Notes:
     *
     * - Ordering of atom types has been changed in places to facilitate NETA descriptions.
     * - Alkylimidazolium H on ring nitrogen mapped as HA in original forcefield, but is remapped as HN here to give zero LJ
     * parameters.
     * - CW carbon next to ring nitrogen with H attached has different LJ parameters to actual CW - mapped to CWH here.
     * - OPLS-AA alcohol carbon (CTO) and hydrogen (H10) not implemented - see OPLSAA2005/Alcohols for the full description.
     * - Phosphonium P renamed from P3 to P4 for consistency with N4 and
     * https://github.com/agiliopadua/ilff/blob/master/ilff_structures.pdf
     * - C1A (polarisable FF only) implemented specifically for pyrrolidinium cation
     */

    // Create atom types
    // dialkylimidazolium JPCB 108 (2004) 2038
    addAtomType(ELEMENT_C, 1, "C1", "nbonds=4,-N,-H,-H", "First tetrahedral carbon in R group on ring nitrogen", -0.17, "CT");
    // C1A - only for polarisable FF - implemented here to cover pyrrolidinium
    addAtomType(ELEMENT_C, 2, "C1A", "nbonds=4,ring(size=5,N,C(n=4)),-N,-C,nh=2", "Carbon next to pyrrolidinium nitrogen",
                -0.17, "CT");
    addAtomType(ELEMENT_C, 3, "C2", "nbonds=4,-C(noring,~[N,P]),-H(n=2),-C", "Second carbon in R group (> ethyl)", 0.01, "CT");
    addAtomType(ELEMENT_C, 4, "CE", "nbonds=4,-C(noring,~[N,P]),-H(n=3)", "Terminal carbon of ethyl R group", -0.05, "CT");
    addAtomType(ELEMENT_C, 5, "CS", "nbonds=4,-C(n=2),-H(n=2)", "General carbon in R group", -0.12, "CT");
    addAtomType(ELEMENT_C, 6, "CT", "nbonds=4,~C(~C(-H(n=2)),~C),-H(n=3)", "Terminal carbon in R group (> ethyl)", -0.18, "CT");
    addAtomType(ELEMENT_C, 7, "CR", "nbonds=3,-H,~&13(n=2)", "Dialkylimidazolium C2 ring carbon", -0.11, "CR");
    addAtomType(ELEMENT_C, 8, "CW", "nbonds=3,-H,~C,~[&13,&19]",
                "Imidazolium C4/5 carbon adjacent to nitrogen with R group (R != H)", -0.13, "CW");
    addAtomType(ELEMENT_H, 9, "HCR", "-&7", "H on carbon adjacent to two N", 0.21, "HA");
    addAtomType(ELEMENT_H, 10, "HCW", "-&8", "H on carbon adjacent to N and C", 0.21, "HA");
    addAtomType(ELEMENT_H, 11, "HC", "", "Hydrogen on other carbon in R group", 0.06, "HC");
    addAtomType(ELEMENT_H, 12, "H1", "-C(-[N,P,S])", "Hydrogen on first carbon in R group", 0.13, "HC");
    addAtomType(ELEMENT_N, 13, "NA", "nbonds=3,ring(size=5,N,C,N,C,C),~C(nh=1,n=2),~C",
                "Nitrogen in dialkylimidazolium with alkyl group attached", 0.15, "NA");
    // alkylimidazolium JPCB 110 (2006) 19586
    addAtomType(ELEMENT_N, 14, "NAH", "nbonds=3,ring(size=5,N,C,N,C,C),~C(nh=1,n=2),-H",
                "Alkylimidazolium nitrogen with hydrogen attached", -0.21, "NA");
    addAtomType(ELEMENT_C, 15, "CRH", "nbonds=3,-H,-&13,-&14", "Alkylimidazolium C2 ring carbon ", 0.00, "CR");
    addAtomType(ELEMENT_C, 16, "CWH", "nbonds=3,-H,-&13,-C,ring(size=5)", "Alkylimidazolium C4 carbon", -0.03, "CWH");
    addAtomType(ELEMENT_H, 17, "HNA", "-&14", "Hydrogen on alkylimidazolium ring nitrogen", 0.37, "HN");
    // dialkylmethylmidazolium JPCB 112 (2008) 5039
    addAtomType(ELEMENT_C, 18, "CRM", "nbonds=3,ring(size=5,N,C,N,C,C),-C(nh=3),-N(-C(nh=3)),-N(-C(nh=3)",
                "Dialkylmethylimidazolium C2 ring carbon", 0.19, "CR");
    addAtomType(ELEMENT_N, 19, "NAM", "nbonds=3,ring(size=5,N,C,N,C,C),-&18,-C(nh=1),-C",
                "Dialkylmethylimidazolium ring nitrogen", 0.04, "NA");
    addAtomType(ELEMENT_C, 20, "CCR", "nbonds=4,nh=3,-&18", "Dialkylmethylimidazolium methyl carbon on C2", -0.26, "CT");
    // aromatic OPLS-AA
    addAtomType(ELEMENT_C, 21, "CA", "nbonds=3,ring(size=6),nh=2,-C,-[C,N]", "Aromatic carbon in 6-membered ring", -0.115,
                "CA");
    addAtomType(ELEMENT_H, 22, "HA", "-&21", "Hydrogen on aromatic carbon in 6-membered ring", 0.115, "HA");
    // toluene JACS 118 (1996) 11217; JCC 25 (2004) 1322
    addAtomType(ELEMENT_C, 23, "CAP", "nbonds=3,nh=1,ring(size=6,C(n=6)),-C(-C(-C(-C(noring))))", "Toluene para carbon", -0.154,
                "CA");
    addAtomType(ELEMENT_C, 24, "CAM", "nbonds=3,nh=1,ring(size=6,C(n=6)),-C(-C(-C(noring)))", "Toluene meta carbon", -0.145,
                "CA");
    addAtomType(ELEMENT_C, 25, "CAO", "nbonds=3,nh=1,ring(size=6,C(n=6)),-C(-C(noring))", "Toluene ortho carbon", -0.147, "CA");
    addAtomType(ELEMENT_C, 26, "CAT", "nbonds=3,ring(size=6,C(n=6)),-&25(n=2),-C", "Toluene carbon with methyl group", -0.081,
                "CA");
    // CTT   CT  ELEMENT_C  0.197  lj    3.50   0.27614
    addAtomType(ELEMENT_H, 27, "HAT", "-[&23,&24,&25]", "Aromatic hydrogen on toluene", 0.148, "HA");
    addAtomType(ELEMENT_H, 28, "HT", "-C(nh=3,-&26)", "Hydrogen on toluene methyl group", 0.092, "HC");
    // benzylimidazolium JPCB 117 (2013) 7416 (OPLS-AA benzyl ok)
    addAtomType(ELEMENT_C, 29, "C1T", "nbonds=4,nh=2,-&26,-N(ring(size=5))", "Benzylimidazolium bridging carbon", 0.039, "CT");
    // pyridinium JPCB 110 (2006) 19586
    addAtomType(ELEMENT_C, 30, "CAPO", "nbonds=3,nh=1,ring(size=6,C(n=5),N),-N(-C(noring))", "Ortho carbon in pyridinium", 0.00,
                "CA");
    addAtomType(ELEMENT_C, 31, "CAPM", "nbonds=3,nh=1,ring(size=6,C(n=5),N),-C(-N(-C(noring)))", "Meta carbon in pyridinium",
                -0.07, "CA");
    addAtomType(ELEMENT_C, 32, "CAPP", "nbonds=3,nh=1,ring(size=6,C(n=5),N),-C(-C(-N(-C(noring))))",
                "Para carbon in pyridinium", 0.02, "CA");
    addAtomType(ELEMENT_N, 33, "NAP", "nbonds=3,ring(size=6,C(n=5),N),-C(noring)", "Pyridinium nitrogen", 0.15, "NA");
    addAtomType(ELEMENT_H, 34, "HAP", "-[&30,&31,&32]", "Hydrogen on pyridinium ring carbon", 0.15, "HA");
    // isoquinolinium
    addAtomType(ELEMENT_N, 35, "NAQ", "ring(size=6,N,C(n=5)),ring(size=10,N,C(n=9)),-C(nh=3)", "Isoquinolinium nitrogen", 0.305,
                "NA");
    addAtomType(ELEMENT_C, 36, "CAQ", "ring(size=6,C(n=6)),ring(size=6,N,C(n=5))", "Isoquinolinium fused carbon", 0.185, "CA");
    // imidazolium fluoroalkyl JPCB 114 (2010) 3592
    // fluoroalkyl OPLS-AA JPCA 105 (2001) 4118
    addAtomType(ELEMENT_C, 37, "C1H", "nbonds=4,nh=2,-C(nh=2,-C(-F(n=2)))",
                "First tetrahedral carbon in R group in fluoroalkylimidazolium", -0.05, "CT");
    addAtomType(ELEMENT_C, 38, "CFH", "nbonds=4,nh=0,-C(nh=2,-C(nh=2)),-F(n=2)",
                "First fluorinated carbon in fluoroalkylimidazolium", 0.12, "CF");
    addAtomType(ELEMENT_C, 39, "CTF", "nbonds=4,-F(n=3)", "Terminal fluorinated carbon in fluoroalkylimidazolium", 0.36, "CF");
    addAtomType(ELEMENT_C, 40, "CSF", "nbonds=4,-F(n=2),-C(-F,n=2)", "Mid-chain fluorinated carbon in fluoroalkylimidazolium",
                0.24, "CF");
    addAtomType(ELEMENT_F, 41, "F", "-[&38,&39,&40]", "Fluorine in fluoroalkylimidazolium", -0.12, "F");
    // ammonium, pyrrolidinium JPCB 108 (2004) 16893
    addAtomType(ELEMENT_N, 42, "N4", "nbonds=4,-C(n=4)", "Tetraalkylammonium nitrogen", 0.12, "NT");
    addAtomType(ELEMENT_N, 43, "N3", "nbonds=4,-C(n=3),-H", "Trialkylammonium nitrogen", 0.03, "NT");
    addAtomType(ELEMENT_H, 44, "H3", "-&43", "Hydrogen on trialkylammonium nitrogen", 0.31, "HN");
    // alcohols OPLS-AA JACS 118 (1996) 11225; JPC 100 (1996) 18010
    // addAtomType(ELEMENT_C, 45, "CTO", "", "", 0.145, "CT");
    // addAtomType(ELEMENT_H, 46, "H1O", "", "", 0.040, "HC");
    addAtomType(ELEMENT_O, 47, "OH", "-C,-H", "Alcohol oxygen", -0.683, "OH");
    addAtomType(ELEMENT_H, 48, "HO", "-O", "Alcohol hydrogen", 0.418, "HO");
    // alkoxy imidazolium JPCB 114 (2010) 3595
    addAtomType(ELEMENT_C, 49, "C2O", "-C(nh=2,-N(ring(size=5,N,C,N,C,C))),nh=2,-O(-H)", "Alkoxyimidazolium alcohol carbon",
                0.275, "CT");
    // cholinium same as OPLS-AA alcohols or ammonium (Ferid, Agilio)
    addAtomType(ELEMENT_C, 50, "COL", "-C(nh=2,-N(noring)),nh=2,-O(-H)", "Cholinium alcohol carbon", 0.275, "CT");
    // guanidinium
    addAtomType(ELEMENT_C, 51, "CG", "-N(nh=2,n=3)", "Guanidinium carbon", 1.06, "CG");
    addAtomType(ELEMENT_N, 52, "NG", "-&51", "Guanidinium nitrogen", -0.94, "NG");
    addAtomType(ELEMENT_H, 53, "HG", "-&52", "Hydrogen on guanidinium nitrogen", 0.46, "HG");
    // phosphonium OPLS-AA, JPCB 110 (2006) 19586
    addAtomType(ELEMENT_P, 54, "P4", "nbonds=4,-C(n=4)", "Tetraalkylphosphonium phosphorus", 0.68, "PT");
    addAtomType(ELEMENT_C, 55, "C1P", "-&54,nh=3", "", -0.31, "CT");
    // lithium cation Aqvist
    addAtomType(ELEMENT_LI, 56, "Li", "", "", 1.00, "Li");
    // sodium cation
    addAtomType(ELEMENT_NA, 57, "Na", "", "", 1.00, "Na");

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
    static std::string desc =
        fmt::format("Ionic liquid cations forcefield of Padua & Canongia Lopes (and contributors), 19 June 2019 "
                    "version, covering .<br/><br/>References: {}",
                    publicationReferences());
    return desc;
}
