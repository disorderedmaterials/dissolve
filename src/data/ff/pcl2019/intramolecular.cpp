// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/angleterm.h"
#include "data/ff/bondterm.h"
#include "data/ff/improperterm.h"
#include "data/ff/pcl2019/base.h"
#include "data/ff/torsionterm.h"
#include "templates/optionalref.h"
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
        {"HC", "CT", SpeciesBond::HarmonicForm, {2845.0, 1.090}},
        {"CT", "CT", SpeciesBond::HarmonicForm, {2242.0, 1.529}},
        // aromatics AMBER JACS 117(1995)5179
        {"CA", "CA", SpeciesBond::HarmonicForm, {3924.6, 1.400}},
        {"CA", "HA", SpeciesBond::HarmonicForm, {3071.1, 1.080}},
        // toluene AMBER JACS 117(1995)5179
        {"CA", "CT", SpeciesBond::HarmonicForm, {2652.7, 1.510}},
        // pyridinium JPCB110(2006)19586
        {"CA", "NA", SpeciesBond::HarmonicForm, {4042.0, 1.340}},
        // dialkylimidazolium JPCB108(2004)2038
        {"CR", "HA", SpeciesBond::HarmonicForm, {2845.0, 1.080}},
        {"CW*", "HA", SpeciesBond::HarmonicForm, {2845.0, 1.080}},
        {"CR", "NA", SpeciesBond::HarmonicForm, {3992.0, 1.315}},
        {"CW*", "NA", SpeciesBond::HarmonicForm, {3574.0, 1.378}},
        {"CW*", "CW*", SpeciesBond::HarmonicForm, {4352.0, 1.341}},
        {"NA", "CT", SpeciesBond::HarmonicForm, {2820.0, 1.466}},
        // alkylimidazolium JPCB 110 (2006) 19586
        {"HNA", "NA", SpeciesBond::HarmonicForm, {3632.0, 1.010}},
        // dialkylimethylmidazolium JPCB112(2008)5039
        {"CR", "CT", SpeciesBond::HarmonicForm, {2653.0, 1.510}},
        // fluoroalkyl JPCA105(2001)4118, JPCA106(2002)10116
        {"CT", "CF", SpeciesBond::HarmonicForm, {2242.6, 1.529}},
        {"FC", "CF", SpeciesBond::HarmonicForm, {3071.1, 1.332}},
        {"CF", "CF", SpeciesBond::HarmonicForm, {2242.6, 1.529}},
        // ammonium, pyrrolidinium OPLS-AA JACS121(1999)4827, AMBER
        {"NT", "CT", SpeciesBond::HarmonicForm, {3196.6, 1.448}},
        {"HN", "NT", SpeciesBond::HarmonicForm, {3632.0, 1.010}},
        // guanidinium
        {"HG", "NG", SpeciesBond::HarmonicForm, {3632.0, 1.010}},
        {"NG", "CG", SpeciesBond::HarmonicForm, {4027.7, 1.340}},
        // phosphonium OPLS-AA JPCB110(2006)19586
        {"PT", "CT", SpeciesBond::HarmonicForm, {3550.0, 1.81}},
        // hydroxyl OPLS-AA JACS 118(1996)11225, JPC 100(1996)18010
        {"CT", "OH", SpeciesBond::HarmonicForm, {2677.8, 1.410}},
        {"HO", "OH", SpeciesBond::HarmonicForm, {4627.5, 0.945}},
        // tetrafluoroborate
        {"B", "F", SpeciesBond::HarmonicForm, {3235.0, 1.394}},
        // hexafluorophosphate JCSPerkin2(1999)2365
        {"P", "F", SpeciesBond::HarmonicForm, {3100.0, 1.606}},
        // triflate and bistriflamide JPCB108(2004)16893
        {"FB", "CF", SpeciesBond::HarmonicForm, {3698.0, 1.323}},
        {"CF", "SB", SpeciesBond::HarmonicForm, {1950.0, 1.818}},
        {"SB", "OB", SpeciesBond::HarmonicForm, {5331.0, 1.437}},
        {"NB", "SB", SpeciesBond::HarmonicForm, {3137.0, 1.570}},
        // bis(fluorosulfonyl)amide
        {"FB", "SB", SpeciesBond::HarmonicForm, {1879.0, 1.575}},
        // dicyanamide JPCB110(2006)19586
        {"N3", "CZ", SpeciesBond::HarmonicForm, {4206.0, 1.310}},
        {"CZ", "NZ", SpeciesBond::HarmonicForm, {5439.2, 1.157}},
        // acetate OPLS-AA
        {"CO", "O2", SpeciesBond::HarmonicForm, {5489.0, 1.250}},
        {"CT", "CO", SpeciesBond::HarmonicForm, {2653.0, 1.522}},
        // trifluoroacetate (copied from old.oplsaa.ff) (JPCB, 110, 24, 2006, 12062)
        {"CF", "CO", SpeciesBond::HarmonicForm, {1087.84, 1.568}},
        {"CO", "O2F", SpeciesBond::HarmonicForm, {2322.12, 1.223}},
        {"CF", "FFA", SpeciesBond::HarmonicForm, {1004.16, 1.334}},
        // alkylsulfates JPCB112(2008)5039
        {"CT", "OC", SpeciesBond::HarmonicForm, {745.8, 1.402}},
        {"OS", "SO", SpeciesBond::HarmonicForm, {5331.0, 1.455}},
        {"OC", "SO", SpeciesBond::HarmonicForm, {1789.6, 1.633}},
        // alkylsulfonates JPCB112(2008)
        {"CT", "SO", SpeciesBond::HarmonicForm, {1970.0, 1.792}},
        // thiocyanate JCP128(2008)154504, our MP2
        {"SK", "CK", SpeciesBond::HarmonicForm, {2836.8, 1.67}},
        {"CK", "NK", SpeciesBond::HarmonicForm, {12221.5, 1.19}},
        // tricyanomethanide MG Martin STTR report 2008
        // C3A  CN  harm   1.408   1799.12
        // CN   NC  harm   1.167   5062.64
        // tricyanomethanide OPLS nitriles
        {"C3A", "CN", SpeciesBond::HarmonicForm, {3347.2, 1.412}},
        {"CN", "NC", SpeciesBond::HarmonicForm, {5439.2, 1.157}},
        // tosylate JPCB 112 (2008) 5039
        {"CA", "SO", SpeciesBond::HarmonicForm, {1970.0, 1.797}}};

    return Forcefield::termMatch_(bondTerms, i, j);
}

