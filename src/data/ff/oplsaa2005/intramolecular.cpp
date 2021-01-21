// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
        {"OW", "HW", SpeciesBond::HarmonicForm, {5020.8, 0.9572}},
        /* { "OW",	"LP",	SpeciesBond::HarmonicForm,	7531.2,	0.175 }, */
        {"C^", "HC", SpeciesBond::HarmonicForm, {2845.12, 1.08}},
        {"C", "C2", SpeciesBond::HarmonicForm, {2652.66, 1.522}},
        {"C", "C3", SpeciesBond::HarmonicForm, {2652.66, 1.522}},
        {"C", "CA", SpeciesBond::HarmonicForm, {3347.2, 1.49}},
        {"C", "CB", SpeciesBond::HarmonicForm, {3740.5, 1.419}},
        {"C", "CD", SpeciesBond::HarmonicForm, {3924.59, 1.4}},
        {"C", "CH", SpeciesBond::HarmonicForm, {2652.66, 1.522}},
        /* { "C",	"CJ",	SpeciesBond::HarmonicForm,	3430.88,	1.444 }, */
        {"C", "CM", SpeciesBond::HarmonicForm, {3430.88, 1.444}},
        {"C", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.522}},
        {"C$", "CY", SpeciesBond::HarmonicForm, {2652.66, 1.522}},
        {"C$", "N$", SpeciesBond::HarmonicForm, {4100.32, 1.335}},
        {"C", "N", SpeciesBond::HarmonicForm, {4100.32, 1.335}},
        {"C", "NM", SpeciesBond::HarmonicForm, {4100.32, 1.335}},
        {"C", "N^", SpeciesBond::HarmonicForm, {3548.03, 1.383}},
        {"C", "NA", SpeciesBond::HarmonicForm, {3497.82, 1.388}},
        {"C", "NC", SpeciesBond::HarmonicForm, {3824.18, 1.358}},
        /* { "C",	"Neq",	SpeciesBond::HarmonicForm,	3824.18,	1.29 }, */
        {"C", "O", SpeciesBond::HarmonicForm, {4769.76, 1.229}},
        {"C$", "O", SpeciesBond::HarmonicForm, {4769.76, 1.229}},
        {"C", "O2", SpeciesBond::HarmonicForm, {5489.41, 1.25}},
        {"C", "OH", SpeciesBond::HarmonicForm, {3765.6, 1.364}},
        /* { "C",	"Seq",	SpeciesBond::HarmonicForm,	3347.2,		1.64 }, */
        {"NO", "ON", SpeciesBond::HarmonicForm, {4602.4, 1.225}},
        {"CT", "NO", SpeciesBond::HarmonicForm, {3138, 1.49}},
        {"CA", "NO", SpeciesBond::HarmonicForm, {3347.2, 1.46}},
        {"CA", "OH", SpeciesBond::HarmonicForm, {3765.6, 1.364}},
        {"CA", "OS", SpeciesBond::HarmonicForm, {3765.6, 1.364}},
        {"CB", "OS", SpeciesBond::HarmonicForm, {2845.12, 1.36}},
        {"CM", "OS", SpeciesBond::HarmonicForm, {3765.6, 1.37}},
        {"CM", "OH", SpeciesBond::HarmonicForm, {3765.6, 1.37}},
        {"C", "OS", SpeciesBond::HarmonicForm, {1790.75, 1.327}},
        {"C^", "C2", SpeciesBond::HarmonicForm, {2652.66, 1.495}},
        {"C^", "CB", SpeciesBond::HarmonicForm, {3246.78, 1.459}},
        /* { "C^",	"CG",	SpeciesBond::HarmonicForm,	4568.93,	1.352 }, */
        /* { "C^",	"CC",	SpeciesBond::HarmonicForm,	4568.93,	1.352 }, */
        {"C^", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.495}},
        {"CS", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.495}},
        {"C^", "CW", SpeciesBond::HarmonicForm, {4568.93, 1.352}},
        {"CA", "CW", SpeciesBond::HarmonicForm, {4568.93, 1.367}},
        {"CS", "CW", SpeciesBond::HarmonicForm, {4568.93, 1.367}},
        {"CS", "CS", SpeciesBond::HarmonicForm, {3924.59, 1.424}},
        {"CS", "CB", SpeciesBond::HarmonicForm, {3924.59, 1.424}},
        {"CS", "HA", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CU", "NB", SpeciesBond::HarmonicForm, {3430.88, 1.32}},
        {"CU", "CA", SpeciesBond::HarmonicForm, {3924.59, 1.421}},
        {"CU", "HA", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"NA", "NB", SpeciesBond::HarmonicForm, {3347.2, 1.349}},
        {"NB", "NB", SpeciesBond::HarmonicForm, {3347.2, 1.28}},
        {"OS", "NB", SpeciesBond::HarmonicForm, {3866.02, 1.399}},
        {"OS", "CR", SpeciesBond::HarmonicForm, {3866.02, 1.357}},
        {"C2", "C2", SpeciesBond::HarmonicForm, {2175.68, 1.526}},
        {"C2", "C3", SpeciesBond::HarmonicForm, {2175.68, 1.526}},
        {"C3", "C3", SpeciesBond::HarmonicForm, {2175.68, 1.526}},
        {"C2", "CA", SpeciesBond::HarmonicForm, {2652.66, 1.51}},
        {"C2", "CD", SpeciesBond::HarmonicForm, {2652.66, 1.51}},
        /* { "C2",	"CC",	SpeciesBond::HarmonicForm,	2652.66,	1.504 }, */
        {"C2", "CH", SpeciesBond::HarmonicForm, {2175.68, 1.526}},
        {"C2", "N", SpeciesBond::HarmonicForm, {2820.02, 1.449}},
        {"C2", "N2", SpeciesBond::HarmonicForm, {2820.02, 1.463}},
        {"C2", "N3", SpeciesBond::HarmonicForm, {3071.06, 1.471}},
        {"CH", "NT", SpeciesBond::HarmonicForm, {3196.58, 1.448}},
        {"C2", "NT", SpeciesBond::HarmonicForm, {3196.58, 1.448}},
        {"C3", "NT", SpeciesBond::HarmonicForm, {3196.58, 1.448}},
        {"CT", "NT", SpeciesBond::HarmonicForm, {3196.58, 1.448}},
        {"NT", "NT", SpeciesBond::HarmonicForm, {2928.8, 1.445}},
        {"C2", "OH", SpeciesBond::HarmonicForm, {3230.05, 1.425}},
        {"C2", "OS", SpeciesBond::HarmonicForm, {2677.76, 1.425}},
        {"CO", "OS", SpeciesBond::HarmonicForm, {2677.76, 1.38}},
        {"CO", "C2", SpeciesBond::HarmonicForm, {2175.68, 1.526}},
        {"CO", "C3", SpeciesBond::HarmonicForm, {2175.68, 1.526}},
        {"CW", "OS", SpeciesBond::HarmonicForm, {2845.12, 1.36}},
        {"C2", "S", SpeciesBond::HarmonicForm, {1857.7, 1.81}},
        {"C2", "SH", SpeciesBond::HarmonicForm, {1857.7, 1.81}},
        {"C3", "CH", SpeciesBond::HarmonicForm, {2175.68, 1.526}},
        {"C3", "CM", SpeciesBond::HarmonicForm, {2652.66, 1.51}},
        {"C3", "N", SpeciesBond::HarmonicForm, {2820.02, 1.449}},
        {"C3", "N^", SpeciesBond::HarmonicForm, {2820.02, 1.475}},
        {"C3", "N2", SpeciesBond::HarmonicForm, {2820.02, 1.463}},
        {"C3", "N3", SpeciesBond::HarmonicForm, {3071.06, 1.471}},
        {"C3", "OH", SpeciesBond::HarmonicForm, {3230.05, 1.425}},
        {"C3", "OS", SpeciesBond::HarmonicForm, {2677.76, 1.425}},
        {"C3", "S", SpeciesBond::HarmonicForm, {1857.7, 1.81}},
        {"C3", "SH", SpeciesBond::HarmonicForm, {1857.7, 1.81}},
        {"CA", "CA", SpeciesBond::HarmonicForm, {3924.59, 1.4}},
        {"CA", "C!", SpeciesBond::HarmonicForm, {3924.59, 1.4}},
        {"C!", "C!", SpeciesBond::HarmonicForm, {3221.68, 1.46}},
        {"C!", "CS", SpeciesBond::HarmonicForm, {3221.68, 1.46}},
        {"C!", "CU", SpeciesBond::HarmonicForm, {3221.68, 1.46}},
        {"C!", "CV", SpeciesBond::HarmonicForm, {3221.68, 1.46}},
        {"C!", "CW", SpeciesBond::HarmonicForm, {3221.68, 1.46}},
        {"C!", "CR", SpeciesBond::HarmonicForm, {3221.68, 1.46}},
        {"C!", "C", SpeciesBond::HarmonicForm, {3221.68, 1.46}},
        {"C!", "CM", SpeciesBond::HarmonicForm, {3221.68, 1.46}},
        {"C!", "NA", SpeciesBond::HarmonicForm, {3221.68, 1.44}},
        {"CA", "CB", SpeciesBond::HarmonicForm, {3924.59, 1.404}},
        {"CA", "CD", SpeciesBond::HarmonicForm, {3924.59, 1.4}},
        /* { "CA",	"CJ",	SpeciesBond::HarmonicForm,	3573.14,	1.433 }, */
        {"CA", "CM", SpeciesBond::HarmonicForm, {3573.14, 1.433}},
        {"CA", "CN", SpeciesBond::HarmonicForm, {3924.59, 1.4}},
        {"CA", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.51}},
        {"CA", "CY", SpeciesBond::HarmonicForm, {2652.66, 1.49}},
        {"CW", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.504}},
        {"CV", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.504}},
        {"CX", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.504}},
        {"CX", "CX", SpeciesBond::HarmonicForm, {4351.36, 1.37}},
        {"CX", "NA", SpeciesBond::HarmonicForm, {3573.14, 1.381}},
        {"CX", "HA", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CA", "NY", SpeciesBond::HarmonicForm, {3196.58, 1.385}},
        {"CA", "NZ", SpeciesBond::HarmonicForm, {4.184, 1.261}},
        {"NY", "H", SpeciesBond::HarmonicForm, {3631.71, 1.01}},
        {"NZ", "H", SpeciesBond::HarmonicForm, {3631.71, 1.01}},
        {"CT", "NY", SpeciesBond::HarmonicForm, {3196.58, 1.448}},
        {"CA", "N2", SpeciesBond::HarmonicForm, {4025.01, 1.34}},
        {"CQ", "N2", SpeciesBond::HarmonicForm, {4025.01, 1.34}},
        {"CR", "N2", SpeciesBond::HarmonicForm, {4025.01, 1.34}},
        {"CA", "NT", SpeciesBond::HarmonicForm, {4025.01, 1.34}},
        {"CA", "NA", SpeciesBond::HarmonicForm, {3573.14, 1.381}},
        {"CQ", "N", SpeciesBond::HarmonicForm, {3573.14, 1.381}},
        {"CA", "NC", SpeciesBond::HarmonicForm, {4041.74, 1.339}},
        {"C!", "NC", SpeciesBond::HarmonicForm, {4041.74, 1.339}},
        {"NC", "NC", SpeciesBond::HarmonicForm, {4.184, 1.32}},
        {"NC", "NZ", SpeciesBond::HarmonicForm, {4602.4, 1.24}},
        {"NZ", "NZ", SpeciesBond::HarmonicForm, {4602.4, 1.12}},
        {"CT", "NZ", SpeciesBond::HarmonicForm, {3263.52, 1.43}},
        {"CA", "NZ", SpeciesBond::HarmonicForm, {3347.2, 1.41}},
        {"CA", "HA", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CB", "CB", SpeciesBond::HarmonicForm, {4351.36, 1.37}},
        {"CR", "CS", SpeciesBond::HarmonicForm, {4351.36, 1.37}},
        {"CV", "CW", SpeciesBond::HarmonicForm, {4351.36, 1.37}},
        {"CU", "CW", SpeciesBond::HarmonicForm, {4351.36, 1.37}},
        {"CB", "CD", SpeciesBond::HarmonicForm, {3924.59, 1.4}},
        {"CB", "CN", SpeciesBond::HarmonicForm, {3740.5, 1.419}},
        {"CB", "N^", SpeciesBond::HarmonicForm, {3648.45, 1.374}},
        {"CB", "NA", SpeciesBond::HarmonicForm, {3648.45, 1.374}},
        {"CB", "NB", SpeciesBond::HarmonicForm, {3464.35, 1.391}},
        {"CB", "NC", SpeciesBond::HarmonicForm, {3857.65, 1.354}},
        {"CR", "NC", SpeciesBond::HarmonicForm, {3857.65, 1.354}},
        /* { "CC",	"CF",	SpeciesBond::HarmonicForm,	4284.42,	1.375 }, */
        /* { "CC",	"CG",	SpeciesBond::HarmonicForm,	4334.62,	1.371 }, */
        /* { "CC",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.504 }, */
        /* { "CC",	"CV",	SpeciesBond::HarmonicForm,	4284.42,	1.375 }, */
        {"CW", "CW", SpeciesBond::HarmonicForm, {4284.42, 1.375}},
        /* { "CC",	"CW",	SpeciesBond::HarmonicForm,	4334.62,	1.371 }, */
        /* { "CC",	"NA",	SpeciesBond::HarmonicForm,	3531.3,	1.385 }, */
        /* { "CC",	"NB",	SpeciesBond::HarmonicForm,	3430.88,	1.394 }, */
        {"CD", "CD", SpeciesBond::HarmonicForm, {3924.59, 1.4}},
        {"CD", "CN", SpeciesBond::HarmonicForm, {3924.59, 1.4}},
        /* { "CA",	"CC",	SpeciesBond::HarmonicForm,	3924.59,	1.4 }, */
        /* { "CD",	"CC",	SpeciesBond::HarmonicForm,	3924.59,	1.4 }, */
        /* { "CE",	"N^",	SpeciesBond::HarmonicForm,	3681.92,	1.371 }, */
        /* { "CE",	"NB",	SpeciesBond::HarmonicForm,	4426.67,	1.304 }, */
        /* { "CF",	"NB",	SpeciesBond::HarmonicForm,	3430.88,	1.394 }, */
        /* { "CG",	"NA",	SpeciesBond::HarmonicForm,	3573.14,	1.381 }, */
        {"CH", "CH", SpeciesBond::HarmonicForm, {2175.68, 1.526}},
        {"CH", "N", SpeciesBond::HarmonicForm, {2820.02, 1.449}},
        {"CH", "N^", SpeciesBond::HarmonicForm, {2820.02, 1.475}},
        {"CH", "OH", SpeciesBond::HarmonicForm, {3230.05, 1.425}},
        {"CH", "OS", SpeciesBond::HarmonicForm, {2677.76, 1.425}},
        /* { "CI",	"NC",	SpeciesBond::HarmonicForm,	4200.74,	1.324 }, */
        /* { "CJ",	"CJ",	SpeciesBond::HarmonicForm,	4594.03,	1.35 }, */
        /* { "CJ",	"CM",	SpeciesBond::HarmonicForm,	4594.03,	1.35 }, */
        /* { "CJ",	"N^",	SpeciesBond::HarmonicForm,	3748.86,	1.365 }, */
        {"CK", "HA", SpeciesBond::HarmonicForm, {2845.12, 1.08}},
        {"CK", "H5", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CK", "N^", SpeciesBond::HarmonicForm, {3681.92, 1.371}},
        {"CK", "NA", SpeciesBond::HarmonicForm, {3681.92, 1.371}},
        {"CK", "NB", SpeciesBond::HarmonicForm, {4426.67, 1.304}},
        {"CM", "CM", SpeciesBond::HarmonicForm, {4594.03, 1.34}},
        {"CM", "C=", SpeciesBond::HarmonicForm, {4594.03, 1.34}},
        {"CW", "C=", SpeciesBond::HarmonicForm, {4594.03, 1.365}},
        {"C=", "C=", SpeciesBond::HarmonicForm, {3221.68, 1.46}},
        /* { "C=",	"Neq",	SpeciesBond::HarmonicForm,	3472.72,	1.428 }, */
        {"C", "C", SpeciesBond::HarmonicForm, {2928.8, 1.51}},
        {"C=", "C", SpeciesBond::HarmonicForm, {3221.68, 1.46}},
        {"CT", "C+", SpeciesBond::HarmonicForm, {4458.47, 1.46}},
        {"C+", "HC", SpeciesBond::HarmonicForm, {4458.47, 1.084}},
        {"CM", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.51}},
        {"C=", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.51}},
        {"CM", "HC", SpeciesBond::HarmonicForm, {2845.12, 1.08}},
        {"CM", "H4", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"C=", "HC", SpeciesBond::HarmonicForm, {2845.12, 1.08}},
        {"HC", "C", SpeciesBond::HarmonicForm, {2845.12, 1.09}},
        {"CT", "CZ", SpeciesBond::HarmonicForm, {3263.52, 1.47}},
        {"CA", "CZ", SpeciesBond::HarmonicForm, {3347.2, 1.451}},
        {"CY", "CZ", SpeciesBond::HarmonicForm, {3347.2, 1.451}},
        {"CZ", "NZ", SpeciesBond::HarmonicForm, {5439.2, 1.157}},
        {"CZ", "CZ", SpeciesBond::HarmonicForm, {9623.2, 1.21}},
        {"HC", "CZ", SpeciesBond::HarmonicForm, {3514.56, 1.08}},
        {"CM", "N^", SpeciesBond::HarmonicForm, {3748.86, 1.365}},
        {"CM", "NA", SpeciesBond::HarmonicForm, {3748.86, 1.365}},
        {"CN", "NA", SpeciesBond::HarmonicForm, {3581.5, 1.38}},
        /* { "CP",	"NA",	SpeciesBond::HarmonicForm,	3991.54,	1.343 }, */
        /* { "CP",	"NB",	SpeciesBond::HarmonicForm,	4083.58,	1.335 }, */
        {"CQ", "HA", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CQ", "H5", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CQ", "NC", SpeciesBond::HarmonicForm, {4200.74, 1.324}},
        {"CR", "HA", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CR", "H5", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CR", "NA", SpeciesBond::HarmonicForm, {3991.54, 1.343}},
        {"CR", "NB", SpeciesBond::HarmonicForm, {4083.58, 1.335}},
        {"CT", "CT", SpeciesBond::HarmonicForm, {2242.62, 1.529}},
        {"CT", "HC", SpeciesBond::HarmonicForm, {2845.12, 1.09}},
        {"DM", "Cl", SpeciesBond::HarmonicForm, {2510.4, 0.3}},
        {"DM", "Br", SpeciesBond::HarmonicForm, {2510.4, 0.3}},
        {"DM", "F", SpeciesBond::HarmonicForm, {2510.4, 0.3}},
        {"DM", "HC", SpeciesBond::HarmonicForm, {2845.12, 0.3}},
        {"DM", "HO", SpeciesBond::HarmonicForm, {2845.12, 0.1}},
        {"DM", "HS", SpeciesBond::HarmonicForm, {2845.12, 0.1}},
        {"DM", "HA", SpeciesBond::HarmonicForm, {2845.12, 0.3}},
        {"DM", "CA", SpeciesBond::HarmonicForm, {3071.06, 0.3}},
        {"DM", "NC", SpeciesBond::HarmonicForm, {3071.06, 0.3}},
        {"DM", "NB", SpeciesBond::HarmonicForm, {3071.06, 0.3}},
        {"DM", "CT", SpeciesBond::HarmonicForm, {2845.12, 0.3}},
        {"DM", "SZ", SpeciesBond::HarmonicForm, {2845.12, 0.5}},
        {"DM", "S", SpeciesBond::HarmonicForm, {2845.12, 0.5}},
        {"DM", "OS", SpeciesBond::HarmonicForm, {2845.12, 0.3}},
        {"DM", "OY", SpeciesBond::HarmonicForm, {2845.12, 0.3}},
        {"DM", "OH", SpeciesBond::HarmonicForm, {2845.12, 0.3}},
        {"CT", "N", SpeciesBond::HarmonicForm, {2820.02, 1.449}},
        {"CT", "NM", SpeciesBond::HarmonicForm, {2820.02, 1.449}},
        {"CT", "NC", SpeciesBond::HarmonicForm, {2820.02, 1.449}},
        {"CY", "N$", SpeciesBond::HarmonicForm, {2820.02, 1.449}},
        {"CT", "N$", SpeciesBond::HarmonicForm, {2820.02, 1.449}},
        {"CY", "N", SpeciesBond::HarmonicForm, {2820.02, 1.449}},
        {"CT", "N^", SpeciesBond::HarmonicForm, {2820.02, 1.475}},
        {"CO", "N^", SpeciesBond::HarmonicForm, {2820.02, 1.475}},
        {"CT", "NA", SpeciesBond::HarmonicForm, {2820.02, 1.475}},
        {"CT", "N2", SpeciesBond::HarmonicForm, {2820.02, 1.463}},
        {"CT", "N3", SpeciesBond::HarmonicForm, {3071.06, 1.471}},
        {"CA", "N3", SpeciesBond::HarmonicForm, {3347.2, 1.45}},
        {"CT", "OH", SpeciesBond::HarmonicForm, {2677.76, 1.41}},
        {"NT", "OH", SpeciesBond::HarmonicForm, {2677.76, 1.45}},
        {"NT", "OS", SpeciesBond::HarmonicForm, {2677.76, 1.45}},
        {"N", "OH", SpeciesBond::HarmonicForm, {3347.2, 1.38}},
        {"CT", "OS", SpeciesBond::HarmonicForm, {2677.76, 1.41}},
        {"OS", "OS", SpeciesBond::HarmonicForm, {2092, 1.47}},
        {"OS", "OH", SpeciesBond::HarmonicForm, {2092, 1.47}},
        {"OS", "Cl", SpeciesBond::HarmonicForm, {1673.6, 1.69}},
        /* ####  0.0  0 */
        {"Si", "CT", SpeciesBond::HarmonicForm, {1564.82, 1.86}},
        {"Si", "Si", SpeciesBond::HarmonicForm, {786.592, 2.32}},
        {"Si", "HC", SpeciesBond::HarmonicForm, {1389.09, 1.48}},
        {"Si", "H", SpeciesBond::HarmonicForm, {1389.09, 1.48}},
        {"Si", "F", SpeciesBond::HarmonicForm, {3857.65, 1.57}},
        {"Si", "Cl", SpeciesBond::HarmonicForm, {1866.06, 2.02}},
        {"Si", "Br", SpeciesBond::HarmonicForm, {1263.57, 2.19}},
        {"Si", "I", SpeciesBond::HarmonicForm, {903.744, 2.44}},
        {"Si", "OH", SpeciesBond::HarmonicForm, {3129.63, 1.64}},
        {"Si", "OS", SpeciesBond::HarmonicForm, {3129.63, 1.64}},
        {"Si", "P", SpeciesBond::HarmonicForm, {903.744, 2.25}},
        {"Si", "NT", SpeciesBond::HarmonicForm, {2225.89, 1.74}},
        {"Si", "S", SpeciesBond::HarmonicForm, {1204.99, 2.15}},
        /* ####  0.0  0 */
        {"CT", "S", SpeciesBond::HarmonicForm, {1857.7, 1.81}},
        {"CY", "S", SpeciesBond::HarmonicForm, {1857.7, 1.81}},
        {"CR", "S", SpeciesBond::HarmonicForm, {2092, 1.76}},
        {"CW", "S", SpeciesBond::HarmonicForm, {2092, 1.74}},
        {"CA", "SH", SpeciesBond::HarmonicForm, {2092, 1.74}},
        {"CT", "SH", SpeciesBond::HarmonicForm, {1857.7, 1.81}},
        {"CT", "Cl", SpeciesBond::HarmonicForm, {2050.16, 1.781}},
        {"CA", "Cl", SpeciesBond::HarmonicForm, {2510.4, 1.725}},
        {"CM", "Cl", SpeciesBond::HarmonicForm, {2510.4, 1.725}},
        {"C", "Cl", SpeciesBond::HarmonicForm, {2510.4, 1.79}},
        {"CZ", "Cl", SpeciesBond::HarmonicForm, {2761.44, 1.637}},
        {"CT", "Br", SpeciesBond::HarmonicForm, {2050.16, 1.945}},
        {"CA", "Br", SpeciesBond::HarmonicForm, {2510.4, 1.87}},
        {"CM", "Br", SpeciesBond::HarmonicForm, {2510.4, 1.9}},
        {"C", "Br", SpeciesBond::HarmonicForm, {2510.4, 1.98}},
        {"CZ", "Br", SpeciesBond::HarmonicForm, {2761.44, 1.784}},
        {"CA", "I", SpeciesBond::HarmonicForm, {2092, 2.08}},
        {"CT", "I", SpeciesBond::HarmonicForm, {1673.6, 2.19}},
        {"CV", "HA", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CV", "H4", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CV", "NB", SpeciesBond::HarmonicForm, {3430.88, 1.394}},
        {"CW", "NB", SpeciesBond::HarmonicForm, {3430.88, 1.394}},
        {"CW", "H4", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CW", "HA", SpeciesBond::HarmonicForm, {3071.06, 1.08}},
        {"CW", "NA", SpeciesBond::HarmonicForm, {3573.14, 1.381}},
        {"CY", "CY", SpeciesBond::HarmonicForm, {2175.68, 1.52}},
        {"CY", "CT", SpeciesBond::HarmonicForm, {2343.04, 1.51}},
        {"CY", "HC", SpeciesBond::HarmonicForm, {2845.12, 1.088}},
        {"H", "N", SpeciesBond::HarmonicForm, {3631.71, 1.01}},
        {"H", "N3", SpeciesBond::HarmonicForm, {3631.71, 1.01}},
        {"H", "N^", SpeciesBond::HarmonicForm, {3631.71, 1.01}},
        {"H", "N2", SpeciesBond::HarmonicForm, {3631.71, 1.01}},
        {"H", "NA", SpeciesBond::HarmonicForm, {3631.71, 1.01}},
        /* { "H2",	"N",	SpeciesBond::HarmonicForm,	3631.71,	1.01 }, */
        /* { "H2",	"N2",	SpeciesBond::HarmonicForm,	3631.71,	1.01 }, */
        {"H", "NT", SpeciesBond::HarmonicForm, {3631.71, 1.01}},
        {"H3", "N2", SpeciesBond::HarmonicForm, {3631.71, 1.01}},
        {"H3", "N3", SpeciesBond::HarmonicForm, {3631.71, 1.01}},
        {"HO", "OH", SpeciesBond::HarmonicForm, {4627.5, 0.945}},
        /* { """"""",	"460.5",	SpeciesBond::HarmonicForm,	8.03328,	0.0 }, */
        {"HO", "OS", SpeciesBond::HarmonicForm, {4627.5, 0.945}},
        {"HS", "SH", SpeciesBond::HarmonicForm, {2292.83, 1.336}},
        {"O2", "P", SpeciesBond::HarmonicForm, {4393.2, 1.48}},
        {"O", "P", SpeciesBond::HarmonicForm, {4393.2, 1.48}},
        {"OH", "P", SpeciesBond::HarmonicForm, {1924.64, 1.61}},
        {"OS", "P", SpeciesBond::HarmonicForm, {1924.64, 1.61}},
        {"CT", "P", SpeciesBond::HarmonicForm, {1774.02, 1.843}},
        {"CA", "P", SpeciesBond::HarmonicForm, {1840.96, 1.78}},
        {"CT", "P+", SpeciesBond::HarmonicForm, {1774.02, 1.82}},
        {"S", "S", SpeciesBond::HarmonicForm, {1389.09, 2.038}},
        {"C9", "C9", SpeciesBond::HarmonicForm, {4435.04, 1.34}},
        {"C9", "C8", SpeciesBond::HarmonicForm, {4435.04, 1.34}},
        {"C9", "C7", SpeciesBond::HarmonicForm, {4435.04, 1.34}},
        {"C8", "C8", SpeciesBond::HarmonicForm, {4435.04, 1.34}},
        {"C8", "C7", SpeciesBond::HarmonicForm, {4435.04, 1.34}},
        {"C7", "C7", SpeciesBond::HarmonicForm, {4435.04, 1.34}},
        {"C8", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.5}},
        {"C8", "CH", SpeciesBond::HarmonicForm, {2652.66, 1.5}},
        {"C8", "C2", SpeciesBond::HarmonicForm, {2652.66, 1.5}},
        {"C8", "C3", SpeciesBond::HarmonicForm, {2652.66, 1.5}},
        {"C7", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.5}},
        {"C7", "CH", SpeciesBond::HarmonicForm, {2652.66, 1.5}},
        {"C7", "C2", SpeciesBond::HarmonicForm, {2652.66, 1.5}},
        {"C7", "C3", SpeciesBond::HarmonicForm, {2652.66, 1.5}},
        {"CT", "C3", SpeciesBond::HarmonicForm, {2175.68, 1.526}},
        {"CT", "C2", SpeciesBond::HarmonicForm, {2175.68, 1.526}},
        {"CA", "NB", SpeciesBond::HarmonicForm, {3464.35, 1.391}},
        {"CA", "N", SpeciesBond::HarmonicForm, {3573.14, 1.381}},
        {"CB", "CT", SpeciesBond::HarmonicForm, {2652.66, 1.51}},
        /* { "CC",	"CB",	SpeciesBond::HarmonicForm,	4351.36,	1.37 }, */
        {"CT", "F", SpeciesBond::HarmonicForm, {3071.06, 1.332}},
        {"C2", "F", SpeciesBond::HarmonicForm, {3071.06, 1.38}},
        {"CA", "F", SpeciesBond::HarmonicForm, {3514.56, 1.354}},
        {"CM", "F", SpeciesBond::HarmonicForm, {3514.56, 1.34}},
        {"CZ", "F", SpeciesBond::HarmonicForm, {3765.6, 1.279}},
        {"C", "F", SpeciesBond::HarmonicForm, {3514.56, 1.357}},
        {"CT", "CO", SpeciesBond::HarmonicForm, {2242.62, 1.529}},
        {"OH", "CO", SpeciesBond::HarmonicForm, {2677.76, 1.38}},
        {"HC", "CO", SpeciesBond::HarmonicForm, {2845.12, 1.09}},
        {"SY", "C3", SpeciesBond::HarmonicForm, {1857.7, 1.81}},
        {"SY", "CA", SpeciesBond::HarmonicForm, {2845.12, 1.77}},
        {"SY", "C8", SpeciesBond::HarmonicForm, {1857.7, 1.76}},
        {"SY", "OY", SpeciesBond::HarmonicForm, {5857.6, 1.44}},
        {"SZ", "OY", SpeciesBond::HarmonicForm, {5857.6, 1.53}},
        {"SY", "N", SpeciesBond::HarmonicForm, {3631.71, 1.67}},
        {"SY", "CT", SpeciesBond::HarmonicForm, {2845.12, 1.77}},
        {"SZ", "CT", SpeciesBond::HarmonicForm, {2845.12, 1.79}},
        {"U", "OU", SpeciesBond::HarmonicForm, {4.184, 1.8}},
        {"CA", "S", SpeciesBond::HarmonicForm, {2092, 1.76}},
        {"CM", "S", SpeciesBond::HarmonicForm, {2092, 1.76}},
        {"CM", "CY", SpeciesBond::HarmonicForm, {2652.66, 1.51}},
        {"CY", "NT", SpeciesBond::HarmonicForm, {3196.58, 1.448}},
        {"SY", "NT", SpeciesBond::HarmonicForm, {2845.12, 1.77}},
        {"C", "NT", SpeciesBond::HarmonicForm, {2652.66, 1.522}},
        {"C", "CW", SpeciesBond::HarmonicForm, {3347.2, 1.49}},
        {"Zn", "N", SpeciesBond::HarmonicForm, {334.72, 2.05}},
        {"Zn", "OH", SpeciesBond::HarmonicForm, {786.592, 1.8}},
        {"Zn", "OW", SpeciesBond::HarmonicForm, {334.72, 2.05}}};

    return Forcefield::termMatch_(bondTerms, i, j);
}

