// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/angleTerm.h"
#include "data/ff/bondTerm.h"
#include "data/ff/improperTerm.h"
#include "data/ff/pcl2019/base.h"
#include "data/ff/torsionTerm.h"
#include "templates/optionalRef.h"
#include <vector>

/*
 * Term Assignment
 */

// Return bond term for the supplied atom type pair (if it exists)
OptionalReferenceWrapper<const ForcefieldBondTerm> PCL2019BaseForcefield::getBondTerm(const ForcefieldAtomType &i,
                                                                                      const ForcefieldAtomType &j) const
{
    static const std::vector<ForcefieldBondTerm> bondTerms = {
        //	i	j	Type (Harmonic)			k	eq
        // i j    pot    re/A    kr/kJmol-1
        // alkanes OPLS-AA JACS118(1996)11225, JPC100(1996)18010
        {"HC", "CT", BondFunctions::Form::Harmonic, "k=2845.0 eq=1.090"},
        {"CT", "CT", BondFunctions::Form::Harmonic, "k=2242.0 eq=1.529"},
        // aromatics AMBER JACS 117(1995)5179
        {"CA", "CA", BondFunctions::Form::Harmonic, "k=3924.6 eq=1.400"},
        {"CA", "HA", BondFunctions::Form::Harmonic, "k=3071.1 eq=1.080"},
        // toluene AMBER JACS 117(1995)5179
        {"CA", "CT", BondFunctions::Form::Harmonic, "k=2652.7 eq=1.510"},
        // pyridinium JPCB110(2006)19586
        {"CA", "NA", BondFunctions::Form::Harmonic, "k=4042.0 eq=1.340"},
        // dialkylimidazolium JPCB108(2004)2038
        {"CR", "HA", BondFunctions::Form::Harmonic, "k=2845.0 eq=1.080"},
        {"CW*", "HA", BondFunctions::Form::Harmonic, "k=2845.0 eq=1.080"},
        {"CR", "NA", BondFunctions::Form::Harmonic, "k=3992.0 eq=1.315"},
        {"CW*", "NA", BondFunctions::Form::Harmonic, "k=3574.0 eq=1.378"},
        {"CW*", "CW*", BondFunctions::Form::Harmonic, "k=4352.0 eq=1.341"},
        {"NA", "CT", BondFunctions::Form::Harmonic, "k=2820.0 eq=1.466"},
        // alkylimidazolium JPCB 110 (2006) 19586
        {"HNA", "NA", BondFunctions::Form::Harmonic, "k=3632.0 eq=1.010"},
        // dialkylimethylmidazolium JPCB112(2008)5039
        {"CR", "CT", BondFunctions::Form::Harmonic, "k=2653.0 eq=1.510"},
        // fluoroalkyl JPCA105(2001)4118, JPCA106(2002)10116
        {"CT", "CF", BondFunctions::Form::Harmonic, "k=2242.6 eq=1.529"},
        {"FC", "CF", BondFunctions::Form::Harmonic, "k=3071.1 eq=1.332"},
        {"CF", "CF", BondFunctions::Form::Harmonic, "k=2242.6 eq=1.529"},
        // ammonium, pyrrolidinium OPLS-AA JACS121(1999)4827, AMBER
        {"NT", "CT", BondFunctions::Form::Harmonic, "k=3196.6 eq=1.448"},
        {"HN", "NT", BondFunctions::Form::Harmonic, "k=3632.0 eq=1.010"},
        // guanidinium
        {"HG", "NG", BondFunctions::Form::Harmonic, "k=3632.0 eq=1.010"},
        {"NG", "CG", BondFunctions::Form::Harmonic, "k=4027.7 eq=1.340"},
        // phosphonium OPLS-AA JPCB110(2006)19586
        {"PT", "CT", BondFunctions::Form::Harmonic, "k=3550.0 eq=1.81"},
        // hydroxyl OPLS-AA JACS 118(1996)11225, JPC 100(1996)18010
        {"CT", "OH", BondFunctions::Form::Harmonic, "k=2677.8 eq=1.410"},
        {"HO", "OH", BondFunctions::Form::Harmonic, "k=4627.5 eq=0.945"},
        // tetrafluoroborate
        {"B", "F", BondFunctions::Form::Harmonic, "k=3235.0 eq=1.394"},
        // hexafluorophosphate JCSPerkin2(1999)2365
        {"P", "F", BondFunctions::Form::Harmonic, "k=3100.0 eq=1.606"},
        // triflate and bistriflamide JPCB108(2004)16893
        {"FB", "CF", BondFunctions::Form::Harmonic, "k=3698.0 eq=1.323"},
        {"CF", "SB", BondFunctions::Form::Harmonic, "k=1950.0 eq=1.818"},
        {"SB", "OB", BondFunctions::Form::Harmonic, "k=5331.0 eq=1.437"},
        {"NB", "SB", BondFunctions::Form::Harmonic, "k=3137.0 eq=1.570"},
        // bis(fluorosulfonyl)amide
        {"FB", "SB", BondFunctions::Form::Harmonic, "k=1879.0 eq=1.575"},
        // dicyanamide JPCB110(2006)19586
        {"N3", "CZ", BondFunctions::Form::Harmonic, "k=4206.0 eq=1.310"},
        {"CZ", "NZ", BondFunctions::Form::Harmonic, "k=5439.2 eq=1.157"},
        // acetate OPLS-AA
        {"CO", "O2", BondFunctions::Form::Harmonic, "k=5489.0 eq=1.250"},
        {"CT", "CO", BondFunctions::Form::Harmonic, "k=2653.0 eq=1.522"},
        // trifluoroacetate (copied from old.oplsaa.ff) (JPCB, 110, 24, 2006, 12062)
        {"CF", "CO", BondFunctions::Form::Harmonic, "k=1087.84 eq=1.568"},
        {"CO", "O2F", BondFunctions::Form::Harmonic, "k=2322.12 eq=1.223"},
        {"CF", "FFA", BondFunctions::Form::Harmonic, "k=1004.16 eq=1.334"},
        // alkylsulfates JPCB112(2008)5039
        {"CT", "OC", BondFunctions::Form::Harmonic, "k=745.8 eq=1.402"},
        {"OS", "SO", BondFunctions::Form::Harmonic, "k=5331.0 eq=1.455"},
        {"OC", "SO", BondFunctions::Form::Harmonic, "k=1789.6 eq=1.633"},
        // alkylsulfonates JPCB112(2008)
        {"CT", "SO", BondFunctions::Form::Harmonic, "k=1970.0 eq=1.792"},
        // thiocyanate JCP128(2008)154504, our MP2
        {"SK", "CK", BondFunctions::Form::Harmonic, "k=2836.8 eq=1.67"},
        {"CK", "NK", BondFunctions::Form::Harmonic, "k=12221.5 eq=1.19"},
        // tricyanomethanide MG Martin STTR report 2008
        // C3A  CN  harm   1.408   1799.12
        // CN   NC  harm   1.167   5062.64
        // tricyanomethanide OPLS nitriles
        {"C3A", "CN", BondFunctions::Form::Harmonic, "k=3347.2 eq=1.412"},
        {"CN", "NC", BondFunctions::Form::Harmonic, "k=5439.2 eq=1.157"},
        // tosylate JPCB 112 (2008) 5039
        {"CA", "SO", BondFunctions::Form::Harmonic, "k=1970.0 eq=1.797"}};

    return Forcefield::termMatch_(bondTerms, i, j);
}

