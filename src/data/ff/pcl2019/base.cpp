// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/pcl2019/base.h"

PCL2019BaseForcefield::PCL2019BaseForcefield()
{
    addParameters("B", {3.58, 0.39750});
    addParameters("Br", {3.97, 0.86000});
    addParameters("C3A", {3.55, 0.29288});
    addParameters("CA", {3.55, 0.29288});
    addParameters("CF", {3.50, 0.27614});
    addParameters("CG", {2.25, 0.20900});
    addParameters("CK", {3.40, 0.35980});
    addParameters("Cl", {3.65, 0.83000});
    addParameters("CN", {3.30, 0.27614});
    addParameters("CO", {3.75, 0.43932});
    addParameters("CR", {3.55, 0.29288});
    addParameters("CT", {3.50, 0.27614});
    addParameters("CW", {3.55, 0.29288});
    addParameters("CWH", {2.93, 0.29288});
    addParameters("CZ", {3.30, 0.27614});
    addParameters("F", {3.12, 0.25520});
    addParameters("FC", {2.95, 0.22175});
    addParameters("FB", {3.118, 0.25540});
    addParameters("HA", {2.42, 0.12552});
    addParameters("HC", {2.50, 0.12552});
    addParameters("HG", {0.00, 0.00000});
    addParameters("HN", {0.00, 0.00000});
    addParameters("HNA", {0.00, 0.00000});
    addParameters("HO", {0.00, 0.00000});
    addParameters("Li", {2.126, 0.07648});
    addParameters("N3", {3.25, 0.71128});
    addParameters("NA", {3.25, 0.71128});
    addParameters("Na", {3.33, 0.01160});
    addParameters("NB", {3.25, 0.71128});
    addParameters("NC", {3.20, 0.71128});
    addParameters("NG", {3.25, 0.71128});
    addParameters("NK", {3.25, 0.71128});
    addParameters("NT", {3.25, 0.71128});
    addParameters("NZ", {3.20, 0.71128});
    addParameters("O2", {2.96, 0.87864});
    addParameters("OB", {3.15, 0.83736});
    addParameters("OC", {2.90, 0.58600});
    addParameters("O2F", {2.96, 0.87864}); // Trifluoroacetate
    addParameters("OH", {3.12, 0.71128});
    addParameters("OS", {3.15, 0.83700});
    addParameters("P", {3.74, 0.83680});
    addParameters("PT", {3.74, 0.83680});
    addParameters("SB", {3.55, 1.04600});
    addParameters("SK", {3.55, 1.04600});
    addParameters("SO", {3.55, 1.04600});
}

PCL2019BaseForcefield::~PCL2019BaseForcefield() {}

/*
 * Definition
 */

// Return formatted publication references
std::string_view PCL2019BaseForcefield::publicationReferences() const { return "TODO!"; }

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType PCL2019BaseForcefield::shortRangeType() const { return Forcefield::LennardJonesGeometricType; }
