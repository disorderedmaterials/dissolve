// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "data/ff/angleterm.h"
#include "data/ff/bondterm.h"
#include "data/ff/improperterm.h"
#include "data/ff/oplsaa2005/base.h"
#include "data/ff/torsionterm.h"
#include <algorithm>
#include <functional>
#include <optional>
#include <vector>

/*
 * Term Assignment
 */

// Return bond term for the supplied atom type pair (if it exists)
OptionalReferenceWrapper<const ForcefieldBondTerm> OPLSAA2005BaseForcefield::getBondTerm(const ForcefieldAtomType &i,
                                                                                         const ForcefieldAtomType &j) const
{
    static const std::vector<ForcefieldBondTerm> bondTerms = {
        //	i	j	Type (Harmonic)			k	eq
        {"OW", "HW", BondFunctions::Form::Harmonic, {5020.8, 0.9572}},
        /* { "OW",	"LP",	BondFunctions::Form::Harmonic,	7531.2,	0.175 }, */
        {"C^", "HC", BondFunctions::Form::Harmonic, {2845.12, 1.08}},
        {"C", "C2", BondFunctions::Form::Harmonic, {2652.66, 1.522}},
        {"C", "C3", BondFunctions::Form::Harmonic, {2652.66, 1.522}},
        {"C", "CA", BondFunctions::Form::Harmonic, {3347.2, 1.49}},
        {"C", "CB", BondFunctions::Form::Harmonic, {3740.5, 1.419}},
        {"C", "CD", BondFunctions::Form::Harmonic, {3924.59, 1.4}},
        {"C", "CH", BondFunctions::Form::Harmonic, {2652.66, 1.522}},
        /* { "C",	"CJ",	BondFunctions::Form::Harmonic,	3430.88,	1.444 }, */
        {"C", "CM", BondFunctions::Form::Harmonic, {3430.88, 1.444}},
        {"C", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.522}},
        {"C$", "CY", BondFunctions::Form::Harmonic, {2652.66, 1.522}},
        {"C$", "N$", BondFunctions::Form::Harmonic, {4100.32, 1.335}},
        {"C", "N", BondFunctions::Form::Harmonic, {4100.32, 1.335}},
        {"C", "NM", BondFunctions::Form::Harmonic, {4100.32, 1.335}},
        {"C", "N^", BondFunctions::Form::Harmonic, {3548.03, 1.383}},
        {"C", "NA", BondFunctions::Form::Harmonic, {3497.82, 1.388}},
        {"C", "NC", BondFunctions::Form::Harmonic, {3824.18, 1.358}},
        /* { "C",	"Neq",	BondFunctions::Form::Harmonic,	3824.18,	1.29 }, */
        {"C", "O", BondFunctions::Form::Harmonic, {4769.76, 1.229}},
        {"C$", "O", BondFunctions::Form::Harmonic, {4769.76, 1.229}},
        {"C", "O2", BondFunctions::Form::Harmonic, {5489.41, 1.25}},
        {"C", "OH", BondFunctions::Form::Harmonic, {3765.6, 1.364}},
        /* { "C",	"Seq",	BondFunctions::Form::Harmonic,	3347.2,		1.64 }, */
        {"NO", "ON", BondFunctions::Form::Harmonic, {4602.4, 1.225}},
        {"CT", "NO", BondFunctions::Form::Harmonic, {3138, 1.49}},
        {"CA", "NO", BondFunctions::Form::Harmonic, {3347.2, 1.46}},
        {"CA", "OH", BondFunctions::Form::Harmonic, {3765.6, 1.364}},
        {"CA", "OS", BondFunctions::Form::Harmonic, {3765.6, 1.364}},
        {"CB", "OS", BondFunctions::Form::Harmonic, {2845.12, 1.36}},
        {"CM", "OS", BondFunctions::Form::Harmonic, {3765.6, 1.37}},
        {"CM", "OH", BondFunctions::Form::Harmonic, {3765.6, 1.37}},
        {"C", "OS", BondFunctions::Form::Harmonic, {1790.75, 1.327}},
        {"C^", "C2", BondFunctions::Form::Harmonic, {2652.66, 1.495}},
        {"C^", "CB", BondFunctions::Form::Harmonic, {3246.78, 1.459}},
        /* { "C^",	"CG",	BondFunctions::Form::Harmonic,	4568.93,	1.352 }, */
        /* { "C^",	"CC",	BondFunctions::Form::Harmonic,	4568.93,	1.352 }, */
        {"C^", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.495}},
        {"CS", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.495}},
        {"C^", "CW", BondFunctions::Form::Harmonic, {4568.93, 1.352}},
        {"CA", "CW", BondFunctions::Form::Harmonic, {4568.93, 1.367}},
        {"CS", "CW", BondFunctions::Form::Harmonic, {4568.93, 1.367}},
        {"CS", "CS", BondFunctions::Form::Harmonic, {3924.59, 1.424}},
        {"CS", "CB", BondFunctions::Form::Harmonic, {3924.59, 1.424}},
        {"CS", "HA", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CU", "NB", BondFunctions::Form::Harmonic, {3430.88, 1.32}},
        {"CU", "CA", BondFunctions::Form::Harmonic, {3924.59, 1.421}},
        {"CU", "HA", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"NA", "NB", BondFunctions::Form::Harmonic, {3347.2, 1.349}},
        {"NB", "NB", BondFunctions::Form::Harmonic, {3347.2, 1.28}},
        {"OS", "NB", BondFunctions::Form::Harmonic, {3866.02, 1.399}},
        {"OS", "CR", BondFunctions::Form::Harmonic, {3866.02, 1.357}},
        {"C2", "C2", BondFunctions::Form::Harmonic, {2175.68, 1.526}},
        {"C2", "C3", BondFunctions::Form::Harmonic, {2175.68, 1.526}},
        {"C3", "C3", BondFunctions::Form::Harmonic, {2175.68, 1.526}},
        {"C2", "CA", BondFunctions::Form::Harmonic, {2652.66, 1.51}},
        {"C2", "CD", BondFunctions::Form::Harmonic, {2652.66, 1.51}},
        /* { "C2",	"CC",	BondFunctions::Form::Harmonic,	2652.66,	1.504 }, */
        {"C2", "CH", BondFunctions::Form::Harmonic, {2175.68, 1.526}},
        {"C2", "N", BondFunctions::Form::Harmonic, {2820.02, 1.449}},
        {"C2", "N2", BondFunctions::Form::Harmonic, {2820.02, 1.463}},
        {"C2", "N3", BondFunctions::Form::Harmonic, {3071.06, 1.471}},
        {"CH", "NT", BondFunctions::Form::Harmonic, {3196.58, 1.448}},
        {"C2", "NT", BondFunctions::Form::Harmonic, {3196.58, 1.448}},
        {"C3", "NT", BondFunctions::Form::Harmonic, {3196.58, 1.448}},
        {"CT", "NT", BondFunctions::Form::Harmonic, {3196.58, 1.448}},
        {"NT", "NT", BondFunctions::Form::Harmonic, {2928.8, 1.445}},
        {"C2", "OH", BondFunctions::Form::Harmonic, {3230.05, 1.425}},
        {"C2", "OS", BondFunctions::Form::Harmonic, {2677.76, 1.425}},
        {"CO", "OS", BondFunctions::Form::Harmonic, {2677.76, 1.38}},
        {"CO", "C2", BondFunctions::Form::Harmonic, {2175.68, 1.526}},
        {"CO", "C3", BondFunctions::Form::Harmonic, {2175.68, 1.526}},
        {"CW", "OS", BondFunctions::Form::Harmonic, {2845.12, 1.36}},
        {"C2", "S", BondFunctions::Form::Harmonic, {1857.7, 1.81}},
        {"C2", "SH", BondFunctions::Form::Harmonic, {1857.7, 1.81}},
        {"C3", "CH", BondFunctions::Form::Harmonic, {2175.68, 1.526}},
        {"C3", "CM", BondFunctions::Form::Harmonic, {2652.66, 1.51}},
        {"C3", "N", BondFunctions::Form::Harmonic, {2820.02, 1.449}},
        {"C3", "N^", BondFunctions::Form::Harmonic, {2820.02, 1.475}},
        {"C3", "N2", BondFunctions::Form::Harmonic, {2820.02, 1.463}},
        {"C3", "N3", BondFunctions::Form::Harmonic, {3071.06, 1.471}},
        {"C3", "OH", BondFunctions::Form::Harmonic, {3230.05, 1.425}},
        {"C3", "OS", BondFunctions::Form::Harmonic, {2677.76, 1.425}},
        {"C3", "S", BondFunctions::Form::Harmonic, {1857.7, 1.81}},
        {"C3", "SH", BondFunctions::Form::Harmonic, {1857.7, 1.81}},
        {"CA", "CA", BondFunctions::Form::Harmonic, {3924.59, 1.4}},
        {"CA", "C!", BondFunctions::Form::Harmonic, {3924.59, 1.4}},
        {"C!", "C!", BondFunctions::Form::Harmonic, {3221.68, 1.46}},
        {"C!", "CS", BondFunctions::Form::Harmonic, {3221.68, 1.46}},
        {"C!", "CU", BondFunctions::Form::Harmonic, {3221.68, 1.46}},
        {"C!", "CV", BondFunctions::Form::Harmonic, {3221.68, 1.46}},
        {"C!", "CW", BondFunctions::Form::Harmonic, {3221.68, 1.46}},
        {"C!", "CR", BondFunctions::Form::Harmonic, {3221.68, 1.46}},
        {"C!", "C", BondFunctions::Form::Harmonic, {3221.68, 1.46}},
        {"C!", "CM", BondFunctions::Form::Harmonic, {3221.68, 1.46}},
        {"C!", "NA", BondFunctions::Form::Harmonic, {3221.68, 1.44}},
        {"CA", "CB", BondFunctions::Form::Harmonic, {3924.59, 1.404}},
        {"CA", "CD", BondFunctions::Form::Harmonic, {3924.59, 1.4}},
        /* { "CA",	"CJ",	BondFunctions::Form::Harmonic,	3573.14,	1.433 }, */
        {"CA", "CM", BondFunctions::Form::Harmonic, {3573.14, 1.433}},
        {"CA", "CN", BondFunctions::Form::Harmonic, {3924.59, 1.4}},
        {"CA", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.51}},
        {"CA", "CY", BondFunctions::Form::Harmonic, {2652.66, 1.49}},
        {"CW", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.504}},
        {"CV", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.504}},
        {"CX", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.504}},
        {"CX", "CX", BondFunctions::Form::Harmonic, {4351.36, 1.37}},
        {"CX", "NA", BondFunctions::Form::Harmonic, {3573.14, 1.381}},
        {"CX", "HA", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CA", "NY", BondFunctions::Form::Harmonic, {3196.58, 1.385}},
        {"CA", "NZ", BondFunctions::Form::Harmonic, {4.184, 1.261}},
        {"NY", "H", BondFunctions::Form::Harmonic, {3631.71, 1.01}},
        {"NZ", "H", BondFunctions::Form::Harmonic, {3631.71, 1.01}},
        {"CT", "NY", BondFunctions::Form::Harmonic, {3196.58, 1.448}},
        {"CA", "N2", BondFunctions::Form::Harmonic, {4025.01, 1.34}},
        {"CQ", "N2", BondFunctions::Form::Harmonic, {4025.01, 1.34}},
        {"CR", "N2", BondFunctions::Form::Harmonic, {4025.01, 1.34}},
        {"CA", "NT", BondFunctions::Form::Harmonic, {4025.01, 1.34}},
        {"CA", "NA", BondFunctions::Form::Harmonic, {3573.14, 1.381}},
        {"CQ", "N", BondFunctions::Form::Harmonic, {3573.14, 1.381}},
        {"CA", "NC", BondFunctions::Form::Harmonic, {4041.74, 1.339}},
        {"C!", "NC", BondFunctions::Form::Harmonic, {4041.74, 1.339}},
        {"NC", "NC", BondFunctions::Form::Harmonic, {4.184, 1.32}},
        {"NC", "NZ", BondFunctions::Form::Harmonic, {4602.4, 1.24}},
        {"NZ", "NZ", BondFunctions::Form::Harmonic, {4602.4, 1.12}},
        {"CT", "NZ", BondFunctions::Form::Harmonic, {3263.52, 1.43}},
        {"CA", "NZ", BondFunctions::Form::Harmonic, {3347.2, 1.41}},
        {"CA", "HA", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CB", "CB", BondFunctions::Form::Harmonic, {4351.36, 1.37}},
        {"CR", "CS", BondFunctions::Form::Harmonic, {4351.36, 1.37}},
        {"CV", "CW", BondFunctions::Form::Harmonic, {4351.36, 1.37}},
        {"CU", "CW", BondFunctions::Form::Harmonic, {4351.36, 1.37}},
        {"CB", "CD", BondFunctions::Form::Harmonic, {3924.59, 1.4}},
        {"CB", "CN", BondFunctions::Form::Harmonic, {3740.5, 1.419}},
        {"CB", "N^", BondFunctions::Form::Harmonic, {3648.45, 1.374}},
        {"CB", "NA", BondFunctions::Form::Harmonic, {3648.45, 1.374}},
        {"CB", "NB", BondFunctions::Form::Harmonic, {3464.35, 1.391}},
        {"CB", "NC", BondFunctions::Form::Harmonic, {3857.65, 1.354}},
        {"CR", "NC", BondFunctions::Form::Harmonic, {3857.65, 1.354}},
        /* { "CC",	"CF",	BondFunctions::Form::Harmonic,	4284.42,	1.375 }, */
        /* { "CC",	"CG",	BondFunctions::Form::Harmonic,	4334.62,	1.371 }, */
        /* { "CC",	"CT",	BondFunctions::Form::Harmonic,	2652.66,	1.504 }, */
        /* { "CC",	"CV",	BondFunctions::Form::Harmonic,	4284.42,	1.375 }, */
        {"CW", "CW", BondFunctions::Form::Harmonic, {4284.42, 1.375}},
        /* { "CC",	"CW",	BondFunctions::Form::Harmonic,	4334.62,	1.371 }, */
        /* { "CC",	"NA",	BondFunctions::Form::Harmonic,	3531.3,	1.385 }, */
        /* { "CC",	"NB",	BondFunctions::Form::Harmonic,	3430.88,	1.394 }, */
        {"CD", "CD", BondFunctions::Form::Harmonic, {3924.59, 1.4}},
        {"CD", "CN", BondFunctions::Form::Harmonic, {3924.59, 1.4}},
        /* { "CA",	"CC",	BondFunctions::Form::Harmonic,	3924.59,	1.4 }, */
        /* { "CD",	"CC",	BondFunctions::Form::Harmonic,	3924.59,	1.4 }, */
        /* { "CE",	"N^",	BondFunctions::Form::Harmonic,	3681.92,	1.371 }, */
        /* { "CE",	"NB",	BondFunctions::Form::Harmonic,	4426.67,	1.304 }, */
        /* { "CF",	"NB",	BondFunctions::Form::Harmonic,	3430.88,	1.394 }, */
        /* { "CG",	"NA",	BondFunctions::Form::Harmonic,	3573.14,	1.381 }, */
        {"CH", "CH", BondFunctions::Form::Harmonic, {2175.68, 1.526}},
        {"CH", "N", BondFunctions::Form::Harmonic, {2820.02, 1.449}},
        {"CH", "N^", BondFunctions::Form::Harmonic, {2820.02, 1.475}},
        {"CH", "OH", BondFunctions::Form::Harmonic, {3230.05, 1.425}},
        {"CH", "OS", BondFunctions::Form::Harmonic, {2677.76, 1.425}},
        /* { "CI",	"NC",	BondFunctions::Form::Harmonic,	4200.74,	1.324 }, */
        /* { "CJ",	"CJ",	BondFunctions::Form::Harmonic,	4594.03,	1.35 }, */
        /* { "CJ",	"CM",	BondFunctions::Form::Harmonic,	4594.03,	1.35 }, */
        /* { "CJ",	"N^",	BondFunctions::Form::Harmonic,	3748.86,	1.365 }, */
        {"CK", "HA", BondFunctions::Form::Harmonic, {2845.12, 1.08}},
        {"CK", "H5", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CK", "N^", BondFunctions::Form::Harmonic, {3681.92, 1.371}},
        {"CK", "NA", BondFunctions::Form::Harmonic, {3681.92, 1.371}},
        {"CK", "NB", BondFunctions::Form::Harmonic, {4426.67, 1.304}},
        {"CM", "CM", BondFunctions::Form::Harmonic, {4594.03, 1.34}},
        {"CM", "C=", BondFunctions::Form::Harmonic, {4594.03, 1.34}},
        {"CW", "C=", BondFunctions::Form::Harmonic, {4594.03, 1.365}},
        {"C=", "C=", BondFunctions::Form::Harmonic, {3221.68, 1.46}},
        /* { "C=",	"Neq",	BondFunctions::Form::Harmonic,	3472.72,	1.428 }, */
        {"C", "C", BondFunctions::Form::Harmonic, {2928.8, 1.51}},
        {"C=", "C", BondFunctions::Form::Harmonic, {3221.68, 1.46}},
        {"CT", "C+", BondFunctions::Form::Harmonic, {4458.47, 1.46}},
        {"C+", "HC", BondFunctions::Form::Harmonic, {4458.47, 1.084}},
        {"CM", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.51}},
        {"C=", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.51}},
        {"CM", "HC", BondFunctions::Form::Harmonic, {2845.12, 1.08}},
        {"CM", "H4", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"C=", "HC", BondFunctions::Form::Harmonic, {2845.12, 1.08}},
        {"HC", "C", BondFunctions::Form::Harmonic, {2845.12, 1.09}},
        {"CT", "CZ", BondFunctions::Form::Harmonic, {3263.52, 1.47}},
        {"CA", "CZ", BondFunctions::Form::Harmonic, {3347.2, 1.451}},
        {"CY", "CZ", BondFunctions::Form::Harmonic, {3347.2, 1.451}},
        {"CZ", "NZ", BondFunctions::Form::Harmonic, {5439.2, 1.157}},
        {"CZ", "CZ", BondFunctions::Form::Harmonic, {9623.2, 1.21}},
        {"HC", "CZ", BondFunctions::Form::Harmonic, {3514.56, 1.08}},
        {"CM", "N^", BondFunctions::Form::Harmonic, {3748.86, 1.365}},
        {"CM", "NA", BondFunctions::Form::Harmonic, {3748.86, 1.365}},
        {"CN", "NA", BondFunctions::Form::Harmonic, {3581.5, 1.38}},
        /* { "CP",	"NA",	BondFunctions::Form::Harmonic,	3991.54,	1.343 }, */
        /* { "CP",	"NB",	BondFunctions::Form::Harmonic,	4083.58,	1.335 }, */
        {"CQ", "HA", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CQ", "H5", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CQ", "NC", BondFunctions::Form::Harmonic, {4200.74, 1.324}},
        {"CR", "HA", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CR", "H5", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CR", "NA", BondFunctions::Form::Harmonic, {3991.54, 1.343}},
        {"CR", "NB", BondFunctions::Form::Harmonic, {4083.58, 1.335}},
        {"CT", "CT", BondFunctions::Form::Harmonic, {2242.62, 1.529}},
        {"CT", "HC", BondFunctions::Form::Harmonic, {2845.12, 1.09}},
        {"DM", "Cl", BondFunctions::Form::Harmonic, {2510.4, 0.3}},
        {"DM", "Br", BondFunctions::Form::Harmonic, {2510.4, 0.3}},
        {"DM", "F", BondFunctions::Form::Harmonic, {2510.4, 0.3}},
        {"DM", "HC", BondFunctions::Form::Harmonic, {2845.12, 0.3}},
        {"DM", "HO", BondFunctions::Form::Harmonic, {2845.12, 0.1}},
        {"DM", "HS", BondFunctions::Form::Harmonic, {2845.12, 0.1}},
        {"DM", "HA", BondFunctions::Form::Harmonic, {2845.12, 0.3}},
        {"DM", "CA", BondFunctions::Form::Harmonic, {3071.06, 0.3}},
        {"DM", "NC", BondFunctions::Form::Harmonic, {3071.06, 0.3}},
        {"DM", "NB", BondFunctions::Form::Harmonic, {3071.06, 0.3}},
        {"DM", "CT", BondFunctions::Form::Harmonic, {2845.12, 0.3}},
        {"DM", "SZ", BondFunctions::Form::Harmonic, {2845.12, 0.5}},
        {"DM", "S", BondFunctions::Form::Harmonic, {2845.12, 0.5}},
        {"DM", "OS", BondFunctions::Form::Harmonic, {2845.12, 0.3}},
        {"DM", "OY", BondFunctions::Form::Harmonic, {2845.12, 0.3}},
        {"DM", "OH", BondFunctions::Form::Harmonic, {2845.12, 0.3}},
        {"CT", "N", BondFunctions::Form::Harmonic, {2820.02, 1.449}},
        {"CT", "NM", BondFunctions::Form::Harmonic, {2820.02, 1.449}},
        {"CT", "NC", BondFunctions::Form::Harmonic, {2820.02, 1.449}},
        {"CY", "N$", BondFunctions::Form::Harmonic, {2820.02, 1.449}},
        {"CT", "N$", BondFunctions::Form::Harmonic, {2820.02, 1.449}},
        {"CY", "N", BondFunctions::Form::Harmonic, {2820.02, 1.449}},
        {"CT", "N^", BondFunctions::Form::Harmonic, {2820.02, 1.475}},
        {"CO", "N^", BondFunctions::Form::Harmonic, {2820.02, 1.475}},
        {"CT", "NA", BondFunctions::Form::Harmonic, {2820.02, 1.475}},
        {"CT", "N2", BondFunctions::Form::Harmonic, {2820.02, 1.463}},
        {"CT", "N3", BondFunctions::Form::Harmonic, {3071.06, 1.471}},
        {"CA", "N3", BondFunctions::Form::Harmonic, {3347.2, 1.45}},
        {"CT", "OH", BondFunctions::Form::Harmonic, {2677.76, 1.41}},
        {"NT", "OH", BondFunctions::Form::Harmonic, {2677.76, 1.45}},
        {"NT", "OS", BondFunctions::Form::Harmonic, {2677.76, 1.45}},
        {"N", "OH", BondFunctions::Form::Harmonic, {3347.2, 1.38}},
        {"CT", "OS", BondFunctions::Form::Harmonic, {2677.76, 1.41}},
        {"OS", "OS", BondFunctions::Form::Harmonic, {2092, 1.47}},
        {"OS", "OH", BondFunctions::Form::Harmonic, {2092, 1.47}},
        {"OS", "Cl", BondFunctions::Form::Harmonic, {1673.6, 1.69}},
        /* ####  0.0  0 */
        {"Si", "CT", BondFunctions::Form::Harmonic, {1564.82, 1.86}},
        {"Si", "Si", BondFunctions::Form::Harmonic, {786.592, 2.32}},
        {"Si", "HC", BondFunctions::Form::Harmonic, {1389.09, 1.48}},
        {"Si", "H", BondFunctions::Form::Harmonic, {1389.09, 1.48}},
        {"Si", "F", BondFunctions::Form::Harmonic, {3857.65, 1.57}},
        {"Si", "Cl", BondFunctions::Form::Harmonic, {1866.06, 2.02}},
        {"Si", "Br", BondFunctions::Form::Harmonic, {1263.57, 2.19}},
        {"Si", "I", BondFunctions::Form::Harmonic, {903.744, 2.44}},
        {"Si", "OH", BondFunctions::Form::Harmonic, {3129.63, 1.64}},
        {"Si", "OS", BondFunctions::Form::Harmonic, {3129.63, 1.64}},
        {"Si", "P", BondFunctions::Form::Harmonic, {903.744, 2.25}},
        {"Si", "NT", BondFunctions::Form::Harmonic, {2225.89, 1.74}},
        {"Si", "S", BondFunctions::Form::Harmonic, {1204.99, 2.15}},
        /* ####  0.0  0 */
        {"CT", "S", BondFunctions::Form::Harmonic, {1857.7, 1.81}},
        {"CY", "S", BondFunctions::Form::Harmonic, {1857.7, 1.81}},
        {"CR", "S", BondFunctions::Form::Harmonic, {2092, 1.76}},
        {"CW", "S", BondFunctions::Form::Harmonic, {2092, 1.74}},
        {"CA", "SH", BondFunctions::Form::Harmonic, {2092, 1.74}},
        {"CT", "SH", BondFunctions::Form::Harmonic, {1857.7, 1.81}},
        {"CT", "Cl", BondFunctions::Form::Harmonic, {2050.16, 1.781}},
        {"CA", "Cl", BondFunctions::Form::Harmonic, {2510.4, 1.725}},
        {"CM", "Cl", BondFunctions::Form::Harmonic, {2510.4, 1.725}},
        {"C", "Cl", BondFunctions::Form::Harmonic, {2510.4, 1.79}},
        {"CZ", "Cl", BondFunctions::Form::Harmonic, {2761.44, 1.637}},
        {"CT", "Br", BondFunctions::Form::Harmonic, {2050.16, 1.945}},
        {"CA", "Br", BondFunctions::Form::Harmonic, {2510.4, 1.87}},
        {"CM", "Br", BondFunctions::Form::Harmonic, {2510.4, 1.9}},
        {"C", "Br", BondFunctions::Form::Harmonic, {2510.4, 1.98}},
        {"CZ", "Br", BondFunctions::Form::Harmonic, {2761.44, 1.784}},
        {"CA", "I", BondFunctions::Form::Harmonic, {2092, 2.08}},
        {"CT", "I", BondFunctions::Form::Harmonic, {1673.6, 2.19}},
        {"CV", "HA", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CV", "H4", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CV", "NB", BondFunctions::Form::Harmonic, {3430.88, 1.394}},
        {"CW", "NB", BondFunctions::Form::Harmonic, {3430.88, 1.394}},
        {"CW", "H4", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CW", "HA", BondFunctions::Form::Harmonic, {3071.06, 1.08}},
        {"CW", "NA", BondFunctions::Form::Harmonic, {3573.14, 1.381}},
        {"CY", "CY", BondFunctions::Form::Harmonic, {2175.68, 1.52}},
        {"CY", "CT", BondFunctions::Form::Harmonic, {2343.04, 1.51}},
        {"CY", "HC", BondFunctions::Form::Harmonic, {2845.12, 1.088}},
        {"H", "N", BondFunctions::Form::Harmonic, {3631.71, 1.01}},
        {"H", "N3", BondFunctions::Form::Harmonic, {3631.71, 1.01}},
        {"H", "N^", BondFunctions::Form::Harmonic, {3631.71, 1.01}},
        {"H", "N2", BondFunctions::Form::Harmonic, {3631.71, 1.01}},
        {"H", "NA", BondFunctions::Form::Harmonic, {3631.71, 1.01}},
        /* { "H2",	"N",	BondFunctions::Form::Harmonic,	3631.71,	1.01 }, */
        /* { "H2",	"N2",	BondFunctions::Form::Harmonic,	3631.71,	1.01 }, */
        {"H", "NT", BondFunctions::Form::Harmonic, {3631.71, 1.01}},
        {"H3", "N2", BondFunctions::Form::Harmonic, {3631.71, 1.01}},
        {"H3", "N3", BondFunctions::Form::Harmonic, {3631.71, 1.01}},
        {"HO", "OH", BondFunctions::Form::Harmonic, {4627.5, 0.945}},
        /* { """"""",	"460.5",	BondFunctions::Form::Harmonic,	8.03328,	0.0 }, */
        {"HO", "OS", BondFunctions::Form::Harmonic, {4627.5, 0.945}},
        {"HS", "SH", BondFunctions::Form::Harmonic, {2292.83, 1.336}},
        {"O2", "P", BondFunctions::Form::Harmonic, {4393.2, 1.48}},
        {"O", "P", BondFunctions::Form::Harmonic, {4393.2, 1.48}},
        {"OH", "P", BondFunctions::Form::Harmonic, {1924.64, 1.61}},
        {"OS", "P", BondFunctions::Form::Harmonic, {1924.64, 1.61}},
        {"CT", "P", BondFunctions::Form::Harmonic, {1774.02, 1.843}},
        {"CA", "P", BondFunctions::Form::Harmonic, {1840.96, 1.78}},
        {"CT", "P+", BondFunctions::Form::Harmonic, {1774.02, 1.82}},
        {"S", "S", BondFunctions::Form::Harmonic, {1389.09, 2.038}},
        {"C9", "C9", BondFunctions::Form::Harmonic, {4435.04, 1.34}},
        {"C9", "C8", BondFunctions::Form::Harmonic, {4435.04, 1.34}},
        {"C9", "C7", BondFunctions::Form::Harmonic, {4435.04, 1.34}},
        {"C8", "C8", BondFunctions::Form::Harmonic, {4435.04, 1.34}},
        {"C8", "C7", BondFunctions::Form::Harmonic, {4435.04, 1.34}},
        {"C7", "C7", BondFunctions::Form::Harmonic, {4435.04, 1.34}},
        {"C8", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.5}},
        {"C8", "CH", BondFunctions::Form::Harmonic, {2652.66, 1.5}},
        {"C8", "C2", BondFunctions::Form::Harmonic, {2652.66, 1.5}},
        {"C8", "C3", BondFunctions::Form::Harmonic, {2652.66, 1.5}},
        {"C7", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.5}},
        {"C7", "CH", BondFunctions::Form::Harmonic, {2652.66, 1.5}},
        {"C7", "C2", BondFunctions::Form::Harmonic, {2652.66, 1.5}},
        {"C7", "C3", BondFunctions::Form::Harmonic, {2652.66, 1.5}},
        {"CT", "C3", BondFunctions::Form::Harmonic, {2175.68, 1.526}},
        {"CT", "C2", BondFunctions::Form::Harmonic, {2175.68, 1.526}},
        {"CA", "NB", BondFunctions::Form::Harmonic, {3464.35, 1.391}},
        {"CA", "N", BondFunctions::Form::Harmonic, {3573.14, 1.381}},
        {"CB", "CT", BondFunctions::Form::Harmonic, {2652.66, 1.51}},
        /* { "CC",	"CB",	BondFunctions::Form::Harmonic,	4351.36,	1.37 }, */
        {"CT", "F", BondFunctions::Form::Harmonic, {3071.06, 1.332}},
        {"C2", "F", BondFunctions::Form::Harmonic, {3071.06, 1.38}},
        {"CA", "F", BondFunctions::Form::Harmonic, {3514.56, 1.354}},
        {"CM", "F", BondFunctions::Form::Harmonic, {3514.56, 1.34}},
        {"CZ", "F", BondFunctions::Form::Harmonic, {3765.6, 1.279}},
        {"C", "F", BondFunctions::Form::Harmonic, {3514.56, 1.357}},
        {"CT", "CO", BondFunctions::Form::Harmonic, {2242.62, 1.529}},
        {"OH", "CO", BondFunctions::Form::Harmonic, {2677.76, 1.38}},
        {"HC", "CO", BondFunctions::Form::Harmonic, {2845.12, 1.09}},
        {"SY", "C3", BondFunctions::Form::Harmonic, {1857.7, 1.81}},
        {"SY", "CA", BondFunctions::Form::Harmonic, {2845.12, 1.77}},
        {"SY", "C8", BondFunctions::Form::Harmonic, {1857.7, 1.76}},
        {"SY", "OY", BondFunctions::Form::Harmonic, {5857.6, 1.44}},
        {"SZ", "OY", BondFunctions::Form::Harmonic, {5857.6, 1.53}},
        {"SY", "N", BondFunctions::Form::Harmonic, {3631.71, 1.67}},
        {"SY", "CT", BondFunctions::Form::Harmonic, {2845.12, 1.77}},
        {"SZ", "CT", BondFunctions::Form::Harmonic, {2845.12, 1.79}},
        {"U", "OU", BondFunctions::Form::Harmonic, {4.184, 1.8}},
        {"CA", "S", BondFunctions::Form::Harmonic, {2092, 1.76}},
        {"CM", "S", BondFunctions::Form::Harmonic, {2092, 1.76}},
        {"CM", "CY", BondFunctions::Form::Harmonic, {2652.66, 1.51}},
        {"CY", "NT", BondFunctions::Form::Harmonic, {3196.58, 1.448}},
        {"SY", "NT", BondFunctions::Form::Harmonic, {2845.12, 1.77}},
        {"C", "NT", BondFunctions::Form::Harmonic, {2652.66, 1.522}},
        {"C", "CW", BondFunctions::Form::Harmonic, {3347.2, 1.49}},
        {"Zn", "N", BondFunctions::Form::Harmonic, {334.72, 2.05}},
        {"Zn", "OH", BondFunctions::Form::Harmonic, {786.592, 1.8}},
        {"Zn", "OW", BondFunctions::Form::Harmonic, {334.72, 2.05}}};

    return Forcefield::termMatch_(bondTerms, i, j);
}