// Return angle term for the supplied atom type trio (if it exists)
OptionalReferenceWrapper<const ForcefieldAngleTerm>
PCL2019BaseForcefield::getAngleTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k) const
{
    static const std::vector<ForcefieldAngleTerm> angleTerms = {
        //	i	j	k	Type (Harmonic)			k	eq
        // alkyl OPLS-AA JACS118(1996)11225, JPC100(1996)18010
        {"CT", "CT", "CT", AngleFunctions::Form::Harmonic, "k=488.3 eq=112.7"},
        {"CT", "CT", "HC", AngleFunctions::Form::Harmonic, "k=313.8 eq=110.7"},
        {"HC", "CT", "HC", AngleFunctions::Form::Harmonic, "k=276.1 eq=107.8"},
        // aromatics AMBER JACS 117(1995)5179
        {"CA", "CA", "HA", AngleFunctions::Form::Harmonic, "k=292.9 eq=120.0"},
        {"CA", "CA", "CA", AngleFunctions::Form::Harmonic, "k=527.2 eq=120.0"},
        // toluene AMBER JACS 117(1995)5179
        {"CA", "CA", "CT", AngleFunctions::Form::Harmonic, "k=585.8 eq=120.0"},
        {"CA", "CT", "HC", AngleFunctions::Form::Harmonic, "k=418.4 eq=109.5"},
        // pyridinium JPCB110(2006)19586
        {"CA", "NA", "CA", AngleFunctions::Form::Harmonic, "k=585.8 eq=120.4"},
        {"CA", "NA", "CT", AngleFunctions::Form::Harmonic, "k=585.8 eq=119.8"},
        {"CA", "CA", "NA", AngleFunctions::Form::Harmonic, "k=585.8 eq=120.0"},
        {"NA", "CA", "HA", AngleFunctions::Form::Harmonic, "k=292.9 eq=120.0"},
        // dialkylimidazolium JPCB108(2004)2038
        {"CW*", "NA", "CR", AngleFunctions::Form::Harmonic, "k=585.8 eq=108.0"},
        {"CW*", "NA", "CT", AngleFunctions::Form::Harmonic, "k=585.8 eq=125.6"},
        {"CR", "NA", "CT", AngleFunctions::Form::Harmonic, "k=585.8 eq=126.4"},
        {"NA", "CR", "HA", AngleFunctions::Form::Harmonic, "k=292.9 eq=125.1"},
        {"NA", "CR", "NA", AngleFunctions::Form::Harmonic, "k=585.8 eq=109.8"},
        {"NA", "CW*", "CW*", AngleFunctions::Form::Harmonic, "k=585.8 eq=107.1"},
        {"NA", "CW*", "HA", AngleFunctions::Form::Harmonic, "k=292.9 eq=122.0"},
        {"CW*", "CW*", "HA", AngleFunctions::Form::Harmonic, "k=292.9 eq=130.9"},
        {"NA", "CT", "HC", AngleFunctions::Form::Harmonic, "k=313.8 eq=110.7"},
        {"NA", "CT", "CT", AngleFunctions::Form::Harmonic, "k=488.3 eq=112.7"},
        // alkylimidazolium JPCB 110 (2006) 19586
        {"CR", "NA", "HNA", AngleFunctions::Form::Harmonic, "k=292.9 eq=125.4"},
        {"CW*", "NA", "HNA", AngleFunctions::Form::Harmonic, "k=292.9 eq=126.6"},
        // dialkylimethylmidazolium JPCB112(2008)5039
        {"CT", "CR", "NA", AngleFunctions::Form::Harmonic, "k=585.8 eq=125.8"},
        {"CR", "CT", "HC", AngleFunctions::Form::Harmonic, "k=313.8 eq=110.7"},
        // benzylimidazolium AMBER
        {"NA", "CT", "CA", AngleFunctions::Form::Harmonic, "k=669.4 eq=111.2"},
        // fluoroalkyl JPCA105(2001)4118, JPCA106(2002)10116
        {"HC", "CT", "CF", AngleFunctions::Form::Harmonic, "k=313.8 eq=110.7"},
        {"FC", "CF", "CT", AngleFunctions::Form::Harmonic, "k=418.4 eq=109.5"},
        {"CT", "CF", "CF", AngleFunctions::Form::Harmonic, "k=488.3 eq=112.7"},
        {"CT", "CT", "CF", AngleFunctions::Form::Harmonic, "k=488.3 eq=112.7"},
        {"FC", "CF", "FC", AngleFunctions::Form::Harmonic, "k=644.3 eq=109.1"},
        {"FC", "CF", "CF", AngleFunctions::Form::Harmonic, "k=418.4 eq=109.5"},
        {"CF", "CF", "CF", AngleFunctions::Form::Harmonic, "k=488.3 eq=112.7"},
        // ammonium, pyrrolidinium OPLS-AA JACS121(1999)4827, AMBER
        {"NT", "CT", "CT", AngleFunctions::Form::Harmonic, "k=470.3 eq=109.5"},
        {"CT", "NT", "CT", AngleFunctions::Form::Harmonic, "k=433.5 eq=107.2"},
        {"HC", "CT", "NT", AngleFunctions::Form::Harmonic, "k=292.9 eq=109.5"},
        {"HN", "NT", "CT", AngleFunctions::Form::Harmonic, "k=292.9 eq=109.5"},
        // guanidinium
        {"CG", "NG", "HG", AngleFunctions::Form::Harmonic, "k=293.1 eq=121.6"},
        {"HG", "NG", "HG", AngleFunctions::Form::Harmonic, "k=293.1 eq=116.8"},
        {"NG", "CG", "NG", AngleFunctions::Form::Harmonic, "k=586.2 eq=120.0"},
        // phosphonium OPLS-AA, JPCB110(2006)19586
        {"CT", "PT", "CT", AngleFunctions::Form::Harmonic, "k=607.8 eq=109.5"},
        {"HC", "CT", "PT", AngleFunctions::Form::Harmonic, "k=389.9 eq=110.1"},
        {"CT", "CT", "PT", AngleFunctions::Form::Harmonic, "k=509.1 eq=115.2"},
        // hydroxyl JACS 118(1996)11225, JPC 100(1996)18010
        {"CT", "CT", "OH", AngleFunctions::Form::Harmonic, "k=418.4 eq=109.5"},
        {"HC", "CT", "OH", AngleFunctions::Form::Harmonic, "k=292.9 eq=109.5"},
        {"CT", "OH", "HO", AngleFunctions::Form::Harmonic, "k=460.2 eq=108.5"},
        // tetrafluoroborate anion
        {"F", "B", "F", AngleFunctions::Form::Harmonic, "k=669.5 eq=109.5"},
        // hexafluorophosphate JCSPerkin2(1999)2365
        {"F", "P", "F", AngleFunctions::Form::Harmonic, "k=1165.0 eq=90.0"},
        // triflate and bistriflamide JPCB108(2004)16893
        {"FB", "CF", "FB", AngleFunctions::Form::Harmonic, "k=781.0 eq=107.1"},
        {"FB", "CF", "SB", AngleFunctions::Form::Harmonic, "k=694.0 eq=111.7"},
        {"OB", "SB", "OB", AngleFunctions::Form::Harmonic, "k=969.0 eq=118.5"},
        {"CF", "SB", "OB", AngleFunctions::Form::Harmonic, "k=870.0 eq=102.6"},
        {"NB", "SB", "OB", AngleFunctions::Form::Harmonic, "k=789.0 eq=113.6"},
        {"NB", "SB", "CF", AngleFunctions::Form::Harmonic, "k=764.0 eq=103.5"},
        {"SB", "NB", "SB", AngleFunctions::Form::Harmonic, "k=671.0 eq=125.6"},
        // longer perfluoroalkanesulfonylamides
        {"SB", "CF", "CF", AngleFunctions::Form::Harmonic, "k=418.4 eq=115.9"},
        {"FB", "CF", "CF", AngleFunctions::Form::Harmonic, "k=418.4 eq=109.5"},
        // bis(fluorosulfonyl)amide
        {"FB", "SB", "OB", AngleFunctions::Form::Harmonic, "k=1077.0 eq=104.1"},
        {"FB", "SB", "NB", AngleFunctions::Form::Harmonic, "k=902.0 eq=103.0"},
        // dicyanamide JPCB110(2006)19586
        {"CZ", "N3", "CZ", AngleFunctions::Form::Harmonic, "k=362.0 eq=118.5"},
        {"N3", "CZ", "NZ", AngleFunctions::Form::Harmonic, "k=425.0 eq=175.2"},
        // acetate OPLS-AA JPCB (2004)
        {"O2*", "CO", "O2*", AngleFunctions::Form::Harmonic, "k=669.4 eq=126.0"},
        {"CT", "CO", "O2", AngleFunctions::Form::Harmonic, "k=585.8 eq=117.0"},
        {"HC", "CT", "CO", AngleFunctions::Form::Harmonic, "k=292.9 eq=109.5"},
        // trifluoroacetate OPLS-AA
        {"FC", "CF", "CO", AngleFunctions::Form::Harmonic, "k=418.4 eq=109.5"},
        {"CF", "CO", "O2F", AngleFunctions::Form::Harmonic, "k=585.8 eq=117.0"},
        // alkylsulfates JPCB112(2008)5039
        {"OS", "SO", "OS", AngleFunctions::Form::Harmonic, "k=969.0 eq=114.0"},
        {"OC", "SO", "OS", AngleFunctions::Form::Harmonic, "k=1239.6 eq=103.5"},
        {"CT", "OC", "SO", AngleFunctions::Form::Harmonic, "k=300.5 eq=116.6"},
        {"HC", "CT", "OC", AngleFunctions::Form::Harmonic, "k=488.7 eq=109.7"},
        {"CT", "CT", "OC", AngleFunctions::Form::Harmonic, "k=765.6 eq=107.8"},
        // alkylsulfonates JPCB112(2008)5039
        {"CT", "SO", "OS", AngleFunctions::Form::Harmonic, "k=870.0 eq=104.5"},
        {"HC", "CT", "SO", AngleFunctions::Form::Harmonic, "k=390.3 eq=107.3"},
        {"CT", "CT", "SO", AngleFunctions::Form::Harmonic, "k=583.0 eq=113.3"},
        // thiocyanate JCP128(2008)154504
        {"SK", "CK", "NK", AngleFunctions::Form::Harmonic, "k=651.9 eq=180.0"},
        // tricyanomethanide OPLS nitriles
        {"C3A", "CN", "NC", AngleFunctions::Form::Harmonic, "k=1255.2 eq=180.0"},
        {"CN", "C3A", "CN", AngleFunctions::Form::Harmonic, "k=585.8 eq=120.0"},
        // tosylate OPLS, JPCB 112 (2008) 5039
        {"CA", "CA", "SO", AngleFunctions::Form::Harmonic, "k=711.28 eq=120.5"},
        {"CA", "SO", "OS", AngleFunctions::Form::Harmonic, "k=870.0 eq=104.2"}};

    return Forcefield::termMatch_(angleTerms, i, j, k);
}

