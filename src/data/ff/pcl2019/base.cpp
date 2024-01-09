// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/pcl2019/base.h"

PCL2019BaseForcefield::PCL2019BaseForcefield()
{
    addParameters("B", "epsilon=0.39750 sigma=3.58");
    addParameters("Br", "epsilon=0.86000 sigma=3.97");
    addParameters("C3A", "epsilon=0.29288 sigma=3.55");
    addParameters("CA", "epsilon=0.29288 sigma=3.55");
    addParameters("CF", "epsilon=0.27614 sigma=3.50");
    addParameters("CG", "epsilon=0.20900 sigma=2.25");
    addParameters("CK", "epsilon=0.35980 sigma=3.40");
    addParameters("Cl", "epsilon=0.83000 sigma=3.65");
    addParameters("CN", "epsilon=0.27614 sigma=3.30");
    addParameters("CO", "epsilon=0.43932 sigma=3.75");
    addParameters("CR", "epsilon=0.29288 sigma=3.55");
    addParameters("CT", "epsilon=0.27614 sigma=3.50");
    addParameters("CW", "epsilon=0.29288 sigma=3.55");
    addParameters("CWH", "epsilon=0.29288 sigma=2.93");
    addParameters("CZ", "epsilon=0.27614 sigma=3.30");
    addParameters("F", "epsilon=0.25520 sigma=3.12");
    addParameters("FC", "epsilon=0.22175 sigma=2.95");
    addParameters("FB", "epsilon=0.25540 sigma=3.118");
    addParameters("HA", "epsilon=0.12552 sigma=2.42");
    addParameters("HC", "epsilon=0.12552 sigma=2.50");
    addParameters("HG", "epsilon=0.00000 sigma=0.00");
    addParameters("HN", "epsilon=0.00000 sigma=0.00");
    addParameters("HNA", "epsilon=0.00000 sigma=0.00");
    addParameters("HO", "epsilon=0.00000 sigma=0.00");
    addParameters("Li", "epsilon=0.07648 sigma=2.126");
    addParameters("N3", "epsilon=0.71128 sigma=3.25");
    addParameters("NA", "epsilon=0.71128 sigma=3.25");
    addParameters("Na", "epsilon=0.01160 sigma=3.33");
    addParameters("NB", "epsilon=0.71128 sigma=3.25");
    addParameters("NC", "epsilon=0.71128 sigma=3.20");
    addParameters("NG", "epsilon=0.71128 sigma=3.25");
    addParameters("NK", "epsilon=0.71128 sigma=3.25");
    addParameters("NT", "epsilon=0.71128 sigma=3.25");
    addParameters("NZ", "epsilon=0.71128 sigma=3.20");
    addParameters("O2", "epsilon=0.87864 sigma=2.96");
    addParameters("OB", "epsilon=0.83736 sigma=3.15");
    addParameters("OC", "epsilon=0.58600 sigma=2.90");
    addParameters("O2F", "epsilon=0.87864 sigma=2.96"); // Trifluoroacetate
    addParameters("OH", "epsilon=0.71128 sigma=3.12");
    addParameters("OS", "epsilon=0.83700 sigma=3.15");
    addParameters("P", "epsilon=0.83680 sigma=3.74");
    addParameters("PT", "epsilon=0.83680 sigma=3.74");
    addParameters("SB", "epsilon=1.04600 sigma=3.55");
    addParameters("SK", "epsilon=1.04600 sigma=3.55");
    addParameters("SO", "epsilon=1.04600 sigma=3.55");
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
