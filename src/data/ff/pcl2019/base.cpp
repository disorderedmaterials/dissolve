// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "data/ff/pcl2019/base.h"

PCL2019BaseForcefield::PCL2019BaseForcefield()
{
    addParameters("B", {0.39750, 3.58});
    addParameters("Br", {0.86000, 3.97});
    addParameters("C3A", {0.29288, 3.55});
    addParameters("CA", {0.29288, 3.55});
    addParameters("CF", {0.27614, 3.50});
    addParameters("CG", {0.20900, 2.25});
    addParameters("CK", {0.35980, 3.40});
    addParameters("Cl", {0.83000, 3.65});
    addParameters("CN", {0.27614, 3.30});
    addParameters("CO", {0.43932, 3.75});
    addParameters("CR", {0.29288, 3.55});
    addParameters("CT", {0.27614, 3.50});
    addParameters("CW", {0.29288, 3.55});
    addParameters("CWH", {0.29288, 2.93});
    addParameters("CZ", {0.27614, 3.30});
    addParameters("F", {0.25520, 3.12});
    addParameters("FC", {0.22175, 2.95});
    addParameters("FB", {0.25540, 3.118});
    addParameters("HA", {0.12552, 2.42});
    addParameters("HC", {0.12552, 2.50});
    addParameters("HG", {0.00000, 0.00});
    addParameters("HN", {0.00000, 0.00});
    addParameters("HNA", {0.00000, 0.00});
    addParameters("HO", {0.00000, 0.00});
    addParameters("Li", {0.07648, 2.126});
    addParameters("N3", {0.71128, 3.25});
    addParameters("NA", {0.71128, 3.25});
    addParameters("Na", {0.01160, 3.33});
    addParameters("NB", {0.71128, 3.25});
    addParameters("NC", {0.71128, 3.20});
    addParameters("NG", {0.71128, 3.25});
    addParameters("NK", {0.71128, 3.25});
    addParameters("NT", {0.71128, 3.25});
    addParameters("NZ", {0.71128, 3.20});
    addParameters("O2", {0.87864, 2.96});
    addParameters("OB", {0.83736, 3.15});
    addParameters("OC", {0.58600, 2.90});
    addParameters("O2F", {0.87864, 2.96}); // Trifluoroacetate
    addParameters("OH", {0.71128, 3.12});
    addParameters("OS", {0.83700, 3.15});
    addParameters("P", {0.83680, 3.74});
    addParameters("PT", {0.83680, 3.74});
    addParameters("SB", {1.04600, 3.55});
    addParameters("SK", {1.04600, 3.55});
    addParameters("SO", {1.04600, 3.55});
}

PCL2019BaseForcefield::~PCL2019BaseForcefield() = default;

/*
 * Definition
 */

// Return formatted publication references
std::string_view PCL2019BaseForcefield::publicationReferences() const
{
    return "The complete ionic liquids forcefield of Padua and Canongia Lopes, as found at <a "
           "href=\"https://github.com/agiliopadua/ilff](https://github.com/agiliopadua/ilff/\"></a> (commit <a "
           "href=\"https://github.com/agiliopadua/ilff/commit/1df07bdaf24897a85540d9e859843442e1fd8355\">1df07bd</a>)), "
           "covering a variety of normal and fluorinated cations and anions.";
}

// Return short-range interaction style for AtomTypes
ShortRangeFunctions::Form PCL2019BaseForcefield::shortRangeForm() const
{
    return ShortRangeFunctions::Form::LennardJonesGeometric;
}