// Return torsion term for the supplied atom type quartet (if it exists)
OptionalReferenceWrapper<const ForcefieldTorsionTerm> PCL2019BaseForcefield::getTorsionTerm(const ForcefieldAtomType &i,
                                                                                            const ForcefieldAtomType &j,
                                                                                            const ForcefieldAtomType &k,
                                                                                            const ForcefieldAtomType &l) const
{
    static std::vector<ForcefieldTorsionTerm> torsionTerms = {
        //	i	j	k	l	Type (CosineForm)		k		n	eq	s
        // alkanes OPLS-AA JACS 118 (1996) 11225; JPC 100 (1996) 18010
        {"HC", "CT", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.2552 k4=0.0000"},
        {"CT", "CT", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.2552 k4=0.0000"},
        {"CT", "CT", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=5.4392 k2=-0.2092 k3=0.8368 k4=0.0000"},
        // aromatics AMBER JACS 117 (1995) 5179
        {"CA", "CA", "CA", "CA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=30.3340 k3=0.0000 k4=0.0000"},
        {"CA", "CA", "CA", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=30.3340 k3=0.0000 k4=0.0000"},
        {"HA", "CA", "CA", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=30.3340 k3=0.0000 k4=0.0000"},
        // toluene AMBER JACS 117 (1995) 5179
        {"CT", "CA", "CA", "CA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=30.3340 k3=0.0000 k4=0.0000"},
        {"CT", "CA", "CA", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=30.3340 k3=0.0000 k4=0.0000"},
        // CA  CA  CT  HC   opls   0.0000    0.0000    0.0000    0.0000
        // pyridinium JPCB 110 (2006) 19586
        {"NA", "CA", "CA", "CA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=30.3340 k3=0.0000 k4=0.0000"},
        {"NA", "CA", "CA", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=30.3340 k3=0.0000 k4=0.0000"},
        {"CA", "NA", "CA", "CA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=12.5520 k3=0.0000 k4=0.0000"},
        {"CA", "NA", "CA", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=12.5520 k3=0.0000 k4=0.0000"},
        {"CT", "NA", "CA", "CA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=12.5520 k3=0.0000 k4=0.0000"},
        {"CT", "NA", "CA", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=12.5520 k3=0.0000 k4=0.0000"},
        {"CA", "NA", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.0000 k4=0.0000"},
        {"CA", "NA", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=1.0920 k3=0.0000 k4=0.7930"},
        // dialkylimidazolium JPCB 108 (2004) 2038
        {"CW*", "NA", "CR", "NA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=19.4600 k3=0.0000 k4=0.0000"},
        {"CW*", "NA", "CR", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=19.4600 k3=0.0000 k4=0.0000"},
        {"CT", "NA", "CR", "NA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=19.4600 k3=0.0000 k4=0.0000"},
        {"CT", "NA", "CR", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=19.4600 k3=0.0000 k4=0.0000"},
        {"CR", "NA", "CW*", "CW*", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=12.5520 k3=0.0000 k4=0.0000"},
        {"CR", "NA", "CW*", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=12.5520 k3=0.0000 k4=0.0000"},
        {"CT", "NA", "CW*", "CW*", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=12.5520 k3=0.0000 k4=0.0000"},
        {"CT", "NA", "CW*", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=12.5520 k3=0.0000 k4=0.0000"},
        {"NA", "CW*", "CW*", "NA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=44.9800 k3=0.0000 k4=0.0000"},
        {"NA", "CW*", "CW*", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=44.9800 k3=0.0000 k4=0.0000"},
        {"HA", "CW*", "CW*", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=44.9800 k3=0.0000 k4=0.0000"},
        {"CW*", "NA", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.5190 k4=0.0000"},
        {"CR", "NA", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.0000 k4=0.0000"},
        {"CW*", "NA", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=-7.1535 k2=6.1064 k3=0.7939 k4=0.0000"},
        {"CR", "NA", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=-5.2691 k2=0.0000 k3=0.0000 k4=0.0000"},
        {"NA", "CT", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=-7.4797 k2=3.1642 k3=-1.2026 k4=0.0000"},
        {"NA", "CT", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.3670 k4=0.0000"},
        // alkylimidazolium JPCB 110 (2006) 19586
        {"HNA", "NA", "CR", "NA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=19.4600 k3=0.0000 k4=0.0000"},
        {"HNA", "NA", "CR", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=19.4600 k3=0.0000 k4=0.0000"},
        {"HNA", "NA", "CW*", "CW*", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=12.5500 k3=0.0000 k4=0.0000"},
        {"HNA", "NA", "CW*", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=12.5500 k3=0.0000 k4=0.0000"},
        // dialkylimethylmidazolium JPCB 112 (2008) 5039
        {"CW*", "NA", "CR", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=19.4600 k3=0.0000 k4=0.0000"},
        {"CT", "NA", "CR", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=19.4600 k3=0.0000 k4=0.0000"},
        {"NA", "CR", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.0000 k4=0.0000"},
        // benzylimidazolium AMBER wildcards
        {"NA", "CT", "CA", "CA", TorsionFunctions::Form::None},
        {"CA", "CT", "NA", "CR", TorsionFunctions::Form::None},
        {"CA", "CT", "NA", "CW", TorsionFunctions::Form::None},
        // fluoroalkyl JPCA 105 (2001) 4118; JPCA 106 (2002) 10116
        {"F*", "CF", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.2134 k4=0.0000"},
        {"F*", "CF", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.9372 k4=0.0000"},
        {"F*", "CF", "CF", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=2.7656 k4=0.0000"},
        {"HC", "CT", "CT", "CF", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.5565 k4=0.0000"},
        {"HC", "CT", "CF", "CF", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.7573 k4=0.0000"},
        {"CF", "CF", "CF", "CT", TorsionFunctions::Form::Cos4, "k1=14.6750 k2=-0.9179 k3=-2.8980 k4=-2.0174"},
        {"CF", "CF", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=-0.5881 k2=-0.7642 k3=-0.3170 k4=-0.3179"},
        {"CF", "CT", "CT", "NA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.0000 k4=0.0000"},
        {"F*", "CF", "CF", "F*", TorsionFunctions::Form::Cos4, "k1=-10.4600 k2=0.0000 k3=1.0460 k4=0.0000"},
        {"F*", "CF", "CF", "CF", TorsionFunctions::Form::Cos4, "k1=1.2552 k2=0.0000 k3=1.6736 k4=0.0000"},
        {"CF", "CF", "CF", "CF", TorsionFunctions::Form::Cos4, "k1=27.7064 k2=3.9664 k3=-5.8074 k4=-8.8617"},
        // ammonium, pyrrolidinium OPLS-AA JACS 121 (1999) 4827
        {"HC", "CT", "CT", "NT", TorsionFunctions::Form::Cos4, "k1=-4.2384 k2=-2.9665 k3=1.9790 k4=0.0000"},
        {"CT", "CT", "CT", "NT", TorsionFunctions::Form::Cos4, "k1=10.0081 k2=-2.8200 k3=2.3012 k4=0.0000"},
        {"CT", "NT", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=1.7405 k2=-0.5356 k3=2.9079 k4=0.0000"},
        {"HC", "CT", "NT", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=2.3430 k4=0.0000"},
        {"HN", "NT", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.6736 k4=0.0000"},
        {"HN", "NT", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=-0.7950 k2=-1.7447 k3=1.7489 k4=0.0000"},
        // cholinium: unpublished
        {"OH", "CT", "CT", "NT", TorsionFunctions::Form::Cos4, "k1=-44.0515 k2=-5.4349 k3=0.0000 k4=0.0000"},
        // OH  CT  CT  NT   opls -44.0515   -5.0148    0.0000   -3.1510
        // guanidinium
        {"HG", "NG", "CG", "NG", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=16.3285 k3=0.0000 k4=0.0000"},
        // phosphonium OPLS-AA, JPCB 110 (2006) 19586
        {"CT", "PT", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.9270 k4=0.0000"},
        {"CT", "PT", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.1330 k4=0.0000"},
        {"PT", "CT", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.4650 k4=0.0000"},
        {"PT", "CT", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=-3.2480 k2=0.9880 k3=-0.7150 k4=0.0000"},
        // hydroxyl JACS 118 (1996) 11225; AMBER98 (OCCO) 117 (1995) 5179
        {"HC", "CT", "OH", "HO", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.8828 k4=0.0000"},
        {"CT", "CT", "OH", "HO", TorsionFunctions::Form::Cos4, "k1=-1.4895 k2=-0.7280 k3=2.0585 k4=0.0000"},
        {"HC", "CT", "CT", "OH", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.9581 k4=0.0000"},
        {"OH", "CT", "CT", "OH", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=-9.8324 k3=1.2050 k4=0.0000"},
        // triflate and bistriflamide JPCB 108 (2004) 16893
        {"OB", "SB", "CF", "FB", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.4510 k4=0.0000"},
        {"NB", "SB", "CF", "FB", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.3220 k4=0.0000"},
        {"OB", "SB", "NB", "SB", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=-0.0150 k4=0.0000"},
        {"SB", "NB", "SB", "CF", TorsionFunctions::Form::Cos4, "k1=32.7730 k2=-10.4200 k3=-3.1950 k4=0.0000"},
        // longer perfluoroalkanesulfonylamides
        {"FC", "CF", "CF", "FB", TorsionFunctions::Form::Cos4, "k1=-10.4600 k2=0.0000 k3=1.0460 k4=0.0000"},
        {"SB", "CF", "CF", "FB", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.4530 k4=0.0000"},
        {"SB", "CF", "CF", "FC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.4530 k4=0.0000"},
        {"OB", "SB", "CF", "CF", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=-0.7400 k4=0.0000"},
        {"NB", "SB", "CF", "CF", TorsionFunctions::Form::Cos4, "k1=-3.0940 k2=0.0000 k3=0.0000 k4=0.0000"},
        {"SB", "CF", "CF", "CF", TorsionFunctions::Form::Cos4, "k1=50.0900 k2=0.0000 k3=-4.6260 k4=-4.0080"},
        // bis(fluorosulfonyl)amide
        {"FB", "SB", "NB", "SB", TorsionFunctions::Form::Cos4, "k1=11.4450 k2=-15.1860 k3=-3.2120 k4=0.0000"},
        // dicyanamide JPCB 110 (2006) 19586
        {"NZ", "CZ", "N3", "CZ", TorsionFunctions::Form::Cos4, "k1=4.0800 k2=0.0000 k3=0.0000 k4=0.0000"},
        // acetate OPLS-AA JPCB (2004)
        {"HC", "CT", "CO", "O2", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.0000 k4=0.0000"},
        // trifluoroacetate
        {"FC", "CF", "CO", "O2F", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.0000 k4=0.0000"},
        // alkylsulfates JPCB 112 (2008) 5039
        {"OS", "SO", "OC", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=2.4815 k4=0.0000"},
        {"SO", "OC", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.6858 k4=0.0000"},
        {"SO", "OC", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=-6.0142 k2=-3.1133 k3=1.4941 k4=0.0000"},
        {"OC", "CT", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=2.0698 k4=0.0000"},
        {"OC", "CT", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=4.3893 k2=-1.8273 k3=2.9705 k4=0.0000"},
        // alkylsulfonates JPCB 112 (2008) 5039
        {"OS", "SO", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.6250 k4=0.0000"},
        {"OS", "SO", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.3938 k4=0.0000"},
        {"SO", "CT", "CT", "HC", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=1.3797 k4=0.0000"},
        {"SO", "CT", "CT", "CT", TorsionFunctions::Form::Cos4, "k1=-16.1000 k2=-2.0046 k3=0.7674 k4=0.0000"},
        // tricyanomethanide
        {"NC", "CN", "C3A", "CN", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.0000 k4=0.0000"},
        // tosylate our MP2, OPLS aromatics
        {"CA", "CA", "SO", "OS", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.0000 k4=0.0000"},
        {"SO", "CA", "CA", "CA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=30.3340 k3=0.0000 k4=0.0000"},
        {"SO", "CA", "CA", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=30.3340 k3=0.0000 k4=0.0000"},
        {"HC", "CT", "CA", "CA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=0.0000 k3=0.0000 k4=0.0000"},
        // alkoxy (taken from old.il.ff)
        {"NA", "CT", "CT", "OH", TorsionFunctions::Form::Cos4, "k1=-3.5787 k2=-1.6564 k3=4.9154 k4=0.0000"}};

    return Forcefield::termMatch_(torsionTerms, i, j, k, l);
}

// Return improper term for the supplied atom type quartet (if it exists)
OptionalReferenceWrapper<const ForcefieldImproperTerm> PCL2019BaseForcefield::getImproperTerm(const ForcefieldAtomType &i,
                                                                                              const ForcefieldAtomType &j,
                                                                                              const ForcefieldAtomType &k,
                                                                                              const ForcefieldAtomType &l) const
{
    // Improper terms from the original forcefield file have the central atom as the third one.
    // Presented here, the original third atom is placed first.
    static std::vector<ForcefieldImproperTerm> improperTerms = {
        // improper C aromatics AMBER JACS 117 (1995) 5179
        {"CA", "CA", "CA", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=9.2048 k3=0.0000 k4=0.0000"},
        {"CA", "CA", "CA", "CA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=9.2048 k3=0.0000 k4=0.0000"},
        {"CA", "CA", "CA", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=9.2048 k3=0.0000 k4=0.0000"},
        {"NA", "CA", "CA", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=9.2048 k3=0.0000 k4=0.0000"},
        {"CA", "CA", "NA", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=9.2048 k3=0.0000 k4=0.0000"},
        // improper imidazolium ring AMBER JACS 117 (1995) 5179
        {"NA", "CR", "CW*", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=8.3680 k3=0.0000 k4=0.0000"},
        {"NA", "CR", "CW*", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=8.3680 k3=0.0000 k4=0.0000"},
        {"CR", "NA", "NA", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=9.2048 k3=0.0000 k4=0.0000"},
        {"CR", "NA", "NA", "CT", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=9.2048 k3=0.0000 k4=0.0000"},
        {"CW*", "NA", "CW*", "HA", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=9.2048 k3=0.0000 k4=0.0000"},
        // improper tricyanomethanide AMBER JACS 117 (1995) 5179
        {"C3A", "CN", "CN", "CN", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=9.2048 k3=0.0000 k4=0.0000"},
        // improper carbonyl AMBER JACS 117 (1995 )5179
        {"CO", "CT", "O2", "O2", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=87.8640 k3=0.0000 k4=0.0000"},
        // improper guanidinium AMBER JACS 117 (1995 )5179
        {"CG", "NG", "NG", "NG", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=8.3680 k3=0.0000 k4=0.0000"},
        {"NG", "CG", "HG", "HG", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=9.2048 k3=0.0000 k4=0.0000"},
        // tosylate aromatics AMBER JACS 117 (1995) 5179
        {"CA", "CA", "CA", "SO", TorsionFunctions::Form::Cos4, "k1=0.0000 k2=9.2048 k3=0.0000 k4=0.0000"}};

    return Forcefield::termMatch_(improperTerms, i, j, k, l);
}