// Return angle term for the supplied atom type trio (if it exists)
OptionalReferenceWrapper<const ForcefieldAngleTerm>
PCL2019BaseForcefield::getAngleTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k) const
{
    static const std::vector<ForcefieldAngleTerm> angleTerms = {
        //	i	j	k	Type (Harmonic)			k	eq
        // alkyl OPLS-AA JACS118(1996)11225, JPC100(1996)18010
        {"CT", "CT", "CT", SpeciesAngle::HarmonicForm, {488.3, 112.7}},
        {"CT", "CT", "HC", SpeciesAngle::HarmonicForm, {313.8, 110.7}},
        {"HC", "CT", "HC", SpeciesAngle::HarmonicForm, {276.1, 107.8}},
        // aromatics AMBER JACS 117(1995)5179
        {"CA", "CA", "HA", SpeciesAngle::HarmonicForm, {292.9, 120.0}},
        {"CA", "CA", "CA", SpeciesAngle::HarmonicForm, {527.2, 120.0}},
        // toluene AMBER JACS 117(1995)5179
        {"CA", "CA", "CT", SpeciesAngle::HarmonicForm, {585.8, 120.0}},
        {"CA", "CT", "HC", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        // pyridinium JPCB110(2006)19586
        {"CA", "NA", "CA", SpeciesAngle::HarmonicForm, {585.8, 120.4}},
        {"CA", "NA", "CT", SpeciesAngle::HarmonicForm, {585.8, 119.8}},
        {"CA", "CA", "NA", SpeciesAngle::HarmonicForm, {585.8, 120.0}},
        {"NA", "CA", "HA", SpeciesAngle::HarmonicForm, {292.9, 120.0}},
        // dialkylimidazolium JPCB108(2004)2038
        {"CW*", "NA", "CR", SpeciesAngle::HarmonicForm, {585.8, 108.0}},
        {"CW*", "NA", "CT", SpeciesAngle::HarmonicForm, {585.8, 125.6}},
        {"CR", "NA", "CT", SpeciesAngle::HarmonicForm, {585.8, 126.4}},
        {"NA", "CR", "HA", SpeciesAngle::HarmonicForm, {292.9, 125.1}},
        {"NA", "CR", "NA", SpeciesAngle::HarmonicForm, {585.8, 109.8}},
        {"NA", "CW*", "CW*", SpeciesAngle::HarmonicForm, {585.8, 107.1}},
        {"NA", "CW*", "HA", SpeciesAngle::HarmonicForm, {292.9, 122.0}},
        {"CW*", "CW*", "HA", SpeciesAngle::HarmonicForm, {292.9, 130.9}},
        {"NA", "CT", "HC", SpeciesAngle::HarmonicForm, {313.8, 110.7}},
        {"NA", "CT", "CT", SpeciesAngle::HarmonicForm, {488.3, 112.7}},
        // alkylimidazolium JPCB 110 (2006) 19586
        {"CR", "NA", "HNA", SpeciesAngle::HarmonicForm, {292.9, 125.4}},
        {"CW*", "NA", "HNA", SpeciesAngle::HarmonicForm, {292.9, 126.6}},
        // dialkylimethylmidazolium JPCB112(2008)5039
        {"CT", "CR", "NA", SpeciesAngle::HarmonicForm, {585.8, 125.8}},
        {"CR", "CT", "HC", SpeciesAngle::HarmonicForm, {313.8, 110.7}},
        // benzylimidazolium AMBER
        {"NA", "CT", "CA", SpeciesAngle::HarmonicForm, {669.4, 111.2}},
        // fluoroalkyl JPCA105(2001)4118, JPCA106(2002)10116
        {"HC", "CT", "CF", SpeciesAngle::HarmonicForm, {313.8, 110.7}},
        {"FC", "CF", "CT", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CT", "CF", "CF", SpeciesAngle::HarmonicForm, {488.3, 112.7}},
        {"CT", "CT", "CF", SpeciesAngle::HarmonicForm, {488.3, 112.7}},
        {"FC", "CF", "FC", SpeciesAngle::HarmonicForm, {644.3, 109.1}},
        {"FC", "CF", "CF", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CF", "CF", "CF", SpeciesAngle::HarmonicForm, {488.3, 112.7}},
        // ammonium, pyrrolidinium OPLS-AA JACS121(1999)4827, AMBER
        {"NT", "CT", "CT", SpeciesAngle::HarmonicForm, {470.3, 109.5}},
        {"CT", "NT", "CT", SpeciesAngle::HarmonicForm, {433.5, 107.2}},
        {"HC", "CT", "NT", SpeciesAngle::HarmonicForm, {292.9, 109.5}},
        {"HN", "NT", "CT", SpeciesAngle::HarmonicForm, {292.9, 109.5}},
        // guanidinium
        {"CG", "NG", "HG", SpeciesAngle::HarmonicForm, {293.1, 121.6}},
        {"HG", "NG", "HG", SpeciesAngle::HarmonicForm, {293.1, 116.8}},
        {"NG", "CG", "NG", SpeciesAngle::HarmonicForm, {586.2, 120.0}},
        // phosphonium OPLS-AA, JPCB110(2006)19586
        {"CT", "PT", "CT", SpeciesAngle::HarmonicForm, {607.8, 109.5}},
        {"HC", "CT", "PT", SpeciesAngle::HarmonicForm, {389.9, 110.1}},
        {"CT", "CT", "PT", SpeciesAngle::HarmonicForm, {509.1, 115.2}},
        // hydroxyl JACS 118(1996)11225, JPC 100(1996)18010
        {"CT", "CT", "OH", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"HC", "CT", "OH", SpeciesAngle::HarmonicForm, {292.9, 109.5}},
        {"CT", "OH", "HO", SpeciesAngle::HarmonicForm, {460.2, 108.5}},
        // tetrafluoroborate anion
        {"F", "B", "F", SpeciesAngle::HarmonicForm, {669.5, 109.5}},
        // hexafluorophosphate JCSPerkin2(1999)2365
        {"F", "P", "F", SpeciesAngle::HarmonicForm, {1165.0, 90.0}},
        // triflate and bistriflamide JPCB108(2004)16893
        {"FB", "CF", "FB", SpeciesAngle::HarmonicForm, {781.0, 107.1}},
        {"FB", "CF", "SB", SpeciesAngle::HarmonicForm, {694.0, 111.7}},
        {"OB", "SB", "OB", SpeciesAngle::HarmonicForm, {969.0, 118.5}},
        {"CF", "SB", "OB", SpeciesAngle::HarmonicForm, {870.0, 102.6}},
        {"NB", "SB", "OB", SpeciesAngle::HarmonicForm, {789.0, 113.6}},
        {"NB", "SB", "CF", SpeciesAngle::HarmonicForm, {764.0, 103.5}},
        {"SB", "NB", "SB", SpeciesAngle::HarmonicForm, {671.0, 125.6}},
        // longer perfluoroalkanesulfonylamides
        {"SB", "CF", "CF", SpeciesAngle::HarmonicForm, {418.4, 115.9}},
        {"FB", "CF", "CF", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        // bis(fluorosulfonyl)amide
        {"FB", "SB", "OB", SpeciesAngle::HarmonicForm, {1077.0, 104.1}},
        {"FB", "SB", "NB", SpeciesAngle::HarmonicForm, {902.0, 103.0}},
        // dicyanamide JPCB110(2006)19586
        {"CZ", "N3", "CZ", SpeciesAngle::HarmonicForm, {362.0, 118.5}},
        {"N3", "CZ", "NZ", SpeciesAngle::HarmonicForm, {425.0, 175.2}},
        // acetate OPLS-AA JPCB (2004)
        {"O2*", "CO", "O2*", SpeciesAngle::HarmonicForm, {669.4, 126.0}},
        {"CT", "CO", "O2", SpeciesAngle::HarmonicForm, {585.8, 117.0}},
        {"HC", "CT", "CO", SpeciesAngle::HarmonicForm, {292.9, 109.5}},
        // trifluoroacetate OPLS-AA
        {"FC", "CF", "CO", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CF", "CO", "O2F", SpeciesAngle::HarmonicForm, {585.8, 117.0}},
        // alkylsulfates JPCB112(2008)5039
        {"OS", "SO", "OS", SpeciesAngle::HarmonicForm, {969.0, 114.0}},
        {"OC", "SO", "OS", SpeciesAngle::HarmonicForm, {1239.6, 103.5}},
        {"CT", "OC", "SO", SpeciesAngle::HarmonicForm, {300.5, 116.6}},
        {"HC", "CT", "OC", SpeciesAngle::HarmonicForm, {488.7, 109.7}},
        {"CT", "CT", "OC", SpeciesAngle::HarmonicForm, {765.6, 107.8}},
        // alkylsulfonates JPCB112(2008)5039
        {"CT", "SO", "OS", SpeciesAngle::HarmonicForm, {870.0, 104.5}},
        {"HC", "CT", "SO", SpeciesAngle::HarmonicForm, {390.3, 107.3}},
        {"CT", "CT", "SO", SpeciesAngle::HarmonicForm, {583.0, 113.3}},
        // thiocyanate JCP128(2008)154504
        {"SK", "CK", "NK", SpeciesAngle::HarmonicForm, {651.9, 180.0}},
        // tricyanomethanide OPLS nitriles
        {"C3A", "CN", "NC", SpeciesAngle::HarmonicForm, {1255.2, 180.0}},
        {"CN", "C3A", "CN", SpeciesAngle::HarmonicForm, {585.8, 120.0}},
        // tosylate OPLS, JPCB 112 (2008) 5039
        {"CA", "CA", "SO", SpeciesAngle::HarmonicForm, {711.28, 120.5}},
        {"CA", "SO", "OS", SpeciesAngle::HarmonicForm, {870.0, 104.2}}};

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
        {"HC", "CT", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.2552, 0.0000}},
        {"CT", "CT", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.2552, 0.0000}},
        {"CT", "CT", "CT", "CT", SpeciesTorsion::Cos4Form, {5.4392, -0.2092, 0.8368, 0.0000}},
        // aromatics AMBER JACS 117 (1995) 5179
        {"CA", "CA", "CA", "CA", SpeciesTorsion::Cos4Form, {0.0000, 30.3340, 0.0000, 0.0000}},
        {"CA", "CA", "CA", "HA", SpeciesTorsion::Cos4Form, {0.0000, 30.3340, 0.0000, 0.0000}},
        {"HA", "CA", "CA", "HA", SpeciesTorsion::Cos4Form, {0.0000, 30.3340, 0.0000, 0.0000}},
        // toluene AMBER JACS 117 (1995) 5179
        {"CT", "CA", "CA", "CA", SpeciesTorsion::Cos4Form, {0.0000, 30.3340, 0.0000, 0.0000}},
        {"CT", "CA", "CA", "HA", SpeciesTorsion::Cos4Form, {0.0000, 30.3340, 0.0000, 0.0000}},
        // CA  CA  CT  HC   opls   0.0000    0.0000    0.0000    0.0000
        // pyridinium JPCB 110 (2006) 19586
        {"NA", "CA", "CA", "CA", SpeciesTorsion::Cos4Form, {0.0000, 30.3340, 0.0000, 0.0000}},
        {"NA", "CA", "CA", "HA", SpeciesTorsion::Cos4Form, {0.0000, 30.3340, 0.0000, 0.0000}},
        {"CA", "NA", "CA", "CA", SpeciesTorsion::Cos4Form, {0.0000, 12.5520, 0.0000, 0.0000}},
        {"CA", "NA", "CA", "HA", SpeciesTorsion::Cos4Form, {0.0000, 12.5520, 0.0000, 0.0000}},
        {"CT", "NA", "CA", "CA", SpeciesTorsion::Cos4Form, {0.0000, 12.5520, 0.0000, 0.0000}},
        {"CT", "NA", "CA", "HA", SpeciesTorsion::Cos4Form, {0.0000, 12.5520, 0.0000, 0.0000}},
        {"CA", "NA", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.0000, 0.0000}},
        {"CA", "NA", "CT", "CT", SpeciesTorsion::Cos4Form, {0.0000, 1.0920, 0.0000, 0.7930}},
        // dialkylimidazolium JPCB 108 (2004) 2038
        {"CW*", "NA", "CR", "NA", SpeciesTorsion::Cos4Form, {0.0000, 19.4600, 0.0000, 0.0000}},
        {"CW*", "NA", "CR", "HA", SpeciesTorsion::Cos4Form, {0.0000, 19.4600, 0.0000, 0.0000}},
        {"CT", "NA", "CR", "NA", SpeciesTorsion::Cos4Form, {0.0000, 19.4600, 0.0000, 0.0000}},
        {"CT", "NA", "CR", "HA", SpeciesTorsion::Cos4Form, {0.0000, 19.4600, 0.0000, 0.0000}},
        {"CR", "NA", "CW*", "CW*", SpeciesTorsion::Cos4Form, {0.0000, 12.5520, 0.0000, 0.0000}},
        {"CR", "NA", "CW*", "HA", SpeciesTorsion::Cos4Form, {0.0000, 12.5520, 0.0000, 0.0000}},
        {"CT", "NA", "CW*", "CW*", SpeciesTorsion::Cos4Form, {0.0000, 12.5520, 0.0000, 0.0000}},
        {"CT", "NA", "CW*", "HA", SpeciesTorsion::Cos4Form, {0.0000, 12.5520, 0.0000, 0.0000}},
        {"NA", "CW*", "CW*", "NA", SpeciesTorsion::Cos4Form, {0.0000, 44.9800, 0.0000, 0.0000}},
        {"NA", "CW*", "CW*", "HA", SpeciesTorsion::Cos4Form, {0.0000, 44.9800, 0.0000, 0.0000}},
        {"HA", "CW*", "CW*", "HA", SpeciesTorsion::Cos4Form, {0.0000, 44.9800, 0.0000, 0.0000}},
        {"CW*", "NA", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.5190, 0.0000}},
        {"CR", "NA", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.0000, 0.0000}},
        {"CW*", "NA", "CT", "CT", SpeciesTorsion::Cos4Form, {-7.1535, 6.1064, 0.7939, 0.0000}},
        {"CR", "NA", "CT", "CT", SpeciesTorsion::Cos4Form, {-5.2691, 0.0000, 0.0000, 0.0000}},
        {"NA", "CT", "CT", "CT", SpeciesTorsion::Cos4Form, {-7.4797, 3.1642, -1.2026, 0.0000}},
        {"NA", "CT", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.3670, 0.0000}},
        // alkylimidazolium JPCB 110 (2006) 19586
        {"HNA", "NA", "CR", "NA", SpeciesTorsion::Cos4Form, {0.0000, 19.4600, 0.0000, 0.0000}},
        {"HNA", "NA", "CR", "HA", SpeciesTorsion::Cos4Form, {0.0000, 19.4600, 0.0000, 0.0000}},
        {"HNA", "NA", "CW*", "CW*", SpeciesTorsion::Cos4Form, {0.0000, 12.5500, 0.0000, 0.0000}},
        {"HNA", "NA", "CW*", "HA", SpeciesTorsion::Cos4Form, {0.0000, 12.5500, 0.0000, 0.0000}},
        // dialkylimethylmidazolium JPCB 112 (2008) 5039
        {"CW*", "NA", "CR", "CT", SpeciesTorsion::Cos4Form, {0.0000, 19.4600, 0.0000, 0.0000}},
        {"CT", "NA", "CR", "CT", SpeciesTorsion::Cos4Form, {0.0000, 19.4600, 0.0000, 0.0000}},
        {"NA", "CR", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.0000, 0.0000}},
        // benzylimidazolium AMBER wildcards
        {"NA", "CT", "CA", "CA", SpeciesTorsion::NoForm},
        {"CA", "CT", "NA", "CR", SpeciesTorsion::NoForm},
        {"CA", "CT", "NA", "CW", SpeciesTorsion::NoForm},
        // fluoroalkyl JPCA 105 (2001) 4118; JPCA 106 (2002) 10116
        {"F*", "CF", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.2134, 0.0000}},
        {"F*", "CF", "CT", "CT", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.9372, 0.0000}},
        {"F*", "CF", "CF", "CT", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 2.7656, 0.0000}},
        {"HC", "CT", "CT", "CF", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.5565, 0.0000}},
        {"HC", "CT", "CF", "CF", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.7573, 0.0000}},
        {"CF", "CF", "CF", "CT", SpeciesTorsion::Cos4Form, {14.6750, -0.9179, -2.8980, -2.0174}},
        {"CF", "CF", "CT", "CT", SpeciesTorsion::Cos4Form, {-0.5881, -0.7642, -0.3170, -0.3179}},
        {"CF", "CT", "CT", "NA", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.0000, 0.0000}},
        {"F*", "CF", "CF", "F*", SpeciesTorsion::Cos4Form, {-10.4600, 0.0000, 1.0460, 0.0000}},
        {"F*", "CF", "CF", "CF", SpeciesTorsion::Cos4Form, {1.2552, 0.0000, 1.6736, 0.0000}},
        {"CF", "CF", "CF", "CF", SpeciesTorsion::Cos4Form, {27.7064, 3.9664, -5.8074, -8.8617}},
        // ammonium, pyrrolidinium OPLS-AA JACS 121 (1999) 4827
        {"HC", "CT", "CT", "NT", SpeciesTorsion::Cos4Form, {-4.2384, -2.9665, 1.9790, 0.0000}},
        {"CT", "CT", "CT", "NT", SpeciesTorsion::Cos4Form, {10.0081, -2.8200, 2.3012, 0.0000}},
        {"CT", "NT", "CT", "CT", SpeciesTorsion::Cos4Form, {1.7405, -0.5356, 2.9079, 0.0000}},
        {"HC", "CT", "NT", "CT", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 2.3430, 0.0000}},
        {"HN", "NT", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.6736, 0.0000}},
        {"HN", "NT", "CT", "CT", SpeciesTorsion::Cos4Form, {-0.7950, -1.7447, 1.7489, 0.0000}},
        // cholinium: unpublished
        {"OH", "CT", "CT", "NT", SpeciesTorsion::Cos4Form, {-44.0515, -5.4349, 0.0000, 0.0000}},
        // OH  CT  CT  NT   opls -44.0515   -5.0148    0.0000   -3.1510
        // guanidinium
        {"HG", "NG", "CG", "NG", SpeciesTorsion::Cos4Form, {0.0000, 16.3285, 0.0000, 0.0000}},
        // phosphonium OPLS-AA, JPCB 110 (2006) 19586
        {"CT", "PT", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.9270, 0.0000}},
        {"CT", "PT", "CT", "CT", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.1330, 0.0000}},
        {"PT", "CT", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.4650, 0.0000}},
        {"PT", "CT", "CT", "CT", SpeciesTorsion::Cos4Form, {-3.2480, 0.9880, -0.7150, 0.0000}},
        // hydroxyl JACS 118 (1996) 11225; AMBER98 (OCCO) 117 (1995) 5179
        {"HC", "CT", "OH", "HO", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.8828, 0.0000}},
        {"CT", "CT", "OH", "HO", SpeciesTorsion::Cos4Form, {-1.4895, -0.7280, 2.0585, 0.0000}},
        {"HC", "CT", "CT", "OH", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.9581, 0.0000}},
        {"OH", "CT", "CT", "OH", SpeciesTorsion::Cos4Form, {0.0000, -9.8324, 1.2050, 0.0000}},
        // triflate and bistriflamide JPCB 108 (2004) 16893
        {"OB", "SB", "CF", "FB", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.4510, 0.0000}},
        {"NB", "SB", "CF", "FB", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.3220, 0.0000}},
        {"OB", "SB", "NB", "SB", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, -0.0150, 0.0000}},
        {"SB", "NB", "SB", "CF", SpeciesTorsion::Cos4Form, {32.7730, -10.4200, -3.1950, 0.0000}},
        // longer perfluoroalkanesulfonylamides
        {"FC", "CF", "CF", "FB", SpeciesTorsion::Cos4Form, {-10.4600, 0.0000, 1.0460, 0.0000}},
        {"SB", "CF", "CF", "FB", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.4530, 0.0000}},
        {"SB", "CF", "CF", "FC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.4530, 0.0000}},
        {"OB", "SB", "CF", "CF", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, -0.7400, 0.0000}},
        {"NB", "SB", "CF", "CF", SpeciesTorsion::Cos4Form, {-3.0940, 0.0000, 0.0000, 0.0000}},
        {"SB", "CF", "CF", "CF", SpeciesTorsion::Cos4Form, {50.0900, 0.0000, -4.6260, -4.0080}},
        // bis(fluorosulfonyl)amide
        {"FB", "SB", "NB", "SB", SpeciesTorsion::Cos4Form, {11.4450, -15.1860, -3.2120, 0.0000}},
        // dicyanamide JPCB 110 (2006) 19586
        {"NZ", "CZ", "N3", "CZ", SpeciesTorsion::Cos4Form, {4.0800, 0.0000, 0.0000, 0.0000}},
        // acetate OPLS-AA JPCB (2004)
        {"HC", "CT", "CO", "O2", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.0000, 0.0000}},
        // trifluoroacetate
        {"FC", "CF", "CO", "O2F", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.0000, 0.0000}},
        // alkylsulfates JPCB 112 (2008) 5039
        {"OS", "SO", "OC", "CT", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 2.4815, 0.0000}},
        {"SO", "OC", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.6858, 0.0000}},
        {"SO", "OC", "CT", "CT", SpeciesTorsion::Cos4Form, {-6.0142, -3.1133, 1.4941, 0.0000}},
        {"OC", "CT", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 2.0698, 0.0000}},
        {"OC", "CT", "CT", "CT", SpeciesTorsion::Cos4Form, {4.3893, -1.8273, 2.9705, 0.0000}},
        // alkylsulfonates JPCB 112 (2008) 5039
        {"OS", "SO", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.6250, 0.0000}},
        {"OS", "SO", "CT", "CT", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.3938, 0.0000}},
        {"SO", "CT", "CT", "HC", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 1.3797, 0.0000}},
        {"SO", "CT", "CT", "CT", SpeciesTorsion::Cos4Form, {-16.1000, -2.0046, 0.7674, 0.0000}},
        // tricyanomethanide
        {"NC", "CN", "C3A", "CN", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.0000, 0.0000}},
        // tosylate our MP2, OPLS aromatics
        {"CA", "CA", "SO", "OS", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.0000, 0.0000}},
        {"SO", "CA", "CA", "CA", SpeciesTorsion::Cos4Form, {0.0000, 30.3340, 0.0000, 0.0000}},
        {"SO", "CA", "CA", "HA", SpeciesTorsion::Cos4Form, {0.0000, 30.3340, 0.0000, 0.0000}},
        {"HC", "CT", "CA", "CA", SpeciesTorsion::Cos4Form, {0.0000, 0.0000, 0.0000, 0.0000}},
        // alkoxy (taken from old.il.ff)
        {"NA", "CT", "CT", "OH", SpeciesTorsion::Cos4Form, {-3.5787, -1.6564, 4.9154, 0.0000}}};

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
        {"CA", "CA", "CA", "HA", SpeciesTorsion::Cos4Form, {0.0000, 9.2048, 0.0000, 0.0000}},
        {"CA", "CA", "CA", "CA", SpeciesTorsion::Cos4Form, {0.0000, 9.2048, 0.0000, 0.0000}},
        {"CA", "CA", "CA", "CT", SpeciesTorsion::Cos4Form, {0.0000, 9.2048, 0.0000, 0.0000}},
        {"NA", "CA", "CA", "CT", SpeciesTorsion::Cos4Form, {0.0000, 9.2048, 0.0000, 0.0000}},
        {"CA", "CA", "NA", "HA", SpeciesTorsion::Cos4Form, {0.0000, 9.2048, 0.0000, 0.0000}},
        // improper imidazolium ring AMBER JACS 117 (1995) 5179
        {"NA", "CR", "CW*", "CT", SpeciesTorsion::Cos4Form, {0.0000, 8.3680, 0.0000, 0.0000}},
        {"NA", "CR", "CW*", "HA", SpeciesTorsion::Cos4Form, {0.0000, 8.3680, 0.0000, 0.0000}},
        {"CR", "NA", "NA", "HA", SpeciesTorsion::Cos4Form, {0.0000, 9.2048, 0.0000, 0.0000}},
        {"CR", "NA", "NA", "CT", SpeciesTorsion::Cos4Form, {0.0000, 9.2048, 0.0000, 0.0000}},
        {"CW*", "NA", "CW*", "HA", SpeciesTorsion::Cos4Form, {0.0000, 9.2048, 0.0000, 0.0000}},
        // improper tricyanomethanide AMBER JACS 117 (1995) 5179
        {"C3A", "CN", "CN", "CN", SpeciesTorsion::Cos4Form, {0.0000, 9.2048, 0.0000, 0.0000}},
        // improper carbonyl AMBER JACS 117 (1995 )5179
        {"CO", "CT", "O2", "O2", SpeciesTorsion::Cos4Form, {0.0000, 87.8640, 0.0000, 0.0000}},
        // improper guanidinium AMBER JACS 117 (1995 )5179
        {"CG", "NG", "NG", "NG", SpeciesTorsion::Cos4Form, {0.0000, 8.3680, 0.0000, 0.0000}},
        {"NG", "CG", "HG", "HG", SpeciesTorsion::Cos4Form, {0.0000, 9.2048, 0.0000, 0.0000}},
        // tosylate aromatics AMBER JACS 117 (1995) 5179
        {"CA", "CA", "CA", "SO", SpeciesTorsion::Cos4Form, {0.0000, 9.2048, 0.0000, 0.0000}}};

    return Forcefield::termMatch_(improperTerms, i, j, k, l);
}