// Return angle term for the supplied atom type trio (if it exists)
OptionalReferenceWrapper<const ForcefieldAngleTerm> OPLSAA2005BaseForcefield::getAngleTerm(const ForcefieldAtomType &i,
                                                                                           const ForcefieldAtomType &j,
                                                                                           const ForcefieldAtomType &k) const
{
    static const std::vector<ForcefieldAngleTerm> angleTerms = {
        //	i	j	k	Type (Harmonic)			k	eq
        {"HW", "OW", "HW", AngleFunctions::Form::Harmonic, {627.6, 109.5}},
        /* { "HW",	"OW",	"LP",	AngleFunctions::Form::Harmonic,	418.4,		54.75 }, */
        {"OU", "U", "OU", AngleFunctions::Form::Harmonic, {1255.2, 180.0}},
        {"HC", "C^", "CW", AngleFunctions::Form::Harmonic, {292.88, 126.8}},
        {"HC", "C^", "CB", AngleFunctions::Form::Harmonic, {292.88, 126.8}},
        {"HC", "CS", "CW", AngleFunctions::Form::Harmonic, {292.88, 126.8}},
        {"HC", "CS", "CB", AngleFunctions::Form::Harmonic, {292.88, 126.8}},
        {"HA", "CA", "CW", AngleFunctions::Form::Harmonic, {292.88, 126.9}},
        {"HC", "C=", "CW", AngleFunctions::Form::Harmonic, {292.88, 122.0}},
        {"HA", "CW", "CA", AngleFunctions::Form::Harmonic, {292.88, 130.7}},
        {"HA", "CW", "C=", AngleFunctions::Form::Harmonic, {292.88, 130.7}},
        {"HA", "CW", "NA", AngleFunctions::Form::Harmonic, {292.88, 121.6}},
        {"CT", "CW", "NA", AngleFunctions::Form::Harmonic, {585.76, 121.6}},
        {"C!", "CW", "NA", AngleFunctions::Form::Harmonic, {585.76, 121.6}},
        {"CT", "CW", "OS", AngleFunctions::Form::Harmonic, {585.76, 121.6}},
        {"C!", "CW", "OS", AngleFunctions::Form::Harmonic, {585.76, 121.6}},
        {"CA", "CW", "NA", AngleFunctions::Form::Harmonic, {585.76, 121.6}},
        {"HA", "CW", "CV", AngleFunctions::Form::Harmonic, {292.88, 130.7}},
        {"HA", "CV", "CW", AngleFunctions::Form::Harmonic, {292.88, 128.2}},
        {"HC", "CT", "CZ", AngleFunctions::Form::Harmonic, {292.88, 108.5}},
        {"CT", "CT", "CZ", AngleFunctions::Form::Harmonic, {488.273, 112.7}},
        {"CT", "CZ", "CZ", AngleFunctions::Form::Harmonic, {1255.2, 180.0}},
        {"CA", "CZ", "CZ", AngleFunctions::Form::Harmonic, {1338.88, 180.0}},
        {"HC", "CZ", "CZ", AngleFunctions::Form::Harmonic, {937.216, 180.0}},
        {"CA", "CA", "CZ", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CT", "CZ", "NZ", AngleFunctions::Form::Harmonic, {1255.2, 180.0}},
        {"N2", "CZ", "NZ", AngleFunctions::Form::Harmonic, {1255.2, 180.0}},
        {"CA", "CZ", "NZ", AngleFunctions::Form::Harmonic, {1255.2, 180.0}},
        {"HC", "CT", "NZ", AngleFunctions::Form::Harmonic, {292.88, 108.5}},
        {"CA", "CA", "NZ", AngleFunctions::Form::Harmonic, {669.44, 120.0}},
        {"CT", "NZ", "CZ", AngleFunctions::Form::Harmonic, {1255.2, 180.0}},
        {"CA", "NZ", "CZ", AngleFunctions::Form::Harmonic, {1422.56, 180.0}},
        {"CT", "CX", "NA", AngleFunctions::Form::Harmonic, {585.76, 121.6}},
        {"HA", "CX", "CX", AngleFunctions::Form::Harmonic, {292.88, 130.7}},
        {"CX", "CX", "NA", AngleFunctions::Form::Harmonic, {585.76, 106.3}},
        {"CX", "NA", "CR", AngleFunctions::Form::Harmonic, {585.76, 109.8}},
        {"C2", "C", "N", AngleFunctions::Form::Harmonic, {585.76, 116.6}},
        {"C2", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 120.4}},
        {"C2", "C", "O2", AngleFunctions::Form::Harmonic, {585.76, 117.0}},
        {"C3", "C", "N", AngleFunctions::Form::Harmonic, {585.76, 116.6}},
        {"C3", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 120.4}},
        {"C3", "C", "O2", AngleFunctions::Form::Harmonic, {585.76, 117.0}},
        {"CA", "C", "CA", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        {"CA", "C", "OH", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CA", "CA", "OH", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CA", "CA", "SH", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CA", "CA", "OS", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CM", "CM", "OS", AngleFunctions::Form::Harmonic, {585.76, 123.0}},
        {"C=", "CM", "OS", AngleFunctions::Form::Harmonic, {585.76, 123.0}},
        {"CM", "CM", "OH", AngleFunctions::Form::Harmonic, {585.76, 123.0}},
        {"C=", "CM", "OH", AngleFunctions::Form::Harmonic, {585.76, 123.0}},
        {"CB", "C", "NA", AngleFunctions::Form::Harmonic, {585.76, 111.3}},
        {"CB", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 128.8}},
        {"CB", "C", "N", AngleFunctions::Form::Harmonic, {585.76, 111.3}},
        {"CS", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 128.2}},
        {"CD", "C", "CD", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        {"CD", "C", "OH", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CH", "C", "N", AngleFunctions::Form::Harmonic, {585.76, 116.6}},
        {"CH", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 120.4}},
        {"CH", "C", "O2", AngleFunctions::Form::Harmonic, {543.92, 117.0}},
        {"CH", "C", "OH", AngleFunctions::Form::Harmonic, {585.76, 115.0}},
        /* { "CJ",	"C",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		114.1 }, */
        /* { "CJ",	"C",	"O",	AngleFunctions::Form::Harmonic,	669.44,		125.3 }, */
        {"CM", "C", "NA", AngleFunctions::Form::Harmonic, {585.76, 114.1}},
        {"CM", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 125.3}},
        {"C=", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 124.0}},
        {"C=", "C", "HC", AngleFunctions::Form::Harmonic, {669.44, 116.0}},
        {"CT", "C", "N", AngleFunctions::Form::Harmonic, {585.76, 116.6}},
        {"CT", "C", "NM", AngleFunctions::Form::Harmonic, {585.76, 116.6}},
        {"CA", "C", "N", AngleFunctions::Form::Harmonic, {585.76, 115.5}},
        {"CM", "C", "N", AngleFunctions::Form::Harmonic, {585.76, 115.5}},
        {"CT", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 120.4}},
        {"CA", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 120.4}},
        /* { "CT",	"C",	"S=",	AngleFunctions::Form::Harmonic,	585.76,		125.0 }, */
        {"CT", "NO", "ON", AngleFunctions::Form::Harmonic, {669.44, 117.5}},
        {"CA", "NO", "ON", AngleFunctions::Form::Harmonic, {669.44, 117.5}},
        {"CT", "CT", "NO", AngleFunctions::Form::Harmonic, {527.184, 111.1}},
        {"HC", "CT", "NO", AngleFunctions::Form::Harmonic, {292.88, 105.0}},
        {"CA", "CA", "NO", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        {"HC", "C", "N", AngleFunctions::Form::Harmonic, {334.72, 114.0}},
        {"HC", "C", "OS", AngleFunctions::Form::Harmonic, {334.72, 115.0}},
        {"HC", "C", "OH", AngleFunctions::Form::Harmonic, {334.72, 115.0}},
        {"O", "C", "HC", AngleFunctions::Form::Harmonic, {292.88, 123.0}},
        /* { "S=",	"C",	"HC",	AngleFunctions::Form::Harmonic,	292.88,		127.0 }, */
        {"NC", "C", "HC", AngleFunctions::Form::Harmonic, {292.88, 122.0}},
        {"CT", "C", "OH", AngleFunctions::Form::Harmonic, {585.76, 108.0}},
        {"CT", "C", "CT", AngleFunctions::Form::Harmonic, {585.76, 116.0}},
        {"CT", "C", "CA", AngleFunctions::Form::Harmonic, {585.76, 116.0}},
        {"C=", "C", "CT", AngleFunctions::Form::Harmonic, {585.76, 116.0}},
        {"CT", "C", "O2", AngleFunctions::Form::Harmonic, {585.76, 117.0}},
        {"CA", "C", "O2", AngleFunctions::Form::Harmonic, {585.76, 117.0}},
        {"CT", "CT", "Cl", AngleFunctions::Form::Harmonic, {577.392, 109.8}},
        {"C", "CT", "Cl", AngleFunctions::Form::Harmonic, {577.392, 109.8}},
        {"CA", "CA", "Cl", AngleFunctions::Form::Harmonic, {627.6, 120.0}},
        {"CM", "CM", "Cl", AngleFunctions::Form::Harmonic, {627.6, 121.5}},
        {"Cl", "CM", "HC", AngleFunctions::Form::Harmonic, {502.08, 114.0}},
        {"Cl", "CT", "Cl", AngleFunctions::Form::Harmonic, {652.704, 111.7}},
        {"HC", "CT", "Cl", AngleFunctions::Form::Harmonic, {426.768, 107.6}},
        {"CT", "CT", "Br", AngleFunctions::Form::Harmonic, {577.392, 110.0}},
        {"CA", "CT", "Br", AngleFunctions::Form::Harmonic, {577.392, 110.0}},
        {"C", "CT", "Br", AngleFunctions::Form::Harmonic, {577.392, 109.8}},
        {"CT", "C", "Cl", AngleFunctions::Form::Harmonic, {627.6, 109.0}},
        {"CT", "C", "Br", AngleFunctions::Form::Harmonic, {627.6, 109.0}},
        {"O", "C", "Cl", AngleFunctions::Form::Harmonic, {627.6, 119.0}},
        {"O", "C", "Br", AngleFunctions::Form::Harmonic, {627.6, 119.0}},
        {"CA", "CA", "Br", AngleFunctions::Form::Harmonic, {627.6, 120.0}},
        {"CM", "CM", "Br", AngleFunctions::Form::Harmonic, {627.6, 120.0}},
        {"Br", "CM", "HC", AngleFunctions::Form::Harmonic, {502.08, 114.0}},
        {"Br", "CT", "Br", AngleFunctions::Form::Harmonic, {652.704, 111.7}},
        {"HC", "CT", "Br", AngleFunctions::Form::Harmonic, {426.768, 107.6}},
        {"CA", "CA", "I", AngleFunctions::Form::Harmonic, {627.6, 120.0}},
        {"CT", "CT", "I", AngleFunctions::Form::Harmonic, {627.6, 112.0}},
        {"HC", "CT", "I", AngleFunctions::Form::Harmonic, {627.6, 111.0}},
        {"CA", "CA", "F", AngleFunctions::Form::Harmonic, {669.44, 120.0}},
        {"CM", "CM", "F", AngleFunctions::Form::Harmonic, {669.44, 121.5}},
        {"C", "CM", "F", AngleFunctions::Form::Harmonic, {669.44, 121.5}},
        {"F", "CM", "HC", AngleFunctions::Form::Harmonic, {418.4, 112.0}},
        {"F", "CM", "F", AngleFunctions::Form::Harmonic, {669.44, 108.0}},
        {"F", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 121.0}},
        {"F", "C", "CT", AngleFunctions::Form::Harmonic, {669.44, 111.0}},
        {"N", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 122.9}},
        {"NM", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 122.9}},
        {"N", "C", "N", AngleFunctions::Form::Harmonic, {585.76, 114.2}},
        /* { "N",	"C",	"S=",	AngleFunctions::Form::Harmonic,	585.76,		125.0 }, */
        {"N^", "C", "NA", AngleFunctions::Form::Harmonic, {585.76, 115.4}},
        {"N^", "C", "NC", AngleFunctions::Form::Harmonic, {585.76, 118.6}},
        {"NA", "C", "NA", AngleFunctions::Form::Harmonic, {585.76, 118.6}},
        {"N^", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 120.9}},
        {"NA", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 120.6}},
        {"NC", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 122.5}},
        {"NC", "C", "NA", AngleFunctions::Form::Harmonic, {585.76, 118.6}},
        {"NA", "CM", "H4", AngleFunctions::Form::Harmonic, {292.88, 119.1}},
        {"N^", "CM", "H4", AngleFunctions::Form::Harmonic, {292.88, 119.1}},
        {"N", "CA", "HA", AngleFunctions::Form::Harmonic, {292.88, 119.1}},
        {"O", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 126.0}},
        {"ON", "NO", "ON", AngleFunctions::Form::Harmonic, {669.44, 125.0}},
        {"O", "C", "OH", AngleFunctions::Form::Harmonic, {669.44, 121.0}},
        {"O2", "C", "O2", AngleFunctions::Form::Harmonic, {669.44, 126.0}},
        {"C2", "C^", "CB", AngleFunctions::Form::Harmonic, {585.76, 128.6}},
        {"C2", "CA", "CB", AngleFunctions::Form::Harmonic, {585.76, 128.6}},
        /* { "C2",	"C^",	"CG",	AngleFunctions::Form::Harmonic,	585.76,		125.0 }, */
        /* { "C2",	"C^",	"CC",	AngleFunctions::Form::Harmonic,	585.76,		125.0 }, */
        {"C2", "C^", "CW", AngleFunctions::Form::Harmonic, {585.76, 125.0}},
        /* { "CB",	"C^",	"CG",	AngleFunctions::Form::Harmonic,	711.28,		106.4 }, */
        {"CB", "C^", "CT", AngleFunctions::Form::Harmonic, {585.76, 128.6}},
        {"CB", "C^", "CW", AngleFunctions::Form::Harmonic, {711.28, 106.4}},
        {"CT", "C^", "CW", AngleFunctions::Form::Harmonic, {585.76, 125.0}},
        /* { "C2",	"CS",	"CG",	AngleFunctions::Form::Harmonic,	585.76,		125.0 }, */
        /* { "C2",	"CS",	"CC",	AngleFunctions::Form::Harmonic,	585.76,		125.0 }, */
        {"C2", "CS", "CW", AngleFunctions::Form::Harmonic, {585.76, 125.0}},
        /* { "CB",	"CS",	"CG",	AngleFunctions::Form::Harmonic,	711.28,		106.4 }, */
        {"CB", "CS", "CT", AngleFunctions::Form::Harmonic, {585.76, 128.6}},
        {"CB", "CS", "CW", AngleFunctions::Form::Harmonic, {711.28, 106.4}},
        {"CT", "CS", "CW", AngleFunctions::Form::Harmonic, {585.76, 125.0}},
        {"C", "C2", "C2", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C", "C2", "CH", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C", "C2", "N", AngleFunctions::Form::Harmonic, {669.44, 110.3}},
        {"C", "C2", "NT", AngleFunctions::Form::Harmonic, {669.44, 111.2}},
        {"C^", "C2", "CH", AngleFunctions::Form::Harmonic, {527.184, 115.6}},
        {"C2", "C2", "C2", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C2", "C2", "C3", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C3", "C2", "C3", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C2", "CH", "C3", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C2", "CH", "C2", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C2", "C2", "CH", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C2", "CH", "OH", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"C3", "C2", "OH", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"C2", "C2", "OH", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"C2", "C2", "N", AngleFunctions::Form::Harmonic, {669.44, 111.2}},
        {"C2", "C2", "N2", AngleFunctions::Form::Harmonic, {669.44, 111.2}},
        {"C2", "C2", "N3", AngleFunctions::Form::Harmonic, {669.44, 111.2}},
        /* { "**C2",	"C2",	"NT",	AngleFunctions::Form::Harmonic,	669.44,		111.2 }, */
        {"CT", "CT", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"C3", "CT", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"C2", "CT", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"CH", "CT", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"CT", "C2", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"C3", "C2", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"C2", "C2", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"CH", "C2", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"CT", "CH", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"C3", "CH", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"C2", "CH", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"CH", "CH", "NT", AngleFunctions::Form::Harmonic, {470.282, 109.47}},
        {"C2", "C2", "OS", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"C3", "CH", "OS", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"C2", "C2", "S", AngleFunctions::Form::Harmonic, {418.4, 114.7}},
        {"C3", "C2", "CH", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C3", "C2", "OS", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"CA", "C2", "CH", AngleFunctions::Form::Harmonic, {527.184, 114.0}},
        {"CD", "C2", "CH", AngleFunctions::Form::Harmonic, {527.184, 114.0}},
        /* { "CC",	"C2",	"CH",	AngleFunctions::Form::Harmonic,	527.184,	113.1 }, */
        {"CH", "C2", "CH", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"CH", "C2", "OH", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"CH", "C2", "OS", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"CH", "C2", "S", AngleFunctions::Form::Harmonic, {418.4, 114.7}},
        {"CH", "C2", "SH", AngleFunctions::Form::Harmonic, {418.4, 108.6}},
        {"C3", "C2", "CT", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C2", "C2", "CA", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"CH", "CA", "CA", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"C2", "CH", "CA", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C", "CA", "CA", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        {"C", "CA", "HA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"C2", "CA", "CA", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"C2", "CA", "CD", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"C2", "CD", "CD", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CA", "CA", "CA", AngleFunctions::Form::Harmonic, {527.184, 120.0}},
        {"CA", "C!", "CA", AngleFunctions::Form::Harmonic, {527.184, 120.0}},
        {"CA", "C!", "C!", AngleFunctions::Form::Harmonic, {527.184, 120.0}},
        {"CA", "CA", "C!", AngleFunctions::Form::Harmonic, {527.184, 120.0}},
        {"CA", "C!", "CR", AngleFunctions::Form::Harmonic, {527.184, 120.0}},
        {"CA", "C!", "CS", AngleFunctions::Form::Harmonic, {527.184, 120.0}},
        {"CA", "C!", "CW", AngleFunctions::Form::Harmonic, {527.184, 120.0}},
        {"CA", "C!", "CU", AngleFunctions::Form::Harmonic, {527.184, 120.0}},
        {"CA", "C!", "CV", AngleFunctions::Form::Harmonic, {527.184, 120.0}},
        {"CA", "CA", "CB", AngleFunctions::Form::Harmonic, {527.184, 120.0}},
        {"CA", "CA", "CN", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        {"CA", "CA", "CM", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CA", "CM", "CT", AngleFunctions::Form::Harmonic, {711.28, 119.7}},
        {"CA", "CA", "CT", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CA", "CA", "NT", AngleFunctions::Form::Harmonic, {585.76, 120.1}},
        {"CA", "CA", "N3", AngleFunctions::Form::Harmonic, {585.76, 120.1}},
        {"CA", "CA", "HA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"C!", "CA", "HA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"CA", "CA", "DM", AngleFunctions::Form::Harmonic, {292.88, 90.0}},
        {"HA", "CA", "DM", AngleFunctions::Form::Harmonic, {83.68, 90.0}},
        {"OY", "SZ", "DM", AngleFunctions::Form::Harmonic, {83.68, 90.0}},
        {"CT", "NC", "NZ", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"NC", "NZ", "NZ", AngleFunctions::Form::Harmonic, {836.8, 180.0}},
        {"CT", "CT", "NC", AngleFunctions::Form::Harmonic, {543.92, 109.0}},
        {"NC", "CA", "HA", AngleFunctions::Form::Harmonic, {292.88, 116.0}},
        {"CA", "CA", "NC", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CA", "C!", "NC", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"C!", "CA", "NC", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"C!", "C!", "NC", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CA", "NC", "CA", AngleFunctions::Form::Harmonic, {585.76, 117.0}},
        {"CA", "NC", "C!", AngleFunctions::Form::Harmonic, {585.76, 117.0}},
        {"CA", "CA", "CW", AngleFunctions::Form::Harmonic, {585.76, 107.4}},
        {"CW", "NA", "CW", AngleFunctions::Form::Harmonic, {585.76, 109.8}},
        {"CV", "CW", "NA", AngleFunctions::Form::Harmonic, {585.76, 106.3}},
        {"CW", "CV", "NB", AngleFunctions::Form::Harmonic, {585.76, 111.0}},
        {"CW", "NA", "CR", AngleFunctions::Form::Harmonic, {585.76, 109.8}},
        {"CB", "NA", "CR", AngleFunctions::Form::Harmonic, {585.76, 109.8}},
        {"CW", "C=", "C=", AngleFunctions::Form::Harmonic, {292.88, 106.0}},
        {"CA", "NA", "CK", AngleFunctions::Form::Harmonic, {585.76, 109.8}},
        {"NC", "CA", "CT", AngleFunctions::Form::Harmonic, {585.76, 116.0}},
        {"NC", "CQ", "CT", AngleFunctions::Form::Harmonic, {585.76, 115.5}},
        {"NB", "CV", "CT", AngleFunctions::Form::Harmonic, {585.76, 124.5}},
        {"CA", "CV", "NB", AngleFunctions::Form::Harmonic, {585.76, 111.0}},
        {"CA", "NC", "NC", AngleFunctions::Form::Harmonic, {585.76, 117.0}},
        {"CT", "NC", "NC", AngleFunctions::Form::Harmonic, {585.76, 117.0}},
        {"OS", "CW", "CS", AngleFunctions::Form::Harmonic, {585.76, 110.6}},
        {"OS", "CW", "C=", AngleFunctions::Form::Harmonic, {585.76, 110.0}},
        {"CW", "OS", "CW", AngleFunctions::Form::Harmonic, {585.76, 106.5}},
        {"CW", "OS", "CB", AngleFunctions::Form::Harmonic, {585.76, 106.5}},
        {"CR", "OS", "CB", AngleFunctions::Form::Harmonic, {585.76, 106.5}},
        {"OS", "CW", "HA", AngleFunctions::Form::Harmonic, {292.88, 113.4}},
        {"S", "CW", "HA", AngleFunctions::Form::Harmonic, {292.88, 113.4}},
        {"S", "CR", "HA", AngleFunctions::Form::Harmonic, {292.88, 113.4}},
        {"S", "CR", "NT", AngleFunctions::Form::Harmonic, {585.76, 120.2}},
        {"NB", "CR", "NT", AngleFunctions::Form::Harmonic, {585.76, 126.1}},
        {"S", "CW", "CV", AngleFunctions::Form::Harmonic, {585.76, 111.0}},
        {"S", "CW", "CS", AngleFunctions::Form::Harmonic, {585.76, 111.0}},
        {"NA", "CW", "CS", AngleFunctions::Form::Harmonic, {585.76, 107.7}},
        {"CW", "CS", "CS", AngleFunctions::Form::Harmonic, {585.76, 107.3}},
        {"CW", "CS", "HA", AngleFunctions::Form::Harmonic, {292.88, 125.7}},
        {"CW", "CS", "C!", AngleFunctions::Form::Harmonic, {585.76, 125.7}},
        {"CS", "CW", "C!", AngleFunctions::Form::Harmonic, {585.76, 132.1}},
        {"CS", "CS", "C!", AngleFunctions::Form::Harmonic, {585.76, 127.5}},
        {"CS", "CW", "HA", AngleFunctions::Form::Harmonic, {292.88, 132.1}},
        {"CS", "CW", "CT", AngleFunctions::Form::Harmonic, {585.76, 132.1}},
        {"CS", "CW", "CA", AngleFunctions::Form::Harmonic, {585.76, 132.1}},
        {"CS", "CS", "HA", AngleFunctions::Form::Harmonic, {292.88, 127.5}},
        {"CU", "NB", "NA", AngleFunctions::Form::Harmonic, {585.76, 104.1}},
        {"CW", "NB", "NA", AngleFunctions::Form::Harmonic, {585.76, 104.1}},
        {"NB", "CU", "HA", AngleFunctions::Form::Harmonic, {292.88, 118.9}},
        {"NB", "CU", "CT", AngleFunctions::Form::Harmonic, {585.76, 118.9}},
        {"NB", "CW", "CT", AngleFunctions::Form::Harmonic, {585.76, 118.9}},
        {"CU", "CS", "CW", AngleFunctions::Form::Harmonic, {585.76, 103.8}},
        {"CW", "CS", "CW", AngleFunctions::Form::Harmonic, {585.76, 103.8}},
        {"NB", "CU", "CS", AngleFunctions::Form::Harmonic, {585.76, 111.9}},
        {"NB", "CW", "CS", AngleFunctions::Form::Harmonic, {585.76, 111.9}},
        {"CA", "CU", "HA", AngleFunctions::Form::Harmonic, {292.88, 128.6}},
        {"CU", "CA", "HA", AngleFunctions::Form::Harmonic, {292.88, 128.2}},
        {"CU", "NB", "OS", AngleFunctions::Form::Harmonic, {585.76, 105.3}},
        {"NB", "NA", "CW", AngleFunctions::Form::Harmonic, {468.608, 113.1}},
        {"CB", "NA", "NB", AngleFunctions::Form::Harmonic, {468.608, 113.1}},
        {"CR", "NA", "NB", AngleFunctions::Form::Harmonic, {468.608, 113.1}},
        {"NB", "NA", "H", AngleFunctions::Form::Harmonic, {468.608, 118.4}},
        {"NB", "NA", "CA", AngleFunctions::Form::Harmonic, {585.76, 118.4}},
        {"NB", "NA", "CT", AngleFunctions::Form::Harmonic, {585.76, 118.4}},
        {"CW", "OS", "NB", AngleFunctions::Form::Harmonic, {585.76, 108.9}},
        {"NB", "CR", "OS", AngleFunctions::Form::Harmonic, {585.76, 115.0}},
        {"NB", "CR", "S", AngleFunctions::Form::Harmonic, {585.76, 115.0}},
        {"CR", "OS", "CW", AngleFunctions::Form::Harmonic, {585.76, 104.0}},
        {"CV", "CW", "OS", AngleFunctions::Form::Harmonic, {585.76, 108.0}},
        {"HA", "CR", "OS", AngleFunctions::Form::Harmonic, {292.88, 117.0}},
        {"OS", "CM", "HC", AngleFunctions::Form::Harmonic, {292.88, 114.5}},
        {"CB", "CA", "HA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        /* { "CC",	"CA",	"HA",	AngleFunctions::Form::Harmonic,	292.88,		120.0 }, */
        {"CB", "CA", "N2", AngleFunctions::Form::Harmonic, {585.76, 123.5}},
        {"CB", "CA", "NC", AngleFunctions::Form::Harmonic, {585.76, 117.3}},
        {"CD", "CA", "CD", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        /* { "CJ",	"CA",	"N2",	AngleFunctions::Form::Harmonic,	585.76,		120.1 }, */
        /* { "CJ",	"CA",	"NC",	AngleFunctions::Form::Harmonic,	585.76,		121.5 }, */
        {"CM", "CA", "N2", AngleFunctions::Form::Harmonic, {585.76, 120.1}},
        {"CA", "CA", "N2", AngleFunctions::Form::Harmonic, {585.76, 120.1}},
        {"CM", "CA", "NC", AngleFunctions::Form::Harmonic, {585.76, 121.5}},
        {"CM", "CA", "NA", AngleFunctions::Form::Harmonic, {585.76, 121.5}},
        {"CN", "CA", "HA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"NY", "CA", "NY", AngleFunctions::Form::Harmonic, {585.76, 111.8}},
        {"NZ", "CA", "NY", AngleFunctions::Form::Harmonic, {585.76, 124.1}},
        {"CA", "NZ", "H", AngleFunctions::Form::Harmonic, {292.88, 113.0}},
        {"CA", "NY", "H", AngleFunctions::Form::Harmonic, {418.4, 112.5}},
        {"CA", "NY", "CT", AngleFunctions::Form::Harmonic, {418.4, 120.5}},
        {"H", "NY", "H", AngleFunctions::Form::Harmonic, {364.845, 106.4}},
        {"CT", "NY", "H", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CT", "CT", "NY", AngleFunctions::Form::Harmonic, {669.44, 111.2}},
        {"HC", "CT", "NY", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"N2", "CA", "N2", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"N2", "CA", "NA", AngleFunctions::Form::Harmonic, {585.76, 116.0}},
        {"N2", "CA", "NC", AngleFunctions::Form::Harmonic, {585.76, 119.3}},
        {"N2", "CQ", "NC", AngleFunctions::Form::Harmonic, {585.76, 119.3}},
        {"N2", "CQ", "N", AngleFunctions::Form::Harmonic, {585.76, 116.0}},
        {"NA", "CA", "NC", AngleFunctions::Form::Harmonic, {585.76, 123.3}},
        {"C", "CB", "CB", AngleFunctions::Form::Harmonic, {711.28, 119.2}},
        {"C", "CB", "NB", AngleFunctions::Form::Harmonic, {585.76, 130.0}},
        {"N", "CQ", "NC", AngleFunctions::Form::Harmonic, {585.76, 123.3}},
        {"C", "CS", "CW", AngleFunctions::Form::Harmonic, {585.76, 130.0}},
        {"C", "CB", "CW", AngleFunctions::Form::Harmonic, {585.76, 130.0}},
        {"C^", "CB", "CA", AngleFunctions::Form::Harmonic, {711.28, 134.9}},
        {"C^", "CB", "CD", AngleFunctions::Form::Harmonic, {711.28, 134.9}},
        {"CA", "CB", "CA", AngleFunctions::Form::Harmonic, {711.28, 134.9}},
        {"C^", "CB", "CN", AngleFunctions::Form::Harmonic, {711.28, 108.8}},
        /* { "CA",	"CB",	"CC",	AngleFunctions::Form::Harmonic,	711.28,		108.8 }, */
        /* { "C^",	"CB",	"CC",	AngleFunctions::Form::Harmonic,	711.28,		108.8 }, */
        {"CS", "CB", "CA", AngleFunctions::Form::Harmonic, {711.28, 134.9}},
        {"CS", "CB", "CD", AngleFunctions::Form::Harmonic, {711.28, 134.9}},
        {"CS", "CB", "CN", AngleFunctions::Form::Harmonic, {711.28, 108.8}},
        /* { "CS",	"CB",	"CC",	AngleFunctions::Form::Harmonic,	711.28,		108.8 }, */
        {"CA", "CB", "CB", AngleFunctions::Form::Harmonic, {711.28, 117.3}},
        {"CA", "CB", "CN", AngleFunctions::Form::Harmonic, {711.28, 116.2}},
        {"CA", "CB", "NB", AngleFunctions::Form::Harmonic, {585.76, 132.4}},
        {"CB", "CB", "N^", AngleFunctions::Form::Harmonic, {585.76, 106.2}},
        {"CB", "CB", "NA", AngleFunctions::Form::Harmonic, {585.76, 106.2}},
        {"CS", "CR", "NA", AngleFunctions::Form::Harmonic, {585.76, 106.2}},
        {"CB", "CB", "NB", AngleFunctions::Form::Harmonic, {585.76, 111.0}},
        {"CR", "CS", "CW", AngleFunctions::Form::Harmonic, {585.76, 110.4}},
        {"CB", "CB", "NC", AngleFunctions::Form::Harmonic, {585.76, 127.7}},
        {"CB", "CB", "N", AngleFunctions::Form::Harmonic, {585.76, 127.7}},
        {"CS", "CR", "NC", AngleFunctions::Form::Harmonic, {585.76, 127.7}},
        {"CD", "CB", "CN", AngleFunctions::Form::Harmonic, {711.28, 116.2}},
        {"N^", "CB", "NC", AngleFunctions::Form::Harmonic, {585.76, 126.2}},
        {"NA", "CB", "NC", AngleFunctions::Form::Harmonic, {585.76, 126.2}},
        {"NB", "CB", "N", AngleFunctions::Form::Harmonic, {585.76, 126.2}},
        {"NB", "CR", "N", AngleFunctions::Form::Harmonic, {585.76, 126.2}},
        {"NA", "CR", "NC", AngleFunctions::Form::Harmonic, {585.76, 126.2}},
        /* { "C2",	"CC",	"CF",	AngleFunctions::Form::Harmonic,	585.76,		131.9 }, */
        /* { "C2",	"CC",	"CG",	AngleFunctions::Form::Harmonic,	585.76,		129.05 }, */
        /* { "C2",	"CC",	"CV",	AngleFunctions::Form::Harmonic,	585.76,		131.9 }, */
        /* { "C2",	"CC",	"CW",	AngleFunctions::Form::Harmonic,	585.76,		129.05 }, */
        /* { "C2",	"CC",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		122.2 }, */
        /* { "C2",	"CC",	"NB",	AngleFunctions::Form::Harmonic,	585.76,		121.05 }, */
        /* { "CF",	"CC",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		105.9 }, */
        /* { "CG",	"CC",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		108.75 }, */
        /* { "CG",	"CC",	"NB",	AngleFunctions::Form::Harmonic,	585.76,		109.9 }, */
        /* { "CT",	"CC",	"CV",	AngleFunctions::Form::Harmonic,	585.76,		120.0 }, */
        {"CT", "CW", "CV", AngleFunctions::Form::Harmonic, {585.76, 130.7}},
        {"CT", "CV", "CW", AngleFunctions::Form::Harmonic, {585.76, 130.7}},
        {"CT", "CX", "CX", AngleFunctions::Form::Harmonic, {585.76, 130.7}},
        {"CT", "CW", "CW", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        /* { "CT",	"CC",	"CW",	AngleFunctions::Form::Harmonic,	585.76,		120.0 }, */
        /* { "CT",	"CC",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		120.0 }, */
        /* { "CT",	"CC",	"NB",	AngleFunctions::Form::Harmonic,	585.76,		120.0 }, */
        /* { "CV",	"CC",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		120.0 }, */
        {"CW", "CW", "NA", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        /* { "CW",	"CC",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		120.0 }, */
        /* { "CW",	"CC",	"NB",	AngleFunctions::Form::Harmonic,	585.76,		120.0 }, */
        {"C", "CD", "CD", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        {"CA", "CD", "CD", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        {"CB", "CD", "CD", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        {"CD", "CD", "CD", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        {"CD", "CD", "CN", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        /* { "CD",	"CD",	"CC",	AngleFunctions::Form::Harmonic,	711.28,		120.0 }, */
        /* { "N^",	"CE",	"NB",	AngleFunctions::Form::Harmonic,	585.76,		113.9 }, */
        /* { "CC",	"CF",	"NB",	AngleFunctions::Form::Harmonic,	585.76,		109.9 }, */
        /* { "C^",	"CG",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		108.7 }, */
        {"CA", "CA", "NA", AngleFunctions::Form::Harmonic, {585.76, 108.7}},
        /* { "C^",	"CC",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		108.7 }, */
        /* { "CC",	"CG",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		105.9 }, */
        {"C", "CH", "C2", AngleFunctions::Form::Harmonic, {527.184, 111.1}},
        {"C", "CH", "C3", AngleFunctions::Form::Harmonic, {527.184, 111.1}},
        {"C", "CH", "CH", AngleFunctions::Form::Harmonic, {527.184, 111.1}},
        {"C", "CH", "N", AngleFunctions::Form::Harmonic, {527.184, 110.1}},
        {"C", "CH", "NT", AngleFunctions::Form::Harmonic, {669.44, 109.7}},
        {"C2", "CH", "CH", AngleFunctions::Form::Harmonic, {527.184, 111.5}},
        {"C2", "CH", "N", AngleFunctions::Form::Harmonic, {669.44, 109.7}},
        {"C2", "CH", "N^", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"C2", "CH", "OS", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"C3", "CH", "C3", AngleFunctions::Form::Harmonic, {527.184, 111.5}},
        {"C3", "CH", "CH", AngleFunctions::Form::Harmonic, {527.184, 111.5}},
        {"C3", "CH", "N", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"C3", "CH", "OH", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"CH", "CH", "CH", AngleFunctions::Form::Harmonic, {527.184, 111.5}},
        {"CH", "CH", "N", AngleFunctions::Form::Harmonic, {669.44, 109.7}},
        {"CH", "CH", "N^", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"CH", "CH", "OH", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"CH", "CH", "OS", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"N^", "CH", "OS", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        /* { "NC",	"CI",	"NC",	AngleFunctions::Form::Harmonic,	585.76,		129.1 }, */
        /* { "C",	"CJ",	"CJ",	AngleFunctions::Form::Harmonic,	711.28,		120.7 }, */
        /* { "CA",	"CJ",	"CJ",	AngleFunctions::Form::Harmonic,	711.28,		117.0 }, */
        /* { "CJ",	"CJ",	"N^",	AngleFunctions::Form::Harmonic,	585.76,		121.2 }, */
        /* { "CM",	"CJ",	"N^",	AngleFunctions::Form::Harmonic,	585.76,		121.2 }, */
        {"N^", "CK", "NB", AngleFunctions::Form::Harmonic, {585.76, 113.9}},
        {"NA", "CK", "NB", AngleFunctions::Form::Harmonic, {585.76, 113.9}},
        {"NA", "CK", "H5", AngleFunctions::Form::Harmonic, {292.88, 123.05}},
        {"N^", "CK", "H5", AngleFunctions::Form::Harmonic, {292.88, 123.05}},
        {"NB", "CK", "H5", AngleFunctions::Form::Harmonic, {292.88, 123.05}},
        {"C", "CM", "C3", AngleFunctions::Form::Harmonic, {711.28, 119.7}},
        /* { "C",	"CM",	"CJ",	AngleFunctions::Form::Harmonic,	711.28,		120.7 }, */
        {"C", "CM", "CM", AngleFunctions::Form::Harmonic, {711.28, 120.7}},
        {"C", "CM", "CT", AngleFunctions::Form::Harmonic, {585.76, 119.7}},
        {"C", "CA", "CT", AngleFunctions::Form::Harmonic, {585.76, 119.7}},
        {"C", "CM", "HC", AngleFunctions::Form::Harmonic, {292.88, 119.7}},
        /* { "C3",	"CM",	"CJ",	AngleFunctions::Form::Harmonic,	711.28,		119.7 }, */
        {"CA", "CM", "CM", AngleFunctions::Form::Harmonic, {711.28, 117.0}},
        {"CA", "CM", "HC", AngleFunctions::Form::Harmonic, {292.88, 123.3}},
        /* { "CJ",	"CM",	"CT",	AngleFunctions::Form::Harmonic,	711.28,		119.7 }, */
        {"CM", "CM", "CT", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CM", "C=", "C=", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CM", "C=", "C", AngleFunctions::Form::Harmonic, {585.76, 118.7}},
        {"CM", "C=", "CT", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"C=", "C=", "CT", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CT", "CM", "C=", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CM", "CT", "CM", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"CM", "CM", "HC", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"CM", "CM", "H4", AngleFunctions::Form::Harmonic, {292.88, 119.7}},
        {"CM", "C=", "HC", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"C=", "CM", "HC", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"C=", "C=", "HC", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"CT", "C=", "HC", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"CT", "C", "HC", AngleFunctions::Form::Harmonic, {292.88, 115.0}},
        {"CA", "C", "HC", AngleFunctions::Form::Harmonic, {292.88, 115.0}},
        {"CT", "CM", "HC", AngleFunctions::Form::Harmonic, {292.88, 117.0}},
        {"HC", "CM", "HC", AngleFunctions::Form::Harmonic, {292.88, 117.0}},
        {"CT", "CM", "CT", AngleFunctions::Form::Harmonic, {585.76, 130.0}},
        {"CT", "C+", "CT", AngleFunctions::Form::Harmonic, {1445.99, 120.0}},
        {"CT", "C+", "HC", AngleFunctions::Form::Harmonic, {1204.99, 120.0}},
        {"CT", "CT", "C+", AngleFunctions::Form::Harmonic, {527.184, 105.0}},
        {"HC", "CT", "C+", AngleFunctions::Form::Harmonic, {292.88, 105.0}},
        {"CM", "CM", "N^", AngleFunctions::Form::Harmonic, {585.76, 121.2}},
        {"CM", "CM", "NA", AngleFunctions::Form::Harmonic, {585.76, 121.2}},
        {"HC", "CM", "N^", AngleFunctions::Form::Harmonic, {292.88, 119.1}},
        {"HC", "CM", "NA", AngleFunctions::Form::Harmonic, {292.88, 119.1}},
        {"CA", "CN", "CB", AngleFunctions::Form::Harmonic, {711.28, 122.7}},
        {"CA", "CN", "NA", AngleFunctions::Form::Harmonic, {585.76, 132.8}},
        {"CB", "CN", "CD", AngleFunctions::Form::Harmonic, {711.28, 122.7}},
        /* { "CB",	"CC",	"CA",	AngleFunctions::Form::Harmonic,	711.28,		122.7 }, */
        /* { "CB",	"CC",	"CD",	AngleFunctions::Form::Harmonic,	711.28,		122.7 }, */
        {"CB", "CA", "CW", AngleFunctions::Form::Harmonic, {527.184, 106.4}},
        {"CB", "CA", "CT", AngleFunctions::Form::Harmonic, {585.76, 128.6}},
        {"CB", "CN", "NA", AngleFunctions::Form::Harmonic, {585.76, 104.4}},
        /* { "CB",	"CC",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		104.4 }, */
        {"CD", "CN", "NA", AngleFunctions::Form::Harmonic, {585.76, 132.8}},
        /* { "CA",	"CC",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		132.8 }, */
        /* { "CD",	"CC",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		132.8 }, */
        /* { "NA",	"CP",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		110.75 }, */
        /* { "NA",	"CP",	"NB",	AngleFunctions::Form::Harmonic,	585.76,		111.6 }, */
        {"HA", "CQ", "NC", AngleFunctions::Form::Harmonic, {292.88, 115.45}},
        {"H5", "CQ", "NC", AngleFunctions::Form::Harmonic, {292.88, 115.45}},
        {"NC", "CQ", "NC", AngleFunctions::Form::Harmonic, {585.76, 129.1}},
        {"HA", "CR", "NA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"HA", "CX", "NA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"HA", "CR", "NB", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"HA", "CK", "N^", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"HA", "CK", "NA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"HA", "CK", "NB", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"NA", "CR", "NA", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"NA", "CR", "NB", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"NA", "CR", "CT", AngleFunctions::Form::Harmonic, {585.76, 125.0}},
        {"NB", "CR", "CT", AngleFunctions::Form::Harmonic, {585.76, 125.0}},
        {"NA", "CR", "SY", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"NB", "CR", "SY", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"NB", "CR", "S", AngleFunctions::Form::Harmonic, {585.76, 113.6}},
        {"C", "CT", "CT", AngleFunctions::Form::Harmonic, {527.184, 111.1}},
        {"CM", "CT", "CT", AngleFunctions::Form::Harmonic, {527.184, 111.1}},
        {"CW", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CV", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CX", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"C", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"C", "CT", "N", AngleFunctions::Form::Harmonic, {527.184, 110.1}},
        {"C", "CT", "NC", AngleFunctions::Form::Harmonic, {527.184, 110.1}},
        {"C^", "CT", "CT", AngleFunctions::Form::Harmonic, {527.184, 115.6}},
        {"C^", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CS", "CT", "CT", AngleFunctions::Form::Harmonic, {527.184, 115.6}},
        {"CS", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CA", "CT", "CT", AngleFunctions::Form::Harmonic, {527.184, 114.0}},
        {"CA", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CA", "N3", "H3", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        /* { "CC",	"CT",	"CT",	AngleFunctions::Form::Harmonic,	527.184,	113.1 }, */
        {"CW", "CT", "CT", AngleFunctions::Form::Harmonic, {527.184, 114.0}},
        {"CV", "CT", "CT", AngleFunctions::Form::Harmonic, {527.184, 114.0}},
        {"CX", "CT", "CT", AngleFunctions::Form::Harmonic, {527.184, 114.0}},
        /* { "CC",	"CT",	"HC",	AngleFunctions::Form::Harmonic,	292.88,		109.5 }, */
        {"CM", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"C=", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CT", "CT", "CT", AngleFunctions::Form::Harmonic, {488.273, 112.7}},
        {"C3", "CT", "C3", AngleFunctions::Form::Harmonic, {334.72, 109.5}},
        {"C2", "CT", "C2", AngleFunctions::Form::Harmonic, {334.72, 109.5}},
        {"C2", "CT", "C3", AngleFunctions::Form::Harmonic, {334.72, 109.5}},
        {"C3", "CT", "C", AngleFunctions::Form::Harmonic, {527.184, 109.5}},
        {"CT", "CT", "HC", AngleFunctions::Form::Harmonic, {313.8, 110.7}},
        {"CT", "HC", "DM", AngleFunctions::Form::Harmonic, {313.8, 109.47}},
        {"CT", "OS", "DM", AngleFunctions::Form::Harmonic, {83.68, 109.47}},
        {"CT", "OH", "DM", AngleFunctions::Form::Harmonic, {83.68, 109.47}},
        {"OH", "HO", "DM", AngleFunctions::Form::Harmonic, {83.68, 109.47}},
        {"CT", "S", "DM", AngleFunctions::Form::Harmonic, {83.68, 109.47}},
        {"CT", "SH", "DM", AngleFunctions::Form::Harmonic, {83.68, 109.47}},
        {"SH", "HS", "DM", AngleFunctions::Form::Harmonic, {83.68, 109.47}},
        {"NZ", "CZ", "DM", AngleFunctions::Form::Harmonic, {83.68, 90.0}},
        {"CT", "CT", "N", AngleFunctions::Form::Harmonic, {669.44, 109.7}},
        {"CT", "CT", "NM", AngleFunctions::Form::Harmonic, {669.44, 109.7}},
        {"CT", "CT", "N^", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"CT", "CO", "N^", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"CT", "CT", "N2", AngleFunctions::Form::Harmonic, {669.44, 111.2}},
        {"C", "CT", "N3", AngleFunctions::Form::Harmonic, {669.44, 111.2}},
        {"C", "CT", "NT", AngleFunctions::Form::Harmonic, {669.44, 111.2}},
        {"CA", "CT", "NT", AngleFunctions::Form::Harmonic, {669.44, 111.2}},
        {"CA", "CT", "NA", AngleFunctions::Form::Harmonic, {669.44, 111.2}},
        {"CT", "CT", "N3", AngleFunctions::Form::Harmonic, {669.44, 111.2}},
        {"CT", "CT", "OH", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"CA", "CT", "OH", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"CT", "CT", "OS", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"CA", "CT", "OS", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"CT", "CT", "S", AngleFunctions::Form::Harmonic, {418.4, 114.7}},
        {"CT", "CT", "SH", AngleFunctions::Form::Harmonic, {418.4, 108.6}},
        {"CA", "CT", "S", AngleFunctions::Form::Harmonic, {418.4, 114.7}},
        {"CW", "CT", "S", AngleFunctions::Form::Harmonic, {418.4, 114.7}},
        {"CT", "NT", "H", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CA", "NT", "H", AngleFunctions::Form::Harmonic, {292.88, 111.0}},
        {"CA", "NT", "CT", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"CT", "NT", "CT", AngleFunctions::Form::Harmonic, {433.462, 107.2}},
        {"CT", "NT", "CH", AngleFunctions::Form::Harmonic, {433.462, 107.2}},
        {"CT", "NT", "C2", AngleFunctions::Form::Harmonic, {433.462, 107.2}},
        {"CT", "NT", "C3", AngleFunctions::Form::Harmonic, {433.462, 107.2}},
        {"CH", "NT", "CH", AngleFunctions::Form::Harmonic, {433.462, 107.2}},
        {"CH", "NT", "C2", AngleFunctions::Form::Harmonic, {433.462, 107.2}},
        {"CH", "NT", "C3", AngleFunctions::Form::Harmonic, {433.462, 107.2}},
        {"C2", "NT", "C2", AngleFunctions::Form::Harmonic, {433.462, 107.2}},
        {"C2", "NT", "C3", AngleFunctions::Form::Harmonic, {433.462, 107.2}},
        {"C3", "NT", "C3", AngleFunctions::Form::Harmonic, {433.462, 107.2}},
        {"HC", "CT", "HC", AngleFunctions::Form::Harmonic, {276.144, 107.8}},
        {"DM", "HC", "DM", AngleFunctions::Form::Harmonic, {276.144, 109.47}},
        {"DM", "HO", "DM", AngleFunctions::Form::Harmonic, {276.144, 109.47}},
        {"DM", "HS", "DM", AngleFunctions::Form::Harmonic, {276.144, 109.47}},
        {"DM", "OS", "DM", AngleFunctions::Form::Harmonic, {276.144, 109.47}},
        {"DM", "OH", "DM", AngleFunctions::Form::Harmonic, {276.144, 109.47}},
        {"DM", "S", "DM", AngleFunctions::Form::Harmonic, {276.144, 109.47}},
        {"DM", "SH", "DM", AngleFunctions::Form::Harmonic, {276.144, 109.47}},
        {"DM", "F", "DM", AngleFunctions::Form::Harmonic, {276.144, 109.47}},
        {"DM", "Cl", "DM", AngleFunctions::Form::Harmonic, {276.144, 109.47}},
        {"DM", "Br", "DM", AngleFunctions::Form::Harmonic, {276.144, 109.47}},
        {"DM", "I", "DM", AngleFunctions::Form::Harmonic, {276.144, 109.47}},
        {"CY", "CY", "HC", AngleFunctions::Form::Harmonic, {313.8, 117.2}},
        {"CY", "CY", "CY", AngleFunctions::Form::Harmonic, {251.04, 79.2}},
        {"CY", "CY", "CT", AngleFunctions::Form::Harmonic, {313.8, 117.2}},
        {"CY", "CT", "HC", AngleFunctions::Form::Harmonic, {313.8, 110.7}},
        {"HC", "CY", "HC", AngleFunctions::Form::Harmonic, {292.88, 114.3}},
        {"HC", "CY", "CT", AngleFunctions::Form::Harmonic, {292.88, 114.3}},
        {"CY", "CY", "N$", AngleFunctions::Form::Harmonic, {669.44, 89.0}},
        {"CY", "N$", "C$", AngleFunctions::Form::Harmonic, {418.4, 94.0}},
        {"N$", "C$", "CY", AngleFunctions::Form::Harmonic, {585.76, 91.0}},
        {"CY", "CY", "C$", AngleFunctions::Form::Harmonic, {527.184, 85.0}},
        {"N$", "C$", "O", AngleFunctions::Form::Harmonic, {669.44, 134.0}},
        {"CY", "C$", "O", AngleFunctions::Form::Harmonic, {669.44, 134.0}},
        {"HC", "CY", "N$", AngleFunctions::Form::Harmonic, {292.88, 111.0}},
        {"HC", "CY", "N", AngleFunctions::Form::Harmonic, {292.88, 108.0}},
        {"HC", "CY", "C$", AngleFunctions::Form::Harmonic, {313.8, 110.0}},
        {"CY", "CY", "N", AngleFunctions::Form::Harmonic, {313.8, 126.0}},
        {"HC", "CY", "S", AngleFunctions::Form::Harmonic, {313.8, 108.0}},
        {"CY", "CY", "S", AngleFunctions::Form::Harmonic, {460.24, 128.0}},
        {"CY", "N", "C", AngleFunctions::Form::Harmonic, {460.24, 128.0}},
        {"CY", "N", "H", AngleFunctions::Form::Harmonic, {334.72, 113.0}},
        {"N", "CY", "C$", AngleFunctions::Form::Harmonic, {585.76, 117.0}},
        {"N$", "CY", "S", AngleFunctions::Form::Harmonic, {460.24, 109.0}},
        {"C$", "N$", "CT", AngleFunctions::Form::Harmonic, {460.24, 127.0}},
        {"CY", "S", "CT", AngleFunctions::Form::Harmonic, {518.816, 94.0}},
        {"CY", "N$", "CT", AngleFunctions::Form::Harmonic, {418.4, 126.0}},
        {"N$", "CT", "CT", AngleFunctions::Form::Harmonic, {669.44, 110.0}},
        {"N$", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"N$", "CT", "C", AngleFunctions::Form::Harmonic, {669.44, 113.0}},
        /* { "CY",	"O$",	"CY",	AngleFunctions::Form::Harmonic,	502.08,		90.0 }, */
        /* { "CY",	"CY",	"O$",	AngleFunctions::Form::Harmonic,	418.4,		90.0 }, */
        /* { "CT",	"CY",	"O$",	AngleFunctions::Form::Harmonic,	313.8,		117.2 }, */
        /* { "HC",	"CY",	"O$",	AngleFunctions::Form::Harmonic,	313.8,		117.2 }, */
        {"HC", "CT", "N", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CT", "NM", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CT", "N^", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CO", "N^", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CT", "NA", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CT", "N2", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CT", "N3", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CT", "NT", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CT", "NC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CT", "OH", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CT", "OS", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CT", "S", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CT", "P", AngleFunctions::Form::Harmonic, {343.088, 109.5}},
        {"CT", "CT", "P", AngleFunctions::Form::Harmonic, {359.824, 109.5}},
        {"CA", "CT", "P", AngleFunctions::Form::Harmonic, {359.824, 109.5}},
        {"CT", "CT", "P+", AngleFunctions::Form::Harmonic, {359.824, 109.5}},
        {"CT", "P+", "CT", AngleFunctions::Form::Harmonic, {376.56, 109.5}},
        {"HC", "CT", "P+", AngleFunctions::Form::Harmonic, {343.088, 109.5}},
        {"HC", "CT", "SH", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"N^", "CT", "OS", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"N^", "CO", "OS", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        /* { "CC",	"CV",	"HA",	AngleFunctions::Form::Harmonic,	292.88,		120.0 }, */
        /* { "CC",	"CV",	"NB",	AngleFunctions::Form::Harmonic,	585.76,		120.0 }, */
        {"CW", "CW", "NB", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"HA", "CV", "NB", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"C^", "CW", "HA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"C^", "CW", "NA", AngleFunctions::Form::Harmonic, {585.76, 108.7}},
        /* { "CC",	"CW",	"HA",	AngleFunctions::Form::Harmonic,	292.88,		120.0 }, */
        /* { "CC",	"CW",	"NA",	AngleFunctions::Form::Harmonic,	585.76,		120.0 }, */
        {"H4", "CW", "NA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"HA", "CA", "NA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"C", "N", "C2", AngleFunctions::Form::Harmonic, {418.4, 121.9}},
        {"C", "N", "C3", AngleFunctions::Form::Harmonic, {418.4, 121.9}},
        {"C", "N", "CH", AngleFunctions::Form::Harmonic, {418.4, 121.9}},
        {"C", "N", "CT", AngleFunctions::Form::Harmonic, {418.4, 121.9}},
        {"C", "NM", "CT", AngleFunctions::Form::Harmonic, {418.4, 121.9}},
        {"C", "N", "CA", AngleFunctions::Form::Harmonic, {418.4, 121.9}},
        {"C", "N", "H", AngleFunctions::Form::Harmonic, {292.88, 119.8}},
        {"C2", "N", "C3", AngleFunctions::Form::Harmonic, {418.4, 121.9}},
        /* { "C",	"N",	"H2",	AngleFunctions::Form::Harmonic,	292.88,		120.0 }, */
        {"C2", "N", "CH", AngleFunctions::Form::Harmonic, {418.4, 118.0}},
        {"C2", "N", "H", AngleFunctions::Form::Harmonic, {317.984, 118.4}},
        {"C3", "N", "H", AngleFunctions::Form::Harmonic, {317.984, 118.4}},
        {"CH", "N", "H", AngleFunctions::Form::Harmonic, {317.984, 118.4}},
        {"CT", "N", "CT", AngleFunctions::Form::Harmonic, {418.4, 118.0}},
        {"CT", "N2", "CT", AngleFunctions::Form::Harmonic, {418.4, 118.0}},
        {"CT", "NM", "CT", AngleFunctions::Form::Harmonic, {418.4, 118.0}},
        {"CA", "N", "CT", AngleFunctions::Form::Harmonic, {418.4, 118.0}},
        {"CA", "NC", "CT", AngleFunctions::Form::Harmonic, {418.4, 118.0}},
        {"CT", "N", "H", AngleFunctions::Form::Harmonic, {317.984, 118.4}},
        {"H", "N", "H", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"H", "N2", "H", AngleFunctions::Form::Harmonic, {292.88, 113.0}},
        {"H3", "N", "H3", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"C", "N^", "CH", AngleFunctions::Form::Harmonic, {585.76, 117.6}},
        /* { "C",	"N^",	"CJ",	AngleFunctions::Form::Harmonic,	585.76,		121.6 }, */
        {"C", "N^", "CM", AngleFunctions::Form::Harmonic, {585.76, 121.6}},
        {"C", "NA", "CM", AngleFunctions::Form::Harmonic, {585.76, 121.6}},
        {"C", "N^", "CT", AngleFunctions::Form::Harmonic, {585.76, 117.6}},
        {"C", "N^", "CO", AngleFunctions::Form::Harmonic, {585.76, 117.6}},
        {"C", "N^", "H", AngleFunctions::Form::Harmonic, {292.88, 119.2}},
        {"C3", "N^", "CB", AngleFunctions::Form::Harmonic, {585.76, 125.8}},
        /* { "C3",	"N^",	"CE",	AngleFunctions::Form::Harmonic,	585.76,		128.8 }, */
        {"C3", "N^", "CK", AngleFunctions::Form::Harmonic, {585.76, 128.8}},
        /* { "CB",	"N^",	"CE",	AngleFunctions::Form::Harmonic,	585.76,		105.4 }, */
        {"CB", "N^", "CH", AngleFunctions::Form::Harmonic, {585.76, 125.8}},
        /* { "CE",	"N^",	"CH",	AngleFunctions::Form::Harmonic,	585.76,		128.8 }, */
        /* { "CE",	"N^",	"CT",	AngleFunctions::Form::Harmonic,	585.76,		128.8 }, */
        /* { "CE",	"N^",	"H",	AngleFunctions::Form::Harmonic,	292.88,		127.3 }, */
        /* { "CH",	"N^",	"CJ",	AngleFunctions::Form::Harmonic,	585.76,		121.2 }, */
        {"CH", "N^", "CK", AngleFunctions::Form::Harmonic, {585.76, 128.8}},
        /* { "CJ",	"N^",	"CT",	AngleFunctions::Form::Harmonic,	585.76,		121.2 }, */
        /* { "CJ",	"N^",	"H",	AngleFunctions::Form::Harmonic,	292.88,		119.2 }, */
        {"CM", "N^", "CT", AngleFunctions::Form::Harmonic, {585.76, 121.2}},
        {"CM", "N^", "CO", AngleFunctions::Form::Harmonic, {585.76, 121.2}},
        {"CM", "N^", "H", AngleFunctions::Form::Harmonic, {292.88, 119.2}},
        {"CM", "NA", "H", AngleFunctions::Form::Harmonic, {292.88, 119.2}},
        {"C2", "N2", "CA", AngleFunctions::Form::Harmonic, {418.4, 123.2}},
        /* { "C2",	"N2",	"H2",	AngleFunctions::Form::Harmonic,	292.88,		118.4 }, */
        {"C2", "N2", "H3", AngleFunctions::Form::Harmonic, {292.88, 118.4}},
        {"C3", "N2", "CA", AngleFunctions::Form::Harmonic, {418.4, 123.2}},
        /* { "C3",	"N2",	"H2",	AngleFunctions::Form::Harmonic,	292.88,		118.4 }, */
        {"CA", "N2", "CT", AngleFunctions::Form::Harmonic, {418.4, 123.2}},
        {"CA", "N2", "H", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"CQ", "N2", "H", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        /* { "CA",	"N2",	"H2",	AngleFunctions::Form::Harmonic,	292.88,		120.0 }, */
        {"CA", "N2", "H3", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"CT", "N2", "H3", AngleFunctions::Form::Harmonic, {292.88, 118.4}},
        {"CT", "N2", "H", AngleFunctions::Form::Harmonic, {292.88, 118.4}},
        /* { "H2",	"N2",	"H2",	AngleFunctions::Form::Harmonic,	292.88,		120.0 }, */
        {"H3", "N2", "H3", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"C2", "N3", "H3", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"C3", "N3", "H3", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CT", "N3", "H3", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"H3", "N3", "H3", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CT", "N3", "CT", AngleFunctions::Form::Harmonic, {418.4, 113.0}},
        {"CA", "N3", "CT", AngleFunctions::Form::Harmonic, {460.24, 114.0}},
        {"C", "NA", "C", AngleFunctions::Form::Harmonic, {585.76, 126.4}},
        {"C", "N", "C", AngleFunctions::Form::Harmonic, {585.76, 126.4}},
        {"C", "NA", "CA", AngleFunctions::Form::Harmonic, {585.76, 125.2}},
        {"C", "N", "CQ", AngleFunctions::Form::Harmonic, {585.76, 125.2}},
        {"C", "NA", "H", AngleFunctions::Form::Harmonic, {292.88, 116.8}},
        {"CA", "NA", "H", AngleFunctions::Form::Harmonic, {292.88, 118.0}},
        {"CQ", "N", "H", AngleFunctions::Form::Harmonic, {292.88, 118.0}},
        /* { "CC",	"NA",	"CP",	AngleFunctions::Form::Harmonic,	585.76,		107.3 }, */
        /* { "CC",	"NA",	"CR",	AngleFunctions::Form::Harmonic,	585.76,		120.0 }, */
        /* { "CC",	"NA",	"H",	AngleFunctions::Form::Harmonic,	292.88,		120.0 }, */
        /* { "CG",	"NA",	"CN",	AngleFunctions::Form::Harmonic,	585.76,		111.6 }, */
        /* { "CA",	"NA",	"CC",	AngleFunctions::Form::Harmonic,	585.76,		111.6 }, */
        /* { "CC",	"NA",	"CC",	AngleFunctions::Form::Harmonic,	585.76,		111.6 }, */
        /* { "CG",	"NA",	"CP",	AngleFunctions::Form::Harmonic,	585.76,		107.3 }, */
        /* { "CG",	"NA",	"CR",	AngleFunctions::Form::Harmonic,	585.76,		107.3 }, */
        /* { "CG",	"NA",	"H",	AngleFunctions::Form::Harmonic,	292.88,		126.35 }, */
        {"CN", "NA", "CW", AngleFunctions::Form::Harmonic, {585.76, 111.6}},
        {"CN", "NA", "H", AngleFunctions::Form::Harmonic, {292.88, 123.1}},
        /* { "CP",	"NA",	"H",	AngleFunctions::Form::Harmonic,	292.88,		126.35 }, */
        {"CR", "NA", "H", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"CW", "NA", "H", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"CX", "NA", "H", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"CB", "N^", "CK", AngleFunctions::Form::Harmonic, {585.76, 105.4}},
        {"CB", "N^", "CT", AngleFunctions::Form::Harmonic, {585.76, 125.8}},
        {"CB", "N^", "CO", AngleFunctions::Form::Harmonic, {585.76, 125.8}},
        {"CB", "N^", "H", AngleFunctions::Form::Harmonic, {251.04, 125.8}},
        {"CK", "N^", "CT", AngleFunctions::Form::Harmonic, {585.76, 128.8}},
        {"CK", "N^", "CO", AngleFunctions::Form::Harmonic, {585.76, 128.8}},
        {"CK", "N^", "H", AngleFunctions::Form::Harmonic, {251.04, 128.8}},
        {"CB", "NA", "CK", AngleFunctions::Form::Harmonic, {585.76, 105.4}},
        {"CB", "NA", "CT", AngleFunctions::Form::Harmonic, {585.76, 125.8}},
        {"CB", "NA", "H", AngleFunctions::Form::Harmonic, {251.04, 125.8}},
        {"CK", "NA", "CT", AngleFunctions::Form::Harmonic, {585.76, 128.8}},
        {"CK", "NA", "H", AngleFunctions::Form::Harmonic, {251.04, 128.8}},
        /* { "CB",	"NB",	"CE",	AngleFunctions::Form::Harmonic,	585.76,		103.8 }, */
        {"CB", "NB", "CK", AngleFunctions::Form::Harmonic, {585.76, 103.8}},
        /* { "CC",	"NB",	"CP",	AngleFunctions::Form::Harmonic,	585.76,		105.3 }, */
        /* { "CC",	"NB",	"CR",	AngleFunctions::Form::Harmonic,	585.76,		117.0 }, */
        /* { "CF",	"NB",	"CP",	AngleFunctions::Form::Harmonic,	585.76,		105.3 }, */
        /* { "CF",	"NB",	"CR",	AngleFunctions::Form::Harmonic,	585.76,		105.3 }, */
        {"CR", "NB", "CV", AngleFunctions::Form::Harmonic, {585.76, 110.0}},
        {"CR", "NB", "CB", AngleFunctions::Form::Harmonic, {585.76, 110.0}},
        {"CR", "NB", "CW", AngleFunctions::Form::Harmonic, {585.76, 110.0}},
        {"C", "NC", "CA", AngleFunctions::Form::Harmonic, {585.76, 120.5}},
        {"CA", "NC", "CB", AngleFunctions::Form::Harmonic, {585.76, 112.2}},
        /* { "CA",	"NC",	"CI",	AngleFunctions::Form::Harmonic,	585.76,		118.6 }, */
        {"CA", "NC", "CQ", AngleFunctions::Form::Harmonic, {585.76, 118.6}},
        {"CQ", "NC", "CQ", AngleFunctions::Form::Harmonic, {585.76, 118.6}},
        /* { "CB",	"NC",	"CI",	AngleFunctions::Form::Harmonic,	585.76,		111.0 }, */
        {"CB", "NC", "CQ", AngleFunctions::Form::Harmonic, {585.76, 111.0}},
        {"CR", "NC", "CQ", AngleFunctions::Form::Harmonic, {585.76, 111.0}},
        {"C2", "NT", "H", AngleFunctions::Form::Harmonic, {361.498, 108.1}},
        {"C3", "NT", "H", AngleFunctions::Form::Harmonic, {361.498, 108.1}},
        {"CH", "NT", "H", AngleFunctions::Form::Harmonic, {361.498, 108.1}},
        {"H", "NT", "H", AngleFunctions::Form::Harmonic, {364.845, 106.4}},
        {"H", "N3", "H", AngleFunctions::Form::Harmonic, {364.845, 109.5}},
        {"H", "N", "OH", AngleFunctions::Form::Harmonic, {292.88, 110.2}},
        {"C", "N", "OH", AngleFunctions::Form::Harmonic, {384.928, 115.7}},
        {"N", "OH", "HO", AngleFunctions::Form::Harmonic, {410.032, 105.4}},
        {"C", "OH", "HO", AngleFunctions::Form::Harmonic, {292.88, 113.0}},
        {"CA", "OH", "HO", AngleFunctions::Form::Harmonic, {292.88, 113.0}},
        {"CM", "OH", "HO", AngleFunctions::Form::Harmonic, {292.88, 109.0}},
        {"C2", "OH", "HO", AngleFunctions::Form::Harmonic, {460.24, 108.5}},
        {"C3", "OH", "HO", AngleFunctions::Form::Harmonic, {460.24, 108.5}},
        {"CH", "OH", "HO", AngleFunctions::Form::Harmonic, {460.24, 108.5}},
        {"CT", "OH", "HO", AngleFunctions::Form::Harmonic, {460.24, 108.5}},
        {"HO", "OH", "P", AngleFunctions::Form::Harmonic, {460.24, 108.5}},
        {"C2", "OS", "C2", AngleFunctions::Form::Harmonic, {836.8, 111.8}},
        {"C2", "OS", "C3", AngleFunctions::Form::Harmonic, {836.8, 111.8}},
        {"CH", "C", "OS", AngleFunctions::Form::Harmonic, {677.808, 111.4}},
        {"C", "OS", "CH", AngleFunctions::Form::Harmonic, {694.544, 116.9}},
        {"C", "OS", "C2", AngleFunctions::Form::Harmonic, {694.544, 116.9}},
        {"C", "OS", "C3", AngleFunctions::Form::Harmonic, {694.544, 116.9}},
        {"O", "C", "OS", AngleFunctions::Form::Harmonic, {694.544, 123.4}},
        {"C", "OS", "CT", AngleFunctions::Form::Harmonic, {694.544, 116.9}},
        {"OS", "C", "CT", AngleFunctions::Form::Harmonic, {677.808, 111.4}},
        {"OS", "C", "CA", AngleFunctions::Form::Harmonic, {677.808, 111.4}},
        {"C", "OS", "CA", AngleFunctions::Form::Harmonic, {694.544, 116.9}},
        {"CA", "CH", "OS", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"OS", "CO", "OH", AngleFunctions::Form::Harmonic, {774.877, 111.55}},
        {"OS", "CO", "OS", AngleFunctions::Form::Harmonic, {774.877, 111.55}},
        {"C3", "OS", "CO", AngleFunctions::Form::Harmonic, {836.8, 113.0}},
        {"C2", "OS", "CO", AngleFunctions::Form::Harmonic, {836.8, 113.0}},
        {"CH", "OS", "CO", AngleFunctions::Form::Harmonic, {836.8, 113.0}},
        {"C2", "CO", "OS", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"C3", "CO", "OS", AngleFunctions::Form::Harmonic, {669.44, 109.5}},
        {"C3", "CO", "C3", AngleFunctions::Form::Harmonic, {334.72, 109.5}},
        {"C2", "C2", "CO", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"C3", "C2", "CO", AngleFunctions::Form::Harmonic, {527.184, 112.4}},
        {"OS", "CO", "CT", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"CO", "CT", "CT", AngleFunctions::Form::Harmonic, {488.273, 112.7}},
        {"CT", "CO", "OH", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"CT", "CO", "HC", AngleFunctions::Form::Harmonic, {313.8, 110.7}},
        {"CO", "OH", "HO", AngleFunctions::Form::Harmonic, {460.24, 108.5}},
        {"OS", "CO", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CO", "OS", "CT", AngleFunctions::Form::Harmonic, {502.08, 109.5}},
        {"CO", "CT", "HC", AngleFunctions::Form::Harmonic, {313.8, 110.7}},
        {"CO", "CT", "OH", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"OH", "CO", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "CO", "HC", AngleFunctions::Form::Harmonic, {276.144, 109.5}},
        {"C2", "OS", "HO", AngleFunctions::Form::Harmonic, {460.24, 108.5}},
        {"CA", "OS", "P", AngleFunctions::Form::Harmonic, {836.8, 120.5}},
        {"C2", "OS", "P", AngleFunctions::Form::Harmonic, {836.8, 120.5}},
        {"C3", "OS", "P", AngleFunctions::Form::Harmonic, {836.8, 120.5}},
        {"CH", "OS", "CH", AngleFunctions::Form::Harmonic, {836.8, 111.8}},
        {"CH", "OS", "HO", AngleFunctions::Form::Harmonic, {460.24, 108.5}},
        {"CH", "OS", "P", AngleFunctions::Form::Harmonic, {836.8, 120.5}},
        {"CT", "OS", "CT", AngleFunctions::Form::Harmonic, {502.08, 109.5}},
        {"Si", "OS", "Si", AngleFunctions::Form::Harmonic, {167.36, 145.0}},
        {"CT", "OS", "Si", AngleFunctions::Form::Harmonic, {334.72, 130.0}},
        {"CT", "Si", "OS", AngleFunctions::Form::Harmonic, {502.08, 100.0}},
        {"CT", "Si", "CT", AngleFunctions::Form::Harmonic, {502.08, 110.0}},
        {"CT", "CT", "Si", AngleFunctions::Form::Harmonic, {502.08, 112.0}},
        {"OS", "Si", "OS", AngleFunctions::Form::Harmonic, {502.08, 110.0}},
        {"Si", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "Si", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"HC", "Si", "CT", AngleFunctions::Form::Harmonic, {292.88, 110.5}},
        {"H", "Si", "H", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"H", "Si", "CT", AngleFunctions::Form::Harmonic, {292.88, 110.5}},
        {"F", "Si", "CT", AngleFunctions::Form::Harmonic, {292.88, 110.5}},
        {"Cl", "Si", "CT", AngleFunctions::Form::Harmonic, {292.88, 110.5}},
        {"Br", "Si", "CT", AngleFunctions::Form::Harmonic, {292.88, 110.5}},
        {"I", "Si", "CT", AngleFunctions::Form::Harmonic, {292.88, 110.5}},
        {"CT", "OS", "CA", AngleFunctions::Form::Harmonic, {627.6, 111.0}},
        {"CT", "OS", "CM", AngleFunctions::Form::Harmonic, {627.6, 111.0}},
        {"CT", "OS", "P", AngleFunctions::Form::Harmonic, {836.8, 120.5}},
        {"CT", "OH", "P", AngleFunctions::Form::Harmonic, {836.8, 120.5}},
        {"O", "C", "O2", AngleFunctions::Form::Harmonic, {669.44, 126.0}},
        {"O2", "P", "O2", AngleFunctions::Form::Harmonic, {1171.52, 119.9}},
        {"O2", "P", "OH", AngleFunctions::Form::Harmonic, {376.56, 108.23}},
        {"O2", "P", "OS", AngleFunctions::Form::Harmonic, {836.8, 108.23}},
        {"OH", "P", "OS", AngleFunctions::Form::Harmonic, {376.56, 102.6}},
        {"OS", "P", "OS", AngleFunctions::Form::Harmonic, {376.56, 102.6}},
        {"O", "P", "OH", AngleFunctions::Form::Harmonic, {836.8, 108.23}},
        {"O", "P", "OS", AngleFunctions::Form::Harmonic, {836.8, 108.23}},
        {"OH", "P", "OH", AngleFunctions::Form::Harmonic, {376.56, 102.6}},
        {"CT", "P", "OS", AngleFunctions::Form::Harmonic, {376.56, 109.5}},
        {"CT", "P", "O2", AngleFunctions::Form::Harmonic, {376.56, 109.5}},
        {"CT", "P", "O", AngleFunctions::Form::Harmonic, {376.56, 109.5}},
        {"CA", "P", "OS", AngleFunctions::Form::Harmonic, {376.56, 109.5}},
        {"CA", "P", "OH", AngleFunctions::Form::Harmonic, {376.56, 109.5}},
        {"CA", "P", "O", AngleFunctions::Form::Harmonic, {376.56, 109.5}},
        {"C2", "S", "C3", AngleFunctions::Form::Harmonic, {518.816, 98.9}},
        {"C2", "S", "LP", AngleFunctions::Form::Harmonic, {1255.2, 96.7}},
        {"C2", "S", "S", AngleFunctions::Form::Harmonic, {569.024, 103.7}},
        {"C3", "S", "LP", AngleFunctions::Form::Harmonic, {1255.2, 96.7}},
        {"C3", "S", "S", AngleFunctions::Form::Harmonic, {569.024, 103.7}},
        {"CT", "S", "CT", AngleFunctions::Form::Harmonic, {518.816, 98.9}},
        {"CR", "S", "CW", AngleFunctions::Form::Harmonic, {619.232, 90.0}},
        {"CW", "S", "CW", AngleFunctions::Form::Harmonic, {619.232, 97.0}},
        {"CT", "S", "LP", AngleFunctions::Form::Harmonic, {1255.2, 96.7}},
        {"CT", "S", "S", AngleFunctions::Form::Harmonic, {569.024, 103.7}},
        {"LP", "S", "LP", AngleFunctions::Form::Harmonic, {0, 160.0}},
        {"LP", "S", "S", AngleFunctions::Form::Harmonic, {1255.2, 96.7}},
        {"C2", "SH", "HS", AngleFunctions::Form::Harmonic, {368.192, 96.0}},
        {"C2", "SH", "LP", AngleFunctions::Form::Harmonic, {1255.2, 96.7}},
        {"C3", "SH", "HS", AngleFunctions::Form::Harmonic, {368.192, 96.0}},
        {"C3", "SH", "LP", AngleFunctions::Form::Harmonic, {1255.2, 96.7}},
        {"CT", "SH", "HS", AngleFunctions::Form::Harmonic, {368.192, 96.0}},
        {"CA", "SH", "HS", AngleFunctions::Form::Harmonic, {418.4, 96.0}},
        {"CT", "SH", "LP", AngleFunctions::Form::Harmonic, {1255.2, 96.7}},
        {"HS", "SH", "HS", AngleFunctions::Form::Harmonic, {292.88, 92.07}},
        {"HS", "SH", "LP", AngleFunctions::Form::Harmonic, {1255.2, 96.7}},
        {"LP", "SH", "LP", AngleFunctions::Form::Harmonic, {0, 160.0}},
        {"P", "OS", "P", AngleFunctions::Form::Harmonic, {836.8, 120.5}},
        {"C9", "C8", "C8", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C9", "C8", "C7", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C9", "C8", "CT", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C9", "C8", "CH", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C9", "C8", "C2", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C9", "C8", "C3", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C9", "C7", "C8", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C9", "C7", "C7", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C9", "C7", "CT", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C9", "C7", "CH", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C9", "C7", "C2", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C9", "C7", "C3", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C8", "C8", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C8", "C7", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C8", "CT", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C8", "CH", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C8", "C2", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C8", "C3", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C7", "C8", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C7", "C7", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C7", "CT", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C7", "CH", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C7", "C2", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "C7", "C3", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C7", "C7", "C7", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C7", "C7", "CT", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C7", "C7", "CH", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C7", "C7", "C2", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C7", "C7", "C3", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"CT", "C8", "CT", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CT", "C8", "CH", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CT", "C8", "C2", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CT", "C8", "C3", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CH", "C8", "CH", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CH", "C8", "C3", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"C2", "C8", "CH", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"C2", "C8", "C2", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"C2", "C8", "C3", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"C3", "C8", "C3", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CT", "C7", "CT", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CT", "C7", "CH", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CT", "C7", "C2", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CT", "C7", "C3", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CH", "C7", "CH", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CH", "C7", "C2", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CH", "C7", "C3", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"C2", "C7", "C2", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"C2", "C7", "C3", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"C3", "C7", "C3", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CA", "OS", "C2", AngleFunctions::Form::Harmonic, {836.8, 111.8}},
        {"CA", "CT", "CA", AngleFunctions::Form::Harmonic, {334.72, 109.5}},
        {"CA", "CT", "C2", AngleFunctions::Form::Harmonic, {527.184, 114.0}},
        {"CA", "CT", "C", AngleFunctions::Form::Harmonic, {527.184, 112.0}},
        {"N", "CT", "C2", AngleFunctions::Form::Harmonic, {669.44, 109.7}},
        {"HC", "CT", "C2", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"C", "CT", "C2", AngleFunctions::Form::Harmonic, {527.184, 111.1}},
        {"CT", "CA", "NA", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CA", "NA", "CA", AngleFunctions::Form::Harmonic, {585.76, 125.2}},
        {"CA", "CA", "NB", AngleFunctions::Form::Harmonic, {585.76, 108.7}},
        {"NA", "CA", "NB", AngleFunctions::Form::Harmonic, {585.76, 123.3}},
        {"CA", "NB", "CA", AngleFunctions::Form::Harmonic, {585.76, 125.2}},
        {"HA", "CA", "NB", AngleFunctions::Form::Harmonic, {292.88, 119.1}},
        {"CA", "CA", "N", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CA", "N", "H", AngleFunctions::Form::Harmonic, {292.88, 119.8}},
        {"CB", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CA", "CB", "CT", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CB", "CA", "NA", AngleFunctions::Form::Harmonic, {585.76, 108.7}},
        /* { "CA",	"CA",	"CC",	AngleFunctions::Form::Harmonic,	711.28,		120.0 }, */
        {"CB", "CB", "CT", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        /* { "CB",	"CB",	"CC",	AngleFunctions::Form::Harmonic,	711.28,		120.0 }, */
        {"CB", "CT", "CT", AngleFunctions::Form::Harmonic, {527.184, 114.0}},
        {"CT", "CT", "F", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"C2", "C2", "F", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"CA", "CT", "F", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"CM", "CT", "F", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"F", "CT", "F", AngleFunctions::Form::Harmonic, {644.336, 109.1}},
        {"HC", "CT", "F", AngleFunctions::Form::Harmonic, {334.72, 107.0}},
        {"CT", "C", "C", AngleFunctions::Form::Harmonic, {669.44, 117.2}},
        {"C", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 121.4}},
        {"C", "C", "N", AngleFunctions::Form::Harmonic, {585.76, 116.6}},
        {"C9", "C8", "SY", AngleFunctions::Form::Harmonic, {585.76, 118.0}},
        {"C8", "SY", "C3", AngleFunctions::Form::Harmonic, {518.816, 98.9}},
        {"OY", "SY", "N", AngleFunctions::Form::Harmonic, {1004.16, 107.0}},
        {"OY", "SY", "OY", AngleFunctions::Form::Harmonic, {870.272, 119.0}},
        {"OY", "SZ", "CT", AngleFunctions::Form::Harmonic, {619.232, 107.0}},
        {"OY", "SY", "CT", AngleFunctions::Form::Harmonic, {619.232, 108.9}},
        {"OY", "SY", "CA", AngleFunctions::Form::Harmonic, {619.232, 107.2}},
        {"N", "SY", "CA", AngleFunctions::Form::Harmonic, {836.8, 103.0}},
        {"SY", "CA", "CA", AngleFunctions::Form::Harmonic, {711.28, 119.4}},
        {"SY", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"SZ", "CT", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CT", "SY", "CT", AngleFunctions::Form::Harmonic, {518.816, 102.0}},
        {"CA", "SY", "CT", AngleFunctions::Form::Harmonic, {518.816, 102.0}},
        {"CR", "SY", "CT", AngleFunctions::Form::Harmonic, {518.816, 102.0}},
        {"CT", "SZ", "CT", AngleFunctions::Form::Harmonic, {518.816, 96.0}},
        {"CT", "CT", "SY", AngleFunctions::Form::Harmonic, {418.4, 108.6}},
        {"CT", "CT", "SZ", AngleFunctions::Form::Harmonic, {418.4, 108.6}},
        {"N", "SY", "CT", AngleFunctions::Form::Harmonic, {836.8, 103.0}},
        {"SY", "N", "CT", AngleFunctions::Form::Harmonic, {418.4, 120.0}},
        {"H", "N", "SY", AngleFunctions::Form::Harmonic, {836.8, 111.0}},
        {"OS", "C", "N", AngleFunctions::Form::Harmonic, {677.808, 111.4}},
        {"CT", "NT", "SY", AngleFunctions::Form::Harmonic, {418.4, 108.6}},
        {"C", "CT", "F", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"SY", "CT", "F", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"SY", "NT", "H", AngleFunctions::Form::Harmonic, {292.88, 115.0}},
        {"C", "CW", "NA", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        {"NT", "C", "CW", AngleFunctions::Form::Harmonic, {585.76, 116.0}},
        {"C", "CW", "CS", AngleFunctions::Form::Harmonic, {711.28, 120.0}},
        {"CB", "CS", "HA", AngleFunctions::Form::Harmonic, {292.88, 120.0}},
        {"CW", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 120.4}},
        {"C", "NT", "CT", AngleFunctions::Form::Harmonic, {527.184, 111.1}},
        {"C", "CT", "C", AngleFunctions::Form::Harmonic, {527.184, 111.1}},
        {"C", "CT", "OS", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"N", "CT", "OS", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"NT", "C", "O", AngleFunctions::Form::Harmonic, {669.44, 120.4}},
        {"NT", "C", "CT", AngleFunctions::Form::Harmonic, {585.76, 116.0}},
        {"CA", "NT", "C", AngleFunctions::Form::Harmonic, {527.184, 112.0}},
        {"CA", "NT", "SY", AngleFunctions::Form::Harmonic, {418.4, 108.6}},
        {"OY", "SY", "NT", AngleFunctions::Form::Harmonic, {619.232, 108.9}},
        {"NT", "SY", "CT", AngleFunctions::Form::Harmonic, {518.816, 102.0}},
        {"NT", "CT", "S", AngleFunctions::Form::Harmonic, {418.4, 114.7}},
        {"HC", "CY", "NT", AngleFunctions::Form::Harmonic, {292.88, 114.3}},
        {"CY", "CY", "NT", AngleFunctions::Form::Harmonic, {313.8, 117.2}},
        {"CA", "NT", "CY", AngleFunctions::Form::Harmonic, {418.4, 109.5}},
        {"NC", "CA", "Cl", AngleFunctions::Form::Harmonic, {627.6, 120.0}},
        {"CA", "NT", "CA", AngleFunctions::Form::Harmonic, {334.72, 109.5}},
        {"NC", "CA", "NT", AngleFunctions::Form::Harmonic, {585.76, 116.0}},
        {"CM", "CM", "CY", AngleFunctions::Form::Harmonic, {585.76, 124.0}},
        {"CM", "CY", "HC", AngleFunctions::Form::Harmonic, {292.88, 109.5}},
        {"CM", "CY", "CY", AngleFunctions::Form::Harmonic, {527.184, 114.0}},
        {"C", "CM", "CY", AngleFunctions::Form::Harmonic, {585.76, 119.7}},
        {"N^", "CM", "CT", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"NA", "CM", "CT", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"S", "CM", "CM", AngleFunctions::Form::Harmonic, {711.28, 119.4}},
        {"S", "CM", "N^", AngleFunctions::Form::Harmonic, {711.28, 119.4}},
        {"S", "CM", "NA", AngleFunctions::Form::Harmonic, {711.28, 119.4}},
        {"N^", "CM", "OS", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"NA", "CM", "OS", AngleFunctions::Form::Harmonic, {585.76, 120.0}},
        {"CA", "S", "CM", AngleFunctions::Form::Harmonic, {518.816, 104.2}},
        {"CM", "OS", "CA", AngleFunctions::Form::Harmonic, {627.6, 111.0}},
        {"CM", "CT", "CA", AngleFunctions::Form::Harmonic, {334.72, 109.5}},
        {"S", "CA", "CA", AngleFunctions::Form::Harmonic, {711.28, 119.4}},
        {"P", "CA", "CA", AngleFunctions::Form::Harmonic, {711.28, 119.4}},
        {"CA", "S", "CT", AngleFunctions::Form::Harmonic, {518.816, 104.2}},
        {"C", "N", "Zn", AngleFunctions::Form::Harmonic, {167.36, 126.0}},
        {"HO", "OH", "Zn", AngleFunctions::Form::Harmonic, {836.8, 126.0}},
        {"N", "Zn", "N", AngleFunctions::Form::Harmonic, {167.36, 109.5}},
        {"N", "Zn", "O", AngleFunctions::Form::Harmonic, {167.36, 109.5}}};

    return Forcefield::termMatch_(angleTerms, i, j, k);
}

// Return torsion term for the supplied atom type quartet (if it exists)
OptionalReferenceWrapper<const ForcefieldTorsionTerm>
OPLSAA2005BaseForcefield::getTorsionTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k,
                                         const ForcefieldAtomType &l) const
{
    static std::vector<ForcefieldTorsionTerm> torsionTerms = {
        //	i	j	k	l	Type (CosineForm)		k		n	eq	s
        {"HC", "CT", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // hydrocarbon
        {"HC", "C", "C", "HC", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                   //
        {"HC", "C=", "C=", "HC", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 //
        {"CT", "C=", "C=", "CT", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 //
        {"CT", "C=", "C=", "HC", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 //
        {"H3", "N3", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // ammonium
        {"HC", "CT", "CT", "CT", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // hydrocarbon
        {"HC", "CT", "CT", "CO", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // acetal
        {"HC", "CO", "CT", "CT", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // acetal
        {"CT", "CT", "CT", "CT", TorsionFunctions::Form::Cos3, {5.4392, -0.2092, 0.8368}},      // hydrocarbon
        {"CT", "CT", "CT", "CO", TorsionFunctions::Form::Cos3, {5.4392, -0.2092, 0.8368}},      // hydrocarbon
        {"HC", "CT", "CT", "OH", TorsionFunctions::Form::Cos3, {0, 0, 1.95811}},                // alcohols,
        {"HC", "CT", "CT", "OS", TorsionFunctions::Form::Cos3, {0, 0, 1.95811}},                // alcohols,
        {"HC", "CT", "CO", "OS", TorsionFunctions::Form::Cos3, {0, 0, 1.95811}},                // alcohols,
        {"CT", "CT", "CT", "O*", TorsionFunctions::Form::Cos3, {7.15882, -2.092, 2.77399}},     // alcohols,
        {"CT", "CT", "CO", "O*", TorsionFunctions::Form::Cos3, {7.15882, -2.092, 2.77399}},     // alcohols,
        {"HC", "CM", "CT", "O*", TorsionFunctions::Form::Cos3, {0, 0, 1.95811}},                // alcohols,
        {"CT", "CM", "CT", "O*", TorsionFunctions::Form::Cos3, {7.15882, -2.092, 2.77399}},     // alcohols,
        {"CT", "CT", "CT", "OH", TorsionFunctions::Form::Cos3, {-6.49357, 0, 0}},               // polyols
        {"HC", "CT", "OH", "HO", TorsionFunctions::Form::Cos3, {0, 0, 1.47444}},                // alcohols
        {"CT", "CT", "OH", "HO", TorsionFunctions::Form::Cos3, {-1.4895, -0.728016, 2.05853}},  // alcohols
        {"OH", "CT", "CT", "OH", TorsionFunctions::Form::Cos3, {39.7815, 0, 0}},                // diols
        {"OH", "CT", "CT", "OH", TorsionFunctions::Form::Cos3, {51.1871, 0, 0}},                // triols
        {"HC", "CT", "OS", "C^", TorsionFunctions::Form::Cos3, {0, 0, 3.17984}},                // ethers
        {"HC", "CT", "OS", "CA", TorsionFunctions::Form::Cos3, {0, 0, 3.17984}},                // ethers
        {"HC", "CT", "OS", "CM", TorsionFunctions::Form::Cos3, {0, 0, 3.17984}},                // ethers
        {"HC", "CM", "OS", "CT", TorsionFunctions::Form::Cos3, {0, 0, 3.17984}},                // ethers
        {"HC", "CO", "OS", "CT", TorsionFunctions::Form::Cos3, {0, 0, 3.17984}},                // ethers
        {"HC", "CT", "OS", "CO", TorsionFunctions::Form::Cos3, {0, 0, 3.17984}},                // ethers
        {"CT", "OS", "CT", "CT", TorsionFunctions::Form::Cos3, {2.7196, -1.046, 2.80328}},      // ethers
        {"CT", "OS", "CM", "CT", TorsionFunctions::Form::Cos3, {2.7196, -1.046, 2.80328}},      // ethers
        {"CT", "OS", "CO", "CT", TorsionFunctions::Form::Cos3, {2.7196, -1.046, 2.80328}},      // ethers
        {"CT", "OS", "CT", "O*", TorsionFunctions::Form::Cos3, {-2.17986, -8.44331, 8.35126}},  // acetals
        {"NT", "CT", "CT", "OH", TorsionFunctions::Form::Cos3, {33.472, 0, 0}},                 // 2-aminoethanol
        {"OS", "CT", "CT", "OS", TorsionFunctions::Form::Cos3, {-2.3012, 0, 0}},                // polyethers,
        {"CT", "OS", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 12.552, 0}},                 // anisole
        {"CM", "C=", "C=", "CT", TorsionFunctions::Form::Cos3, {0, 0, -1.55645}},               // diene-
        {"CM", "C=", "C=", "HC", TorsionFunctions::Form::Cos3, {0, 0, -1.55645}},               // diene-
        {"CM", "C=", "C=", "CM", TorsionFunctions::Form::Cos3, {5.95383, 16.9661, 3.58987}},    // diene
        {"HC", "CT", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // ethyl
        {"H3", "N3", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // anilinium
        {"HC", "CT", "CW", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"HC", "CT", "CV", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"HC", "CT", "CR", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"HC", "CT", "CS", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"HC", "CT", "CQ", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"HC", "CT", "CU", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"HC", "CT", "CK", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"HC", "CT", "C^", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"CT", "CT", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // ethyl
        {"CT", "N3", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // anilinium
        {"CT", "CT", "CW", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"CT", "CT", "CV", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"CT", "CT", "CR", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"CT", "CT", "CS", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"CT", "CT", "CQ", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"CT", "CT", "CU", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"CT", "CT", "CK", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"CT", "CT", "C^", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aromatics
        {"O*", "CT", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // benzyl
        {"C^", "CT", "NA", "C^", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // heterocycles
        {"H*", "CT", "NA", "C^", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // heterocycles
        {"C^", "CT", "N^", "C^", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // heterocycles
        {"H*", "CT", "N^", "C^", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // heterocycles
        {"O", "C", "CR", "O*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // heterocycles
        {"O", "C", "CR", "N^", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // heterocycles
        {"CA", "CA", "CT", "N^", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // aromatics
        {"*", "Zn", "N", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                         // JACS
        {"*", "Zn", "O", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                         // JACS
        {"HC", "CT", "CT", "CA", TorsionFunctions::Form::Cos3, {0, 0, 1.93301}},                // ethyl
        {"HC", "CT", "N3", "CA", TorsionFunctions::Form::Cos3, {0, 0, 1.93301}},                // anilinium
        {"HC", "CT", "CT", "CW", TorsionFunctions::Form::Cos3, {0, 0, 1.93301}},                // aromatics
        {"HC", "CT", "CT", "CV", TorsionFunctions::Form::Cos3, {0, 0, 1.93301}},                // aromatics
        {"HC", "CT", "CT", "CR", TorsionFunctions::Form::Cos3, {0, 0, 1.93301}},                // aromatics
        {"HC", "CT", "CT", "CS", TorsionFunctions::Form::Cos3, {0, 0, 1.93301}},                // aromatics
        {"HC", "CT", "CT", "CQ", TorsionFunctions::Form::Cos3, {0, 0, 1.93301}},                // aromatics
        {"HC", "CT", "CT", "CU", TorsionFunctions::Form::Cos3, {0, 0, 1.93301}},                // aromatics
        {"HC", "CT", "CT", "CK", TorsionFunctions::Form::Cos3, {0, 0, 1.93301}},                // aromatics
        {"HC", "CT", "CT", "C^", TorsionFunctions::Form::Cos3, {0, 0, 1.93301}},                // aromatics
        {"CT", "S", "S", "CT", TorsionFunctions::Form::Cos3, {0, -31.0202, 7.13372}},           // disulfide
        {"HC", "CT", "S", "S", TorsionFunctions::Form::Cos3, {0, 0, 2.33467}},                  // disulfide
        {"CT", "CT", "S", "S", TorsionFunctions::Form::Cos3, {8.12114, -3.49782, 3.91204}},     // disulfide
        {"HC", "CT", "SH", "HS", TorsionFunctions::Form::Cos3, {0, 0, 2.00832}},                // thiol
        {"CT", "CT", "SH", "HS", TorsionFunctions::Form::Cos3, {-3.17566, -1.17989, 2.84512}},  // thiol
        {"HC", "CT", "CT", "SH", TorsionFunctions::Form::Cos3, {0, 0, 1.89117}},                // thiol
        {"HC", "CT", "CT", "SY", TorsionFunctions::Form::Cos3, {0, 0, 1.89117}},                //
        {"HC", "CT", "CT", "S", TorsionFunctions::Form::Cos3, {0, 0, 1.89117}},                 // sulfide
        {"CT", "CT", "CT", "SH", TorsionFunctions::Form::Cos3, {5.28021, -0.828432, 1.94556}},  // thiol
        {"CT", "CT", "CT", "SY", TorsionFunctions::Form::Cos3, {5.28021, -0.828432, 1.94556}},  // (mod
        {"HC", "CT", "NT", "H", TorsionFunctions::Form::Cos3, {0, 0, 1.6736}},                  // amine
        {"HC", "CT", "CT", "NT", TorsionFunctions::Form::Cos3, {-4.23839, -2.96646, 1.97903}},  // amine
        {"CT", "CT", "NT", "H", TorsionFunctions::Form::Cos3, {-0.79496, -1.74473, 1.74891}},   // amine
        {"CT", "CT", "N2", "H", TorsionFunctions::Form::Cos3, {-0.79496, -1.74473, 1.74891}},   // guanidinium
        {"CT", "CT", "N2", "H3", TorsionFunctions::Form::Cos3, {-0.79496, -1.74473, 1.74891}},  // guanidinium
        {"CT", "CT", "CT", "NT", TorsionFunctions::Form::Cos3, {10.0081, -2.82002, 2.3012}},    // amine
        {"HO", "OH", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 7.03749, 0}},                // phenol
        {"N", "CT", "CT", "CT", TorsionFunctions::Form::Cos3, {3.53548, -4.02501, 2.98319}},    // Chi-1
        {"N", "CT", "CT", "C^", TorsionFunctions::Form::Cos3, {3.53548, -4.02501, 2.98319}},    // Chi-1
        {"N", "CT", "CT", "CA", TorsionFunctions::Form::Cos3, {3.53548, -4.02501, 2.98319}},    // Chi-1
        {"C", "N", "CT", "C", TorsionFunctions::Form::Cos3, {-9.89516, 3.81581, -3.5564}},      // Phi
        {"N", "CT", "C", "N", TorsionFunctions::Form::Cos3, {7.59814, 5.11285, 6.6149}},        // Psi
        {"C", "N", "CT", "CT", TorsionFunctions::Form::Cos3, {0, 1.93301, 0}},                  // Phi'
        {"CT", "CT", "C", "N", TorsionFunctions::Form::Cos3, {4.90783, 0.790776, -5.0208}},     // Psi'
        {"C", "N", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // Phi"
        {"HC", "CT", "C", "N", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // Psi"
        {"HC", "CM", "C", "N", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        //
        {"HC", "CT", "C", "NM", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       //
        {"HC", "CM", "C", "NM", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       //
        {"CQ", "N", "CT", "CT", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       //
        {"CQ", "N", "CT", "CA", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       //
        {"H", "N", "CT", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                         // peptides
        {"H", "N", "CT", "CT", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // peptides
        {"H", "N", "CT", "C", TorsionFunctions::Form::Cos3, {0, 0, 0}},                         // peptides
        {"*", "CT", "C", "O", TorsionFunctions::Form::Cos3, {0, 0, 0}},                         // peptides
        {"CT", "CT", "C", "O", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // peptides
        {"N", "CT", "C", "O", TorsionFunctions::Form::Cos3, {0, 0, 0}},                         // peptides
        {"CT", "C", "N", "H", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},                   // amides
        {"CM", "C", "N", "H", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},                   // amides
        {"HC", "C", "N", "H", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},                   // amides
        {"CT", "C", "NM", "CT", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},                 // tert.
        {"O", "C", "NM", "CT", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},                  // tert.
        {"CT", "C", "N", "CT", TorsionFunctions::Form::Cos3, {9.6232, 25.4764, 0}},             // amides-
        {"CT", "C", "N", "CA", TorsionFunctions::Form::Cos3, {9.6232, 25.4764, 0}},             //
        {"HC", "C", "N", "CT", TorsionFunctions::Form::Cos3, {9.6232, 25.4764, 0}},             // amides-
        {"O", "C", "N", "H", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},                    // amides
        {"O", "C", "N", "CT", TorsionFunctions::Form::Cos3, {0, 25.4764, 0}},                   // amides
        {"O", "C", "N", "CA", TorsionFunctions::Form::Cos3, {0, 25.4764, 0}},                   // amides
        {"O", "C", "N", "CM", TorsionFunctions::Form::Cos3, {0, 25.4764, 0}},                   // amides
        {"N", "C", "N", "H", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},                    // imides
        {"N", "C", "N", "C", TorsionFunctions::Form::Cos3, {9.6232, 25.4764, 0}},               // imides
        {"CT", "N", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // tert.
        {"CT", "N2", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // tert.
        {"CA", "N", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // "
        {"CT", "NM", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // tert.
        {"CA", "NM", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // "
        {"C^", "CA", "SH", "HS", TorsionFunctions::Form::Cos3, {0, 4.6024, 0}},                 // aromatic
        {"CA", "CA", "SH", "HS", TorsionFunctions::Form::Cos3, {0, 4.6024, 0}},                 // aromatic
        {"N^", "CA", "SH", "HS", TorsionFunctions::Form::Cos3, {-12.552, 0, 0}},                // "
        {"HC", "CT", "S", "CT", TorsionFunctions::Form::Cos3, {0, 0, 2.70705}},                 // sulfide
        {"HC", "CT", "S", "CA", TorsionFunctions::Form::Cos3, {0, 0, 2.70705}},                 // sulfide
        {"CT", "CT", "CT", "S", TorsionFunctions::Form::Cos3, {10.9579, -2.59408, 1.07947}},    // sulfide
        {"CT", "CT", "S", "CT", TorsionFunctions::Form::Cos3, {3.8702, -2.40998, 2.83257}},     // sulfide
        {"H", "N2", "CA", "N2", TorsionFunctions::Form::Cos3, {0, 16.3176, 0}},                 // guanidinium
        {"H", "N2", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 16.3176, 0}},                 // guanidinium
        {"CT", "N2", "CA", "N2", TorsionFunctions::Form::Cos3, {0, 33.2042, 0}},                // methylguanidinium
        {"HC", "CT", "N2", "CA", TorsionFunctions::Form::Cos3, {0, 0, 0.740568}},               // methylguanidinium
        {"HC", "CT", "N2", "H", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // methylguanidinium
        {"CT", "CT", "N2", "CA", TorsionFunctions::Form::Cos3, {7.65254, 1.01671, -2.08363}},   // ethylguanidinium
        {"HC", "CT", "CT", "N2", TorsionFunctions::Form::Cos3, {0, 0, -2.43509}},               // ethylguanidinium
        {"C", "N", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, -0.581576}},                // N-methylformamide
        {"HC", "CT", "N", "H", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // N-methylformamide
        {"C", "N", "CT", "CT", TorsionFunctions::Form::Cos3, {-5.84086, -1.78657, 0}},          // N-ethylformamide
        {"H", "N", "CT", "CT", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // N-ethylformamide
        {"N", "CT", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 1.94138}},                 // N-ethylformamide
        {"N", "CT", "CT", "CT", TorsionFunctions::Form::Cos3, {8.21738, 0, 2.75726}},           // N-propylformamide
        {"C", "NM", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, -0.581576}},               // tertiary
        {"C", "NM", "CT", "CT", TorsionFunctions::Form::Cos3, {-5.84086, -1.78657, 0}},         // tertiary
        {"NM", "CT", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 1.94138}},                // tertiary
        {"NM", "CT", "CT", "CT", TorsionFunctions::Form::Cos3, {8.21738, 0, 2.75726}},          // tertiary
        {"CM", "CM", "C", "NM", TorsionFunctions::Form::Cos3, {8.368, 0, 0}},                   // tertiary
        {"CM", "CM", "C", "N", TorsionFunctions::Form::Cos3, {8.368, 0, 0}},                    // vinyl
        {"HC", "CT", "C", "O", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // all
        {"HC", "C", "C", "O", TorsionFunctions::Form::Cos3, {0, 0, 0}},                         //
        {"HC", "CM", "C", "O", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        //
        {"CT", "CT", "C", "N", TorsionFunctions::Form::Cos3, {13.598, -1.68197, -0.569024}},    // propanamide
        {"CT", "CT", "C", "O", TorsionFunctions::Form::Cos3, {0, 4.87854, 0}},                  // propanamide
        {"HC", "CT", "CT", "C", TorsionFunctions::Form::Cos3, {0, 0, -0.4184}},                 // all
        {"C", "CT", "CT", "CT", TorsionFunctions::Form::Cos3, {-8.61904, -1.30959, 1.31796}},   // butanamide
        {"N", "CT", "CT", "OH", TorsionFunctions::Form::Cos3, {26.2755, -6.13793, 8.49352}},    // Chi
        {"C", "CT", "CT", "OH", TorsionFunctions::Form::Cos3, {-25.8571, 0, 0}},                // Chi
        {"N", "CT", "CT", "S*", TorsionFunctions::Form::Cos3, {5.97475, 0.359824, 0.121336}},   // Chi
        {"C", "CT", "CT", "S*", TorsionFunctions::Form::Cos3, {-18.1753, -7.17138, 0}},         // Chi
        {"HC", "CT", "C^", "CW", TorsionFunctions::Form::Cos3, {0, 0, -2.00832}},               // 3-methylindole
        {"HC", "CT", "C^", "CB", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // 3-methylindole
        {"CT", "CT", "C^", "CW", TorsionFunctions::Form::Cos3, {-2.98738, 0, 0}},               // 3-ethylindole
        {"CT", "CT", "C^", "CB", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // 3-ethylindole
        {"HC", "CT", "CC", "N^", TorsionFunctions::Form::Cos3, {0, 0, 1.7531}},                 // HID,
        {"CT", "CT", "CC", "N^", TorsionFunctions::Form::Cos3, {9.89934, -1.09621, 2.11292}},   // "
        {"HC", "CT", "N3", "H", TorsionFunctions::Form::Cos3, {0, 0, 1.09202}},                 // ammonium
        {"CT", "CT", "N3", "H", TorsionFunctions::Form::Cos3, {0, 0, 1.45185}},                 // ammonium
        {"HC", "CT", "CT", "N3", TorsionFunctions::Form::Cos3, {0, 0, 1.60666}},                // ammonium
        {"CT", "CT", "CT", "N3", TorsionFunctions::Form::Cos3, {11.4307, -0.958136, 2.02924}},  // ammonium
        {"HC", "CT", "NO", "ON", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // nitro
        {"*T", "CT", "C", "O", TorsionFunctions::Form::Cos3, {0, 3.43088, 0}},                  // carboxylate
        {"*T", "CT", "C", "O2", TorsionFunctions::Form::Cos3, {0, 3.43088, 0}},                 // carboxylate
        {"HC", "CT", "CT", "NO", TorsionFunctions::Form::Cos3, {0, 0, -0.9414}},                // nitroethane
        {"CT", "CT", "CT", "C", TorsionFunctions::Form::Cos3, {-13.326, -3.4518, 2.06271}},     // carboxylate
        {"HC", "CT", "C", "O2", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // caboxylates
        {"HC", "CT", "C", "OH", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // acids
        {"CT", "CT", "C", "O", TorsionFunctions::Form::Cos3, {0, 2.28446, 0}},                  // RCOOH
        {"CA", "CT", "C", "O", TorsionFunctions::Form::Cos3, {0, 2.28446, 0}},                  // RCOOH
        {"CT", "CT", "C", "OH", TorsionFunctions::Form::Cos3, {4.184, 2.28446, 1.8828}},        // RCOOH
        {"NT", "CT", "C", "OH", TorsionFunctions::Form::Cos3, {22.0078, 3.43088, 0}},           // neutral
        {"HC", "CT", "OH", "HO", TorsionFunctions::Form::Cos3, {-10.8324, -4.69863, 1.12968}},  // axial
        {"C", "CT", "CT", "C", TorsionFunctions::Form::Cos3, {-2.3012, 0, 4.184}},              // dicarboxylic
        {"O", "C", "OH", "HO", TorsionFunctions::Form::Cos3, {0, 23.012, 0}},                   // carboxylic
        {"CT", "C", "OH", "HO", TorsionFunctions::Form::Cos3, {6.276, 23.012, 0}},              // carboxylic
        {"HC", "C", "OH", "HO", TorsionFunctions::Form::Cos3, {6.276, 23.012, 0}},              // carboxylic
        {"C", "CT", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0.309616}},                // dicarboxylic
        {"CT", "CT", "C", "O", TorsionFunctions::Form::Cos3, {-3.138, -2.3012, -1.046}},        // dicarboxylic
        {"CT", "CT", "C", "OH", TorsionFunctions::Form::Cos3, {0, 5.90781, 0}},                 // dicarboxylic
        {"CT", "N", "CT", "C", TorsionFunctions::Form::Cos3, {-7.26761, 5.23418, -14.6482}},    // Proline
        {"CT", "N", "CT", "CT", TorsionFunctions::Form::Cos3, {19.8866, -3.07106, 0}},          // "
        {"CT", "CT", "N", "CT", TorsionFunctions::Form::Cos3, {11.9621, 8.61067, -47.1369}},    // "
        {"CT", "NM", "CT", "C", TorsionFunctions::Form::Cos3, {-7.26761, 5.23418, -14.6482}},   //
        {"CT", "NM", "CT", "CT", TorsionFunctions::Form::Cos3, {19.8866, -3.07106, 0}},         //
        {"CT", "CT", "NM", "CT", TorsionFunctions::Form::Cos3, {11.9621, 8.61067, -47.1369}},   //
        {"CT", "CT", "C+", "CT", TorsionFunctions::Form::Cos3, {0, -4.184, 0}},                 // carbocation
        {"CT", "CT", "C+", "HC", TorsionFunctions::Form::Cos3, {0, -4.184, 0}},                 // carbocation
        {"HC", "CT", "C", "CT", TorsionFunctions::Form::Cos3, {0, 0, 1.1506}},                  // ketone
        {"HC", "CT", "C", "HC", TorsionFunctions::Form::Cos3, {0, 0, 1.50624}},                 // aldehyde
        {"HC", "CT", "C", "F", TorsionFunctions::Form::Cos3, {0, 0, 1.50624}},                  // acyl
        {"HC", "CT", "C", "Cl", TorsionFunctions::Form::Cos3, {0, 0, 1.50624}},                 // acyl
        {"HC", "CT", "C", "Br", TorsionFunctions::Form::Cos3, {0, 0, 1.50624}},                 // acyl
        {"CT", "CT", "C", "F", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // acyl
        {"CT", "CT", "C", "Cl", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // acyl
        {"CT", "CT", "C", "Br", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // acyl
        {"CT", "CT", "C", "O", TorsionFunctions::Form::Cos3, {-1.15897, 5.13795, -2.9037}},     // aldehyde
        {"HC", "CT", "CT", "C(O)", TorsionFunctions::Form::Cos3, {0, 0, -0.317984}},            // aldehyde
        {"CT", "CT", "CT", "C", TorsionFunctions::Form::Cos3, {-7.10025, -1.9079, 2.44764}},    // aldehyde
        {"CA", "CT", "CT", "C", TorsionFunctions::Form::Cos3, {-7.10025, -1.9079, 2.44764}},    // aldehyde
        {"C^", "CT", "CT", "C", TorsionFunctions::Form::Cos3, {-7.10025, -1.9079, 2.44764}},    //
        {"CT", "CT", "C", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // aldehyde
        {"CT", "CT", "C", "CT", TorsionFunctions::Form::Cos3, {6.08354, -0.602496, -3.2426}},   // ketone
        {"C", "C", "OH", "HO", TorsionFunctions::Form::Cos3, {12.552, 23.012, 0}},              // oxalic
        {"CA", "CT", "P", "OS", TorsionFunctions::Form::Cos3, {9.414, 0, 0}},                   // phosphonates
        {"CT", "P", "OS", "CT", TorsionFunctions::Form::Cos3, {14.644, -13.8072, 6.276}},       // phosphonates
        {"P", "OS", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                  // phosphonates
        {"O2", "P", "OS", "CT", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // phosphonates
        {"O", "P", "OS", "CA", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // phosphonates
        {"O", "P", "OH", "HO", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // phosphonates
        {"OH", "P", "OH", "HO", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // phosphonates
        {"CA", "CA", "CT", "P", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // phosphonates
        {"CA", "CT", "P", "O2", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // phosphonates
        {"HC", "CT", "P", "O2", TorsionFunctions::Form::Cos3, {0, 0, 1.046}},                   // phosphonates
        {"HC", "CT", "P", "OS", TorsionFunctions::Form::Cos3, {0, 0, 1.046}},                   // phosphonates
        {"O2", "P", "OS", "C^", TorsionFunctions::Form::Cos3, {0, 0, 2.35141}},                 // MeOPO3
        {"O2", "P", "OS", "CT", TorsionFunctions::Form::Cos3, {3.7656, -12.2591, 11.0458}},     // dimethyl
        {"CA", "CA", "OS", "P", TorsionFunctions::Form::Cos3, {0, 12.5102, 0}},                 // PhOPO3
        {"NT", "CT", "CT", "Cl", TorsionFunctions::Form::Cos3, {8.368, 0, 0}},                  // 2-chloroethylamines
        {"HC", "CT", "CT", "Cl", TorsionFunctions::Form::Cos3, {0, 0, 1.6736}},                 // alkyl
        {"CT", "CT", "CT", "Cl", TorsionFunctions::Form::Cos3, {0, 0, 1.6736}},                 // alkyl
        {"Cl", "CT", "CT", "Cl", TorsionFunctions::Form::Cos3, {-1.046, 0, 0}},                 // dichloride
        {"HC", "CT", "CT", "I", TorsionFunctions::Form::Cos3, {0, 0, 1.6736}},                  // alkyl
        {"CT", "CT", "CT", "I", TorsionFunctions::Form::Cos3, {0, 0, 1.6736}},                  // alkyl
        {"HO", "OH", "CT", "CT", TorsionFunctions::Form::Cos3, {18.736, -9.09853, 0}},          // trifluoroethanol
        {"HC", "CT", "OH", "HO", TorsionFunctions::Form::Cos3, {0, 0, 1.99158}},                // trifluoroethanol
        {"F", "CT", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 1.31252}},                 // trifluoroethanol
        {"F", "CT", "CT", "OH", TorsionFunctions::Form::Cos3, {0, 0, 2.25978}},                 // trifluoroethanol
        {"CT", "C", "OH", "HO", TorsionFunctions::Form::Cos3, {13.3888, 20.5016, 0}},           // 1,2-diacid
        {"CT", "CT", "C", "O", TorsionFunctions::Form::Cos3, {-4.184, -7.9496, -3.7656}},       // 1,2-diacid
        {"C", "CT", "CT", "C", TorsionFunctions::Form::Cos3, {3.3472, 0, 3.7656}},              // 1,2-diacid
        {"H", "N", "SY", "CA", TorsionFunctions::Form::Cos3, {6.99146, -20.5058, 2.7991}},      // sulfonamide
        {"HC", "CT", "N", "SY", TorsionFunctions::Form::Cos3, {5.69861, -6.09609, 0.623416}},   // sulfonamide
        {"CT", "N", "SY", "CA", TorsionFunctions::Form::Cos3, {8.67762, -12.4097, 10.347}},     // sulfonamide
        {"CT", "CT", "N", "SY", TorsionFunctions::Form::Cos3, {12.2549, -10.5981, 2.07945}},    // sulfonamide
        {"CA", "CA", "SY", "N", TorsionFunctions::Form::Cos3, {6.9287, -3.21331, -0.489528}},   // sulfonamide
        {"*", "*", "SY", "OY", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // sulfonamide
        {"*", "CA", "SY", "OY", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // sulfonamide
        {"*", "N", "SY", "OY", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // sulfonamide
        {"*", "CT", "SY", "OY", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // sulfonamide
        {"*", "CT", "SY", "N", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // sulfonamide
        {"HC", "CT", "CT", "F", TorsionFunctions::Form::Cos3, {0, 0, 1.6736}},                  // alkyl
        {"CT", "CT", "CT", "F", TorsionFunctions::Form::Cos3, {1.2552, -1.6736, 1.6736}},       // alkyl
        {"F", "CT", "CT", "F", TorsionFunctions::Form::Cos3, {-10.46, 0, 1.046}},               // 1,2-difluoride
        {"CT", "CT", "CT", "O*", TorsionFunctions::Form::Cos3, {-5.58982, 0, 0}},               // hexopyranoses
        {"CT", "CT", "OH", "HO", TorsionFunctions::Form::Cos3, {11.188, -12.0625, 4.29278}},    // hexopyranoses
        {"OH", "CT", "CT", "OH", TorsionFunctions::Form::Cos3, {37.9321, 0, 0}},                // hexopyranoses
        {"CT", "OS", "CO", "OH", TorsionFunctions::Form::Cos3, {-1.569, -5.68187, 0.016736}},   // hexopyranoses
        {"CT", "OS", "CO", "OS", TorsionFunctions::Form::Cos3, {-1.569, -5.68187, 0.016736}},   // hexopyranoses
        {"OS", "CO", "OH", "HO", TorsionFunctions::Form::Cos3, {-5.25929, -7.5563, 0.012552}},  // hexopyranoses
        {"OH", "CT", "CT", "OS", TorsionFunctions::Form::Cos3, {18.0707, 0, 0}},                // hexopyranoses
        {"HC", "CT", "CT", "Br", TorsionFunctions::Form::Cos3, {0, 0, 1.6736}},                 // alkyl
        {"CT", "CT", "CT", "Br", TorsionFunctions::Form::Cos3, {0, 0, 1.6736}},                 // alkyl
        {"CA", "CA", "CT", "Br", TorsionFunctions::Form::Cos3, {0, 0, 1.6736}},                 // alkyl
        {"*", "CA", "CA", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   // in
        {"CA", "CA", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "CA", "CA", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CA", "CA", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "CA", "C!", "CA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "CA", "C!", "C!", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "CA", "CA", "C!", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C!", "CA", "CA", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CA", "C!", "C!", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CA", "C!", "CA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "CA", "C!", "NC", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "C!", "NC", "CA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C!", "C!", "CA", "NC", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "C!", "CA", "NC", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C!", "CA", "CA", "NC", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C!", "C!", "NC", "CA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C!", "C!", "N", "C", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"C!", "C!", "CM", "C", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"NC", "C!", "CA", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "CA", "CA", "F", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"CA", "CA", "CA", "Cl", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "CA", "CA", "Br", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "CA", "CA", "I", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"HA", "CA", "CA", "F", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"HA", "CA", "CA", "Cl", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CA", "CA", "Br", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CA", "CA", "I", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"CA", "CA", "CA", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "C=", "C=", "C^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C", "C=", "C=", "C", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"HA", "CA", "CA", "CT", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CT", "CA", "CA", "CT", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CT", "CA", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "CA", "CA", "CB", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CA", "CA", "CB", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "CA", "CA", "O*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CA", "CA", "O*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CT", "CA", "CA", "O*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"O*", "CA", "CA", "O*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "CA", "CA", "S*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CA", "CA", "S*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CT", "CA", "CA", "S*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"S*", "CA", "CA", "S*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CM", "C!", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CM", "C!", "N", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"HA", "CA", "CA", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CT", "CA", "CA", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"O*", "CA", "CA", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"N^", "CA", "CA", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"*", "CB", "CB", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"C^", "CB", "CB", "C^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"N^", "CB", "CB", "C^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"N^", "CB", "CB", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"*", "CB", "CS", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"*", "CS", "C", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                    //
        {"*", "CR", "CS", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"*", "CS", "CW", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"C^", "CS", "CW", "C^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C^", "CS", "CW", "H*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C^", "CS", "CW", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CS", "CW", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CS", "CW", "C^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CS", "CW", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CS", "CS", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CA", "CU", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"HA", "CW", "OS", "CW", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"*", "NC", "CA", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"CS", "CW", "OS", "CW", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "NC", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C^", "NC", "CA", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "NC", "CA", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "NC", "NC", "CT", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CA", "NC", "NC", "CA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CT", "NC", "NC", "CT", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"*", "NC", "CB", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"C^", "NC", "CB", "C^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C^", "NC", "CB", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"*", "NA", "CB", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"*", "NB", "CB", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"*", "NB", "CR", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"*", "NB", "CU", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"C^", "NA", "CB", "C^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C^", "NA", "CB", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C^", "NB", "CB", "C^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"N^", "CR", "S", "CW", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"CR", "NB", "CV", "CW", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CR", "NB", "CV", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CU", "CW", "S", "CR", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"CR", "S", "CW", "CV", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"HA", "CW", "S", "CR", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"CW", "S", "CR", "NB", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"S", "CR", "NB", "CU", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"S", "CR", "NB", "CV", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"CR", "NB", "CU", "CW", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CU", "CW", "OS", "CR", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"CW", "OS", "CR", "NB", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"OS", "CR", "NB", "CU", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"NB", "CU", "CW", "OS", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"NB", "CU", "CW", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"NB", "CU", "CW", "S", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"NB", "CV", "CW", "OS", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"NB", "CV", "CW", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"NB", "CV", "CW", "S", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"*", "N", "CB", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                    //
        {"HA", "CU", "CW", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"HA", "CW", "CU", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"*", "NC", "CR", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"*", "NC", "CQ", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                   //
        {"C^", "NC", "CQ", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"C^", "NC", "CQ", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                 //
        {"*", "N", "CQ", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                    //
        {"C^", "N", "CQ", "N^", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"C^", "N", "CQ", "HA", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                  //
        {"H", "N", "CQ", "*", TorsionFunctions::Form::Cos3, {0, 30.334, 0}},                    //
        {"*", "CW", "NA", "*", TorsionFunctions::Form::Cos3, {0, 11.7152, 0}},                  //
        {"*", "NA", "CR", "*", TorsionFunctions::Form::Cos3, {0, 19.4556, 0}},                  //
        {"HA", "CR", "NB", "*", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                   //
        {"N^", "CR", "NB", "C^", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                  //
        {"*", "CR", "NB", "*", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                    //
        {"*", "CW", "NB", "*", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                    //
        {"*", "CR", "NA", "*", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                    //
        {"HA", "CR", "NA", "*", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                   //
        {"N^", "CR", "NA", "C^", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                  //
        {"*", "CR", "NC", "*", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                    //
        {"*", "CK", "NB", "*", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                    //
        {"*", "CK", "NA", "*", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                    //
        {"*", "CK", "NC", "*", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                    //
        {"*", "NA", "NB", "*", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                    //
        {"*", "NB", "NB", "*", TorsionFunctions::Form::Cos3, {0, 41.84, 0}},                    //
        {"*", "NB", "CV", "*", TorsionFunctions::Form::Cos3, {0, 20.0832, 0}},                  //
        {"*", "CW", "CV", "*", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                   //
        {"C^", "CW", "CV", "C^", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                 //
        {"C^", "CW", "CV", "HA", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                 //
        {"HA", "CW", "CV", "C^", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                 //
        {"HA", "CW", "CV", "HA", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                 //
        {"*", "CW", "CW", "*", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                   //
        {"C^", "CW", "CW", "C^", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                 //
        {"C^", "CW", "CW", "HA", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                 //
        {"H*", "CW", "CW", "C^", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                 //
        {"H*", "CW", "CW", "HA", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                 //
        {"HA", "CW", "CW", "HA", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                 //
        {"S", "CW", "CW", "HA", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                  //
        {"S", "CW", "CU", "HA", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                  //
        {"S", "CW", "CV", "HA", TorsionFunctions::Form::Cos3, {0, 44.978, 0}},                  //
        {"*", "NA", "CW", "*", TorsionFunctions::Form::Cos3, {0, 20.92, 0}},                    //
        {"C^", "NA", "CW", "C^", TorsionFunctions::Form::Cos3, {0, 20.92, 0}},                  //
        {"N^", "NA", "CW", "C^", TorsionFunctions::Form::Cos3, {0, 20.92, 0}},                  //
        {"C^", "NA", "CW", "H*", TorsionFunctions::Form::Cos3, {0, 20.92, 0}},                  //
        {"N^", "NA", "CW", "H*", TorsionFunctions::Form::Cos3, {0, 20.92, 0}},                  //
        {"H", "NA", "CW", "*", TorsionFunctions::Form::Cos3, {0, 20.92, 0}},                    //
        {"H", "NA", "CR", "*", TorsionFunctions::Form::Cos3, {0, 20.92, 0}},                    //
        {"H", "NA", "CB", "*", TorsionFunctions::Form::Cos3, {0, 20.92, 0}},                    //
        {"*", "C^", "CW", "*", TorsionFunctions::Form::Cos3, {0, 54.6012, 0}},                  //
        {"*", "C^", "CB", "*", TorsionFunctions::Form::Cos3, {0, 14.0164, 0}},                  //
        {"*", "CA", "CB", "*", TorsionFunctions::Form::Cos3, {0, 29.288, 0}},                   //
        {"C^", "CA", "CB", "C^", TorsionFunctions::Form::Cos3, {0, 29.288, 0}},                 //
        {"N^", "CA", "CB", "N^", TorsionFunctions::Form::Cos3, {0, 29.288, 0}},                 //
        {"C^", "CA", "CB", "N^", TorsionFunctions::Form::Cos3, {0, 29.288, 0}},                 //
        {"N^", "CA", "CB", "C^", TorsionFunctions::Form::Cos3, {0, 29.288, 0}},                 //
        {"*", "C", "CB", "*", TorsionFunctions::Form::Cos3, {0, 29.288, 0}},                    //
        {"O", "C", "CB", "C^", TorsionFunctions::Form::Cos3, {0, 29.288, 0}},                   //
        {"N^", "C", "CB", "N^", TorsionFunctions::Form::Cos3, {0, 29.288, 0}},                  //
        {"O", "C", "CB", "N^", TorsionFunctions::Form::Cos3, {0, 29.288, 0}},                   //
        {"N^", "C", "CB", "C^", TorsionFunctions::Form::Cos3, {0, 29.288, 0}},                  //
        {"*", "CB", "CN", "*", TorsionFunctions::Form::Cos3, {0, 25.104, 0}},                   //
        {"*", "NA", "CN", "*", TorsionFunctions::Form::Cos3, {0, 12.7612, 0}},                  //
        {"*", "CW", "NA", "*", TorsionFunctions::Form::Cos3, {0, 12.552, 0}},                   //
        {"CT", "CT", "OS", "P", TorsionFunctions::Form::Cos3, {-5.94128, -2.59408, 0.4184}},    // in
        {"HC", "CT", "C", "OS", TorsionFunctions::Form::Cos3, {0, 0, 0.552288}},                // esters
        {"CT", "C", "OS", "CT", TorsionFunctions::Form::Cos3, {19.5351, 21.4388, 0}},           // esters
        {"HC", "C", "OS", "CT", TorsionFunctions::Form::Cos3, {19.5351, 21.4388, 0}},           // esters
        {"O", "C", "OS", "CT", TorsionFunctions::Form::Cos3, {0, 21.4388, 0}},                  // esters
        {"C", "OS", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0.828432}},                // esters
        {"CT", "CT", "C", "OS", TorsionFunctions::Form::Cos3, {0, 0, -2.31375}},                // esters
        {"CT", "CT", "OS", "C", TorsionFunctions::Form::Cos3, {-5.10448, -0.527184, 1.76565}},  // esters
        {"CA", "CT", "CT", "N3", TorsionFunctions::Form::Cos3, {4.184, 0, 0}},                  // phenethylammonium-
        {"CA", "CT", "CT", "NT", TorsionFunctions::Form::Cos3, {-3.3472, 0, 0}},                // phenethylamines-
        {"CT", "CT", "NO", "ON", TorsionFunctions::Form::Cos3, {0, 1.6736, 0}},                 // nitroethane
        {"CA", "CA", "NO", "ON", TorsionFunctions::Form::Cos3, {0, 4.8116, 0}},                 // nitrobenzene
        {"CT", "NY", "CA", "NZ", TorsionFunctions::Form::Cos3, {0, 15.2758, 0}},                // neutral
        {"CT", "CT", "NT", "H", TorsionFunctions::Form::Cos3, {0, 16.736, 0}},                  // azetidine-
        {"CT", "CT", "NT", "H", TorsionFunctions::Form::Cos3, {0.8368, -1.74473, 1.74891}},     // pyrrolidine
        {"CT", "NT", "CT", "CT", TorsionFunctions::Form::Cos3, {1.74054, -0.535552, 2.90788}},  // amine
        {"CT", "NT", "CT", "CT", TorsionFunctions::Form::Cos3, {6.42662, -0.535552, 2.90788}},  // exocyclic
        {"CT", "NT", "CT", "CT", TorsionFunctions::Form::Cos3, {6.12538, -0.535552, 2.90788}},  // exocyclic
        {"CT", "CT", "NT", "H", TorsionFunctions::Form::Cos3, {3.4267, -1.74473, 1.74891}},     // cyclic
        {"CT", "CT", "NT", "H", TorsionFunctions::Form::Cos3, {6.36805, -1.74473, 1.74891}},    // cyclic
        {"NT", "CT", "CT", "NT", TorsionFunctions::Form::Cos3, {46.1704, -4.05011, 1.12968}},   // amine
        {"CT", "NT", "CT", "CT", TorsionFunctions::Form::Cos3, {1.74054, -0.535552, 2.90788}},  // amine
        {"HC", "CT", "NT", "CT", TorsionFunctions::Form::Cos3, {0, 0, 2.34304}},                // amine
        {"CT", "CT", "CW", "NA", TorsionFunctions::Form::Cos3, {7.1128, -2.5104, 0}},           // 2-ethyl
        {"CT", "C=", "C=", "CM", TorsionFunctions::Form::Cos3, {3.7656, 0.96232, -2.11292}},    // 2-Me-1,3-butadiene
        {"CT", "C=", "C", "O*", TorsionFunctions::Form::Cos3, {3.7656, 0.96232, -2.11292}},     // 2-Me-1,3-butadiene-like
        {"CT", "CM", "C", "O*", TorsionFunctions::Form::Cos3, {3.7656, 0.96232, -2.11292}},     // 2-Me-1,3-butadiene-like
        {"CT", "C", "C=", "CM", TorsionFunctions::Form::Cos3, {3.3472, -12.552, 0}},            // methyl
        {"CM", "C=", "C", "OH", TorsionFunctions::Form::Cos3, {13.3888, -12.552, 0}},           // acrylic
        {"CM", "CM", "C", "OH", TorsionFunctions::Form::Cos3, {13.3888, -12.552, 0}},           // acrylic
        {"CM", "C=", "C", "O", TorsionFunctions::Form::Cos3, {10.46, 25.104, 0}},               // acrolein
        {"CM", "CM", "C", "O", TorsionFunctions::Form::Cos3, {10.46, 25.104, 0}},               // acrolein-like
        {"CA", "CA", "C", "CT", TorsionFunctions::Form::Cos3, {0, 0.8368, 0}},                  // aryl
        {"CA", "CA", "C", "HC", TorsionFunctions::Form::Cos3, {0, 0.8368, 0}},                  // aryl
        {"CA", "CA", "N", "*", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                   // N-phenylamide
        {"CA", "CA", "N", "H", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                   // N-phenylamide
        {"CA", "CA", "N", "C", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                   // N-phenylamide
        {"CA", "CA", "N", "CT", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                  // N-phenylamide
        {"CA", "CA", "N", "CR", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                  // diarylamine
        {"CA", "CA", "N", "CW", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                  // diarylamine
        {"CA", "CA", "NT", "CR", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                 // diarylamine
        {"CA", "CA", "NT", "CW", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                 // diarylamine
        {"S", "CR", "N", "CA", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                   // diarylamine
        {"S", "CR", "NT", "CA", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                  // diarylamine
        {"NB", "CR", "N", "CA", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                  // diarylamine
        {"NB", "CR", "NT", "CA", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                 // diarylamine
        {"S", "CR", "N", "H", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                    // diarylamine
        {"S", "CR", "NT", "H", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                   // diarylamine
        {"NB", "CR", "N", "H", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                   // diarylamine
        {"NB", "CR", "NT", "H", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                  // diarylamine
        {"OS", "CR", "N", "CA", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                  // diarylamine
        {"OS", "CR", "N", "H", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                   // diarylamine
        {"OS", "CW", "N", "CA", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                  // diarylamine
        {"OS", "CW", "N", "H", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                   // diarylamine
        {"S", "CW", "N", "CA", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                   // diarylamine
        {"S", "CW", "N", "H", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                    // diarylamine
        {"CA", "CA", "N", "CQ", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                  // diarylamine
        {"CA", "CA", "NT", "CQ", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                 // diarylamine
        {"CA", "CA", "C", "O", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                   // aryl
        {"CA", "CA", "C", "OS", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                  // aryl
        {"CA", "CA", "C", "OH", TorsionFunctions::Form::Cos3, {0, 8.7864, 0}},                  // aryl
        {"CA", "CA", "OS", "C", TorsionFunctions::Form::Cos3, {0, 10.46, 0}},                   // phenyl
        {"O", "C", "OH", "HO", TorsionFunctions::Form::Cos3, {0, 20.92, 0}},                    // benzoic
        {"O", "C", "OS", "CT", TorsionFunctions::Form::Cos3, {0, 20.92, 0}},                    // benzoic
        {"CA", "C", "OH", "HO", TorsionFunctions::Form::Cos3, {16.736, 20.92, 0}},              // benzoic
        {"CA", "C", "OS", "CT", TorsionFunctions::Form::Cos3, {16.736, 20.92, 0}},              // benzoic
        {"O", "C", "OS", "CA", TorsionFunctions::Form::Cos3, {0, 20.92, 0}},                    // phenyl
        {"CT", "C", "OS", "CA", TorsionFunctions::Form::Cos3, {6.276, 20.92, 0}},               // phenyl
        {"CA", "CA", "C", "N", TorsionFunctions::Form::Cos3, {0, 4.6024, 0}},                   // aryl
        {"CA", "CA", "NT", "H", TorsionFunctions::Form::Cos3, {0, 8.49352, 0}},                 // aniline
        {"*", "CA", "N2", "H", TorsionFunctions::Form::Cos3, {0, 8.49352, 0}},                  // aniline-like
        {"*", "CQ", "N2", "H", TorsionFunctions::Form::Cos3, {0, 8.49352, 0}},                  // aniline-like
        {"N^", "CA", "N2", "H", TorsionFunctions::Form::Cos3, {0, 8.49352, 0}},                 // aniline-like
        {"CB", "CA", "N2", "H", TorsionFunctions::Form::Cos3, {0, 8.49352, 0}},                 // aniline-like
        {"S", "CR", "N", "H", TorsionFunctions::Form::Cos3, {0, 8.49352, 0}},                   // aniline-like
        {"OS", "CR", "N", "H", TorsionFunctions::Form::Cos3, {0, 8.49352, 0}},                  // aniline-like
        {"CA", "CA", "NT", "CT", TorsionFunctions::Form::Cos3, {-31.7231, 14.3553, 13.3804}},   // substituted-aniline
        {"CT", "CM", "CT", "CT", TorsionFunctions::Form::Cos3, {11.7863, -0.707096, 2.27191}},  // alkenes
        {"CM", "CM", "CT", "CT", TorsionFunctions::Form::Cos3, {1.44766, 1.69452, -3.78234}},   // alkenes
        {"C=", "CM", "CT", "CT", TorsionFunctions::Form::Cos3, {1.44766, 1.69452, -3.78234}},   // alkenes
        {"CM", "CM", "CA", "CA", TorsionFunctions::Form::Cos3, {5.19234, 14.029, -1.19662}},    // styrene
        {"CT", "CM", "CA", "CA", TorsionFunctions::Form::Cos3, {0.85772, -2.2217, 0}},          // 1-methylstyrene
        {"Z", "CM", "X", "Y", TorsionFunctions::Form::Cos3, {0, 125.52, 0}},                    // improper
        {"*", "CM", "CM", "*", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                   // alkene
        {"CT", "CM", "CM", "CT", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // alkene
        {"CT", "CM", "C=", "CT", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // alkene
        {"CT", "CM", "C=", "HC", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // alkene
        {"HC", "CM", "C=", "HC", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // alkene
        {"HC", "CM", "C=", "CT", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // alkene
        {"CT", "CM", "CM", "HC", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // alkene
        {"C", "CM", "CM", "HC", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                  // alkene
        {"HC", "CM", "CM", "HC", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // alkene
        {"HC", "CM", "C=", "C=", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // alkene
        {"Cl", "CM", "CM", "HC", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // chloroalkene
        {"HC", "CM", "CM", "OS", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // vinyl
        {"CT", "CM", "CM", "OS", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // vinyl
        {"HC", "CM", "CM", "OH", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // vinyl
        {"CT", "CM", "CM", "OH", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                 // vinyl
        {"C", "CM", "CM", "N", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                   // conj.
        {"CT", "C", "NC", "CT", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                  // imine
        {"HC", "C", "NC", "HC", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                  // imine
        {"CT", "C", "NC", "O*", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                  // oxime
        {"HC", "C", "NC", "O*", TorsionFunctions::Form::Cos3, {0, 58.576, 0}},                  // oxime
        {"CA", "C!", "C!", "CA", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl
        {"CA", "C!", "C!", "NC", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "C", "CA", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                 // biphenyl
        {"CA", "C!", "NA", "NB", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "NA", "CW", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CW", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CW", "NA", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CW", "OS", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CS", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CM", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CU", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CU", "NB", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CV", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CV", "NB", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CR", "OS", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CR", "NA", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CR", "NB", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CW", "S", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                 // biphenyl-like
        {"CA", "C!", "CR", "S", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                 // biphenyl-like
        {"CA", "C!", "N", "C", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                  // biphenyl-like
        {"CA", "C!", "N", "CM", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                 // biphenyl-like
        {"NC", "C!", "N", "C", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                  // biphenyl-like
        {"NC", "C!", "N", "CM", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                 // biphenyl-like
        {"CS", "CS", "CS", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CS", "CS", "CS", "N^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"OS", "CS", "CS", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"OS", "CS", "CS", "N^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"NA", "CS", "CS", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"NA", "CS", "CS", "N^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CW", "CV", "CS", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CW", "CV", "CS", "N^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"NB", "CV", "CS", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"NB", "CV", "CS", "N^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"N^", "CR", "CS", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"N^", "CR", "CW", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"*", "N", "CU", "*", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                   // biphenyl-like
        {"*", "N", "CW", "*", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                   // biphenyl-like
        {"*", "N", "CS", "*", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                   // biphenyl-like
        {"*", "CM", "CU", "*", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                  // biphenyl-like
        {"*", "CM", "CW", "*", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                  // biphenyl-like
        {"*", "CM", "CS", "*", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                  // biphenyl-like
        {"NC", "C!", "CU", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"NC", "C!", "CU", "N^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CU", "C^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"CA", "C!", "CU", "N^", TorsionFunctions::Form::Cos3, {0, 9.07928, 0}},                // biphenyl-like
        {"Cl", "CM", "CM", "Cl", TorsionFunctions::Form::Cos3, {-6.6944, 58.576, 0}},           // chloroalkene
        {"HC", "CT", "P+", "CT", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // phosphonium
        {"HC", "CT", "CT", "P+", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // "
        {"CT", "CT", "P+", "CT", TorsionFunctions::Form::Cos3, {4.184, -2.092, 2.092}},         // "
        {"CK", "N^", "CT", "OS", TorsionFunctions::Form::Cos3, {13.1043, -6.23834, 11.4809}},   // Ping
        {"CK", "NA", "CT", "CT", TorsionFunctions::Form::Cos3, {11.5311, -3.64845, -15.3971}},  // Ping
        {"C", "NC", "OH", "HO", TorsionFunctions::Form::Cos3, {12.552, 12.552, 0}},             // oxime
        {"C", "NC", "OS", "CT", TorsionFunctions::Form::Cos3, {12.552, 12.552, 0}},             // oxime
        {"CM", "CM", "OS", "CT", TorsionFunctions::Form::Cos3, {-14.644, 12.552, 0}},           // vinyl
        {"C=", "CM", "OS", "CT", TorsionFunctions::Form::Cos3, {-14.644, 12.552, 0}},           // vinyl
        {"CM", "CM", "CT", "F", TorsionFunctions::Form::Cos3, {2.092, 0, 0}},                   // allyl
        {"CM", "CM", "CT", "O*", TorsionFunctions::Form::Cos3, {2.092, 0, 0}},                  // allyl
        {"C=", "CM", "CT", "O*", TorsionFunctions::Form::Cos3, {2.092, 0, 0}},                  // allyl
        {"CM", "C=", "CT", "O*", TorsionFunctions::Form::Cos3, {2.092, 0, 0}},                  // allyl
        {"CM", "CT", "OH", "HO", TorsionFunctions::Form::Cos3, {-3.7656, 0, 0}},                // allyl
        {"CA", "CT", "OH", "HO", TorsionFunctions::Form::Cos3, {-3.7656, 0, 0}},                // benzyl
        {"C=", "CT", "OH", "HO", TorsionFunctions::Form::Cos3, {-3.7656, 0, 0}},                // allyl
        {"HC", "CT", "SY", "OY", TorsionFunctions::Form::Cos3, {0, 0, 1.4644}},                 // sulfone
        {"HC", "CT", "SY", "CT", TorsionFunctions::Form::Cos3, {0, 0, 1.4644}},                 // sulfone
        {"HC", "CT", "SY", "CA", TorsionFunctions::Form::Cos3, {0, 0, 1.4644}},                 // sulfone
        {"CT", "CT", "SY", "CT", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // sulfone
        {"CT", "CT", "SY", "OY", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // sulfone
        {"HC", "CT", "CT", "SY", TorsionFunctions::Form::Cos3, {0, 0, 1.4644}},                 // sulfone
        {"HC", "CT", "N3", "CT", TorsionFunctions::Form::Cos3, {0, 0, 1.26231}},                // 2ary
        {"CT", "CT", "N3", "CT", TorsionFunctions::Form::Cos3, {6.01617, -0.517979, 1.10416}},  // 2ary
        {"*", "CT", "CZ", "NZ", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // nitriles
        {"*", "CT", "NZ", "CZ", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // isonitriles
        {"HC", "CT", "NZ", "CZ", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // isonitriles
        {"*", "CA", "NZ", "CZ", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // isonitriles
        {"CA", "CA", "NZ", "CZ", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // isonitriles
        {"*", "CZ", "CZ", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // alkynes
        {"*", "CT", "CZ", "CZ", TorsionFunctions::Form::Cos3, {0, 0, 0}},                       // alkynes
        {"CT", "CT", "CZ", "CZ", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // alkynes
        {"HC", "CT", "CZ", "CZ", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // alkynes
        {"HC", "CZ", "CZ", "CT", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // alkynes
        {"CT", "CZ", "CZ", "CT", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // alkynes
        {"CA", "CA", "SY", "CT", TorsionFunctions::Form::Cos3, {0, -3.7656, 0}},                // sulfone
        {"CR", "NA", "CT", "OS", TorsionFunctions::Form::Cos3, {-6.276, -6.276, 0}},            // imidazoles,
        {"CR", "N^", "CT", "OS", TorsionFunctions::Form::Cos3, {-6.276, -6.276, 0}},            // imidazoles,
        {"CK", "NA", "CT", "OS", TorsionFunctions::Form::Cos3, {-6.276, -6.276, 0}},            // imidazoles,
        {"CK", "N^", "CT", "OS", TorsionFunctions::Form::Cos3, {-6.276, -6.276, 0}},            // imidazoles,
        {"C^", "NA", "CT", "OS", TorsionFunctions::Form::Cos3, {6.276, -6.276, 0}},             // imidazoles,
        {"C^", "N^", "CT", "OS", TorsionFunctions::Form::Cos3, {6.276, -6.276, 0}},             // imidazoles,
        {"CR", "NA", "CT", "CT", TorsionFunctions::Form::Cos3, {-4.184, -1.4644, 0}},           // imidazoles,
        {"CR", "N^", "CT", "CT", TorsionFunctions::Form::Cos3, {-4.184, -1.4644, 0}},           // imidazoles,
        {"CK", "NA", "CT", "CT", TorsionFunctions::Form::Cos3, {-4.184, -1.4644, 0}},           // imidazoles,
        {"CK", "N^", "CT", "CT", TorsionFunctions::Form::Cos3, {-4.184, -1.4644, 0}},           // imidazoles,
        {"C^", "NA", "CT", "CT", TorsionFunctions::Form::Cos3, {4.184, -1.4644, 0}},            // imidazoles,
        {"C^", "N^", "CT", "CT", TorsionFunctions::Form::Cos3, {4.184, -1.4644, 0}},            // imidazoles,
        {"N^", "CT", "OS", "CT", TorsionFunctions::Form::Cos3, {-2.092, -6.276, 4.184}},        // imidazoles,
        {"C^", "NA", "CT", "OS", TorsionFunctions::Form::Cos3, {0, -7.84918, 0}},               // Ping
        {"C^", "NA", "CT", "CT", TorsionFunctions::Form::Cos3, {0, -2.40998, 0}},               // Ping
        {"*", "CY", "CY", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // small
        {"*", "CY", "Ndoll", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                     // small
        {"*", "CY", "Cdoll", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                     // small
        {"CY", "CY", "Cdoll", "Ndoll", TorsionFunctions::Form::Cos3, {0, 0, 0}},                // small
        {"CY", "CY", "Cdoll", "O", TorsionFunctions::Form::Cos3, {0, 0, 0}},                    // small
        {"*", "CY", "S", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                         // small
        {"CY", "CY", "Ndoll", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                    // small
        {"HC", "CY", "Ndoll", "C^", TorsionFunctions::Form::Cos3, {0, 0, 0}},                   // small
        {"HC", "CY", "Cdoll", "O", TorsionFunctions::Form::Cos3, {0, 0, 0}},                    // small
        {"HC", "CY", "Cdoll", "Ndoll", TorsionFunctions::Form::Cos3, {0, 0, 0}},                // small
        {"N", "CY", "CY", "*", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // small
        {"CY", "CY", "N", "H", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // small
        {"Cdoll", "CY", "N", "H", TorsionFunctions::Form::Cos3, {0, 0, 0}},                     // small
        {"HC", "CY", "N", "C", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // small
        {"HC", "CY", "N", "H", TorsionFunctions::Form::Cos3, {0, 0, 0}},                        // small
        {"C^", "Ndoll", "CT", "C^", TorsionFunctions::Form::Cos3, {0, 0, 0}},                   // small
        {"C^", "Ndoll", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0}},                   // small
        {"CY", "Cdoll", "Ndoll", "CT", TorsionFunctions::Form::Cos3, {9.6232, 25.4764, 0}},     // small
        {"CY", "Cdoll", "Ndoll", "CY", TorsionFunctions::Form::Cos3, {9.6232, 25.4764, 0}},     // small
        {"CY", "N", "CT", "CT", TorsionFunctions::Form::Cos3, {9.6232, 25.4764, 0}},            // small
        {"CY", "Cdoll", "Ndoll", "CA", TorsionFunctions::Form::Cos3, {9.6232, 25.4764, 0}},     // small
        {"O", "Cdoll", "Ndoll", "H", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},            // small
        {"CY", "Cdoll", "Ndoll", "H", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},           // small
        {"O", "Cdoll", "Ndoll", "CT", TorsionFunctions::Form::Cos3, {0, 25.4764, 0}},           // small
        {"O", "Cdoll", "Ndoll", "CA", TorsionFunctions::Form::Cos3, {0, 25.4764, 0}},           // small
        {"O", "C", "N", "CY", TorsionFunctions::Form::Cos3, {0, 25.4764, 0}},                   // small
        {"C", "N", "CY", "CY", TorsionFunctions::Form::Cos3, {-5.84086, -1.78657, 0}},          // small
        {"C", "N", "CY", "Cdoll", TorsionFunctions::Form::Cos3, {-5.84086, -1.78657, 0}},       // small
        {"O", "Cdoll", "Ndoll", "CY", TorsionFunctions::Form::Cos3, {0, 83.68, 0}},             // small
        {"HC", "CY", "CY", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // small
        {"HC", "CY", "CY", "CY", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // small
        {"HC", "CY", "CY", "CT", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // small
        {"CT", "CY", "CY", "CT", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // small
        {"CY", "CY", "CY", "CY", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // small
        {"HC", "CT", "CY", "CY", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // small
        {"HC", "CT", "CM", "CM", TorsionFunctions::Form::Cos3, {0, 0, -1.55645}},               // alkenes
        {"HC", "CT", "CM", "C=", TorsionFunctions::Form::Cos3, {0, 0, -1.55645}},               // alkenes
        {"HC", "CT", "C=", "C=", TorsionFunctions::Form::Cos3, {0, 0, -1.55645}},               // alkenes
        {"HC", "CT", "C=", "CM", TorsionFunctions::Form::Cos3, {0, 0, -1.55645}},               // alkenes
        {"HC", "CM", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 1.33051}},                // alkene
        {"HC", "CT", "CT", "CM", TorsionFunctions::Form::Cos3, {0, 0, 1.53134}},                // alkene
        {"HC", "CT", "CT", "CZ", TorsionFunctions::Form::Cos3, {0, 0, 1.53134}},                // alkyne,
        {"CT", "CT", "CT", "CZ", TorsionFunctions::Form::Cos3, {0, -2.7196, 0}},                // alkyne,
        {"HC", "CT", "CM", "CT", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // alkenes
        {"CT", "NT", "OS", "CT", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // generic
        {"CT", "NT", "OH", "HO", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // generic
        {"H", "NT", "OS", "CT", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                  // generic
        {"H", "NT", "OH", "HO", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                  // generic
        {"CT", "NT", "NT", "CT", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                 // generic
        {"CT", "NT", "NT", "H", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                  // generic
        {"H", "NT", "NT", "H", TorsionFunctions::Form::Cos3, {0, 0, 1.2552}},                   // generic
        {"OS", "C", "N", "CT", TorsionFunctions::Form::Cos3, {9.6232, 25.4764, 0}},             // carbamates
        {"OH", "C", "N", "CT", TorsionFunctions::Form::Cos3, {9.6232, 25.4764, 0}},             // carbamates
        {"OS", "C", "N", "H", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},                   // carbamates
        {"OH", "C", "N", "H", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},                   // carbamates
        {"N", "C", "OS", "CT", TorsionFunctions::Form::Cos3, {-8.368, 20.92, 0}},               // carbamates
        {"N", "C", "OH", "HO", TorsionFunctions::Form::Cos3, {-8.368, 20.92, 0}},               // carbamates
        {"HC", "NC", "NZ", "NZ", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // azides
        {"CT", "NC", "NZ", "NZ", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // azides
        {"CT", "CT", "NC", "NZ", TorsionFunctions::Form::Cos3, {0, 0, 0}},                      // azides
        /* { "HC",	"CT",	"C",	"Seq",	TorsionFunctions::Form::Cos3,	0,	0,	0,	0 }, */ // thiocarbonyl
        /* { "Seq",	"C",	"N",	"H",	TorsionFunctions::Form::Cos3,	4.184,	20.92,	0,	0 }, */ // thioamides
        /* { "Seq",	"C",	"N",	"CT",	TorsionFunctions::Form::Cos3,	0,	27.196,	0,	0 }, */     // thioamides
        /* { "Seq",	"C",	"N",	"CA",	TorsionFunctions::Form::Cos3,	0,	27.196,	0,	0 }, */     // thioamides
        /* { "Seq",	"C",	"N",	"CM",	TorsionFunctions::Form::Cos3,	0,	27.196,	0,	0 }, */     // thioamides
        /* { "Seq",	"C",	"OS",	"CT",	TorsionFunctions::Form::Cos3,	0,	20.92,	0,	0 }, */     // thioesters
        /* { "Seq",	"C",	"OH",	"HO",	TorsionFunctions::Form::Cos3,	0,	23.012,	0,	0 }, */     // thioacids
        {"CT", "S", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 2.5104, 0}},                          // thioanisole
        {"O", "C", "C", "CT", TorsionFunctions::Form::Cos3, {0, 2.092, 0}},                             // dicarbonyls
        {"O", "C", "C", "HC", TorsionFunctions::Form::Cos3, {0, 0.8368, 0}},                            // dicarbonyls
        {"HC", "C", "C", "CT", TorsionFunctions::Form::Cos3, {3.3472, -3.17984, 0}},                    // dicarbonyls
        {"HC", "C", "C", "HC", TorsionFunctions::Form::Cos3, {3.3472, 0, 0}},                           // dicarbonyls
        {"CT", "C", "C", "CT", TorsionFunctions::Form::Cos3, {2.9288, -6.276, 0}},                      // dicarbonyls
        {"C", "C", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0.35564}},                          // dicarbonyls
        {"N", "C", "C", "O", TorsionFunctions::Form::Cos3, {0, 0, 0}},                                  // dicarbonyls
        {"N", "C", "C", "HC", TorsionFunctions::Form::Cos3, {-3.7656, 1.2552, 0}},                      // dicarbonyls
        {"N", "C", "C", "CT", TorsionFunctions::Form::Cos3, {-2.092, 0.8368, 0}},                       // dicarbonyls
        {"C", "C", "N", "CT", TorsionFunctions::Form::Cos3, {1.6736, 20.5016, 0}},                      // dicarbonyls
        {"H", "N", "C", "C", TorsionFunctions::Form::Cos3, {0, 20.5016, 0}},                            // dicarbonyls
        {"O", "C", "C", "O", TorsionFunctions::Form::Cos3, {6.6944, 13.3888, 0}},                       // dicarbonyls
        {"CT", "C", "N", "OH", TorsionFunctions::Form::Cos3, {19.0037, 27.627, 4.37228}},               // hydroxamic
        {"O", "C", "N", "OH", TorsionFunctions::Form::Cos3, {0, 27.627, 0}},                            // hydroxamic
        {"C", "N", "OH", "HO", TorsionFunctions::Form::Cos3, {23.0915, -28.0328, 2.4309}},              // hydroxamic
        {"H", "N", "OH", "HO", TorsionFunctions::Form::Cos3, {11.3888, -21.5643, 0}},                   // hydroxamic
        {"F", "CT", "CA", "CA", TorsionFunctions::Form::Cos3, {0, 1.8828, 0}},                          // fluoromethyl
        {"F", "CT", "CW", "*", TorsionFunctions::Form::Cos3, {0, 1.8828, 0}},                           // fluoromethyl
        {"F", "CT", "CS", "*", TorsionFunctions::Form::Cos3, {0, 1.8828, 0}},                           // fluoromethyl
        {"Cl", "CT", "CA", "CA", TorsionFunctions::Form::Cos3, {0, -1.6736, 0}},                        // chloromethyl
        {"Cl", "CT", "CW", "*", TorsionFunctions::Form::Cos3, {0, -1.6736, 0}},                         // chloromethyl
        {"Cl", "CT", "CS", "*", TorsionFunctions::Form::Cos3, {0, -1.6736, 0}},                         // chloromethyl
        {"Cl", "CT", "C", "O", TorsionFunctions::Form::Cos3, {-2.7196, 0, 0}},                          // 2-chloroamide
        {"Cl", "CT", "C", "N", TorsionFunctions::Form::Cos3, {2.7196, 0, 0}},                           // 2-chloroamide
        {"CT", "CT", "CT", "Si", TorsionFunctions::Form::Cos3, {0, 0, 0}},                              // silane
        {"CT", "CT", "Si", "CT", TorsionFunctions::Form::Cos3, {0, 0, 0}},                              // silane
        {"CT", "CT", "Si", "H", TorsionFunctions::Form::Cos3, {0, 0, 0.6276}},                          // silane
        {"HC", "CT", "Si", "H", TorsionFunctions::Form::Cos3, {0, 0, 0.75312}},                         // silane
        {"Si", "CT", "CT", "Si", TorsionFunctions::Form::Cos3, {21.7568, -2.092, 0}},                   // silane
        {"Si", "CT", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0.6276}},                         // silane
        {"CT", "Si", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0.75312}},                        // silane
        {"OS", "Si", "CT", "HC", TorsionFunctions::Form::Cos3, {0, 0, 0}},                              // tentative
        {"OS", "Si", "OS", "Si", TorsionFunctions::Form::Cos3, {0, 0, 0}},                              // tentative
        {"Si", "OS", "Si", "CT", TorsionFunctions::Form::Cos3, {0, 0, 0}},                              // tentative
        {"CT", "CT", "CT", "CT", TorsionFunctions::Form::Cos4, {27.7064, 3.96643, -5.80739, -8.86171}}, // perfluoroalkane
        {"N2", "CA", "CA", "CA", TorsionFunctions::Form::Cos4, {0, 6.77808, 0, -1.84096}}               // benzamidine
    };

    return Forcefield::termMatch_(torsionTerms, i, j, k, l);
}

// Return improper term for the supplied atom type quartet (if it exists)
OptionalReferenceWrapper<const ForcefieldImproperTerm>
OPLSAA2005BaseForcefield::getImproperTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k,
                                          const ForcefieldAtomType &l) const
{
    static std::vector<ForcefieldImproperTerm> improperTerms = {};
    return Forcefield::termMatch_(improperTerms, i, j, k, l);
}
