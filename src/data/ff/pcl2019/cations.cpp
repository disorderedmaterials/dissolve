// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/pcl2019/cations.h"

/*
 * Set Up
 */

//  Set up / create all forcefield terms
bool Forcefield_PCL2019_Cations::setUp()
{
    // Create atom types
    // dialkylimidazolium JPCB 108 (2004) 2038
    addAtomType(Elements::C, 1, "C1", "nbonds=4,-N,nh>=2", "First tetrahedral carbon in R group on ring nitrogen", -0.17, "CT");
    // C1A - only for polarisable FF - implemented here to cover pyrrolidinium
    //     addAtomType(Elements::C, 2, "C1A", "nbonds=4,ring(size=5,N,C(n=4)),-N,-C,nh=2", "Carbon next to pyrrolidinium
    //     nitrogen",
    //                 -0.17, "CT");
    addAtomType(Elements::C, 3, "C2", "nbonds=4,-C(-[N,P]),-H(n=2),-C", "Second carbon in R group (> ethyl)", 0.01, "CT");
    addAtomType(Elements::C, 4, "CE", "nbonds=4,-C(!ring(),-[N,P]),-H(n=3)", "Terminal carbon of ethyl R group", -0.05, "CT");
    addAtomType(Elements::C, 5, "CS", "nbonds=4,-C(n=2),-H(n=2)", "General carbon in R group", -0.12, "CT");
    addAtomType(Elements::C, 6, "CT", "nbonds=4,-C(-C(-H(n=2))),-H(n=3)", "Terminal carbon in R group (> ethyl)", -0.18, "CT");
    addAtomType(Elements::C, 7, "CR", "nbonds=3,-H,-&13(n=2)", "Dialkylimidazolium C2 ring carbon", -0.11, "CR");
    addAtomType(Elements::C, 8, "CW", "nbonds=3,-H,-C,-[&13,&19]",
                "Imidazolium C4/5 carbon adjacent to nitrogen with R group (R != H)", -0.13, "CW");
    addAtomType(Elements::H, 9, "HCR", "-[&7,&15]", "H on carbon adjacent to two N", 0.21, "HA");
    addAtomType(Elements::H, 10, "HCW", "-[&8,&16]", "H on carbon adjacent to N and C", 0.21, "HA");
    addAtomType(Elements::H, 11, "HC", "-C(!-[N,P])", "Hydrogen on other carbon in R group", 0.06, "HC");
    addAtomType(Elements::H, 12, "H1", "-C(-[N,P]|-C(ring(size=5,N,C,N,C,C)))", "Hydrogen on first carbon in R group", 0.13,
                "HC");
    addAtomType(Elements::N, 13, "NA", "nbonds=3,nh=0,-C(nh=1,n=2),ring(size=5,N,C,N,C,C)",
                "Nitrogen in dialkylimidazolium with alkyl group attached", 0.15, "NA");
    // alkylimidazolium JPCB 110 (2006) 19586
    addAtomType(Elements::N, 14, "NAH", "nbonds=3,-C(nh=1,n=2),-H,ring(size=5,N,C,N,C,C)",
                "Alkylimidazolium nitrogen with hydrogen attached", -0.21, "NA");
    addAtomType(Elements::C, 15, "CRH", "nbonds=3,-H,-&13,-&14", "Alkylimidazolium C2 ring carbon ", 0.00, "CR");
    addAtomType(Elements::C, 16, "CWH", "nbonds=3,-H,-&14,-C,ring(size=5)", "Alkylimidazolium C4 carbon", -0.03, "CWH");
    addAtomType(Elements::H, 17, "HNA", "-&14", "Hydrogen on alkylimidazolium ring nitrogen", 0.37, "HNA");
    // dialkylmethylmidazolium JPCB 112 (2008) 5039
    addAtomType(Elements::C, 18, "CRM", "nbonds=3,-C(nh=3),-N(-C(nh>=2),n=2),ring(size=5,N,C,N,C,C)",
                "Dialkylmethylimidazolium C2 ring carbon", 0.19, "CR");
    addAtomType(Elements::N, 19, "NAM", "nbonds=3,-&18,-C(nh=1),-C,ring(size=5,N,C,N,C,C)",
                "Dialkylmethylimidazolium ring nitrogen", 0.04, "NA");
    addAtomType(Elements::C, 20, "CCR", "nbonds=4,nh=3,-&18", "Dialkylmethylimidazolium methyl carbon on C2", -0.26, "CT");
    // aromatic OPLS-AA
    addAtomType(Elements::C, 21, "CA", "nbonds=3,nh=1,-C,-[C,N],ring(size=6)", "Aromatic carbon in 6-membered ring", -0.115,
                "CA");
    addAtomType(Elements::H, 22, "HA", "-&21", "Hydrogen on aromatic carbon in 6-membered ring", 0.115, "HA");
    // toluene JACS 118 (1996) 11217; JCC 25 (2004) 1322
    addAtomType(Elements::C, 23, "CAP", "nbonds=3,nh=1,-C(-C(-C(-C(!ring())))),ring(size=6,C(n=6))", "Toluene para carbon",
                -0.154, "CA");
    addAtomType(Elements::C, 24, "CAM", "nbonds=3,nh=1,-C(-C(-C(!ring()))),ring(size=6,C(n=6))", "Toluene meta carbon", -0.145,
                "CA");
    addAtomType(Elements::C, 25, "CAO", "nbonds=3,nh=1,-C(-C(!ring())),ring(size=6,C(n=6))", "Toluene ortho carbon", -0.147,
                "CA");
    addAtomType(Elements::C, 26, "CAT", "nbonds=3,-&25(n=2),-C,ring(size=6,C(n=6))", "Toluene carbon with methyl group", -0.081,
                "CA");
    // CTT   CT  Elements::C  0.197  lj    3.50   0.27614
    addAtomType(Elements::H, 27, "HAT", "-[&23,&24,&25]", "Aromatic hydrogen on toluene", 0.148, "HA");
    addAtomType(Elements::H, 28, "HT", "-C(nh=3,-&26)", "Hydrogen on toluene methyl group", 0.092, "HC");
    // benzylimidazolium JPCB 117 (2013) 7416 (OPLS-AA benzyl ok)
    addAtomType(Elements::C, 29, "C1T", "nbonds=4,nh=2,-&26,-N(ring(size=5))", "Benzylimidazolium bridging carbon", 0.039,
                "CT");
    // pyridinium JPCB 110 (2006) 19586
    addAtomType(Elements::C, 30, "CAPO", "nbonds=3,nh=1,!ring(size=10),-N(-C(!ring())),ring(size=6,C(n=5),N)",
                "Ortho carbon in pyridinium", 0.00, "CA");
    addAtomType(Elements::C, 31, "CAPM", "nbonds=3,nh=1,!ring(size=10),-C(-N(-C(!ring()))),ring(size=6,C(n=5),N)",
                "Meta carbon in pyridinium", -0.07, "CA");
    addAtomType(Elements::C, 32, "CAPP", "nbonds=3,nh=1,!ring(size=10),-C(-C(-N(-C(!ring())))),ring(size=6,C(n=5),N)",
                "Para carbon in pyridinium", 0.02, "CA");
    addAtomType(Elements::N, 33, "NAP", "nbonds=3,-C(!ring()),ring(size=6,C(n=5),N)", "Pyridinium nitrogen", 0.15, "NA");
    addAtomType(Elements::H, 34, "HAP", "-C(ring(size=6,C(n=5),N))", "Hydrogen on pyridinium ring carbon", 0.15, "HA");
    // isoquinolinium
    addAtomType(Elements::N, 35, "NAQ", "-C(!ring()),ring(size=6,N,C(n=5)),ring(size=10,N,C(n=9))", "Isoquinolinium nitrogen",
                0.305, "NA");
    addAtomType(Elements::C, 36, "CAQ", "ring(size=6,N,C(n=5)),ring(size=6,C(n=6))", "Isoquinolinium fused carbon", 0.185,
                "CA");
    // imidazolium fluoroalkyl JPCB 114 (2010) 3592
    // fluoroalkyl OPLS-AA JPCA 105 (2001) 4118
    addAtomType(Elements::C, 37, "C1H", "nbonds=4,nh=2,-C(nh=2,-C(-F(n=2)))",
                "First tetrahedral carbon in R group in fluoroalkylimidazolium", -0.05, "CT");
    addAtomType(Elements::C, 38, "CFH", "nbonds=4,nh=0,-C(nh=2,-C(nh=2)),-F(n=2)",
                "First fluorinated carbon in fluoroalkylimidazolium", 0.12, "CF");
    addAtomType(Elements::C, 39, "CTF", "nbonds=4,-F(n=3)", "Terminal fluorinated carbon in fluoroalkylimidazolium", 0.36,
                "CF");
    addAtomType(Elements::C, 40, "CSF", "nbonds=4,-F(n=2),-C(-F,n=2)", "Mid-chain fluorinated carbon in fluoroalkylimidazolium",
                0.24, "CF");
    addAtomType(Elements::F, 41, "F", "-[&38,&39,&40]", "Fluorine in fluoroalkylimidazolium", -0.12, "FC");
    // ammonium, pyrrolidinium JPCB 108 (2004) 16893
    addAtomType(Elements::N, 42, "N4", "nbonds=4,-C(n=4)", "Tetraalkylammonium nitrogen", 0.12, "NT");
    addAtomType(Elements::N, 43, "N3", "nbonds=4,-C(n=3),-H", "Trialkylammonium nitrogen", 0.03, "NT");
    addAtomType(Elements::H, 44, "H3", "-&43", "Hydrogen on trialkylammonium nitrogen", 0.31, "HN");
    // alcohols OPLS-AA JACS 118 (1996) 11225; JPC 100 (1996) 18010
    // addAtomType(Elements::C, 45, "CTO", "", "", 0.145, "CT");
    // addAtomType(Elements::H, 46, "H1O", "", "", 0.040, "HC");
    addAtomType(Elements::O, 47, "OH", "-C,-H", "Alcohol oxygen", -0.683, "OH");
    addAtomType(Elements::H, 48, "HO", "-O", "Alcohol hydrogen", 0.418, "HO");
    // alkoxy imidazolium JPCB 114 (2010) 3595
    addAtomType(Elements::C, 49, "C2O", "-C(nh=2,-N(ring(size=5,N,C,N,C,C))),nh=2,-O(-H)", "Alkoxyimidazolium alcohol carbon",
                0.275, "CT");
    // cholinium same as OPLS-AA alcohols or ammonium (Ferid, Agilio)
    addAtomType(Elements::C, 50, "COL", "-C(nh=2,-N(!ring())),nh=2,-O(-H)", "Cholinium alcohol carbon", 0.275, "CT");
    // guanidinium
    addAtomType(Elements::C, 51, "CG", "-N(nh=2,n=3)", "Guanidinium carbon", 1.06, "CG");
    addAtomType(Elements::N, 52, "NG", "-&51", "Guanidinium nitrogen", -0.94, "NG");
    addAtomType(Elements::H, 53, "HG", "-&52", "Hydrogen on guanidinium nitrogen", 0.46, "HG");
    // phosphonium OPLS-AA, JPCB 110 (2006) 19586
    addAtomType(Elements::P, 54, "P4", "nbonds=4,-C(n=4)", "Tetraalkylphosphonium phosphorus", 0.68, "PT");
    addAtomType(Elements::C, 55, "C1P", "-&54", "Carbon on tetraalkylphosphonium phosphorus", -0.31, "CT");
    // lithium cation Aqvist
    addAtomType(Elements::Li, 56, "Li", "", "", 1.00, "Li");
    // sodium cationDetermining
    addAtomType(Elements::Na, 57, "Na", "", "", 1.00, "Na");

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