// Return angle term for the supplied atom type trio (if it exists)
OptionalReferenceWrapper<const ForcefieldAngleTerm> OPLSAA2005BaseForcefield::getAngleTerm(const ForcefieldAtomType &i,
                                                                                           const ForcefieldAtomType &j,
                                                                                           const ForcefieldAtomType &k) const
{
    static const std::vector<ForcefieldAngleTerm> angleTerms = {
        //	i	j	k	Type (Harmonic)			k	eq
        {"HW", "OW", "HW", SpeciesAngle::HarmonicForm, {627.6, 109.5}},
        /* { "HW",	"OW",	"LP",	SpeciesAngle::HarmonicForm,	418.4,		54.75 }, */
        {"OU", "U", "OU", SpeciesAngle::HarmonicForm, {1255.2, 180.0}},
        {"HC", "C^", "CW", SpeciesAngle::HarmonicForm, {292.88, 126.8}},
        {"HC", "C^", "CB", SpeciesAngle::HarmonicForm, {292.88, 126.8}},
        {"HC", "CS", "CW", SpeciesAngle::HarmonicForm, {292.88, 126.8}},
        {"HC", "CS", "CB", SpeciesAngle::HarmonicForm, {292.88, 126.8}},
        {"HA", "CA", "CW", SpeciesAngle::HarmonicForm, {292.88, 126.9}},
        {"HC", "C=", "CW", SpeciesAngle::HarmonicForm, {292.88, 122.0}},
        {"HA", "CW", "CA", SpeciesAngle::HarmonicForm, {292.88, 130.7}},
        {"HA", "CW", "C=", SpeciesAngle::HarmonicForm, {292.88, 130.7}},
        {"HA", "CW", "NA", SpeciesAngle::HarmonicForm, {292.88, 121.6}},
        {"CT", "CW", "NA", SpeciesAngle::HarmonicForm, {585.76, 121.6}},
        {"C!", "CW", "NA", SpeciesAngle::HarmonicForm, {585.76, 121.6}},
        {"CT", "CW", "OS", SpeciesAngle::HarmonicForm, {585.76, 121.6}},
        {"C!", "CW", "OS", SpeciesAngle::HarmonicForm, {585.76, 121.6}},
        {"CA", "CW", "NA", SpeciesAngle::HarmonicForm, {585.76, 121.6}},
        {"HA", "CW", "CV", SpeciesAngle::HarmonicForm, {292.88, 130.7}},
        {"HA", "CV", "CW", SpeciesAngle::HarmonicForm, {292.88, 128.2}},
        {"HC", "CT", "CZ", SpeciesAngle::HarmonicForm, {292.88, 108.5}},
        {"CT", "CT", "CZ", SpeciesAngle::HarmonicForm, {488.273, 112.7}},
        {"CT", "CZ", "CZ", SpeciesAngle::HarmonicForm, {1255.2, 180.0}},
        {"CA", "CZ", "CZ", SpeciesAngle::HarmonicForm, {1338.88, 180.0}},
        {"HC", "CZ", "CZ", SpeciesAngle::HarmonicForm, {937.216, 180.0}},
        {"CA", "CA", "CZ", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CT", "CZ", "NZ", SpeciesAngle::HarmonicForm, {1255.2, 180.0}},
        {"N2", "CZ", "NZ", SpeciesAngle::HarmonicForm, {1255.2, 180.0}},
        {"CA", "CZ", "NZ", SpeciesAngle::HarmonicForm, {1255.2, 180.0}},
        {"HC", "CT", "NZ", SpeciesAngle::HarmonicForm, {292.88, 108.5}},
        {"CA", "CA", "NZ", SpeciesAngle::HarmonicForm, {669.44, 120.0}},
        {"CT", "NZ", "CZ", SpeciesAngle::HarmonicForm, {1255.2, 180.0}},
        {"CA", "NZ", "CZ", SpeciesAngle::HarmonicForm, {1422.56, 180.0}},
        {"CT", "CX", "NA", SpeciesAngle::HarmonicForm, {585.76, 121.6}},
        {"HA", "CX", "CX", SpeciesAngle::HarmonicForm, {292.88, 130.7}},
        {"CX", "CX", "NA", SpeciesAngle::HarmonicForm, {585.76, 106.3}},
        {"CX", "NA", "CR", SpeciesAngle::HarmonicForm, {585.76, 109.8}},
        {"C2", "C", "N", SpeciesAngle::HarmonicForm, {585.76, 116.6}},
        {"C2", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 120.4}},
        {"C2", "C", "O2", SpeciesAngle::HarmonicForm, {585.76, 117.0}},
        {"C3", "C", "N", SpeciesAngle::HarmonicForm, {585.76, 116.6}},
        {"C3", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 120.4}},
        {"C3", "C", "O2", SpeciesAngle::HarmonicForm, {585.76, 117.0}},
        {"CA", "C", "CA", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        {"CA", "C", "OH", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CA", "CA", "OH", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CA", "CA", "SH", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CA", "CA", "OS", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CM", "CM", "OS", SpeciesAngle::HarmonicForm, {585.76, 123.0}},
        {"C=", "CM", "OS", SpeciesAngle::HarmonicForm, {585.76, 123.0}},
        {"CM", "CM", "OH", SpeciesAngle::HarmonicForm, {585.76, 123.0}},
        {"C=", "CM", "OH", SpeciesAngle::HarmonicForm, {585.76, 123.0}},
        {"CB", "C", "NA", SpeciesAngle::HarmonicForm, {585.76, 111.3}},
        {"CB", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 128.8}},
        {"CB", "C", "N", SpeciesAngle::HarmonicForm, {585.76, 111.3}},
        {"CS", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 128.2}},
        {"CD", "C", "CD", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        {"CD", "C", "OH", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CH", "C", "N", SpeciesAngle::HarmonicForm, {585.76, 116.6}},
        {"CH", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 120.4}},
        {"CH", "C", "O2", SpeciesAngle::HarmonicForm, {543.92, 117.0}},
        {"CH", "C", "OH", SpeciesAngle::HarmonicForm, {585.76, 115.0}},
        /* { "CJ",	"C",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		114.1 }, */
        /* { "CJ",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		125.3 }, */
        {"CM", "C", "NA", SpeciesAngle::HarmonicForm, {585.76, 114.1}},
        {"CM", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 125.3}},
        {"C=", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 124.0}},
        {"C=", "C", "HC", SpeciesAngle::HarmonicForm, {669.44, 116.0}},
        {"CT", "C", "N", SpeciesAngle::HarmonicForm, {585.76, 116.6}},
        {"CT", "C", "NM", SpeciesAngle::HarmonicForm, {585.76, 116.6}},
        {"CA", "C", "N", SpeciesAngle::HarmonicForm, {585.76, 115.5}},
        {"CM", "C", "N", SpeciesAngle::HarmonicForm, {585.76, 115.5}},
        {"CT", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 120.4}},
        {"CA", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 120.4}},
        /* { "CT",	"C",	"S=",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
        {"CT", "NO", "ON", SpeciesAngle::HarmonicForm, {669.44, 117.5}},
        {"CA", "NO", "ON", SpeciesAngle::HarmonicForm, {669.44, 117.5}},
        {"CT", "CT", "NO", SpeciesAngle::HarmonicForm, {527.184, 111.1}},
        {"HC", "CT", "NO", SpeciesAngle::HarmonicForm, {292.88, 105.0}},
        {"CA", "CA", "NO", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        {"HC", "C", "N", SpeciesAngle::HarmonicForm, {334.72, 114.0}},
        {"HC", "C", "OS", SpeciesAngle::HarmonicForm, {334.72, 115.0}},
        {"HC", "C", "OH", SpeciesAngle::HarmonicForm, {334.72, 115.0}},
        {"O", "C", "HC", SpeciesAngle::HarmonicForm, {292.88, 123.0}},
        /* { "S=",	"C",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		127.0 }, */
        {"NC", "C", "HC", SpeciesAngle::HarmonicForm, {292.88, 122.0}},
        {"CT", "C", "OH", SpeciesAngle::HarmonicForm, {585.76, 108.0}},
        {"CT", "C", "CT", SpeciesAngle::HarmonicForm, {585.76, 116.0}},
        {"CT", "C", "CA", SpeciesAngle::HarmonicForm, {585.76, 116.0}},
        {"C=", "C", "CT", SpeciesAngle::HarmonicForm, {585.76, 116.0}},
        {"CT", "C", "O2", SpeciesAngle::HarmonicForm, {585.76, 117.0}},
        {"CA", "C", "O2", SpeciesAngle::HarmonicForm, {585.76, 117.0}},
        {"CT", "CT", "Cl", SpeciesAngle::HarmonicForm, {577.392, 109.8}},
        {"C", "CT", "Cl", SpeciesAngle::HarmonicForm, {577.392, 109.8}},
        {"CA", "CA", "Cl", SpeciesAngle::HarmonicForm, {627.6, 120.0}},
        {"CM", "CM", "Cl", SpeciesAngle::HarmonicForm, {627.6, 121.5}},
        {"Cl", "CM", "HC", SpeciesAngle::HarmonicForm, {502.08, 114.0}},
        {"Cl", "CT", "Cl", SpeciesAngle::HarmonicForm, {652.704, 111.7}},
        {"HC", "CT", "Cl", SpeciesAngle::HarmonicForm, {426.768, 107.6}},
        {"CT", "CT", "Br", SpeciesAngle::HarmonicForm, {577.392, 110.0}},
        {"CA", "CT", "Br", SpeciesAngle::HarmonicForm, {577.392, 110.0}},
        {"C", "CT", "Br", SpeciesAngle::HarmonicForm, {577.392, 109.8}},
        {"CT", "C", "Cl", SpeciesAngle::HarmonicForm, {627.6, 109.0}},
        {"CT", "C", "Br", SpeciesAngle::HarmonicForm, {627.6, 109.0}},
        {"O", "C", "Cl", SpeciesAngle::HarmonicForm, {627.6, 119.0}},
        {"O", "C", "Br", SpeciesAngle::HarmonicForm, {627.6, 119.0}},
        {"CA", "CA", "Br", SpeciesAngle::HarmonicForm, {627.6, 120.0}},
        {"CM", "CM", "Br", SpeciesAngle::HarmonicForm, {627.6, 120.0}},
        {"Br", "CM", "HC", SpeciesAngle::HarmonicForm, {502.08, 114.0}},
        {"Br", "CT", "Br", SpeciesAngle::HarmonicForm, {652.704, 111.7}},
        {"HC", "CT", "Br", SpeciesAngle::HarmonicForm, {426.768, 107.6}},
        {"CA", "CA", "I", SpeciesAngle::HarmonicForm, {627.6, 120.0}},
        {"CT", "CT", "I", SpeciesAngle::HarmonicForm, {627.6, 112.0}},
        {"HC", "CT", "I", SpeciesAngle::HarmonicForm, {627.6, 111.0}},
        {"CA", "CA", "F", SpeciesAngle::HarmonicForm, {669.44, 120.0}},
        {"CM", "CM", "F", SpeciesAngle::HarmonicForm, {669.44, 121.5}},
        {"C", "CM", "F", SpeciesAngle::HarmonicForm, {669.44, 121.5}},
        {"F", "CM", "HC", SpeciesAngle::HarmonicForm, {418.4, 112.0}},
        {"F", "CM", "F", SpeciesAngle::HarmonicForm, {669.44, 108.0}},
        {"F", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 121.0}},
        {"F", "C", "CT", SpeciesAngle::HarmonicForm, {669.44, 111.0}},
        {"N", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 122.9}},
        {"NM", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 122.9}},
        {"N", "C", "N", SpeciesAngle::HarmonicForm, {585.76, 114.2}},
        /* { "N",	"C",	"S=",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
        {"N^", "C", "NA", SpeciesAngle::HarmonicForm, {585.76, 115.4}},
        {"N^", "C", "NC", SpeciesAngle::HarmonicForm, {585.76, 118.6}},
        {"NA", "C", "NA", SpeciesAngle::HarmonicForm, {585.76, 118.6}},
        {"N^", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 120.9}},
        {"NA", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 120.6}},
        {"NC", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 122.5}},
        {"NC", "C", "NA", SpeciesAngle::HarmonicForm, {585.76, 118.6}},
        {"NA", "CM", "H4", SpeciesAngle::HarmonicForm, {292.88, 119.1}},
        {"N^", "CM", "H4", SpeciesAngle::HarmonicForm, {292.88, 119.1}},
        {"N", "CA", "HA", SpeciesAngle::HarmonicForm, {292.88, 119.1}},
        {"O", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 126.0}},
        {"ON", "NO", "ON", SpeciesAngle::HarmonicForm, {669.44, 125.0}},
        {"O", "C", "OH", SpeciesAngle::HarmonicForm, {669.44, 121.0}},
        {"O2", "C", "O2", SpeciesAngle::HarmonicForm, {669.44, 126.0}},
        {"C2", "C^", "CB", SpeciesAngle::HarmonicForm, {585.76, 128.6}},
        {"C2", "CA", "CB", SpeciesAngle::HarmonicForm, {585.76, 128.6}},
        /* { "C2",	"C^",	"CG",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
        /* { "C2",	"C^",	"CC",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
        {"C2", "C^", "CW", SpeciesAngle::HarmonicForm, {585.76, 125.0}},
        /* { "CB",	"C^",	"CG",	SpeciesAngle::HarmonicForm,	711.28,		106.4 }, */
        {"CB", "C^", "CT", SpeciesAngle::HarmonicForm, {585.76, 128.6}},
        {"CB", "C^", "CW", SpeciesAngle::HarmonicForm, {711.28, 106.4}},
        {"CT", "C^", "CW", SpeciesAngle::HarmonicForm, {585.76, 125.0}},
        /* { "C2",	"CS",	"CG",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
        /* { "C2",	"CS",	"CC",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
        {"C2", "CS", "CW", SpeciesAngle::HarmonicForm, {585.76, 125.0}},
        /* { "CB",	"CS",	"CG",	SpeciesAngle::HarmonicForm,	711.28,		106.4 }, */
        {"CB", "CS", "CT", SpeciesAngle::HarmonicForm, {585.76, 128.6}},
        {"CB", "CS", "CW", SpeciesAngle::HarmonicForm, {711.28, 106.4}},
        {"CT", "CS", "CW", SpeciesAngle::HarmonicForm, {585.76, 125.0}},
        {"C", "C2", "C2", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C", "C2", "CH", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C", "C2", "N", SpeciesAngle::HarmonicForm, {669.44, 110.3}},
        {"C", "C2", "NT", SpeciesAngle::HarmonicForm, {669.44, 111.2}},
        {"C^", "C2", "CH", SpeciesAngle::HarmonicForm, {527.184, 115.6}},
        {"C2", "C2", "C2", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C2", "C2", "C3", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C3", "C2", "C3", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C2", "CH", "C3", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C2", "CH", "C2", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C2", "C2", "CH", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C2", "CH", "OH", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"C3", "C2", "OH", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"C2", "C2", "OH", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"C2", "C2", "N", SpeciesAngle::HarmonicForm, {669.44, 111.2}},
        {"C2", "C2", "N2", SpeciesAngle::HarmonicForm, {669.44, 111.2}},
        {"C2", "C2", "N3", SpeciesAngle::HarmonicForm, {669.44, 111.2}},
        /* { "**C2",	"C2",	"NT",	SpeciesAngle::HarmonicForm,	669.44,		111.2 }, */
        {"CT", "CT", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"C3", "CT", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"C2", "CT", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"CH", "CT", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"CT", "C2", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"C3", "C2", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"C2", "C2", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"CH", "C2", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"CT", "CH", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"C3", "CH", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"C2", "CH", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"CH", "CH", "NT", SpeciesAngle::HarmonicForm, {470.282, 109.47}},
        {"C2", "C2", "OS", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"C3", "CH", "OS", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"C2", "C2", "S", SpeciesAngle::HarmonicForm, {418.4, 114.7}},
        {"C3", "C2", "CH", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C3", "C2", "OS", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"CA", "C2", "CH", SpeciesAngle::HarmonicForm, {527.184, 114.0}},
        {"CD", "C2", "CH", SpeciesAngle::HarmonicForm, {527.184, 114.0}},
        /* { "CC",	"C2",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	113.1 }, */
        {"CH", "C2", "CH", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"CH", "C2", "OH", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"CH", "C2", "OS", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"CH", "C2", "S", SpeciesAngle::HarmonicForm, {418.4, 114.7}},
        {"CH", "C2", "SH", SpeciesAngle::HarmonicForm, {418.4, 108.6}},
        {"C3", "C2", "CT", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C2", "C2", "CA", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"CH", "CA", "CA", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"C2", "CH", "CA", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C", "CA", "CA", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        {"C", "CA", "HA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"C2", "CA", "CA", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"C2", "CA", "CD", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"C2", "CD", "CD", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CA", "CA", "CA", SpeciesAngle::HarmonicForm, {527.184, 120.0}},
        {"CA", "C!", "CA", SpeciesAngle::HarmonicForm, {527.184, 120.0}},
        {"CA", "C!", "C!", SpeciesAngle::HarmonicForm, {527.184, 120.0}},
        {"CA", "CA", "C!", SpeciesAngle::HarmonicForm, {527.184, 120.0}},
        {"CA", "C!", "CR", SpeciesAngle::HarmonicForm, {527.184, 120.0}},
        {"CA", "C!", "CS", SpeciesAngle::HarmonicForm, {527.184, 120.0}},
        {"CA", "C!", "CW", SpeciesAngle::HarmonicForm, {527.184, 120.0}},
        {"CA", "C!", "CU", SpeciesAngle::HarmonicForm, {527.184, 120.0}},
        {"CA", "C!", "CV", SpeciesAngle::HarmonicForm, {527.184, 120.0}},
        {"CA", "CA", "CB", SpeciesAngle::HarmonicForm, {527.184, 120.0}},
        {"CA", "CA", "CN", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        {"CA", "CA", "CM", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CA", "CM", "CT", SpeciesAngle::HarmonicForm, {711.28, 119.7}},
        {"CA", "CA", "CT", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CA", "CA", "NT", SpeciesAngle::HarmonicForm, {585.76, 120.1}},
        {"CA", "CA", "N3", SpeciesAngle::HarmonicForm, {585.76, 120.1}},
        {"CA", "CA", "HA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"C!", "CA", "HA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"CA", "CA", "DM", SpeciesAngle::HarmonicForm, {292.88, 90.0}},
        {"HA", "CA", "DM", SpeciesAngle::HarmonicForm, {83.68, 90.0}},
        {"OY", "SZ", "DM", SpeciesAngle::HarmonicForm, {83.68, 90.0}},
        {"CT", "NC", "NZ", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"NC", "NZ", "NZ", SpeciesAngle::HarmonicForm, {836.8, 180.0}},
        {"CT", "CT", "NC", SpeciesAngle::HarmonicForm, {543.92, 109.0}},
        {"NC", "CA", "HA", SpeciesAngle::HarmonicForm, {292.88, 116.0}},
        {"CA", "CA", "NC", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CA", "C!", "NC", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"C!", "CA", "NC", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"C!", "C!", "NC", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CA", "NC", "CA", SpeciesAngle::HarmonicForm, {585.76, 117.0}},
        {"CA", "NC", "C!", SpeciesAngle::HarmonicForm, {585.76, 117.0}},
        {"CA", "CA", "CW", SpeciesAngle::HarmonicForm, {585.76, 107.4}},
        {"CW", "NA", "CW", SpeciesAngle::HarmonicForm, {585.76, 109.8}},
        {"CV", "CW", "NA", SpeciesAngle::HarmonicForm, {585.76, 106.3}},
        {"CW", "CV", "NB", SpeciesAngle::HarmonicForm, {585.76, 111.0}},
        {"CW", "NA", "CR", SpeciesAngle::HarmonicForm, {585.76, 109.8}},
        {"CB", "NA", "CR", SpeciesAngle::HarmonicForm, {585.76, 109.8}},
        {"CW", "C=", "C=", SpeciesAngle::HarmonicForm, {292.88, 106.0}},
        {"CA", "NA", "CK", SpeciesAngle::HarmonicForm, {585.76, 109.8}},
        {"NC", "CA", "CT", SpeciesAngle::HarmonicForm, {585.76, 116.0}},
        {"NC", "CQ", "CT", SpeciesAngle::HarmonicForm, {585.76, 115.5}},
        {"NB", "CV", "CT", SpeciesAngle::HarmonicForm, {585.76, 124.5}},
        {"CA", "CV", "NB", SpeciesAngle::HarmonicForm, {585.76, 111.0}},
        {"CA", "NC", "NC", SpeciesAngle::HarmonicForm, {585.76, 117.0}},
        {"CT", "NC", "NC", SpeciesAngle::HarmonicForm, {585.76, 117.0}},
        {"OS", "CW", "CS", SpeciesAngle::HarmonicForm, {585.76, 110.6}},
        {"OS", "CW", "C=", SpeciesAngle::HarmonicForm, {585.76, 110.0}},
        {"CW", "OS", "CW", SpeciesAngle::HarmonicForm, {585.76, 106.5}},
        {"CW", "OS", "CB", SpeciesAngle::HarmonicForm, {585.76, 106.5}},
        {"CR", "OS", "CB", SpeciesAngle::HarmonicForm, {585.76, 106.5}},
        {"OS", "CW", "HA", SpeciesAngle::HarmonicForm, {292.88, 113.4}},
        {"S", "CW", "HA", SpeciesAngle::HarmonicForm, {292.88, 113.4}},
        {"S", "CR", "HA", SpeciesAngle::HarmonicForm, {292.88, 113.4}},
        {"S", "CR", "NT", SpeciesAngle::HarmonicForm, {585.76, 120.2}},
        {"NB", "CR", "NT", SpeciesAngle::HarmonicForm, {585.76, 126.1}},
        {"S", "CW", "CV", SpeciesAngle::HarmonicForm, {585.76, 111.0}},
        {"S", "CW", "CS", SpeciesAngle::HarmonicForm, {585.76, 111.0}},
        {"NA", "CW", "CS", SpeciesAngle::HarmonicForm, {585.76, 107.7}},
        {"CW", "CS", "CS", SpeciesAngle::HarmonicForm, {585.76, 107.3}},
        {"CW", "CS", "HA", SpeciesAngle::HarmonicForm, {292.88, 125.7}},
        {"CW", "CS", "C!", SpeciesAngle::HarmonicForm, {585.76, 125.7}},
        {"CS", "CW", "C!", SpeciesAngle::HarmonicForm, {585.76, 132.1}},
        {"CS", "CS", "C!", SpeciesAngle::HarmonicForm, {585.76, 127.5}},
        {"CS", "CW", "HA", SpeciesAngle::HarmonicForm, {292.88, 132.1}},
        {"CS", "CW", "CT", SpeciesAngle::HarmonicForm, {585.76, 132.1}},
        {"CS", "CW", "CA", SpeciesAngle::HarmonicForm, {585.76, 132.1}},
        {"CS", "CS", "HA", SpeciesAngle::HarmonicForm, {292.88, 127.5}},
        {"CU", "NB", "NA", SpeciesAngle::HarmonicForm, {585.76, 104.1}},
        {"CW", "NB", "NA", SpeciesAngle::HarmonicForm, {585.76, 104.1}},
        {"NB", "CU", "HA", SpeciesAngle::HarmonicForm, {292.88, 118.9}},
        {"NB", "CU", "CT", SpeciesAngle::HarmonicForm, {585.76, 118.9}},
        {"NB", "CW", "CT", SpeciesAngle::HarmonicForm, {585.76, 118.9}},
        {"CU", "CS", "CW", SpeciesAngle::HarmonicForm, {585.76, 103.8}},
        {"CW", "CS", "CW", SpeciesAngle::HarmonicForm, {585.76, 103.8}},
        {"NB", "CU", "CS", SpeciesAngle::HarmonicForm, {585.76, 111.9}},
        {"NB", "CW", "CS", SpeciesAngle::HarmonicForm, {585.76, 111.9}},
        {"CA", "CU", "HA", SpeciesAngle::HarmonicForm, {292.88, 128.6}},
        {"CU", "CA", "HA", SpeciesAngle::HarmonicForm, {292.88, 128.2}},
        {"CU", "NB", "OS", SpeciesAngle::HarmonicForm, {585.76, 105.3}},
        {"NB", "NA", "CW", SpeciesAngle::HarmonicForm, {468.608, 113.1}},
        {"CB", "NA", "NB", SpeciesAngle::HarmonicForm, {468.608, 113.1}},
        {"CR", "NA", "NB", SpeciesAngle::HarmonicForm, {468.608, 113.1}},
        {"NB", "NA", "H", SpeciesAngle::HarmonicForm, {468.608, 118.4}},
        {"NB", "NA", "CA", SpeciesAngle::HarmonicForm, {585.76, 118.4}},
        {"NB", "NA", "CT", SpeciesAngle::HarmonicForm, {585.76, 118.4}},
        {"CW", "OS", "NB", SpeciesAngle::HarmonicForm, {585.76, 108.9}},
        {"NB", "CR", "OS", SpeciesAngle::HarmonicForm, {585.76, 115.0}},
        {"NB", "CR", "S", SpeciesAngle::HarmonicForm, {585.76, 115.0}},
        {"CR", "OS", "CW", SpeciesAngle::HarmonicForm, {585.76, 104.0}},
        {"CV", "CW", "OS", SpeciesAngle::HarmonicForm, {585.76, 108.0}},
        {"HA", "CR", "OS", SpeciesAngle::HarmonicForm, {292.88, 117.0}},
        {"OS", "CM", "HC", SpeciesAngle::HarmonicForm, {292.88, 114.5}},
        {"CB", "CA", "HA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        /* { "CC",	"CA",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
        {"CB", "CA", "N2", SpeciesAngle::HarmonicForm, {585.76, 123.5}},
        {"CB", "CA", "NC", SpeciesAngle::HarmonicForm, {585.76, 117.3}},
        {"CD", "CA", "CD", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        /* { "CJ",	"CA",	"N2",	SpeciesAngle::HarmonicForm,	585.76,		120.1 }, */
        /* { "CJ",	"CA",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		121.5 }, */
        {"CM", "CA", "N2", SpeciesAngle::HarmonicForm, {585.76, 120.1}},
        {"CA", "CA", "N2", SpeciesAngle::HarmonicForm, {585.76, 120.1}},
        {"CM", "CA", "NC", SpeciesAngle::HarmonicForm, {585.76, 121.5}},
        {"CM", "CA", "NA", SpeciesAngle::HarmonicForm, {585.76, 121.5}},
        {"CN", "CA", "HA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"NY", "CA", "NY", SpeciesAngle::HarmonicForm, {585.76, 111.8}},
        {"NZ", "CA", "NY", SpeciesAngle::HarmonicForm, {585.76, 124.1}},
        {"CA", "NZ", "H", SpeciesAngle::HarmonicForm, {292.88, 113.0}},
        {"CA", "NY", "H", SpeciesAngle::HarmonicForm, {418.4, 112.5}},
        {"CA", "NY", "CT", SpeciesAngle::HarmonicForm, {418.4, 120.5}},
        {"H", "NY", "H", SpeciesAngle::HarmonicForm, {364.845, 106.4}},
        {"CT", "NY", "H", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CT", "CT", "NY", SpeciesAngle::HarmonicForm, {669.44, 111.2}},
        {"HC", "CT", "NY", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"N2", "CA", "N2", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"N2", "CA", "NA", SpeciesAngle::HarmonicForm, {585.76, 116.0}},
        {"N2", "CA", "NC", SpeciesAngle::HarmonicForm, {585.76, 119.3}},
        {"N2", "CQ", "NC", SpeciesAngle::HarmonicForm, {585.76, 119.3}},
        {"N2", "CQ", "N", SpeciesAngle::HarmonicForm, {585.76, 116.0}},
        {"NA", "CA", "NC", SpeciesAngle::HarmonicForm, {585.76, 123.3}},
        {"C", "CB", "CB", SpeciesAngle::HarmonicForm, {711.28, 119.2}},
        {"C", "CB", "NB", SpeciesAngle::HarmonicForm, {585.76, 130.0}},
        {"N", "CQ", "NC", SpeciesAngle::HarmonicForm, {585.76, 123.3}},
        {"C", "CS", "CW", SpeciesAngle::HarmonicForm, {585.76, 130.0}},
        {"C", "CB", "CW", SpeciesAngle::HarmonicForm, {585.76, 130.0}},
        {"C^", "CB", "CA", SpeciesAngle::HarmonicForm, {711.28, 134.9}},
        {"C^", "CB", "CD", SpeciesAngle::HarmonicForm, {711.28, 134.9}},
        {"CA", "CB", "CA", SpeciesAngle::HarmonicForm, {711.28, 134.9}},
        {"C^", "CB", "CN", SpeciesAngle::HarmonicForm, {711.28, 108.8}},
        /* { "CA",	"CB",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		108.8 }, */
        /* { "C^",	"CB",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		108.8 }, */
        {"CS", "CB", "CA", SpeciesAngle::HarmonicForm, {711.28, 134.9}},
        {"CS", "CB", "CD", SpeciesAngle::HarmonicForm, {711.28, 134.9}},
        {"CS", "CB", "CN", SpeciesAngle::HarmonicForm, {711.28, 108.8}},
        /* { "CS",	"CB",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		108.8 }, */
        {"CA", "CB", "CB", SpeciesAngle::HarmonicForm, {711.28, 117.3}},
        {"CA", "CB", "CN", SpeciesAngle::HarmonicForm, {711.28, 116.2}},
        {"CA", "CB", "NB", SpeciesAngle::HarmonicForm, {585.76, 132.4}},
        {"CB", "CB", "N^", SpeciesAngle::HarmonicForm, {585.76, 106.2}},
        {"CB", "CB", "NA", SpeciesAngle::HarmonicForm, {585.76, 106.2}},
        {"CS", "CR", "NA", SpeciesAngle::HarmonicForm, {585.76, 106.2}},
        {"CB", "CB", "NB", SpeciesAngle::HarmonicForm, {585.76, 111.0}},
        {"CR", "CS", "CW", SpeciesAngle::HarmonicForm, {585.76, 110.4}},
        {"CB", "CB", "NC", SpeciesAngle::HarmonicForm, {585.76, 127.7}},
        {"CB", "CB", "N", SpeciesAngle::HarmonicForm, {585.76, 127.7}},
        {"CS", "CR", "NC", SpeciesAngle::HarmonicForm, {585.76, 127.7}},
        {"CD", "CB", "CN", SpeciesAngle::HarmonicForm, {711.28, 116.2}},
        {"N^", "CB", "NC", SpeciesAngle::HarmonicForm, {585.76, 126.2}},
        {"NA", "CB", "NC", SpeciesAngle::HarmonicForm, {585.76, 126.2}},
        {"NB", "CB", "N", SpeciesAngle::HarmonicForm, {585.76, 126.2}},
        {"NB", "CR", "N", SpeciesAngle::HarmonicForm, {585.76, 126.2}},
        {"NA", "CR", "NC", SpeciesAngle::HarmonicForm, {585.76, 126.2}},
        /* { "C2",	"CC",	"CF",	SpeciesAngle::HarmonicForm,	585.76,		131.9 }, */
        /* { "C2",	"CC",	"CG",	SpeciesAngle::HarmonicForm,	585.76,		129.05 }, */
        /* { "C2",	"CC",	"CV",	SpeciesAngle::HarmonicForm,	585.76,		131.9 }, */
        /* { "C2",	"CC",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		129.05 }, */
        /* { "C2",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		122.2 }, */
        /* { "C2",	"CC",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		121.05 }, */
        /* { "CF",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		105.9 }, */
        /* { "CG",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		108.75 }, */
        /* { "CG",	"CC",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		109.9 }, */
        /* { "CT",	"CC",	"CV",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
        {"CT", "CW", "CV", SpeciesAngle::HarmonicForm, {585.76, 130.7}},
        {"CT", "CV", "CW", SpeciesAngle::HarmonicForm, {585.76, 130.7}},
        {"CT", "CX", "CX", SpeciesAngle::HarmonicForm, {585.76, 130.7}},
        {"CT", "CW", "CW", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        /* { "CT",	"CC",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
        /* { "CT",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
        /* { "CT",	"CC",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
        /* { "CV",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
        {"CW", "CW", "NA", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        /* { "CW",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
        /* { "CW",	"CC",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
        {"C", "CD", "CD", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        {"CA", "CD", "CD", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        {"CB", "CD", "CD", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        {"CD", "CD", "CD", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        {"CD", "CD", "CN", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        /* { "CD",	"CD",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		120.0 }, */
        /* { "N^",	"CE",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		113.9 }, */
        /* { "CC",	"CF",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		109.9 }, */
        /* { "C^",	"CG",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		108.7 }, */
        {"CA", "CA", "NA", SpeciesAngle::HarmonicForm, {585.76, 108.7}},
        /* { "C^",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		108.7 }, */
        /* { "CC",	"CG",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		105.9 }, */
        {"C", "CH", "C2", SpeciesAngle::HarmonicForm, {527.184, 111.1}},
        {"C", "CH", "C3", SpeciesAngle::HarmonicForm, {527.184, 111.1}},
        {"C", "CH", "CH", SpeciesAngle::HarmonicForm, {527.184, 111.1}},
        {"C", "CH", "N", SpeciesAngle::HarmonicForm, {527.184, 110.1}},
        {"C", "CH", "NT", SpeciesAngle::HarmonicForm, {669.44, 109.7}},
        {"C2", "CH", "CH", SpeciesAngle::HarmonicForm, {527.184, 111.5}},
        {"C2", "CH", "N", SpeciesAngle::HarmonicForm, {669.44, 109.7}},
        {"C2", "CH", "N^", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"C2", "CH", "OS", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"C3", "CH", "C3", SpeciesAngle::HarmonicForm, {527.184, 111.5}},
        {"C3", "CH", "CH", SpeciesAngle::HarmonicForm, {527.184, 111.5}},
        {"C3", "CH", "N", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"C3", "CH", "OH", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"CH", "CH", "CH", SpeciesAngle::HarmonicForm, {527.184, 111.5}},
        {"CH", "CH", "N", SpeciesAngle::HarmonicForm, {669.44, 109.7}},
        {"CH", "CH", "N^", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"CH", "CH", "OH", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"CH", "CH", "OS", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"N^", "CH", "OS", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        /* { "NC",	"CI",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		129.1 }, */
        /* { "C",	"CJ",	"CJ",	SpeciesAngle::HarmonicForm,	711.28,		120.7 }, */
        /* { "CA",	"CJ",	"CJ",	SpeciesAngle::HarmonicForm,	711.28,		117.0 }, */
        /* { "CJ",	"CJ",	"N^",	SpeciesAngle::HarmonicForm,	585.76,		121.2 }, */
        /* { "CM",	"CJ",	"N^",	SpeciesAngle::HarmonicForm,	585.76,		121.2 }, */
        {"N^", "CK", "NB", SpeciesAngle::HarmonicForm, {585.76, 113.9}},
        {"NA", "CK", "NB", SpeciesAngle::HarmonicForm, {585.76, 113.9}},
        {"NA", "CK", "H5", SpeciesAngle::HarmonicForm, {292.88, 123.05}},
        {"N^", "CK", "H5", SpeciesAngle::HarmonicForm, {292.88, 123.05}},
        {"NB", "CK", "H5", SpeciesAngle::HarmonicForm, {292.88, 123.05}},
        {"C", "CM", "C3", SpeciesAngle::HarmonicForm, {711.28, 119.7}},
        /* { "C",	"CM",	"CJ",	SpeciesAngle::HarmonicForm,	711.28,		120.7 }, */
        {"C", "CM", "CM", SpeciesAngle::HarmonicForm, {711.28, 120.7}},
        {"C", "CM", "CT", SpeciesAngle::HarmonicForm, {585.76, 119.7}},
        {"C", "CA", "CT", SpeciesAngle::HarmonicForm, {585.76, 119.7}},
        {"C", "CM", "HC", SpeciesAngle::HarmonicForm, {292.88, 119.7}},
        /* { "C3",	"CM",	"CJ",	SpeciesAngle::HarmonicForm,	711.28,		119.7 }, */
        {"CA", "CM", "CM", SpeciesAngle::HarmonicForm, {711.28, 117.0}},
        {"CA", "CM", "HC", SpeciesAngle::HarmonicForm, {292.88, 123.3}},
        /* { "CJ",	"CM",	"CT",	SpeciesAngle::HarmonicForm,	711.28,		119.7 }, */
        {"CM", "CM", "CT", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CM", "C=", "C=", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CM", "C=", "C", SpeciesAngle::HarmonicForm, {585.76, 118.7}},
        {"CM", "C=", "CT", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"C=", "C=", "CT", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CT", "CM", "C=", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CM", "CT", "CM", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"CM", "CM", "HC", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"CM", "CM", "H4", SpeciesAngle::HarmonicForm, {292.88, 119.7}},
        {"CM", "C=", "HC", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"C=", "CM", "HC", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"C=", "C=", "HC", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"CT", "C=", "HC", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"CT", "C", "HC", SpeciesAngle::HarmonicForm, {292.88, 115.0}},
        {"CA", "C", "HC", SpeciesAngle::HarmonicForm, {292.88, 115.0}},
        {"CT", "CM", "HC", SpeciesAngle::HarmonicForm, {292.88, 117.0}},
        {"HC", "CM", "HC", SpeciesAngle::HarmonicForm, {292.88, 117.0}},
        {"CT", "CM", "CT", SpeciesAngle::HarmonicForm, {585.76, 130.0}},
        {"CT", "C+", "CT", SpeciesAngle::HarmonicForm, {1445.99, 120.0}},
        {"CT", "C+", "HC", SpeciesAngle::HarmonicForm, {1204.99, 120.0}},
        {"CT", "CT", "C+", SpeciesAngle::HarmonicForm, {527.184, 105.0}},
        {"HC", "CT", "C+", SpeciesAngle::HarmonicForm, {292.88, 105.0}},
        {"CM", "CM", "N^", SpeciesAngle::HarmonicForm, {585.76, 121.2}},
        {"CM", "CM", "NA", SpeciesAngle::HarmonicForm, {585.76, 121.2}},
        {"HC", "CM", "N^", SpeciesAngle::HarmonicForm, {292.88, 119.1}},
        {"HC", "CM", "NA", SpeciesAngle::HarmonicForm, {292.88, 119.1}},
        {"CA", "CN", "CB", SpeciesAngle::HarmonicForm, {711.28, 122.7}},
        {"CA", "CN", "NA", SpeciesAngle::HarmonicForm, {585.76, 132.8}},
        {"CB", "CN", "CD", SpeciesAngle::HarmonicForm, {711.28, 122.7}},
        /* { "CB",	"CC",	"CA",	SpeciesAngle::HarmonicForm,	711.28,		122.7 }, */
        /* { "CB",	"CC",	"CD",	SpeciesAngle::HarmonicForm,	711.28,		122.7 }, */
        {"CB", "CA", "CW", SpeciesAngle::HarmonicForm, {527.184, 106.4}},
        {"CB", "CA", "CT", SpeciesAngle::HarmonicForm, {585.76, 128.6}},
        {"CB", "CN", "NA", SpeciesAngle::HarmonicForm, {585.76, 104.4}},
        /* { "CB",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		104.4 }, */
        {"CD", "CN", "NA", SpeciesAngle::HarmonicForm, {585.76, 132.8}},
        /* { "CA",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		132.8 }, */
        /* { "CD",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		132.8 }, */
        /* { "NA",	"CP",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		110.75 }, */
        /* { "NA",	"CP",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		111.6 }, */
        {"HA", "CQ", "NC", SpeciesAngle::HarmonicForm, {292.88, 115.45}},
        {"H5", "CQ", "NC", SpeciesAngle::HarmonicForm, {292.88, 115.45}},
        {"NC", "CQ", "NC", SpeciesAngle::HarmonicForm, {585.76, 129.1}},
        {"HA", "CR", "NA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"HA", "CX", "NA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"HA", "CR", "NB", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"HA", "CK", "N^", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"HA", "CK", "NA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"HA", "CK", "NB", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"NA", "CR", "NA", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"NA", "CR", "NB", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"NA", "CR", "CT", SpeciesAngle::HarmonicForm, {585.76, 125.0}},
        {"NB", "CR", "CT", SpeciesAngle::HarmonicForm, {585.76, 125.0}},
        {"NA", "CR", "SY", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"NB", "CR", "SY", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"NB", "CR", "S", SpeciesAngle::HarmonicForm, {585.76, 113.6}},
        {"C", "CT", "CT", SpeciesAngle::HarmonicForm, {527.184, 111.1}},
        {"CM", "CT", "CT", SpeciesAngle::HarmonicForm, {527.184, 111.1}},
        {"CW", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CV", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CX", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"C", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"C", "CT", "N", SpeciesAngle::HarmonicForm, {527.184, 110.1}},
        {"C", "CT", "NC", SpeciesAngle::HarmonicForm, {527.184, 110.1}},
        {"C^", "CT", "CT", SpeciesAngle::HarmonicForm, {527.184, 115.6}},
        {"C^", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CS", "CT", "CT", SpeciesAngle::HarmonicForm, {527.184, 115.6}},
        {"CS", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CA", "CT", "CT", SpeciesAngle::HarmonicForm, {527.184, 114.0}},
        {"CA", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CA", "N3", "H3", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        /* { "CC",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	113.1 }, */
        {"CW", "CT", "CT", SpeciesAngle::HarmonicForm, {527.184, 114.0}},
        {"CV", "CT", "CT", SpeciesAngle::HarmonicForm, {527.184, 114.0}},
        {"CX", "CT", "CT", SpeciesAngle::HarmonicForm, {527.184, 114.0}},
        /* { "CC",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 }, */
        {"CM", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"C=", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CT", "CT", "CT", SpeciesAngle::HarmonicForm, {488.273, 112.7}},
        {"C3", "CT", "C3", SpeciesAngle::HarmonicForm, {334.72, 109.5}},
        {"C2", "CT", "C2", SpeciesAngle::HarmonicForm, {334.72, 109.5}},
        {"C2", "CT", "C3", SpeciesAngle::HarmonicForm, {334.72, 109.5}},
        {"C3", "CT", "C", SpeciesAngle::HarmonicForm, {527.184, 109.5}},
        {"CT", "CT", "HC", SpeciesAngle::HarmonicForm, {313.8, 110.7}},
        {"CT", "HC", "DM", SpeciesAngle::HarmonicForm, {313.8, 109.47}},
        {"CT", "OS", "DM", SpeciesAngle::HarmonicForm, {83.68, 109.47}},
        {"CT", "OH", "DM", SpeciesAngle::HarmonicForm, {83.68, 109.47}},
        {"OH", "HO", "DM", SpeciesAngle::HarmonicForm, {83.68, 109.47}},
        {"CT", "S", "DM", SpeciesAngle::HarmonicForm, {83.68, 109.47}},
        {"CT", "SH", "DM", SpeciesAngle::HarmonicForm, {83.68, 109.47}},
        {"SH", "HS", "DM", SpeciesAngle::HarmonicForm, {83.68, 109.47}},
        {"NZ", "CZ", "DM", SpeciesAngle::HarmonicForm, {83.68, 90.0}},
        {"CT", "CT", "N", SpeciesAngle::HarmonicForm, {669.44, 109.7}},
        {"CT", "CT", "NM", SpeciesAngle::HarmonicForm, {669.44, 109.7}},
        {"CT", "CT", "N^", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CT", "CO", "N^", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CT", "CT", "N2", SpeciesAngle::HarmonicForm, {669.44, 111.2}},
        {"C", "CT", "N3", SpeciesAngle::HarmonicForm, {669.44, 111.2}},
        {"C", "CT", "NT", SpeciesAngle::HarmonicForm, {669.44, 111.2}},
        {"CA", "CT", "NT", SpeciesAngle::HarmonicForm, {669.44, 111.2}},
        {"CA", "CT", "NA", SpeciesAngle::HarmonicForm, {669.44, 111.2}},
        {"CT", "CT", "N3", SpeciesAngle::HarmonicForm, {669.44, 111.2}},
        {"CT", "CT", "OH", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CA", "CT", "OH", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CT", "CT", "OS", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CA", "CT", "OS", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CT", "CT", "S", SpeciesAngle::HarmonicForm, {418.4, 114.7}},
        {"CT", "CT", "SH", SpeciesAngle::HarmonicForm, {418.4, 108.6}},
        {"CA", "CT", "S", SpeciesAngle::HarmonicForm, {418.4, 114.7}},
        {"CW", "CT", "S", SpeciesAngle::HarmonicForm, {418.4, 114.7}},
        {"CT", "NT", "H", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CA", "NT", "H", SpeciesAngle::HarmonicForm, {292.88, 111.0}},
        {"CA", "NT", "CT", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CT", "NT", "CT", SpeciesAngle::HarmonicForm, {433.462, 107.2}},
        {"CT", "NT", "CH", SpeciesAngle::HarmonicForm, {433.462, 107.2}},
        {"CT", "NT", "C2", SpeciesAngle::HarmonicForm, {433.462, 107.2}},
        {"CT", "NT", "C3", SpeciesAngle::HarmonicForm, {433.462, 107.2}},
        {"CH", "NT", "CH", SpeciesAngle::HarmonicForm, {433.462, 107.2}},
        {"CH", "NT", "C2", SpeciesAngle::HarmonicForm, {433.462, 107.2}},
        {"CH", "NT", "C3", SpeciesAngle::HarmonicForm, {433.462, 107.2}},
        {"C2", "NT", "C2", SpeciesAngle::HarmonicForm, {433.462, 107.2}},
        {"C2", "NT", "C3", SpeciesAngle::HarmonicForm, {433.462, 107.2}},
        {"C3", "NT", "C3", SpeciesAngle::HarmonicForm, {433.462, 107.2}},
        {"HC", "CT", "HC", SpeciesAngle::HarmonicForm, {276.144, 107.8}},
        {"DM", "HC", "DM", SpeciesAngle::HarmonicForm, {276.144, 109.47}},
        {"DM", "HO", "DM", SpeciesAngle::HarmonicForm, {276.144, 109.47}},
        {"DM", "HS", "DM", SpeciesAngle::HarmonicForm, {276.144, 109.47}},
        {"DM", "OS", "DM", SpeciesAngle::HarmonicForm, {276.144, 109.47}},
        {"DM", "OH", "DM", SpeciesAngle::HarmonicForm, {276.144, 109.47}},
        {"DM", "S", "DM", SpeciesAngle::HarmonicForm, {276.144, 109.47}},
        {"DM", "SH", "DM", SpeciesAngle::HarmonicForm, {276.144, 109.47}},
        {"DM", "F", "DM", SpeciesAngle::HarmonicForm, {276.144, 109.47}},
        {"DM", "Cl", "DM", SpeciesAngle::HarmonicForm, {276.144, 109.47}},
        {"DM", "Br", "DM", SpeciesAngle::HarmonicForm, {276.144, 109.47}},
        {"DM", "I", "DM", SpeciesAngle::HarmonicForm, {276.144, 109.47}},
        {"CY", "CY", "HC", SpeciesAngle::HarmonicForm, {313.8, 117.2}},
        {"CY", "CY", "CY", SpeciesAngle::HarmonicForm, {251.04, 79.2}},
        {"CY", "CY", "CT", SpeciesAngle::HarmonicForm, {313.8, 117.2}},
        {"CY", "CT", "HC", SpeciesAngle::HarmonicForm, {313.8, 110.7}},
        {"HC", "CY", "HC", SpeciesAngle::HarmonicForm, {292.88, 114.3}},
        {"HC", "CY", "CT", SpeciesAngle::HarmonicForm, {292.88, 114.3}},
        {"CY", "CY", "N$", SpeciesAngle::HarmonicForm, {669.44, 89.0}},
        {"CY", "N$", "C$", SpeciesAngle::HarmonicForm, {418.4, 94.0}},
        {"N$", "C$", "CY", SpeciesAngle::HarmonicForm, {585.76, 91.0}},
        {"CY", "CY", "C$", SpeciesAngle::HarmonicForm, {527.184, 85.0}},
        {"N$", "C$", "O", SpeciesAngle::HarmonicForm, {669.44, 134.0}},
        {"CY", "C$", "O", SpeciesAngle::HarmonicForm, {669.44, 134.0}},
        {"HC", "CY", "N$", SpeciesAngle::HarmonicForm, {292.88, 111.0}},
        {"HC", "CY", "N", SpeciesAngle::HarmonicForm, {292.88, 108.0}},
        {"HC", "CY", "C$", SpeciesAngle::HarmonicForm, {313.8, 110.0}},
        {"CY", "CY", "N", SpeciesAngle::HarmonicForm, {313.8, 126.0}},
        {"HC", "CY", "S", SpeciesAngle::HarmonicForm, {313.8, 108.0}},
        {"CY", "CY", "S", SpeciesAngle::HarmonicForm, {460.24, 128.0}},
        {"CY", "N", "C", SpeciesAngle::HarmonicForm, {460.24, 128.0}},
        {"CY", "N", "H", SpeciesAngle::HarmonicForm, {334.72, 113.0}},
        {"N", "CY", "C$", SpeciesAngle::HarmonicForm, {585.76, 117.0}},
        {"N$", "CY", "S", SpeciesAngle::HarmonicForm, {460.24, 109.0}},
        {"C$", "N$", "CT", SpeciesAngle::HarmonicForm, {460.24, 127.0}},
        {"CY", "S", "CT", SpeciesAngle::HarmonicForm, {518.816, 94.0}},
        {"CY", "N$", "CT", SpeciesAngle::HarmonicForm, {418.4, 126.0}},
        {"N$", "CT", "CT", SpeciesAngle::HarmonicForm, {669.44, 110.0}},
        {"N$", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"N$", "CT", "C", SpeciesAngle::HarmonicForm, {669.44, 113.0}},
        /* { "CY",	"O$",	"CY",	SpeciesAngle::HarmonicForm,	502.08,		90.0 }, */
        /* { "CY",	"CY",	"O$",	SpeciesAngle::HarmonicForm,	418.4,		90.0 }, */
        /* { "CT",	"CY",	"O$",	SpeciesAngle::HarmonicForm,	313.8,		117.2 }, */
        /* { "HC",	"CY",	"O$",	SpeciesAngle::HarmonicForm,	313.8,		117.2 }, */
        {"HC", "CT", "N", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CT", "NM", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CT", "N^", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CO", "N^", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CT", "NA", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CT", "N2", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CT", "N3", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CT", "NT", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CT", "NC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CT", "OH", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CT", "OS", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CT", "S", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CT", "P", SpeciesAngle::HarmonicForm, {343.088, 109.5}},
        {"CT", "CT", "P", SpeciesAngle::HarmonicForm, {359.824, 109.5}},
        {"CA", "CT", "P", SpeciesAngle::HarmonicForm, {359.824, 109.5}},
        {"CT", "CT", "P+", SpeciesAngle::HarmonicForm, {359.824, 109.5}},
        {"CT", "P+", "CT", SpeciesAngle::HarmonicForm, {376.56, 109.5}},
        {"HC", "CT", "P+", SpeciesAngle::HarmonicForm, {343.088, 109.5}},
        {"HC", "CT", "SH", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"N^", "CT", "OS", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"N^", "CO", "OS", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        /* { "CC",	"CV",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
        /* { "CC",	"CV",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
        {"CW", "CW", "NB", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"HA", "CV", "NB", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"C^", "CW", "HA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"C^", "CW", "NA", SpeciesAngle::HarmonicForm, {585.76, 108.7}},
        /* { "CC",	"CW",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
        /* { "CC",	"CW",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
        {"H4", "CW", "NA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"HA", "CA", "NA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"C", "N", "C2", SpeciesAngle::HarmonicForm, {418.4, 121.9}},
        {"C", "N", "C3", SpeciesAngle::HarmonicForm, {418.4, 121.9}},
        {"C", "N", "CH", SpeciesAngle::HarmonicForm, {418.4, 121.9}},
        {"C", "N", "CT", SpeciesAngle::HarmonicForm, {418.4, 121.9}},
        {"C", "NM", "CT", SpeciesAngle::HarmonicForm, {418.4, 121.9}},
        {"C", "N", "CA", SpeciesAngle::HarmonicForm, {418.4, 121.9}},
        {"C", "N", "H", SpeciesAngle::HarmonicForm, {292.88, 119.8}},
        {"C2", "N", "C3", SpeciesAngle::HarmonicForm, {418.4, 121.9}},
        /* { "C",	"N",	"H2",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
        {"C2", "N", "CH", SpeciesAngle::HarmonicForm, {418.4, 118.0}},
        {"C2", "N", "H", SpeciesAngle::HarmonicForm, {317.984, 118.4}},
        {"C3", "N", "H", SpeciesAngle::HarmonicForm, {317.984, 118.4}},
        {"CH", "N", "H", SpeciesAngle::HarmonicForm, {317.984, 118.4}},
        {"CT", "N", "CT", SpeciesAngle::HarmonicForm, {418.4, 118.0}},
        {"CT", "N2", "CT", SpeciesAngle::HarmonicForm, {418.4, 118.0}},
        {"CT", "NM", "CT", SpeciesAngle::HarmonicForm, {418.4, 118.0}},
        {"CA", "N", "CT", SpeciesAngle::HarmonicForm, {418.4, 118.0}},
        {"CA", "NC", "CT", SpeciesAngle::HarmonicForm, {418.4, 118.0}},
        {"CT", "N", "H", SpeciesAngle::HarmonicForm, {317.984, 118.4}},
        {"H", "N", "H", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"H", "N2", "H", SpeciesAngle::HarmonicForm, {292.88, 113.0}},
        {"H3", "N", "H3", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"C", "N^", "CH", SpeciesAngle::HarmonicForm, {585.76, 117.6}},
        /* { "C",	"N^",	"CJ",	SpeciesAngle::HarmonicForm,	585.76,		121.6 }, */
        {"C", "N^", "CM", SpeciesAngle::HarmonicForm, {585.76, 121.6}},
        {"C", "NA", "CM", SpeciesAngle::HarmonicForm, {585.76, 121.6}},
        {"C", "N^", "CT", SpeciesAngle::HarmonicForm, {585.76, 117.6}},
        {"C", "N^", "CO", SpeciesAngle::HarmonicForm, {585.76, 117.6}},
        {"C", "N^", "H", SpeciesAngle::HarmonicForm, {292.88, 119.2}},
        {"C3", "N^", "CB", SpeciesAngle::HarmonicForm, {585.76, 125.8}},
        /* { "C3",	"N^",	"CE",	SpeciesAngle::HarmonicForm,	585.76,		128.8 }, */
        {"C3", "N^", "CK", SpeciesAngle::HarmonicForm, {585.76, 128.8}},
        /* { "CB",	"N^",	"CE",	SpeciesAngle::HarmonicForm,	585.76,		105.4 }, */
        {"CB", "N^", "CH", SpeciesAngle::HarmonicForm, {585.76, 125.8}},
        /* { "CE",	"N^",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		128.8 }, */
        /* { "CE",	"N^",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		128.8 }, */
        /* { "CE",	"N^",	"H",	SpeciesAngle::HarmonicForm,	292.88,		127.3 }, */
        /* { "CH",	"N^",	"CJ",	SpeciesAngle::HarmonicForm,	585.76,		121.2 }, */
        {"CH", "N^", "CK", SpeciesAngle::HarmonicForm, {585.76, 128.8}},
        /* { "CJ",	"N^",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		121.2 }, */
        /* { "CJ",	"N^",	"H",	SpeciesAngle::HarmonicForm,	292.88,		119.2 }, */
        {"CM", "N^", "CT", SpeciesAngle::HarmonicForm, {585.76, 121.2}},
        {"CM", "N^", "CO", SpeciesAngle::HarmonicForm, {585.76, 121.2}},
        {"CM", "N^", "H", SpeciesAngle::HarmonicForm, {292.88, 119.2}},
        {"CM", "NA", "H", SpeciesAngle::HarmonicForm, {292.88, 119.2}},
        {"C2", "N2", "CA", SpeciesAngle::HarmonicForm, {418.4, 123.2}},
        /* { "C2",	"N2",	"H2",	SpeciesAngle::HarmonicForm,	292.88,		118.4 }, */
        {"C2", "N2", "H3", SpeciesAngle::HarmonicForm, {292.88, 118.4}},
        {"C3", "N2", "CA", SpeciesAngle::HarmonicForm, {418.4, 123.2}},
        /* { "C3",	"N2",	"H2",	SpeciesAngle::HarmonicForm,	292.88,		118.4 }, */
        {"CA", "N2", "CT", SpeciesAngle::HarmonicForm, {418.4, 123.2}},
        {"CA", "N2", "H", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"CQ", "N2", "H", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        /* { "CA",	"N2",	"H2",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
        {"CA", "N2", "H3", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"CT", "N2", "H3", SpeciesAngle::HarmonicForm, {292.88, 118.4}},
        {"CT", "N2", "H", SpeciesAngle::HarmonicForm, {292.88, 118.4}},
        /* { "H2",	"N2",	"H2",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
        {"H3", "N2", "H3", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"C2", "N3", "H3", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"C3", "N3", "H3", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CT", "N3", "H3", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"H3", "N3", "H3", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CT", "N3", "CT", SpeciesAngle::HarmonicForm, {418.4, 113.0}},
        {"CA", "N3", "CT", SpeciesAngle::HarmonicForm, {460.24, 114.0}},
        {"C", "NA", "C", SpeciesAngle::HarmonicForm, {585.76, 126.4}},
        {"C", "N", "C", SpeciesAngle::HarmonicForm, {585.76, 126.4}},
        {"C", "NA", "CA", SpeciesAngle::HarmonicForm, {585.76, 125.2}},
        {"C", "N", "CQ", SpeciesAngle::HarmonicForm, {585.76, 125.2}},
        {"C", "NA", "H", SpeciesAngle::HarmonicForm, {292.88, 116.8}},
        {"CA", "NA", "H", SpeciesAngle::HarmonicForm, {292.88, 118.0}},
        {"CQ", "N", "H", SpeciesAngle::HarmonicForm, {292.88, 118.0}},
        /* { "CC",	"NA",	"CP",	SpeciesAngle::HarmonicForm,	585.76,		107.3 }, */
        /* { "CC",	"NA",	"CR",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
        /* { "CC",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
        /* { "CG",	"NA",	"CN",	SpeciesAngle::HarmonicForm,	585.76,		111.6 }, */
        /* { "CA",	"NA",	"CC",	SpeciesAngle::HarmonicForm,	585.76,		111.6 }, */
        /* { "CC",	"NA",	"CC",	SpeciesAngle::HarmonicForm,	585.76,		111.6 }, */
        /* { "CG",	"NA",	"CP",	SpeciesAngle::HarmonicForm,	585.76,		107.3 }, */
        /* { "CG",	"NA",	"CR",	SpeciesAngle::HarmonicForm,	585.76,		107.3 }, */
        /* { "CG",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		126.35 }, */
        {"CN", "NA", "CW", SpeciesAngle::HarmonicForm, {585.76, 111.6}},
        {"CN", "NA", "H", SpeciesAngle::HarmonicForm, {292.88, 123.1}},
        /* { "CP",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		126.35 }, */
        {"CR", "NA", "H", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"CW", "NA", "H", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"CX", "NA", "H", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"CB", "N^", "CK", SpeciesAngle::HarmonicForm, {585.76, 105.4}},
        {"CB", "N^", "CT", SpeciesAngle::HarmonicForm, {585.76, 125.8}},
        {"CB", "N^", "CO", SpeciesAngle::HarmonicForm, {585.76, 125.8}},
        {"CB", "N^", "H", SpeciesAngle::HarmonicForm, {251.04, 125.8}},
        {"CK", "N^", "CT", SpeciesAngle::HarmonicForm, {585.76, 128.8}},
        {"CK", "N^", "CO", SpeciesAngle::HarmonicForm, {585.76, 128.8}},
        {"CK", "N^", "H", SpeciesAngle::HarmonicForm, {251.04, 128.8}},
        {"CB", "NA", "CK", SpeciesAngle::HarmonicForm, {585.76, 105.4}},
        {"CB", "NA", "CT", SpeciesAngle::HarmonicForm, {585.76, 125.8}},
        {"CB", "NA", "H", SpeciesAngle::HarmonicForm, {251.04, 125.8}},
        {"CK", "NA", "CT", SpeciesAngle::HarmonicForm, {585.76, 128.8}},
        {"CK", "NA", "H", SpeciesAngle::HarmonicForm, {251.04, 128.8}},
        /* { "CB",	"NB",	"CE",	SpeciesAngle::HarmonicForm,	585.76,		103.8 }, */
        {"CB", "NB", "CK", SpeciesAngle::HarmonicForm, {585.76, 103.8}},
        /* { "CC",	"NB",	"CP",	SpeciesAngle::HarmonicForm,	585.76,		105.3 }, */
        /* { "CC",	"NB",	"CR",	SpeciesAngle::HarmonicForm,	585.76,		117.0 }, */
        /* { "CF",	"NB",	"CP",	SpeciesAngle::HarmonicForm,	585.76,		105.3 }, */
        /* { "CF",	"NB",	"CR",	SpeciesAngle::HarmonicForm,	585.76,		105.3 }, */
        {"CR", "NB", "CV", SpeciesAngle::HarmonicForm, {585.76, 110.0}},
        {"CR", "NB", "CB", SpeciesAngle::HarmonicForm, {585.76, 110.0}},
        {"CR", "NB", "CW", SpeciesAngle::HarmonicForm, {585.76, 110.0}},
        {"C", "NC", "CA", SpeciesAngle::HarmonicForm, {585.76, 120.5}},
        {"CA", "NC", "CB", SpeciesAngle::HarmonicForm, {585.76, 112.2}},
        /* { "CA",	"NC",	"CI",	SpeciesAngle::HarmonicForm,	585.76,		118.6 }, */
        {"CA", "NC", "CQ", SpeciesAngle::HarmonicForm, {585.76, 118.6}},
        {"CQ", "NC", "CQ", SpeciesAngle::HarmonicForm, {585.76, 118.6}},
        /* { "CB",	"NC",	"CI",	SpeciesAngle::HarmonicForm,	585.76,		111.0 }, */
        {"CB", "NC", "CQ", SpeciesAngle::HarmonicForm, {585.76, 111.0}},
        {"CR", "NC", "CQ", SpeciesAngle::HarmonicForm, {585.76, 111.0}},
        {"C2", "NT", "H", SpeciesAngle::HarmonicForm, {361.498, 108.1}},
        {"C3", "NT", "H", SpeciesAngle::HarmonicForm, {361.498, 108.1}},
        {"CH", "NT", "H", SpeciesAngle::HarmonicForm, {361.498, 108.1}},
        {"H", "NT", "H", SpeciesAngle::HarmonicForm, {364.845, 106.4}},
        {"H", "N3", "H", SpeciesAngle::HarmonicForm, {364.845, 109.5}},
        {"H", "N", "OH", SpeciesAngle::HarmonicForm, {292.88, 110.2}},
        {"C", "N", "OH", SpeciesAngle::HarmonicForm, {384.928, 115.7}},
        {"N", "OH", "HO", SpeciesAngle::HarmonicForm, {410.032, 105.4}},
        {"C", "OH", "HO", SpeciesAngle::HarmonicForm, {292.88, 113.0}},
        {"CA", "OH", "HO", SpeciesAngle::HarmonicForm, {292.88, 113.0}},
        {"CM", "OH", "HO", SpeciesAngle::HarmonicForm, {292.88, 109.0}},
        {"C2", "OH", "HO", SpeciesAngle::HarmonicForm, {460.24, 108.5}},
        {"C3", "OH", "HO", SpeciesAngle::HarmonicForm, {460.24, 108.5}},
        {"CH", "OH", "HO", SpeciesAngle::HarmonicForm, {460.24, 108.5}},
        {"CT", "OH", "HO", SpeciesAngle::HarmonicForm, {460.24, 108.5}},
        {"HO", "OH", "P", SpeciesAngle::HarmonicForm, {460.24, 108.5}},
        {"C2", "OS", "C2", SpeciesAngle::HarmonicForm, {836.8, 111.8}},
        {"C2", "OS", "C3", SpeciesAngle::HarmonicForm, {836.8, 111.8}},
        {"CH", "C", "OS", SpeciesAngle::HarmonicForm, {677.808, 111.4}},
        {"C", "OS", "CH", SpeciesAngle::HarmonicForm, {694.544, 116.9}},
        {"C", "OS", "C2", SpeciesAngle::HarmonicForm, {694.544, 116.9}},
        {"C", "OS", "C3", SpeciesAngle::HarmonicForm, {694.544, 116.9}},
        {"O", "C", "OS", SpeciesAngle::HarmonicForm, {694.544, 123.4}},
        {"C", "OS", "CT", SpeciesAngle::HarmonicForm, {694.544, 116.9}},
        {"OS", "C", "CT", SpeciesAngle::HarmonicForm, {677.808, 111.4}},
        {"OS", "C", "CA", SpeciesAngle::HarmonicForm, {677.808, 111.4}},
        {"C", "OS", "CA", SpeciesAngle::HarmonicForm, {694.544, 116.9}},
        {"CA", "CH", "OS", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"OS", "CO", "OH", SpeciesAngle::HarmonicForm, {774.877, 111.55}},
        {"OS", "CO", "OS", SpeciesAngle::HarmonicForm, {774.877, 111.55}},
        {"C3", "OS", "CO", SpeciesAngle::HarmonicForm, {836.8, 113.0}},
        {"C2", "OS", "CO", SpeciesAngle::HarmonicForm, {836.8, 113.0}},
        {"CH", "OS", "CO", SpeciesAngle::HarmonicForm, {836.8, 113.0}},
        {"C2", "CO", "OS", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"C3", "CO", "OS", SpeciesAngle::HarmonicForm, {669.44, 109.5}},
        {"C3", "CO", "C3", SpeciesAngle::HarmonicForm, {334.72, 109.5}},
        {"C2", "C2", "CO", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"C3", "C2", "CO", SpeciesAngle::HarmonicForm, {527.184, 112.4}},
        {"OS", "CO", "CT", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CO", "CT", "CT", SpeciesAngle::HarmonicForm, {488.273, 112.7}},
        {"CT", "CO", "OH", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CT", "CO", "HC", SpeciesAngle::HarmonicForm, {313.8, 110.7}},
        {"CO", "OH", "HO", SpeciesAngle::HarmonicForm, {460.24, 108.5}},
        {"OS", "CO", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CO", "OS", "CT", SpeciesAngle::HarmonicForm, {502.08, 109.5}},
        {"CO", "CT", "HC", SpeciesAngle::HarmonicForm, {313.8, 110.7}},
        {"CO", "CT", "OH", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"OH", "CO", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "CO", "HC", SpeciesAngle::HarmonicForm, {276.144, 109.5}},
        {"C2", "OS", "HO", SpeciesAngle::HarmonicForm, {460.24, 108.5}},
        {"CA", "OS", "P", SpeciesAngle::HarmonicForm, {836.8, 120.5}},
        {"C2", "OS", "P", SpeciesAngle::HarmonicForm, {836.8, 120.5}},
        {"C3", "OS", "P", SpeciesAngle::HarmonicForm, {836.8, 120.5}},
        {"CH", "OS", "CH", SpeciesAngle::HarmonicForm, {836.8, 111.8}},
        {"CH", "OS", "HO", SpeciesAngle::HarmonicForm, {460.24, 108.5}},
        {"CH", "OS", "P", SpeciesAngle::HarmonicForm, {836.8, 120.5}},
        {"CT", "OS", "CT", SpeciesAngle::HarmonicForm, {502.08, 109.5}},
        {"Si", "OS", "Si", SpeciesAngle::HarmonicForm, {167.36, 145.0}},
        {"CT", "OS", "Si", SpeciesAngle::HarmonicForm, {334.72, 130.0}},
        {"CT", "Si", "OS", SpeciesAngle::HarmonicForm, {502.08, 100.0}},
        {"CT", "Si", "CT", SpeciesAngle::HarmonicForm, {502.08, 110.0}},
        {"CT", "CT", "Si", SpeciesAngle::HarmonicForm, {502.08, 112.0}},
        {"OS", "Si", "OS", SpeciesAngle::HarmonicForm, {502.08, 110.0}},
        {"Si", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "Si", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"HC", "Si", "CT", SpeciesAngle::HarmonicForm, {292.88, 110.5}},
        {"H", "Si", "H", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"H", "Si", "CT", SpeciesAngle::HarmonicForm, {292.88, 110.5}},
        {"F", "Si", "CT", SpeciesAngle::HarmonicForm, {292.88, 110.5}},
        {"Cl", "Si", "CT", SpeciesAngle::HarmonicForm, {292.88, 110.5}},
        {"Br", "Si", "CT", SpeciesAngle::HarmonicForm, {292.88, 110.5}},
        {"I", "Si", "CT", SpeciesAngle::HarmonicForm, {292.88, 110.5}},
        {"CT", "OS", "CA", SpeciesAngle::HarmonicForm, {627.6, 111.0}},
        {"CT", "OS", "CM", SpeciesAngle::HarmonicForm, {627.6, 111.0}},
        {"CT", "OS", "P", SpeciesAngle::HarmonicForm, {836.8, 120.5}},
        {"CT", "OH", "P", SpeciesAngle::HarmonicForm, {836.8, 120.5}},
        {"O", "C", "O2", SpeciesAngle::HarmonicForm, {669.44, 126.0}},
        {"O2", "P", "O2", SpeciesAngle::HarmonicForm, {1171.52, 119.9}},
        {"O2", "P", "OH", SpeciesAngle::HarmonicForm, {376.56, 108.23}},
        {"O2", "P", "OS", SpeciesAngle::HarmonicForm, {836.8, 108.23}},
        {"OH", "P", "OS", SpeciesAngle::HarmonicForm, {376.56, 102.6}},
        {"OS", "P", "OS", SpeciesAngle::HarmonicForm, {376.56, 102.6}},
        {"O", "P", "OH", SpeciesAngle::HarmonicForm, {836.8, 108.23}},
        {"O", "P", "OS", SpeciesAngle::HarmonicForm, {836.8, 108.23}},
        {"OH", "P", "OH", SpeciesAngle::HarmonicForm, {376.56, 102.6}},
        {"CT", "P", "OS", SpeciesAngle::HarmonicForm, {376.56, 109.5}},
        {"CT", "P", "O2", SpeciesAngle::HarmonicForm, {376.56, 109.5}},
        {"CT", "P", "O", SpeciesAngle::HarmonicForm, {376.56, 109.5}},
        {"CA", "P", "OS", SpeciesAngle::HarmonicForm, {376.56, 109.5}},
        {"CA", "P", "OH", SpeciesAngle::HarmonicForm, {376.56, 109.5}},
        {"CA", "P", "O", SpeciesAngle::HarmonicForm, {376.56, 109.5}},
        {"C2", "S", "C3", SpeciesAngle::HarmonicForm, {518.816, 98.9}},
        {"C2", "S", "LP", SpeciesAngle::HarmonicForm, {1255.2, 96.7}},
        {"C2", "S", "S", SpeciesAngle::HarmonicForm, {569.024, 103.7}},
        {"C3", "S", "LP", SpeciesAngle::HarmonicForm, {1255.2, 96.7}},
        {"C3", "S", "S", SpeciesAngle::HarmonicForm, {569.024, 103.7}},
        {"CT", "S", "CT", SpeciesAngle::HarmonicForm, {518.816, 98.9}},
        {"CR", "S", "CW", SpeciesAngle::HarmonicForm, {619.232, 90.0}},
        {"CW", "S", "CW", SpeciesAngle::HarmonicForm, {619.232, 97.0}},
        {"CT", "S", "LP", SpeciesAngle::HarmonicForm, {1255.2, 96.7}},
        {"CT", "S", "S", SpeciesAngle::HarmonicForm, {569.024, 103.7}},
        {"LP", "S", "LP", SpeciesAngle::HarmonicForm, {0, 160.0}},
        {"LP", "S", "S", SpeciesAngle::HarmonicForm, {1255.2, 96.7}},
        {"C2", "SH", "HS", SpeciesAngle::HarmonicForm, {368.192, 96.0}},
        {"C2", "SH", "LP", SpeciesAngle::HarmonicForm, {1255.2, 96.7}},
        {"C3", "SH", "HS", SpeciesAngle::HarmonicForm, {368.192, 96.0}},
        {"C3", "SH", "LP", SpeciesAngle::HarmonicForm, {1255.2, 96.7}},
        {"CT", "SH", "HS", SpeciesAngle::HarmonicForm, {368.192, 96.0}},
        {"CA", "SH", "HS", SpeciesAngle::HarmonicForm, {418.4, 96.0}},
        {"CT", "SH", "LP", SpeciesAngle::HarmonicForm, {1255.2, 96.7}},
        {"HS", "SH", "HS", SpeciesAngle::HarmonicForm, {292.88, 92.07}},
        {"HS", "SH", "LP", SpeciesAngle::HarmonicForm, {1255.2, 96.7}},
        {"LP", "SH", "LP", SpeciesAngle::HarmonicForm, {0, 160.0}},
        {"P", "OS", "P", SpeciesAngle::HarmonicForm, {836.8, 120.5}},
        {"C9", "C8", "C8", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C9", "C8", "C7", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C9", "C8", "CT", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C9", "C8", "CH", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C9", "C8", "C2", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C9", "C8", "C3", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C9", "C7", "C8", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C9", "C7", "C7", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C9", "C7", "CT", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C9", "C7", "CH", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C9", "C7", "C2", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C9", "C7", "C3", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C8", "C8", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C8", "C7", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C8", "CT", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C8", "CH", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C8", "C2", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C8", "C3", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C7", "C8", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C7", "C7", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C7", "CT", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C7", "CH", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C7", "C2", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "C7", "C3", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C7", "C7", "C7", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C7", "C7", "CT", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C7", "C7", "CH", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C7", "C7", "C2", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C7", "C7", "C3", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"CT", "C8", "CT", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CT", "C8", "CH", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CT", "C8", "C2", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CT", "C8", "C3", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CH", "C8", "CH", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CH", "C8", "C3", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"C2", "C8", "CH", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"C2", "C8", "C2", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"C2", "C8", "C3", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"C3", "C8", "C3", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CT", "C7", "CT", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CT", "C7", "CH", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CT", "C7", "C2", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CT", "C7", "C3", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CH", "C7", "CH", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CH", "C7", "C2", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CH", "C7", "C3", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"C2", "C7", "C2", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"C2", "C7", "C3", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"C3", "C7", "C3", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CA", "OS", "C2", SpeciesAngle::HarmonicForm, {836.8, 111.8}},
        {"CA", "CT", "CA", SpeciesAngle::HarmonicForm, {334.72, 109.5}},
        {"CA", "CT", "C2", SpeciesAngle::HarmonicForm, {527.184, 114.0}},
        {"CA", "CT", "C", SpeciesAngle::HarmonicForm, {527.184, 112.0}},
        {"N", "CT", "C2", SpeciesAngle::HarmonicForm, {669.44, 109.7}},
        {"HC", "CT", "C2", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"C", "CT", "C2", SpeciesAngle::HarmonicForm, {527.184, 111.1}},
        {"CT", "CA", "NA", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CA", "NA", "CA", SpeciesAngle::HarmonicForm, {585.76, 125.2}},
        {"CA", "CA", "NB", SpeciesAngle::HarmonicForm, {585.76, 108.7}},
        {"NA", "CA", "NB", SpeciesAngle::HarmonicForm, {585.76, 123.3}},
        {"CA", "NB", "CA", SpeciesAngle::HarmonicForm, {585.76, 125.2}},
        {"HA", "CA", "NB", SpeciesAngle::HarmonicForm, {292.88, 119.1}},
        {"CA", "CA", "N", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CA", "N", "H", SpeciesAngle::HarmonicForm, {292.88, 119.8}},
        {"CB", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CA", "CB", "CT", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CB", "CA", "NA", SpeciesAngle::HarmonicForm, {585.76, 108.7}},
        /* { "CA",	"CA",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		120.0 }, */
        {"CB", "CB", "CT", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        /* { "CB",	"CB",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		120.0 }, */
        {"CB", "CT", "CT", SpeciesAngle::HarmonicForm, {527.184, 114.0}},
        {"CT", "CT", "F", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"C2", "C2", "F", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CA", "CT", "F", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"CM", "CT", "F", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"F", "CT", "F", SpeciesAngle::HarmonicForm, {644.336, 109.1}},
        {"HC", "CT", "F", SpeciesAngle::HarmonicForm, {334.72, 107.0}},
        {"CT", "C", "C", SpeciesAngle::HarmonicForm, {669.44, 117.2}},
        {"C", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 121.4}},
        {"C", "C", "N", SpeciesAngle::HarmonicForm, {585.76, 116.6}},
        {"C9", "C8", "SY", SpeciesAngle::HarmonicForm, {585.76, 118.0}},
        {"C8", "SY", "C3", SpeciesAngle::HarmonicForm, {518.816, 98.9}},
        {"OY", "SY", "N", SpeciesAngle::HarmonicForm, {1004.16, 107.0}},
        {"OY", "SY", "OY", SpeciesAngle::HarmonicForm, {870.272, 119.0}},
        {"OY", "SZ", "CT", SpeciesAngle::HarmonicForm, {619.232, 107.0}},
        {"OY", "SY", "CT", SpeciesAngle::HarmonicForm, {619.232, 108.9}},
        {"OY", "SY", "CA", SpeciesAngle::HarmonicForm, {619.232, 107.2}},
        {"N", "SY", "CA", SpeciesAngle::HarmonicForm, {836.8, 103.0}},
        {"SY", "CA", "CA", SpeciesAngle::HarmonicForm, {711.28, 119.4}},
        {"SY", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"SZ", "CT", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CT", "SY", "CT", SpeciesAngle::HarmonicForm, {518.816, 102.0}},
        {"CA", "SY", "CT", SpeciesAngle::HarmonicForm, {518.816, 102.0}},
        {"CR", "SY", "CT", SpeciesAngle::HarmonicForm, {518.816, 102.0}},
        {"CT", "SZ", "CT", SpeciesAngle::HarmonicForm, {518.816, 96.0}},
        {"CT", "CT", "SY", SpeciesAngle::HarmonicForm, {418.4, 108.6}},
        {"CT", "CT", "SZ", SpeciesAngle::HarmonicForm, {418.4, 108.6}},
        {"N", "SY", "CT", SpeciesAngle::HarmonicForm, {836.8, 103.0}},
        {"SY", "N", "CT", SpeciesAngle::HarmonicForm, {418.4, 120.0}},
        {"H", "N", "SY", SpeciesAngle::HarmonicForm, {836.8, 111.0}},
        {"OS", "C", "N", SpeciesAngle::HarmonicForm, {677.808, 111.4}},
        {"CT", "NT", "SY", SpeciesAngle::HarmonicForm, {418.4, 108.6}},
        {"C", "CT", "F", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"SY", "CT", "F", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"SY", "NT", "H", SpeciesAngle::HarmonicForm, {292.88, 115.0}},
        {"C", "CW", "NA", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        {"NT", "C", "CW", SpeciesAngle::HarmonicForm, {585.76, 116.0}},
        {"C", "CW", "CS", SpeciesAngle::HarmonicForm, {711.28, 120.0}},
        {"CB", "CS", "HA", SpeciesAngle::HarmonicForm, {292.88, 120.0}},
        {"CW", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 120.4}},
        {"C", "NT", "CT", SpeciesAngle::HarmonicForm, {527.184, 111.1}},
        {"C", "CT", "C", SpeciesAngle::HarmonicForm, {527.184, 111.1}},
        {"C", "CT", "OS", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"N", "CT", "OS", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"NT", "C", "O", SpeciesAngle::HarmonicForm, {669.44, 120.4}},
        {"NT", "C", "CT", SpeciesAngle::HarmonicForm, {585.76, 116.0}},
        {"CA", "NT", "C", SpeciesAngle::HarmonicForm, {527.184, 112.0}},
        {"CA", "NT", "SY", SpeciesAngle::HarmonicForm, {418.4, 108.6}},
        {"OY", "SY", "NT", SpeciesAngle::HarmonicForm, {619.232, 108.9}},
        {"NT", "SY", "CT", SpeciesAngle::HarmonicForm, {518.816, 102.0}},
        {"NT", "CT", "S", SpeciesAngle::HarmonicForm, {418.4, 114.7}},
        {"HC", "CY", "NT", SpeciesAngle::HarmonicForm, {292.88, 114.3}},
        {"CY", "CY", "NT", SpeciesAngle::HarmonicForm, {313.8, 117.2}},
        {"CA", "NT", "CY", SpeciesAngle::HarmonicForm, {418.4, 109.5}},
        {"NC", "CA", "Cl", SpeciesAngle::HarmonicForm, {627.6, 120.0}},
        {"CA", "NT", "CA", SpeciesAngle::HarmonicForm, {334.72, 109.5}},
        {"NC", "CA", "NT", SpeciesAngle::HarmonicForm, {585.76, 116.0}},
        {"CM", "CM", "CY", SpeciesAngle::HarmonicForm, {585.76, 124.0}},
        {"CM", "CY", "HC", SpeciesAngle::HarmonicForm, {292.88, 109.5}},
        {"CM", "CY", "CY", SpeciesAngle::HarmonicForm, {527.184, 114.0}},
        {"C", "CM", "CY", SpeciesAngle::HarmonicForm, {585.76, 119.7}},
        {"N^", "CM", "CT", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"NA", "CM", "CT", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"S", "CM", "CM", SpeciesAngle::HarmonicForm, {711.28, 119.4}},
        {"S", "CM", "N^", SpeciesAngle::HarmonicForm, {711.28, 119.4}},
        {"S", "CM", "NA", SpeciesAngle::HarmonicForm, {711.28, 119.4}},
        {"N^", "CM", "OS", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"NA", "CM", "OS", SpeciesAngle::HarmonicForm, {585.76, 120.0}},
        {"CA", "S", "CM", SpeciesAngle::HarmonicForm, {518.816, 104.2}},
        {"CM", "OS", "CA", SpeciesAngle::HarmonicForm, {627.6, 111.0}},
        {"CM", "CT", "CA", SpeciesAngle::HarmonicForm, {334.72, 109.5}},
        {"S", "CA", "CA", SpeciesAngle::HarmonicForm, {711.28, 119.4}},
        {"P", "CA", "CA", SpeciesAngle::HarmonicForm, {711.28, 119.4}},
        {"CA", "S", "CT", SpeciesAngle::HarmonicForm, {518.816, 104.2}},
        {"C", "N", "Zn", SpeciesAngle::HarmonicForm, {167.36, 126.0}},
        {"HO", "OH", "Zn", SpeciesAngle::HarmonicForm, {836.8, 126.0}},
        {"N", "Zn", "N", SpeciesAngle::HarmonicForm, {167.36, 109.5}},
        {"N", "Zn", "O", SpeciesAngle::HarmonicForm, {167.36, 109.5}}};

    return Forcefield::termMatch_(angleTerms, i, j, k);
}

// Return torsion term for the supplied atom type quartet (if it exists)
OptionalReferenceWrapper<const ForcefieldTorsionTerm>
OPLSAA2005BaseForcefield::getTorsionTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k,
                                         const ForcefieldAtomType &l) const
{
    static std::vector<ForcefieldTorsionTerm> torsionTerms = {
        //	i	j	k	l	Type (CosineForm)		k		n	eq	s
        {"HC", "CT", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // hydrocarbon
        {"HC", "C", "C", "HC", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                           //
        {"HC", "C=", "C=", "HC", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         //
        {"CT", "C=", "C=", "CT", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         //
        {"CT", "C=", "C=", "HC", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         //
        {"H3", "N3", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // ammonium
        {"HC", "CT", "CT", "CT", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // hydrocarbon
        {"HC", "CT", "CT", "CO", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // acetal
        {"HC", "CO", "CT", "CT", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // acetal
        {"CT", "CT", "CT", "CT", SpeciesTorsion::Cos3Form, {5.4392, -0.2092, 0.8368}},              // hydrocarbon
        {"CT", "CT", "CT", "CO", SpeciesTorsion::Cos3Form, {5.4392, -0.2092, 0.8368}},              // hydrocarbon
        {"HC", "CT", "CT", "OH", SpeciesTorsion::Cos3Form, {0, 0, 1.95811}},                        // alcohols,
        {"HC", "CT", "CT", "OS", SpeciesTorsion::Cos3Form, {0, 0, 1.95811}},                        // alcohols,
        {"HC", "CT", "CO", "OS", SpeciesTorsion::Cos3Form, {0, 0, 1.95811}},                        // alcohols,
        {"CT", "CT", "CT", "O*", SpeciesTorsion::Cos3Form, {7.15882, -2.092, 2.77399}},             // alcohols,
        {"CT", "CT", "CO", "O*", SpeciesTorsion::Cos3Form, {7.15882, -2.092, 2.77399}},             // alcohols,
        {"HC", "CM", "CT", "O*", SpeciesTorsion::Cos3Form, {0, 0, 1.95811}},                        // alcohols,
        {"CT", "CM", "CT", "O*", SpeciesTorsion::Cos3Form, {7.15882, -2.092, 2.77399}},             // alcohols,
        {"CT", "CT", "CT", "OH", SpeciesTorsion::Cos3Form, {-6.49357, 0, 0}},                       // polyols
        {"HC", "CT", "OH", "HO", SpeciesTorsion::Cos3Form, {0, 0, 1.47444}},                        // alcohols
        {"CT", "CT", "OH", "HO", SpeciesTorsion::Cos3Form, {-1.4895, -0.728016, 2.05853}},          // alcohols
        {"OH", "CT", "CT", "OH", SpeciesTorsion::Cos3Form, {39.7815, 0, 0}},                        // diols
        {"OH", "CT", "CT", "OH", SpeciesTorsion::Cos3Form, {51.1871, 0, 0}},                        // triols
        {"HC", "CT", "OS", "C^", SpeciesTorsion::Cos3Form, {0, 0, 3.17984}},                        // ethers
        {"HC", "CT", "OS", "CA", SpeciesTorsion::Cos3Form, {0, 0, 3.17984}},                        // ethers
        {"HC", "CT", "OS", "CM", SpeciesTorsion::Cos3Form, {0, 0, 3.17984}},                        // ethers
        {"HC", "CM", "OS", "CT", SpeciesTorsion::Cos3Form, {0, 0, 3.17984}},                        // ethers
        {"HC", "CO", "OS", "CT", SpeciesTorsion::Cos3Form, {0, 0, 3.17984}},                        // ethers
        {"HC", "CT", "OS", "CO", SpeciesTorsion::Cos3Form, {0, 0, 3.17984}},                        // ethers
        {"CT", "OS", "CT", "CT", SpeciesTorsion::Cos3Form, {2.7196, -1.046, 2.80328}},              // ethers
        {"CT", "OS", "CM", "CT", SpeciesTorsion::Cos3Form, {2.7196, -1.046, 2.80328}},              // ethers
        {"CT", "OS", "CO", "CT", SpeciesTorsion::Cos3Form, {2.7196, -1.046, 2.80328}},              // ethers
        {"CT", "OS", "CT", "O*", SpeciesTorsion::Cos3Form, {-2.17986, -8.44331, 8.35126}},          // acetals
        {"NT", "CT", "CT", "OH", SpeciesTorsion::Cos3Form, {33.472, 0, 0}},                         // 2-aminoethanol
        {"OS", "CT", "CT", "OS", SpeciesTorsion::Cos3Form, {-2.3012, 0, 0}},                        // polyethers,
        {"CT", "OS", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 12.552, 0}},                         // anisole
        {"CM", "C=", "C=", "CT", SpeciesTorsion::Cos3Form, {0, 0, -1.55645}},                       // diene-
        {"CM", "C=", "C=", "HC", SpeciesTorsion::Cos3Form, {0, 0, -1.55645}},                       // diene-
        {"CM", "C=", "C=", "CM", SpeciesTorsion::Cos3Form, {5.95383, 16.9661, 3.58987}},            // diene
        {"HC", "CT", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // ethyl
        {"H3", "N3", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // anilinium
        {"HC", "CT", "CW", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"HC", "CT", "CV", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"HC", "CT", "CR", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"HC", "CT", "CS", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"HC", "CT", "CQ", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"HC", "CT", "CU", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"HC", "CT", "CK", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"HC", "CT", "C^", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"CT", "CT", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // ethyl
        {"CT", "N3", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // anilinium
        {"CT", "CT", "CW", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"CT", "CT", "CV", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"CT", "CT", "CR", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"CT", "CT", "CS", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"CT", "CT", "CQ", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"CT", "CT", "CU", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"CT", "CT", "CK", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"CT", "CT", "C^", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aromatics
        {"O*", "CT", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // benzyl
        {"C^", "CT", "NA", "C^", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // heterocycles
        {"H*", "CT", "NA", "C^", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // heterocycles
        {"C^", "CT", "N^", "C^", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // heterocycles
        {"H*", "CT", "N^", "C^", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // heterocycles
        {"O", "C", "CR", "O*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // heterocycles
        {"O", "C", "CR", "N^", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // heterocycles
        {"CA", "CA", "CT", "N^", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // aromatics
        {"*", "Zn", "N", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                 // JACS
        {"*", "Zn", "O", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                 // JACS
        {"HC", "CT", "CT", "CA", SpeciesTorsion::Cos3Form, {0, 0, 1.93301}},                        // ethyl
        {"HC", "CT", "N3", "CA", SpeciesTorsion::Cos3Form, {0, 0, 1.93301}},                        // anilinium
        {"HC", "CT", "CT", "CW", SpeciesTorsion::Cos3Form, {0, 0, 1.93301}},                        // aromatics
        {"HC", "CT", "CT", "CV", SpeciesTorsion::Cos3Form, {0, 0, 1.93301}},                        // aromatics
        {"HC", "CT", "CT", "CR", SpeciesTorsion::Cos3Form, {0, 0, 1.93301}},                        // aromatics
        {"HC", "CT", "CT", "CS", SpeciesTorsion::Cos3Form, {0, 0, 1.93301}},                        // aromatics
        {"HC", "CT", "CT", "CQ", SpeciesTorsion::Cos3Form, {0, 0, 1.93301}},                        // aromatics
        {"HC", "CT", "CT", "CU", SpeciesTorsion::Cos3Form, {0, 0, 1.93301}},                        // aromatics
        {"HC", "CT", "CT", "CK", SpeciesTorsion::Cos3Form, {0, 0, 1.93301}},                        // aromatics
        {"HC", "CT", "CT", "C^", SpeciesTorsion::Cos3Form, {0, 0, 1.93301}},                        // aromatics
        {"CT", "S", "S", "CT", SpeciesTorsion::Cos3Form, {0, -31.0202, 7.13372}},                   // disulfide
        {"HC", "CT", "S", "S", SpeciesTorsion::Cos3Form, {0, 0, 2.33467}},                          // disulfide
        {"CT", "CT", "S", "S", SpeciesTorsion::Cos3Form, {8.12114, -3.49782, 3.91204}},             // disulfide
        {"HC", "CT", "SH", "HS", SpeciesTorsion::Cos3Form, {0, 0, 2.00832}},                        // thiol
        {"CT", "CT", "SH", "HS", SpeciesTorsion::Cos3Form, {-3.17566, -1.17989, 2.84512}},          // thiol
        {"HC", "CT", "CT", "SH", SpeciesTorsion::Cos3Form, {0, 0, 1.89117}},                        // thiol
        {"HC", "CT", "CT", "SY", SpeciesTorsion::Cos3Form, {0, 0, 1.89117}},                        //
        {"HC", "CT", "CT", "S", SpeciesTorsion::Cos3Form, {0, 0, 1.89117}},                         // sulfide
        {"CT", "CT", "CT", "SH", SpeciesTorsion::Cos3Form, {5.28021, -0.828432, 1.94556}},          // thiol
        {"CT", "CT", "CT", "SY", SpeciesTorsion::Cos3Form, {5.28021, -0.828432, 1.94556}},          // (mod
        {"HC", "CT", "NT", "H", SpeciesTorsion::Cos3Form, {0, 0, 1.6736}},                          // amine
        {"HC", "CT", "CT", "NT", SpeciesTorsion::Cos3Form, {-4.23839, -2.96646, 1.97903}},          // amine
        {"CT", "CT", "NT", "H", SpeciesTorsion::Cos3Form, {-0.79496, -1.74473, 1.74891}},           // amine
        {"CT", "CT", "N2", "H", SpeciesTorsion::Cos3Form, {-0.79496, -1.74473, 1.74891}},           // guanidinium
        {"CT", "CT", "N2", "H3", SpeciesTorsion::Cos3Form, {-0.79496, -1.74473, 1.74891}},          // guanidinium
        {"CT", "CT", "CT", "NT", SpeciesTorsion::Cos3Form, {10.0081, -2.82002, 2.3012}},            // amine
        {"HO", "OH", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 7.03749, 0}},                        // phenol
        {"N", "CT", "CT", "CT", SpeciesTorsion::Cos3Form, {3.53548, -4.02501, 2.98319}},            // Chi-1
        {"N", "CT", "CT", "C^", SpeciesTorsion::Cos3Form, {3.53548, -4.02501, 2.98319}},            // Chi-1
        {"N", "CT", "CT", "CA", SpeciesTorsion::Cos3Form, {3.53548, -4.02501, 2.98319}},            // Chi-1
        {"C", "N", "CT", "C", SpeciesTorsion::Cos3Form, {-9.89516, 3.81581, -3.5564}},              // Phi
        {"N", "CT", "C", "N", SpeciesTorsion::Cos3Form, {7.59814, 5.11285, 6.6149}},                // Psi
        {"C", "N", "CT", "CT", SpeciesTorsion::Cos3Form, {0, 1.93301, 0}},                          // Phi'
        {"CT", "CT", "C", "N", SpeciesTorsion::Cos3Form, {4.90783, 0.790776, -5.0208}},             // Psi'
        {"C", "N", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // Phi"
        {"HC", "CT", "C", "N", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // Psi"
        {"HC", "CM", "C", "N", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                //
        {"HC", "CT", "C", "NM", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               //
        {"HC", "CM", "C", "NM", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               //
        {"CQ", "N", "CT", "CT", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               //
        {"CQ", "N", "CT", "CA", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               //
        {"H", "N", "CT", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                 // peptides
        {"H", "N", "CT", "CT", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // peptides
        {"H", "N", "CT", "C", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                 // peptides
        {"*", "CT", "C", "O", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                 // peptides
        {"CT", "CT", "C", "O", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // peptides
        {"N", "CT", "C", "O", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                 // peptides
        {"CT", "C", "N", "H", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                           // amides
        {"CM", "C", "N", "H", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                           // amides
        {"HC", "C", "N", "H", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                           // amides
        {"CT", "C", "NM", "CT", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                         // tert.
        {"O", "C", "NM", "CT", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                          // tert.
        {"CT", "C", "N", "CT", SpeciesTorsion::Cos3Form, {9.6232, 25.4764, 0}},                     // amides-
        {"CT", "C", "N", "CA", SpeciesTorsion::Cos3Form, {9.6232, 25.4764, 0}},                     //
        {"HC", "C", "N", "CT", SpeciesTorsion::Cos3Form, {9.6232, 25.4764, 0}},                     // amides-
        {"O", "C", "N", "H", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                            // amides
        {"O", "C", "N", "CT", SpeciesTorsion::Cos3Form, {0, 25.4764, 0}},                           // amides
        {"O", "C", "N", "CA", SpeciesTorsion::Cos3Form, {0, 25.4764, 0}},                           // amides
        {"O", "C", "N", "CM", SpeciesTorsion::Cos3Form, {0, 25.4764, 0}},                           // amides
        {"N", "C", "N", "H", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                            // imides
        {"N", "C", "N", "C", SpeciesTorsion::Cos3Form, {9.6232, 25.4764, 0}},                       // imides
        {"CT", "N", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // tert.
        {"CT", "N2", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // tert.
        {"CA", "N", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // "
        {"CT", "NM", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // tert.
        {"CA", "NM", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // "
        {"C^", "CA", "SH", "HS", SpeciesTorsion::Cos3Form, {0, 4.6024, 0}},                         // aromatic
        {"CA", "CA", "SH", "HS", SpeciesTorsion::Cos3Form, {0, 4.6024, 0}},                         // aromatic
        {"N^", "CA", "SH", "HS", SpeciesTorsion::Cos3Form, {-12.552, 0, 0}},                        // "
        {"HC", "CT", "S", "CT", SpeciesTorsion::Cos3Form, {0, 0, 2.70705}},                         // sulfide
        {"HC", "CT", "S", "CA", SpeciesTorsion::Cos3Form, {0, 0, 2.70705}},                         // sulfide
        {"CT", "CT", "CT", "S", SpeciesTorsion::Cos3Form, {10.9579, -2.59408, 1.07947}},            // sulfide
        {"CT", "CT", "S", "CT", SpeciesTorsion::Cos3Form, {3.8702, -2.40998, 2.83257}},             // sulfide
        {"H", "N2", "CA", "N2", SpeciesTorsion::Cos3Form, {0, 16.3176, 0}},                         // guanidinium
        {"H", "N2", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 16.3176, 0}},                         // guanidinium
        {"CT", "N2", "CA", "N2", SpeciesTorsion::Cos3Form, {0, 33.2042, 0}},                        // methylguanidinium
        {"HC", "CT", "N2", "CA", SpeciesTorsion::Cos3Form, {0, 0, 0.740568}},                       // methylguanidinium
        {"HC", "CT", "N2", "H", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // methylguanidinium
        {"CT", "CT", "N2", "CA", SpeciesTorsion::Cos3Form, {7.65254, 1.01671, -2.08363}},           // ethylguanidinium
        {"HC", "CT", "CT", "N2", SpeciesTorsion::Cos3Form, {0, 0, -2.43509}},                       // ethylguanidinium
        {"C", "N", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, -0.581576}},                        // N-methylformamide
        {"HC", "CT", "N", "H", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // N-methylformamide
        {"C", "N", "CT", "CT", SpeciesTorsion::Cos3Form, {-5.84086, -1.78657, 0}},                  // N-ethylformamide
        {"H", "N", "CT", "CT", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // N-ethylformamide
        {"N", "CT", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 1.94138}},                         // N-ethylformamide
        {"N", "CT", "CT", "CT", SpeciesTorsion::Cos3Form, {8.21738, 0, 2.75726}},                   // N-propylformamide
        {"C", "NM", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, -0.581576}},                       // tertiary
        {"C", "NM", "CT", "CT", SpeciesTorsion::Cos3Form, {-5.84086, -1.78657, 0}},                 // tertiary
        {"NM", "CT", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 1.94138}},                        // tertiary
        {"NM", "CT", "CT", "CT", SpeciesTorsion::Cos3Form, {8.21738, 0, 2.75726}},                  // tertiary
        {"CM", "CM", "C", "NM", SpeciesTorsion::Cos3Form, {8.368, 0, 0}},                           // tertiary
        {"CM", "CM", "C", "N", SpeciesTorsion::Cos3Form, {8.368, 0, 0}},                            // vinyl
        {"HC", "CT", "C", "O", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // all
        {"HC", "C", "C", "O", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                 //
        {"HC", "CM", "C", "O", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                //
        {"CT", "CT", "C", "N", SpeciesTorsion::Cos3Form, {13.598, -1.68197, -0.569024}},            // propanamide
        {"CT", "CT", "C", "O", SpeciesTorsion::Cos3Form, {0, 4.87854, 0}},                          // propanamide
        {"HC", "CT", "CT", "C", SpeciesTorsion::Cos3Form, {0, 0, -0.4184}},                         // all
        {"C", "CT", "CT", "CT", SpeciesTorsion::Cos3Form, {-8.61904, -1.30959, 1.31796}},           // butanamide
        {"N", "CT", "CT", "OH", SpeciesTorsion::Cos3Form, {26.2755, -6.13793, 8.49352}},            // Chi
        {"C", "CT", "CT", "OH", SpeciesTorsion::Cos3Form, {-25.8571, 0, 0}},                        // Chi
        {"N", "CT", "CT", "S*", SpeciesTorsion::Cos3Form, {5.97475, 0.359824, 0.121336}},           // Chi
        {"C", "CT", "CT", "S*", SpeciesTorsion::Cos3Form, {-18.1753, -7.17138, 0}},                 // Chi
        {"HC", "CT", "C^", "CW", SpeciesTorsion::Cos3Form, {0, 0, -2.00832}},                       // 3-methylindole
        {"HC", "CT", "C^", "CB", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // 3-methylindole
        {"CT", "CT", "C^", "CW", SpeciesTorsion::Cos3Form, {-2.98738, 0, 0}},                       // 3-ethylindole
        {"CT", "CT", "C^", "CB", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // 3-ethylindole
        {"HC", "CT", "CC", "N^", SpeciesTorsion::Cos3Form, {0, 0, 1.7531}},                         // HID,
        {"CT", "CT", "CC", "N^", SpeciesTorsion::Cos3Form, {9.89934, -1.09621, 2.11292}},           // "
        {"HC", "CT", "N3", "H", SpeciesTorsion::Cos3Form, {0, 0, 1.09202}},                         // ammonium
        {"CT", "CT", "N3", "H", SpeciesTorsion::Cos3Form, {0, 0, 1.45185}},                         // ammonium
        {"HC", "CT", "CT", "N3", SpeciesTorsion::Cos3Form, {0, 0, 1.60666}},                        // ammonium
        {"CT", "CT", "CT", "N3", SpeciesTorsion::Cos3Form, {11.4307, -0.958136, 2.02924}},          // ammonium
        {"HC", "CT", "NO", "ON", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // nitro
        {"*T", "CT", "C", "O", SpeciesTorsion::Cos3Form, {0, 3.43088, 0}},                          // carboxylate
        {"*T", "CT", "C", "O2", SpeciesTorsion::Cos3Form, {0, 3.43088, 0}},                         // carboxylate
        {"HC", "CT", "CT", "NO", SpeciesTorsion::Cos3Form, {0, 0, -0.9414}},                        // nitroethane
        {"CT", "CT", "CT", "C", SpeciesTorsion::Cos3Form, {-13.326, -3.4518, 2.06271}},             // carboxylate
        {"HC", "CT", "C", "O2", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // caboxylates
        {"HC", "CT", "C", "OH", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // acids
        {"CT", "CT", "C", "O", SpeciesTorsion::Cos3Form, {0, 2.28446, 0}},                          // RCOOH
        {"CA", "CT", "C", "O", SpeciesTorsion::Cos3Form, {0, 2.28446, 0}},                          // RCOOH
        {"CT", "CT", "C", "OH", SpeciesTorsion::Cos3Form, {4.184, 2.28446, 1.8828}},                // RCOOH
        {"NT", "CT", "C", "OH", SpeciesTorsion::Cos3Form, {22.0078, 3.43088, 0}},                   // neutral
        {"HC", "CT", "OH", "HO", SpeciesTorsion::Cos3Form, {-10.8324, -4.69863, 1.12968}},          // axial
        {"C", "CT", "CT", "C", SpeciesTorsion::Cos3Form, {-2.3012, 0, 4.184}},                      // dicarboxylic
        {"O", "C", "OH", "HO", SpeciesTorsion::Cos3Form, {0, 23.012, 0}},                           // carboxylic
        {"CT", "C", "OH", "HO", SpeciesTorsion::Cos3Form, {6.276, 23.012, 0}},                      // carboxylic
        {"HC", "C", "OH", "HO", SpeciesTorsion::Cos3Form, {6.276, 23.012, 0}},                      // carboxylic
        {"C", "CT", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0.309616}},                        // dicarboxylic
        {"CT", "CT", "C", "O", SpeciesTorsion::Cos3Form, {-3.138, -2.3012, -1.046}},                // dicarboxylic
        {"CT", "CT", "C", "OH", SpeciesTorsion::Cos3Form, {0, 5.90781, 0}},                         // dicarboxylic
        {"CT", "N", "CT", "C", SpeciesTorsion::Cos3Form, {-7.26761, 5.23418, -14.6482}},            // Proline
        {"CT", "N", "CT", "CT", SpeciesTorsion::Cos3Form, {19.8866, -3.07106, 0}},                  // "
        {"CT", "CT", "N", "CT", SpeciesTorsion::Cos3Form, {11.9621, 8.61067, -47.1369}},            // "
        {"CT", "NM", "CT", "C", SpeciesTorsion::Cos3Form, {-7.26761, 5.23418, -14.6482}},           //
        {"CT", "NM", "CT", "CT", SpeciesTorsion::Cos3Form, {19.8866, -3.07106, 0}},                 //
        {"CT", "CT", "NM", "CT", SpeciesTorsion::Cos3Form, {11.9621, 8.61067, -47.1369}},           //
        {"CT", "CT", "C+", "CT", SpeciesTorsion::Cos3Form, {0, -4.184, 0}},                         // carbocation
        {"CT", "CT", "C+", "HC", SpeciesTorsion::Cos3Form, {0, -4.184, 0}},                         // carbocation
        {"HC", "CT", "C", "CT", SpeciesTorsion::Cos3Form, {0, 0, 1.1506}},                          // ketone
        {"HC", "CT", "C", "HC", SpeciesTorsion::Cos3Form, {0, 0, 1.50624}},                         // aldehyde
        {"HC", "CT", "C", "F", SpeciesTorsion::Cos3Form, {0, 0, 1.50624}},                          // acyl
        {"HC", "CT", "C", "Cl", SpeciesTorsion::Cos3Form, {0, 0, 1.50624}},                         // acyl
        {"HC", "CT", "C", "Br", SpeciesTorsion::Cos3Form, {0, 0, 1.50624}},                         // acyl
        {"CT", "CT", "C", "F", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // acyl
        {"CT", "CT", "C", "Cl", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // acyl
        {"CT", "CT", "C", "Br", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // acyl
        {"CT", "CT", "C", "O", SpeciesTorsion::Cos3Form, {-1.15897, 5.13795, -2.9037}},             // aldehyde
        {"HC", "CT", "CT", "C(O)", SpeciesTorsion::Cos3Form, {0, 0, -0.317984}},                    // aldehyde
        {"CT", "CT", "CT", "C", SpeciesTorsion::Cos3Form, {-7.10025, -1.9079, 2.44764}},            // aldehyde
        {"CA", "CT", "CT", "C", SpeciesTorsion::Cos3Form, {-7.10025, -1.9079, 2.44764}},            // aldehyde
        {"C^", "CT", "CT", "C", SpeciesTorsion::Cos3Form, {-7.10025, -1.9079, 2.44764}},            //
        {"CT", "CT", "C", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // aldehyde
        {"CT", "CT", "C", "CT", SpeciesTorsion::Cos3Form, {6.08354, -0.602496, -3.2426}},           // ketone
        {"C", "C", "OH", "HO", SpeciesTorsion::Cos3Form, {12.552, 23.012, 0}},                      // oxalic
        {"CA", "CT", "P", "OS", SpeciesTorsion::Cos3Form, {9.414, 0, 0}},                           // phosphonates
        {"CT", "P", "OS", "CT", SpeciesTorsion::Cos3Form, {14.644, -13.8072, 6.276}},               // phosphonates
        {"P", "OS", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                          // phosphonates
        {"O2", "P", "OS", "CT", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // phosphonates
        {"O", "P", "OS", "CA", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // phosphonates
        {"O", "P", "OH", "HO", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // phosphonates
        {"OH", "P", "OH", "HO", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // phosphonates
        {"CA", "CA", "CT", "P", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // phosphonates
        {"CA", "CT", "P", "O2", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // phosphonates
        {"HC", "CT", "P", "O2", SpeciesTorsion::Cos3Form, {0, 0, 1.046}},                           // phosphonates
        {"HC", "CT", "P", "OS", SpeciesTorsion::Cos3Form, {0, 0, 1.046}},                           // phosphonates
        {"O2", "P", "OS", "C^", SpeciesTorsion::Cos3Form, {0, 0, 2.35141}},                         // MeOPO3
        {"O2", "P", "OS", "CT", SpeciesTorsion::Cos3Form, {3.7656, -12.2591, 11.0458}},             // dimethyl
        {"CA", "CA", "OS", "P", SpeciesTorsion::Cos3Form, {0, 12.5102, 0}},                         // PhOPO3
        {"NT", "CT", "CT", "Cl", SpeciesTorsion::Cos3Form, {8.368, 0, 0}},                          // 2-chloroethylamines
        {"HC", "CT", "CT", "Cl", SpeciesTorsion::Cos3Form, {0, 0, 1.6736}},                         // alkyl
        {"CT", "CT", "CT", "Cl", SpeciesTorsion::Cos3Form, {0, 0, 1.6736}},                         // alkyl
        {"Cl", "CT", "CT", "Cl", SpeciesTorsion::Cos3Form, {-1.046, 0, 0}},                         // dichloride
        {"HC", "CT", "CT", "I", SpeciesTorsion::Cos3Form, {0, 0, 1.6736}},                          // alkyl
        {"CT", "CT", "CT", "I", SpeciesTorsion::Cos3Form, {0, 0, 1.6736}},                          // alkyl
        {"HO", "OH", "CT", "CT", SpeciesTorsion::Cos3Form, {18.736, -9.09853, 0}},                  // trifluoroethanol
        {"HC", "CT", "OH", "HO", SpeciesTorsion::Cos3Form, {0, 0, 1.99158}},                        // trifluoroethanol
        {"F", "CT", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 1.31252}},                         // trifluoroethanol
        {"F", "CT", "CT", "OH", SpeciesTorsion::Cos3Form, {0, 0, 2.25978}},                         // trifluoroethanol
        {"CT", "C", "OH", "HO", SpeciesTorsion::Cos3Form, {13.3888, 20.5016, 0}},                   // 1,2-diacid
        {"CT", "CT", "C", "O", SpeciesTorsion::Cos3Form, {-4.184, -7.9496, -3.7656}},               // 1,2-diacid
        {"C", "CT", "CT", "C", SpeciesTorsion::Cos3Form, {3.3472, 0, 3.7656}},                      // 1,2-diacid
        {"H", "N", "SY", "CA", SpeciesTorsion::Cos3Form, {6.99146, -20.5058, 2.7991}},              // sulfonamide
        {"HC", "CT", "N", "SY", SpeciesTorsion::Cos3Form, {5.69861, -6.09609, 0.623416}},           // sulfonamide
        {"CT", "N", "SY", "CA", SpeciesTorsion::Cos3Form, {8.67762, -12.4097, 10.347}},             // sulfonamide
        {"CT", "CT", "N", "SY", SpeciesTorsion::Cos3Form, {12.2549, -10.5981, 2.07945}},            // sulfonamide
        {"CA", "CA", "SY", "N", SpeciesTorsion::Cos3Form, {6.9287, -3.21331, -0.489528}},           // sulfonamide
        {"*", "*", "SY", "OY", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // sulfonamide
        {"*", "CA", "SY", "OY", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // sulfonamide
        {"*", "N", "SY", "OY", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // sulfonamide
        {"*", "CT", "SY", "OY", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // sulfonamide
        {"*", "CT", "SY", "N", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // sulfonamide
        {"HC", "CT", "CT", "F", SpeciesTorsion::Cos3Form, {0, 0, 1.6736}},                          // alkyl
        {"CT", "CT", "CT", "F", SpeciesTorsion::Cos3Form, {1.2552, -1.6736, 1.6736}},               // alkyl
        {"F", "CT", "CT", "F", SpeciesTorsion::Cos3Form, {-10.46, 0, 1.046}},                       // 1,2-difluoride
        {"CT", "CT", "CT", "O*", SpeciesTorsion::Cos3Form, {-5.58982, 0, 0}},                       // hexopyranoses
        {"CT", "CT", "OH", "HO", SpeciesTorsion::Cos3Form, {11.188, -12.0625, 4.29278}},            // hexopyranoses
        {"OH", "CT", "CT", "OH", SpeciesTorsion::Cos3Form, {37.9321, 0, 0}},                        // hexopyranoses
        {"CT", "OS", "CO", "OH", SpeciesTorsion::Cos3Form, {-1.569, -5.68187, 0.016736}},           // hexopyranoses
        {"CT", "OS", "CO", "OS", SpeciesTorsion::Cos3Form, {-1.569, -5.68187, 0.016736}},           // hexopyranoses
        {"OS", "CO", "OH", "HO", SpeciesTorsion::Cos3Form, {-5.25929, -7.5563, 0.012552}},          // hexopyranoses
        {"OH", "CT", "CT", "OS", SpeciesTorsion::Cos3Form, {18.0707, 0, 0}},                        // hexopyranoses
        {"HC", "CT", "CT", "Br", SpeciesTorsion::Cos3Form, {0, 0, 1.6736}},                         // alkyl
        {"CT", "CT", "CT", "Br", SpeciesTorsion::Cos3Form, {0, 0, 1.6736}},                         // alkyl
        {"CA", "CA", "CT", "Br", SpeciesTorsion::Cos3Form, {0, 0, 1.6736}},                         // alkyl
        {"*", "CA", "CA", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           // in
        {"CA", "CA", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "CA", "CA", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CA", "CA", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "CA", "C!", "CA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "CA", "C!", "C!", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "CA", "CA", "C!", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C!", "CA", "CA", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CA", "C!", "C!", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CA", "C!", "CA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "CA", "C!", "NC", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "C!", "NC", "CA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C!", "C!", "CA", "NC", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "C!", "CA", "NC", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C!", "CA", "CA", "NC", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C!", "C!", "NC", "CA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C!", "C!", "N", "C", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"C!", "C!", "CM", "C", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"NC", "C!", "CA", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "CA", "CA", "F", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"CA", "CA", "CA", "Cl", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "CA", "CA", "Br", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "CA", "CA", "I", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"HA", "CA", "CA", "F", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"HA", "CA", "CA", "Cl", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CA", "CA", "Br", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CA", "CA", "I", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"CA", "CA", "CA", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "C=", "C=", "C^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C", "C=", "C=", "C", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"HA", "CA", "CA", "CT", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CT", "CA", "CA", "CT", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CT", "CA", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "CA", "CA", "CB", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CA", "CA", "CB", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "CA", "CA", "O*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CA", "CA", "O*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CT", "CA", "CA", "O*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"O*", "CA", "CA", "O*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "CA", "CA", "S*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CA", "CA", "S*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CT", "CA", "CA", "S*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"S*", "CA", "CA", "S*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CM", "C!", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CM", "C!", "N", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"HA", "CA", "CA", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CT", "CA", "CA", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"O*", "CA", "CA", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"N^", "CA", "CA", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"*", "CB", "CB", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"C^", "CB", "CB", "C^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"N^", "CB", "CB", "C^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"N^", "CB", "CB", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"*", "CB", "CS", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"*", "CS", "C", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                            //
        {"*", "CR", "CS", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"*", "CS", "CW", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"C^", "CS", "CW", "C^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C^", "CS", "CW", "H*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C^", "CS", "CW", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CS", "CW", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CS", "CW", "C^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CS", "CW", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CS", "CS", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CA", "CU", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"HA", "CW", "OS", "CW", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"*", "NC", "CA", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"CS", "CW", "OS", "CW", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "NC", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C^", "NC", "CA", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "NC", "CA", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "NC", "NC", "CT", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CA", "NC", "NC", "CA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CT", "NC", "NC", "CT", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"*", "NC", "CB", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"C^", "NC", "CB", "C^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C^", "NC", "CB", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"*", "NA", "CB", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"*", "NB", "CB", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"*", "NB", "CR", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"*", "NB", "CU", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"C^", "NA", "CB", "C^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C^", "NA", "CB", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C^", "NB", "CB", "C^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"N^", "CR", "S", "CW", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"CR", "NB", "CV", "CW", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CR", "NB", "CV", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CU", "CW", "S", "CR", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"CR", "S", "CW", "CV", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"HA", "CW", "S", "CR", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"CW", "S", "CR", "NB", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"S", "CR", "NB", "CU", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"S", "CR", "NB", "CV", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"CR", "NB", "CU", "CW", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CU", "CW", "OS", "CR", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"CW", "OS", "CR", "NB", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"OS", "CR", "NB", "CU", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"NB", "CU", "CW", "OS", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"NB", "CU", "CW", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"NB", "CU", "CW", "S", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"NB", "CV", "CW", "OS", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"NB", "CV", "CW", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"NB", "CV", "CW", "S", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"*", "N", "CB", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                            //
        {"HA", "CU", "CW", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"HA", "CW", "CU", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"*", "NC", "CR", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"*", "NC", "CQ", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                           //
        {"C^", "NC", "CQ", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"C^", "NC", "CQ", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                         //
        {"*", "N", "CQ", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                            //
        {"C^", "N", "CQ", "N^", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"C^", "N", "CQ", "HA", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                          //
        {"H", "N", "CQ", "*", SpeciesTorsion::Cos3Form, {0, 30.334, 0}},                            //
        {"*", "CW", "NA", "*", SpeciesTorsion::Cos3Form, {0, 11.7152, 0}},                          //
        {"*", "NA", "CR", "*", SpeciesTorsion::Cos3Form, {0, 19.4556, 0}},                          //
        {"HA", "CR", "NB", "*", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                           //
        {"N^", "CR", "NB", "C^", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                          //
        {"*", "CR", "NB", "*", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                            //
        {"*", "CW", "NB", "*", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                            //
        {"*", "CR", "NA", "*", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                            //
        {"HA", "CR", "NA", "*", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                           //
        {"N^", "CR", "NA", "C^", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                          //
        {"*", "CR", "NC", "*", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                            //
        {"*", "CK", "NB", "*", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                            //
        {"*", "CK", "NA", "*", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                            //
        {"*", "CK", "NC", "*", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                            //
        {"*", "NA", "NB", "*", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                            //
        {"*", "NB", "NB", "*", SpeciesTorsion::Cos3Form, {0, 41.84, 0}},                            //
        {"*", "NB", "CV", "*", SpeciesTorsion::Cos3Form, {0, 20.0832, 0}},                          //
        {"*", "CW", "CV", "*", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                           //
        {"C^", "CW", "CV", "C^", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                         //
        {"C^", "CW", "CV", "HA", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                         //
        {"HA", "CW", "CV", "C^", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                         //
        {"HA", "CW", "CV", "HA", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                         //
        {"*", "CW", "CW", "*", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                           //
        {"C^", "CW", "CW", "C^", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                         //
        {"C^", "CW", "CW", "HA", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                         //
        {"H*", "CW", "CW", "C^", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                         //
        {"H*", "CW", "CW", "HA", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                         //
        {"HA", "CW", "CW", "HA", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                         //
        {"S", "CW", "CW", "HA", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                          //
        {"S", "CW", "CU", "HA", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                          //
        {"S", "CW", "CV", "HA", SpeciesTorsion::Cos3Form, {0, 44.978, 0}},                          //
        {"*", "NA", "CW", "*", SpeciesTorsion::Cos3Form, {0, 20.92, 0}},                            //
        {"C^", "NA", "CW", "C^", SpeciesTorsion::Cos3Form, {0, 20.92, 0}},                          //
        {"N^", "NA", "CW", "C^", SpeciesTorsion::Cos3Form, {0, 20.92, 0}},                          //
        {"C^", "NA", "CW", "H*", SpeciesTorsion::Cos3Form, {0, 20.92, 0}},                          //
        {"N^", "NA", "CW", "H*", SpeciesTorsion::Cos3Form, {0, 20.92, 0}},                          //
        {"H", "NA", "CW", "*", SpeciesTorsion::Cos3Form, {0, 20.92, 0}},                            //
        {"H", "NA", "CR", "*", SpeciesTorsion::Cos3Form, {0, 20.92, 0}},                            //
        {"H", "NA", "CB", "*", SpeciesTorsion::Cos3Form, {0, 20.92, 0}},                            //
        {"*", "C^", "CW", "*", SpeciesTorsion::Cos3Form, {0, 54.6012, 0}},                          //
        {"*", "C^", "CB", "*", SpeciesTorsion::Cos3Form, {0, 14.0164, 0}},                          //
        {"*", "CA", "CB", "*", SpeciesTorsion::Cos3Form, {0, 29.288, 0}},                           //
        {"C^", "CA", "CB", "C^", SpeciesTorsion::Cos3Form, {0, 29.288, 0}},                         //
        {"N^", "CA", "CB", "N^", SpeciesTorsion::Cos3Form, {0, 29.288, 0}},                         //
        {"C^", "CA", "CB", "N^", SpeciesTorsion::Cos3Form, {0, 29.288, 0}},                         //
        {"N^", "CA", "CB", "C^", SpeciesTorsion::Cos3Form, {0, 29.288, 0}},                         //
        {"*", "C", "CB", "*", SpeciesTorsion::Cos3Form, {0, 29.288, 0}},                            //
        {"O", "C", "CB", "C^", SpeciesTorsion::Cos3Form, {0, 29.288, 0}},                           //
        {"N^", "C", "CB", "N^", SpeciesTorsion::Cos3Form, {0, 29.288, 0}},                          //
        {"O", "C", "CB", "N^", SpeciesTorsion::Cos3Form, {0, 29.288, 0}},                           //
        {"N^", "C", "CB", "C^", SpeciesTorsion::Cos3Form, {0, 29.288, 0}},                          //
        {"*", "CB", "CN", "*", SpeciesTorsion::Cos3Form, {0, 25.104, 0}},                           //
        {"*", "NA", "CN", "*", SpeciesTorsion::Cos3Form, {0, 12.7612, 0}},                          //
        {"*", "CW", "NA", "*", SpeciesTorsion::Cos3Form, {0, 12.552, 0}},                           //
        {"CT", "CT", "OS", "P", SpeciesTorsion::Cos3Form, {-5.94128, -2.59408, 0.4184}},            // in
        {"HC", "CT", "C", "OS", SpeciesTorsion::Cos3Form, {0, 0, 0.552288}},                        // esters
        {"CT", "C", "OS", "CT", SpeciesTorsion::Cos3Form, {19.5351, 21.4388, 0}},                   // esters
        {"HC", "C", "OS", "CT", SpeciesTorsion::Cos3Form, {19.5351, 21.4388, 0}},                   // esters
        {"O", "C", "OS", "CT", SpeciesTorsion::Cos3Form, {0, 21.4388, 0}},                          // esters
        {"C", "OS", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0.828432}},                        // esters
        {"CT", "CT", "C", "OS", SpeciesTorsion::Cos3Form, {0, 0, -2.31375}},                        // esters
        {"CT", "CT", "OS", "C", SpeciesTorsion::Cos3Form, {-5.10448, -0.527184, 1.76565}},          // esters
        {"CA", "CT", "CT", "N3", SpeciesTorsion::Cos3Form, {4.184, 0, 0}},                          // phenethylammonium-
        {"CA", "CT", "CT", "NT", SpeciesTorsion::Cos3Form, {-3.3472, 0, 0}},                        // phenethylamines-
        {"CT", "CT", "NO", "ON", SpeciesTorsion::Cos3Form, {0, 1.6736, 0}},                         // nitroethane
        {"CA", "CA", "NO", "ON", SpeciesTorsion::Cos3Form, {0, 4.8116, 0}},                         // nitrobenzene
        {"CT", "NY", "CA", "NZ", SpeciesTorsion::Cos3Form, {0, 15.2758, 0}},                        // neutral
        {"CT", "CT", "NT", "H", SpeciesTorsion::Cos3Form, {0, 16.736, 0}},                          // azetidine-
        {"CT", "CT", "NT", "H", SpeciesTorsion::Cos3Form, {0.8368, -1.74473, 1.74891}},             // pyrrolidine
        {"CT", "NT", "CT", "CT", SpeciesTorsion::Cos3Form, {1.74054, -0.535552, 2.90788}},          // amine
        {"CT", "NT", "CT", "CT", SpeciesTorsion::Cos3Form, {6.42662, -0.535552, 2.90788}},          // exocyclic
        {"CT", "NT", "CT", "CT", SpeciesTorsion::Cos3Form, {6.12538, -0.535552, 2.90788}},          // exocyclic
        {"CT", "CT", "NT", "H", SpeciesTorsion::Cos3Form, {3.4267, -1.74473, 1.74891}},             // cyclic
        {"CT", "CT", "NT", "H", SpeciesTorsion::Cos3Form, {6.36805, -1.74473, 1.74891}},            // cyclic
        {"NT", "CT", "CT", "NT", SpeciesTorsion::Cos3Form, {46.1704, -4.05011, 1.12968}},           // amine
        {"CT", "NT", "CT", "CT", SpeciesTorsion::Cos3Form, {1.74054, -0.535552, 2.90788}},          // amine
        {"HC", "CT", "NT", "CT", SpeciesTorsion::Cos3Form, {0, 0, 2.34304}},                        // amine
        {"CT", "CT", "CW", "NA", SpeciesTorsion::Cos3Form, {7.1128, -2.5104, 0}},                   // 2-ethyl
        {"CT", "C=", "C=", "CM", SpeciesTorsion::Cos3Form, {3.7656, 0.96232, -2.11292}},            // 2-Me-1,3-butadiene
        {"CT", "C=", "C", "O*", SpeciesTorsion::Cos3Form, {3.7656, 0.96232, -2.11292}},             // 2-Me-1,3-butadiene-like
        {"CT", "CM", "C", "O*", SpeciesTorsion::Cos3Form, {3.7656, 0.96232, -2.11292}},             // 2-Me-1,3-butadiene-like
        {"CT", "C", "C=", "CM", SpeciesTorsion::Cos3Form, {3.3472, -12.552, 0}},                    // methyl
        {"CM", "C=", "C", "OH", SpeciesTorsion::Cos3Form, {13.3888, -12.552, 0}},                   // acrylic
        {"CM", "CM", "C", "OH", SpeciesTorsion::Cos3Form, {13.3888, -12.552, 0}},                   // acrylic
        {"CM", "C=", "C", "O", SpeciesTorsion::Cos3Form, {10.46, 25.104, 0}},                       // acrolein
        {"CM", "CM", "C", "O", SpeciesTorsion::Cos3Form, {10.46, 25.104, 0}},                       // acrolein-like
        {"CA", "CA", "C", "CT", SpeciesTorsion::Cos3Form, {0, 0.8368, 0}},                          // aryl
        {"CA", "CA", "C", "HC", SpeciesTorsion::Cos3Form, {0, 0.8368, 0}},                          // aryl
        {"CA", "CA", "N", "*", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                           // N-phenylamide
        {"CA", "CA", "N", "H", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                           // N-phenylamide
        {"CA", "CA", "N", "C", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                           // N-phenylamide
        {"CA", "CA", "N", "CT", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                          // N-phenylamide
        {"CA", "CA", "N", "CR", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                          // diarylamine
        {"CA", "CA", "N", "CW", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                          // diarylamine
        {"CA", "CA", "NT", "CR", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                         // diarylamine
        {"CA", "CA", "NT", "CW", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                         // diarylamine
        {"S", "CR", "N", "CA", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                           // diarylamine
        {"S", "CR", "NT", "CA", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                          // diarylamine
        {"NB", "CR", "N", "CA", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                          // diarylamine
        {"NB", "CR", "NT", "CA", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                         // diarylamine
        {"S", "CR", "N", "H", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                            // diarylamine
        {"S", "CR", "NT", "H", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                           // diarylamine
        {"NB", "CR", "N", "H", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                           // diarylamine
        {"NB", "CR", "NT", "H", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                          // diarylamine
        {"OS", "CR", "N", "CA", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                          // diarylamine
        {"OS", "CR", "N", "H", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                           // diarylamine
        {"OS", "CW", "N", "CA", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                          // diarylamine
        {"OS", "CW", "N", "H", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                           // diarylamine
        {"S", "CW", "N", "CA", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                           // diarylamine
        {"S", "CW", "N", "H", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                            // diarylamine
        {"CA", "CA", "N", "CQ", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                          // diarylamine
        {"CA", "CA", "NT", "CQ", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                         // diarylamine
        {"CA", "CA", "C", "O", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                           // aryl
        {"CA", "CA", "C", "OS", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                          // aryl
        {"CA", "CA", "C", "OH", SpeciesTorsion::Cos3Form, {0, 8.7864, 0}},                          // aryl
        {"CA", "CA", "OS", "C", SpeciesTorsion::Cos3Form, {0, 10.46, 0}},                           // phenyl
        {"O", "C", "OH", "HO", SpeciesTorsion::Cos3Form, {0, 20.92, 0}},                            // benzoic
        {"O", "C", "OS", "CT", SpeciesTorsion::Cos3Form, {0, 20.92, 0}},                            // benzoic
        {"CA", "C", "OH", "HO", SpeciesTorsion::Cos3Form, {16.736, 20.92, 0}},                      // benzoic
        {"CA", "C", "OS", "CT", SpeciesTorsion::Cos3Form, {16.736, 20.92, 0}},                      // benzoic
        {"O", "C", "OS", "CA", SpeciesTorsion::Cos3Form, {0, 20.92, 0}},                            // phenyl
        {"CT", "C", "OS", "CA", SpeciesTorsion::Cos3Form, {6.276, 20.92, 0}},                       // phenyl
        {"CA", "CA", "C", "N", SpeciesTorsion::Cos3Form, {0, 4.6024, 0}},                           // aryl
        {"CA", "CA", "NT", "H", SpeciesTorsion::Cos3Form, {0, 8.49352, 0}},                         // aniline
        {"*", "CA", "N2", "H", SpeciesTorsion::Cos3Form, {0, 8.49352, 0}},                          // aniline-like
        {"*", "CQ", "N2", "H", SpeciesTorsion::Cos3Form, {0, 8.49352, 0}},                          // aniline-like
        {"N^", "CA", "N2", "H", SpeciesTorsion::Cos3Form, {0, 8.49352, 0}},                         // aniline-like
        {"CB", "CA", "N2", "H", SpeciesTorsion::Cos3Form, {0, 8.49352, 0}},                         // aniline-like
        {"S", "CR", "N", "H", SpeciesTorsion::Cos3Form, {0, 8.49352, 0}},                           // aniline-like
        {"OS", "CR", "N", "H", SpeciesTorsion::Cos3Form, {0, 8.49352, 0}},                          // aniline-like
        {"CA", "CA", "NT", "CT", SpeciesTorsion::Cos3Form, {-31.7231, 14.3553, 13.3804}},           // substituted-aniline
        {"CT", "CM", "CT", "CT", SpeciesTorsion::Cos3Form, {11.7863, -0.707096, 2.27191}},          // alkenes
        {"CM", "CM", "CT", "CT", SpeciesTorsion::Cos3Form, {1.44766, 1.69452, -3.78234}},           // alkenes
        {"C=", "CM", "CT", "CT", SpeciesTorsion::Cos3Form, {1.44766, 1.69452, -3.78234}},           // alkenes
        {"CM", "CM", "CA", "CA", SpeciesTorsion::Cos3Form, {5.19234, 14.029, -1.19662}},            // styrene
        {"CT", "CM", "CA", "CA", SpeciesTorsion::Cos3Form, {0.85772, -2.2217, 0}},                  // 1-methylstyrene
        {"Z", "CM", "X", "Y", SpeciesTorsion::Cos3Form, {0, 125.52, 0}},                            // improper
        {"*", "CM", "CM", "*", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                           // alkene
        {"CT", "CM", "CM", "CT", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // alkene
        {"CT", "CM", "C=", "CT", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // alkene
        {"CT", "CM", "C=", "HC", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // alkene
        {"HC", "CM", "C=", "HC", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // alkene
        {"HC", "CM", "C=", "CT", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // alkene
        {"CT", "CM", "CM", "HC", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // alkene
        {"C", "CM", "CM", "HC", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                          // alkene
        {"HC", "CM", "CM", "HC", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // alkene
        {"HC", "CM", "C=", "C=", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // alkene
        {"Cl", "CM", "CM", "HC", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // chloroalkene
        {"HC", "CM", "CM", "OS", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // vinyl
        {"CT", "CM", "CM", "OS", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // vinyl
        {"HC", "CM", "CM", "OH", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // vinyl
        {"CT", "CM", "CM", "OH", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                         // vinyl
        {"C", "CM", "CM", "N", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                           // conj.
        {"CT", "C", "NC", "CT", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                          // imine
        {"HC", "C", "NC", "HC", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                          // imine
        {"CT", "C", "NC", "O*", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                          // oxime
        {"HC", "C", "NC", "O*", SpeciesTorsion::Cos3Form, {0, 58.576, 0}},                          // oxime
        {"CA", "C!", "C!", "CA", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl
        {"CA", "C!", "C!", "NC", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "C", "CA", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                         // biphenyl
        {"CA", "C!", "NA", "NB", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "NA", "CW", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CW", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CW", "NA", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CW", "OS", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CS", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CM", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CU", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CU", "NB", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CV", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CV", "NB", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CR", "OS", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CR", "NA", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CR", "NB", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CW", "S", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                         // biphenyl-like
        {"CA", "C!", "CR", "S", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                         // biphenyl-like
        {"CA", "C!", "N", "C", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                          // biphenyl-like
        {"CA", "C!", "N", "CM", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                         // biphenyl-like
        {"NC", "C!", "N", "C", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                          // biphenyl-like
        {"NC", "C!", "N", "CM", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                         // biphenyl-like
        {"CS", "CS", "CS", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CS", "CS", "CS", "N^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"OS", "CS", "CS", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"OS", "CS", "CS", "N^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"NA", "CS", "CS", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"NA", "CS", "CS", "N^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CW", "CV", "CS", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CW", "CV", "CS", "N^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"NB", "CV", "CS", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"NB", "CV", "CS", "N^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"N^", "CR", "CS", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"N^", "CR", "CW", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"*", "N", "CU", "*", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                           // biphenyl-like
        {"*", "N", "CW", "*", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                           // biphenyl-like
        {"*", "N", "CS", "*", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                           // biphenyl-like
        {"*", "CM", "CU", "*", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                          // biphenyl-like
        {"*", "CM", "CW", "*", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                          // biphenyl-like
        {"*", "CM", "CS", "*", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                          // biphenyl-like
        {"NC", "C!", "CU", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"NC", "C!", "CU", "N^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CU", "C^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"CA", "C!", "CU", "N^", SpeciesTorsion::Cos3Form, {0, 9.07928, 0}},                        // biphenyl-like
        {"Cl", "CM", "CM", "Cl", SpeciesTorsion::Cos3Form, {-6.6944, 58.576, 0}},                   // chloroalkene
        {"HC", "CT", "P+", "CT", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // phosphonium
        {"HC", "CT", "CT", "P+", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // "
        {"CT", "CT", "P+", "CT", SpeciesTorsion::Cos3Form, {4.184, -2.092, 2.092}},                 // "
        {"CK", "N^", "CT", "OS", SpeciesTorsion::Cos3Form, {13.1043, -6.23834, 11.4809}},           // Ping
        {"CK", "NA", "CT", "CT", SpeciesTorsion::Cos3Form, {11.5311, -3.64845, -15.3971}},          // Ping
        {"C", "NC", "OH", "HO", SpeciesTorsion::Cos3Form, {12.552, 12.552, 0}},                     // oxime
        {"C", "NC", "OS", "CT", SpeciesTorsion::Cos3Form, {12.552, 12.552, 0}},                     // oxime
        {"CM", "CM", "OS", "CT", SpeciesTorsion::Cos3Form, {-14.644, 12.552, 0}},                   // vinyl
        {"C=", "CM", "OS", "CT", SpeciesTorsion::Cos3Form, {-14.644, 12.552, 0}},                   // vinyl
        {"CM", "CM", "CT", "F", SpeciesTorsion::Cos3Form, {2.092, 0, 0}},                           // allyl
        {"CM", "CM", "CT", "O*", SpeciesTorsion::Cos3Form, {2.092, 0, 0}},                          // allyl
        {"C=", "CM", "CT", "O*", SpeciesTorsion::Cos3Form, {2.092, 0, 0}},                          // allyl
        {"CM", "C=", "CT", "O*", SpeciesTorsion::Cos3Form, {2.092, 0, 0}},                          // allyl
        {"CM", "CT", "OH", "HO", SpeciesTorsion::Cos3Form, {-3.7656, 0, 0}},                        // allyl
        {"CA", "CT", "OH", "HO", SpeciesTorsion::Cos3Form, {-3.7656, 0, 0}},                        // benzyl
        {"C=", "CT", "OH", "HO", SpeciesTorsion::Cos3Form, {-3.7656, 0, 0}},                        // allyl
        {"HC", "CT", "SY", "OY", SpeciesTorsion::Cos3Form, {0, 0, 1.4644}},                         // sulfone
        {"HC", "CT", "SY", "CT", SpeciesTorsion::Cos3Form, {0, 0, 1.4644}},                         // sulfone
        {"HC", "CT", "SY", "CA", SpeciesTorsion::Cos3Form, {0, 0, 1.4644}},                         // sulfone
        {"CT", "CT", "SY", "CT", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // sulfone
        {"CT", "CT", "SY", "OY", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // sulfone
        {"HC", "CT", "CT", "SY", SpeciesTorsion::Cos3Form, {0, 0, 1.4644}},                         // sulfone
        {"HC", "CT", "N3", "CT", SpeciesTorsion::Cos3Form, {0, 0, 1.26231}},                        // 2ary
        {"CT", "CT", "N3", "CT", SpeciesTorsion::Cos3Form, {6.01617, -0.517979, 1.10416}},          // 2ary
        {"*", "CT", "CZ", "NZ", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // nitriles
        {"*", "CT", "NZ", "CZ", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // isonitriles
        {"HC", "CT", "NZ", "CZ", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // isonitriles
        {"*", "CA", "NZ", "CZ", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // isonitriles
        {"CA", "CA", "NZ", "CZ", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // isonitriles
        {"*", "CZ", "CZ", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // alkynes
        {"*", "CT", "CZ", "CZ", SpeciesTorsion::Cos3Form, {0, 0, 0}},                               // alkynes
        {"CT", "CT", "CZ", "CZ", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // alkynes
        {"HC", "CT", "CZ", "CZ", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // alkynes
        {"HC", "CZ", "CZ", "CT", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // alkynes
        {"CT", "CZ", "CZ", "CT", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // alkynes
        {"CA", "CA", "SY", "CT", SpeciesTorsion::Cos3Form, {0, -3.7656, 0}},                        // sulfone
        {"CR", "NA", "CT", "OS", SpeciesTorsion::Cos3Form, {-6.276, -6.276, 0}},                    // imidazoles,
        {"CR", "N^", "CT", "OS", SpeciesTorsion::Cos3Form, {-6.276, -6.276, 0}},                    // imidazoles,
        {"CK", "NA", "CT", "OS", SpeciesTorsion::Cos3Form, {-6.276, -6.276, 0}},                    // imidazoles,
        {"CK", "N^", "CT", "OS", SpeciesTorsion::Cos3Form, {-6.276, -6.276, 0}},                    // imidazoles,
        {"C^", "NA", "CT", "OS", SpeciesTorsion::Cos3Form, {6.276, -6.276, 0}},                     // imidazoles,
        {"C^", "N^", "CT", "OS", SpeciesTorsion::Cos3Form, {6.276, -6.276, 0}},                     // imidazoles,
        {"CR", "NA", "CT", "CT", SpeciesTorsion::Cos3Form, {-4.184, -1.4644, 0}},                   // imidazoles,
        {"CR", "N^", "CT", "CT", SpeciesTorsion::Cos3Form, {-4.184, -1.4644, 0}},                   // imidazoles,
        {"CK", "NA", "CT", "CT", SpeciesTorsion::Cos3Form, {-4.184, -1.4644, 0}},                   // imidazoles,
        {"CK", "N^", "CT", "CT", SpeciesTorsion::Cos3Form, {-4.184, -1.4644, 0}},                   // imidazoles,
        {"C^", "NA", "CT", "CT", SpeciesTorsion::Cos3Form, {4.184, -1.4644, 0}},                    // imidazoles,
        {"C^", "N^", "CT", "CT", SpeciesTorsion::Cos3Form, {4.184, -1.4644, 0}},                    // imidazoles,
        {"N^", "CT", "OS", "CT", SpeciesTorsion::Cos3Form, {-2.092, -6.276, 4.184}},                // imidazoles,
        {"C^", "NA", "CT", "OS", SpeciesTorsion::Cos3Form, {0, -7.84918, 0}},                       // Ping
        {"C^", "NA", "CT", "CT", SpeciesTorsion::Cos3Form, {0, -2.40998, 0}},                       // Ping
        {"*", "CY", "CY", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // small
        {"*", "CY", "Ndoll", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                             // small
        {"*", "CY", "Cdoll", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                             // small
        {"CY", "CY", "Cdoll", "Ndoll", SpeciesTorsion::Cos3Form, {0, 0, 0}},                        // small
        {"CY", "CY", "Cdoll", "O", SpeciesTorsion::Cos3Form, {0, 0, 0}},                            // small
        {"*", "CY", "S", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                 // small
        {"CY", "CY", "Ndoll", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                            // small
        {"HC", "CY", "Ndoll", "C^", SpeciesTorsion::Cos3Form, {0, 0, 0}},                           // small
        {"HC", "CY", "Cdoll", "O", SpeciesTorsion::Cos3Form, {0, 0, 0}},                            // small
        {"HC", "CY", "Cdoll", "Ndoll", SpeciesTorsion::Cos3Form, {0, 0, 0}},                        // small
        {"N", "CY", "CY", "*", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // small
        {"CY", "CY", "N", "H", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // small
        {"Cdoll", "CY", "N", "H", SpeciesTorsion::Cos3Form, {0, 0, 0}},                             // small
        {"HC", "CY", "N", "C", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // small
        {"HC", "CY", "N", "H", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                // small
        {"C^", "Ndoll", "CT", "C^", SpeciesTorsion::Cos3Form, {0, 0, 0}},                           // small
        {"C^", "Ndoll", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0}},                           // small
        {"CY", "Cdoll", "Ndoll", "CT", SpeciesTorsion::Cos3Form, {9.6232, 25.4764, 0}},             // small
        {"CY", "Cdoll", "Ndoll", "CY", SpeciesTorsion::Cos3Form, {9.6232, 25.4764, 0}},             // small
        {"CY", "N", "CT", "CT", SpeciesTorsion::Cos3Form, {9.6232, 25.4764, 0}},                    // small
        {"CY", "Cdoll", "Ndoll", "CA", SpeciesTorsion::Cos3Form, {9.6232, 25.4764, 0}},             // small
        {"O", "Cdoll", "Ndoll", "H", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                    // small
        {"CY", "Cdoll", "Ndoll", "H", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                   // small
        {"O", "Cdoll", "Ndoll", "CT", SpeciesTorsion::Cos3Form, {0, 25.4764, 0}},                   // small
        {"O", "Cdoll", "Ndoll", "CA", SpeciesTorsion::Cos3Form, {0, 25.4764, 0}},                   // small
        {"O", "C", "N", "CY", SpeciesTorsion::Cos3Form, {0, 25.4764, 0}},                           // small
        {"C", "N", "CY", "CY", SpeciesTorsion::Cos3Form, {-5.84086, -1.78657, 0}},                  // small
        {"C", "N", "CY", "Cdoll", SpeciesTorsion::Cos3Form, {-5.84086, -1.78657, 0}},               // small
        {"O", "Cdoll", "Ndoll", "CY", SpeciesTorsion::Cos3Form, {0, 83.68, 0}},                     // small
        {"HC", "CY", "CY", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // small
        {"HC", "CY", "CY", "CY", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // small
        {"HC", "CY", "CY", "CT", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // small
        {"CT", "CY", "CY", "CT", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // small
        {"CY", "CY", "CY", "CY", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // small
        {"HC", "CT", "CY", "CY", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // small
        {"HC", "CT", "CM", "CM", SpeciesTorsion::Cos3Form, {0, 0, -1.55645}},                       // alkenes
        {"HC", "CT", "CM", "C=", SpeciesTorsion::Cos3Form, {0, 0, -1.55645}},                       // alkenes
        {"HC", "CT", "C=", "C=", SpeciesTorsion::Cos3Form, {0, 0, -1.55645}},                       // alkenes
        {"HC", "CT", "C=", "CM", SpeciesTorsion::Cos3Form, {0, 0, -1.55645}},                       // alkenes
        {"HC", "CM", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 1.33051}},                        // alkene
        {"HC", "CT", "CT", "CM", SpeciesTorsion::Cos3Form, {0, 0, 1.53134}},                        // alkene
        {"HC", "CT", "CT", "CZ", SpeciesTorsion::Cos3Form, {0, 0, 1.53134}},                        // alkyne,
        {"CT", "CT", "CT", "CZ", SpeciesTorsion::Cos3Form, {0, -2.7196, 0}},                        // alkyne,
        {"HC", "CT", "CM", "CT", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // alkenes
        {"CT", "NT", "OS", "CT", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // generic
        {"CT", "NT", "OH", "HO", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // generic
        {"H", "NT", "OS", "CT", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                          // generic
        {"H", "NT", "OH", "HO", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                          // generic
        {"CT", "NT", "NT", "CT", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                         // generic
        {"CT", "NT", "NT", "H", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                          // generic
        {"H", "NT", "NT", "H", SpeciesTorsion::Cos3Form, {0, 0, 1.2552}},                           // generic
        {"OS", "C", "N", "CT", SpeciesTorsion::Cos3Form, {9.6232, 25.4764, 0}},                     // carbamates
        {"OH", "C", "N", "CT", SpeciesTorsion::Cos3Form, {9.6232, 25.4764, 0}},                     // carbamates
        {"OS", "C", "N", "H", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                           // carbamates
        {"OH", "C", "N", "H", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                           // carbamates
        {"N", "C", "OS", "CT", SpeciesTorsion::Cos3Form, {-8.368, 20.92, 0}},                       // carbamates
        {"N", "C", "OH", "HO", SpeciesTorsion::Cos3Form, {-8.368, 20.92, 0}},                       // carbamates
        {"HC", "NC", "NZ", "NZ", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // azides
        {"CT", "NC", "NZ", "NZ", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // azides
        {"CT", "CT", "NC", "NZ", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // azides
        /* { "HC",	"CT",	"C",	"Seq",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 }, */         // thiocarbonyl
        /* { "Seq",	"C",	"N",	"H",	SpeciesTorsion::Cos3Form,	4.184,	20.92,	0,	0 }, */ // thioamides
        /* { "Seq",	"C",	"N",	"CT",	SpeciesTorsion::Cos3Form,	0,	27.196,	0,	0 }, */     // thioamides
        /* { "Seq",	"C",	"N",	"CA",	SpeciesTorsion::Cos3Form,	0,	27.196,	0,	0 }, */     // thioamides
        /* { "Seq",	"C",	"N",	"CM",	SpeciesTorsion::Cos3Form,	0,	27.196,	0,	0 }, */     // thioamides
        /* { "Seq",	"C",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 }, */     // thioesters
        /* { "Seq",	"C",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	0,	23.012,	0,	0 }, */     // thioacids
        {"CT", "S", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 2.5104, 0}},                          // thioanisole
        {"O", "C", "C", "CT", SpeciesTorsion::Cos3Form, {0, 2.092, 0}},                             // dicarbonyls
        {"O", "C", "C", "HC", SpeciesTorsion::Cos3Form, {0, 0.8368, 0}},                            // dicarbonyls
        {"HC", "C", "C", "CT", SpeciesTorsion::Cos3Form, {3.3472, -3.17984, 0}},                    // dicarbonyls
        {"HC", "C", "C", "HC", SpeciesTorsion::Cos3Form, {3.3472, 0, 0}},                           // dicarbonyls
        {"CT", "C", "C", "CT", SpeciesTorsion::Cos3Form, {2.9288, -6.276, 0}},                      // dicarbonyls
        {"C", "C", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0.35564}},                          // dicarbonyls
        {"N", "C", "C", "O", SpeciesTorsion::Cos3Form, {0, 0, 0}},                                  // dicarbonyls
        {"N", "C", "C", "HC", SpeciesTorsion::Cos3Form, {-3.7656, 1.2552, 0}},                      // dicarbonyls
        {"N", "C", "C", "CT", SpeciesTorsion::Cos3Form, {-2.092, 0.8368, 0}},                       // dicarbonyls
        {"C", "C", "N", "CT", SpeciesTorsion::Cos3Form, {1.6736, 20.5016, 0}},                      // dicarbonyls
        {"H", "N", "C", "C", SpeciesTorsion::Cos3Form, {0, 20.5016, 0}},                            // dicarbonyls
        {"O", "C", "C", "O", SpeciesTorsion::Cos3Form, {6.6944, 13.3888, 0}},                       // dicarbonyls
        {"CT", "C", "N", "OH", SpeciesTorsion::Cos3Form, {19.0037, 27.627, 4.37228}},               // hydroxamic
        {"O", "C", "N", "OH", SpeciesTorsion::Cos3Form, {0, 27.627, 0}},                            // hydroxamic
        {"C", "N", "OH", "HO", SpeciesTorsion::Cos3Form, {23.0915, -28.0328, 2.4309}},              // hydroxamic
        {"H", "N", "OH", "HO", SpeciesTorsion::Cos3Form, {11.3888, -21.5643, 0}},                   // hydroxamic
        {"F", "CT", "CA", "CA", SpeciesTorsion::Cos3Form, {0, 1.8828, 0}},                          // fluoromethyl
        {"F", "CT", "CW", "*", SpeciesTorsion::Cos3Form, {0, 1.8828, 0}},                           // fluoromethyl
        {"F", "CT", "CS", "*", SpeciesTorsion::Cos3Form, {0, 1.8828, 0}},                           // fluoromethyl
        {"Cl", "CT", "CA", "CA", SpeciesTorsion::Cos3Form, {0, -1.6736, 0}},                        // chloromethyl
        {"Cl", "CT", "CW", "*", SpeciesTorsion::Cos3Form, {0, -1.6736, 0}},                         // chloromethyl
        {"Cl", "CT", "CS", "*", SpeciesTorsion::Cos3Form, {0, -1.6736, 0}},                         // chloromethyl
        {"Cl", "CT", "C", "O", SpeciesTorsion::Cos3Form, {-2.7196, 0, 0}},                          // 2-chloroamide
        {"Cl", "CT", "C", "N", SpeciesTorsion::Cos3Form, {2.7196, 0, 0}},                           // 2-chloroamide
        {"CT", "CT", "CT", "Si", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // silane
        {"CT", "CT", "Si", "CT", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // silane
        {"CT", "CT", "Si", "H", SpeciesTorsion::Cos3Form, {0, 0, 0.6276}},                          // silane
        {"HC", "CT", "Si", "H", SpeciesTorsion::Cos3Form, {0, 0, 0.75312}},                         // silane
        {"Si", "CT", "CT", "Si", SpeciesTorsion::Cos3Form, {21.7568, -2.092, 0}},                   // silane
        {"Si", "CT", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0.6276}},                         // silane
        {"CT", "Si", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0.75312}},                        // silane
        {"OS", "Si", "CT", "HC", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // tentative
        {"OS", "Si", "OS", "Si", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // tentative
        {"Si", "OS", "Si", "CT", SpeciesTorsion::Cos3Form, {0, 0, 0}},                              // tentative
        {"CT", "CT", "CT", "CT", SpeciesTorsion::Cos4Form, {27.7064, 3.96643, -5.80739, -8.86171}}, // perfluoroalkane
        {"N2", "CA", "CA", "CA", SpeciesTorsion::Cos4Form, {0, 6.77808, 0, -1.84096}}               // benzamidine
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
