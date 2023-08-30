// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/species.h"
#include "data/elements.h"

namespace UnitTest
{
// Return argon test species
const Species &argonSpecies()
{
    static Species argon_;
    if (argon_.nAtoms() == 0)
    {
        argon_.setName("Argon");
        argon_.addAtom(Elements::Ar, {0.000000, 0.000000, 0.000000});
    }

    return argon_;
}

// Return diatomic test species
const Species &diatomicSpecies()
{
    static Species diatomic_;
    if (diatomic_.nAtoms() == 0)
    {
        diatomic_.setName("Diatomic");
        diatomic_.addAtom(Elements::N, {0.000000, 0.000000, 0.000000});
        diatomic_.addAtom(Elements::O, {0.000000, 1.100000, 0.000000});
        diatomic_.addMissingBonds();
    }

    return diatomic_;
}

// Return methane test species
const Species &methaneSpecies()
{
    static Species methane_;
    if (methane_.nAtoms() == 0)
    {
        methane_.setName("Methane");
        methane_.addAtom(Elements::C, {0.000000, 0.000000, 0.000000});
        methane_.addAtom(Elements::H, {0.000000, 1.080000, 0.000000});
        methane_.addAtom(Elements::H, {0.000000, -0.360511, -1.018053});
        methane_.addAtom(Elements::H, {0.881973, -0.359744, 0.509026});
        methane_.addAtom(Elements::H, {-0.881973, -0.359744, 0.509026});
        methane_.addMissingBonds();
    }
    return methane_;
}

// Return ethane test species
const Species &ethaneSpecies()
{
    static Species ethane_;
    if (ethane_.nAtoms() == 0)
    {
        ethane_.setName("Ethane");
        ethane_.addAtom(Elements::C, {0.000000, 0.000000, 0.000000});
        ethane_.addAtom(Elements::C, {0.000000, 1.540000, 0.000000});
        ethane_.addAtom(Elements::H, {0.000000, -0.360511, -1.018053});
        ethane_.addAtom(Elements::H, {0.881973, -0.359744, 0.509026});
        ethane_.addAtom(Elements::H, {-0.881973, -0.359744, 0.509026});
        ethane_.addAtom(Elements::H, {0.000000, 1.900511, 1.018053});
        ethane_.addAtom(Elements::H, {0.881973, 1.899744, -0.509026});
        ethane_.addAtom(Elements::H, {-0.881973, 1.899744, -0.509026});
        ethane_.addMissingBonds();
    }
    return ethane_;
}

// Construct methanol test species
const Species &methanolSpecies()
{
    static Species methanol_;
    if (methanol_.nAtoms() == 0)
    {
        methanol_.setName("Methanol");
        methanol_.addAtom(Elements::C, {0.000078, -0.353546, -0.169274});
        methanol_.addAtom(Elements::H, {0.000078, -0.714057, -1.187327});
        methanol_.addAtom(Elements::H, {0.882051, -0.713290, 0.339752});
        methanol_.addAtom(Elements::H, {-0.881895, -0.713290, 0.339752});
        methanol_.addAtom(Elements::O, {-0.000129, 1.066453, -0.170343});
        methanol_.addAtom(Elements::H, {-0.000177, 1.394279, 0.753199});
        methanol_.addMissingBonds();
    }
    return methanol_;
}

// Return 1,4-difluorobenzene test species
const Species &difluorobenzeneSpecies()
{
    static Species dfb_;
    if (dfb_.nAtoms() == 0)
    {
        dfb_.setName("Difluorobenzene");
        dfb_.addAtom(Elements::C, {-1.399000e+00, 1.600000e-01, 0.000000e+00});
        dfb_.addAtom(Elements::C, {-5.610000e-01, 1.293000e+00, 0.000000e+00});
        dfb_.addAtom(Elements::C, {8.390000e-01, 1.132000e+00, 0.000000e+00});
        dfb_.addAtom(Elements::C, {1.399000e+00, -1.600000e-01, 0.000000e+00});
        dfb_.addAtom(Elements::C, {5.600000e-01, -1.293000e+00, 0.000000e+00});
        dfb_.addAtom(Elements::C, {-8.390000e-01, -1.132000e+00, 0.000000e+00});
        dfb_.addAtom(Elements::F, {1.483000e+00, 2.001000e+00, 0.000000e+00});
        dfb_.addAtom(Elements::H, {2.472000e+00, -2.840000e-01, 0.000000e+00});
        dfb_.addAtom(Elements::H, {9.910000e-01, -2.284000e+00, 0.000000e+00});
        dfb_.addAtom(Elements::F, {-1.483000e+00, -2.000000e+00, 0.000000e+00});
        dfb_.addAtom(Elements::H, {-2.472000e+00, 2.820000e-01, 0.000000e+00});
        dfb_.addAtom(Elements::H, {-9.900000e-01, 2.284000e+00, 0.000000e+00});
        dfb_.addMissingBonds();
    }
    return dfb_;
}

// Return rings test species
const Species &ringsSpecies()
{
    static Species rings_;
    if (rings_.nAtoms() == 0)
    {
        rings_.setName("Rings");
        rings_.addAtom(Elements::C, {-0.208763, -0.975789, -0.095504});
        rings_.addAtom(Elements::C, {0.045228, 0.295623, -0.380595});
        rings_.addAtom(Elements::C, {1.300571, 0.857602, -0.348819});
        rings_.addAtom(Elements::C, {2.360934, -0.005443, -0.027080});
        rings_.addAtom(Elements::C, {2.111776, -1.366987, 0.240107});
        rings_.addAtom(Elements::C, {0.805131, -1.869462, 0.219207});
        rings_.addAtom(Elements::H, {1.468477, 1.902060, -0.572229});
        rings_.addAtom(Elements::H, {3.373539, 0.371667, 0.002685});
        rings_.addAtom(Elements::H, {2.939423, -2.029266, 0.465935});
        rings_.addAtom(Elements::H, {0.596002, -2.910744, 0.426097});
        rings_.addAtom(Elements::N, {-1.289150, 0.716693, -0.702798});
        rings_.addAtom(Elements::C, {-1.646291, -0.686037, -0.306734});
        rings_.addAtom(Elements::C, {-1.785682, 1.724996, 0.228719});
        rings_.addAtom(Elements::H, {-1.619801, 1.449463, 1.291036});
        rings_.addAtom(Elements::H, {-1.288061, 2.694431, 0.011695});
        rings_.addAtom(Elements::H, {-2.873536, 1.867631, 0.046101});
        rings_.addAtom(Elements::H, {-2.075934, -1.268474, -1.147377});
        rings_.addAtom(Elements::H, {-2.213863, -0.767963, 0.649555});
        rings_.addMissingBonds();
    }
    return rings_;
}

// Return geometry test species
const Species &geometricSpecies()
{
    static Species geometric_;
    if (geometric_.nAtoms() == 0)
    {
        geometric_.setName("Geometric");
        geometric_.addAtom(Elements::P, {-5.824000, 3.981000, 0.000000});
        geometric_.addAtom(Elements::P, {-1.824000, 3.981000, 0.000000});
        geometric_.addAtom(Elements::F, {-1.824000, 4.981000, 0.000000});
        geometric_.addAtom(Elements::P, {2.176000, 3.981000, 0.000000});
        geometric_.addAtom(Elements::F, {2.176000, 4.981000, 0.000000});
        geometric_.addAtom(Elements::F, {2.176000, 2.981000, 0.000000});
        geometric_.addAtom(Elements::P, {6.176000, 4.003000, 0.000000});
        geometric_.addAtom(Elements::F, {6.176000, 5.003000, 0.000000});
        geometric_.addAtom(Elements::F, {5.241000, 3.463000, 0.000000});
        geometric_.addAtom(Elements::F, {7.111000, 3.463000, 0.000000});
        geometric_.addAtom(Elements::P, {-5.000000, 0.000000, 0.000000});
        geometric_.addAtom(Elements::F, {-5.000000, 1.080000, 0.000000});
        geometric_.addAtom(Elements::F, {-5.000000, -0.361000, -1.018000});
        geometric_.addAtom(Elements::F, {-4.118000, -0.360000, 0.509000});
        geometric_.addAtom(Elements::F, {-5.882000, -0.360000, 0.509000});
        geometric_.addAtom(Elements::P, {0.000000, 0.000000, 0.000000});
        geometric_.addAtom(Elements::F, {0.000000, 1.200000, 0.000000});
        geometric_.addAtom(Elements::F, {0.000000, -1.200000, 0.000000});
        geometric_.addAtom(Elements::F, {-1.280000, 0.000000, 0.000000});
        geometric_.addAtom(Elements::F, {1.280000, 0.000000, 0.000000});
        geometric_.addAtom(Elements::F, {0.000000, 0.000000, -1.200000});
        geometric_.addAtom(Elements::F, {0.000000, 0.000000, 1.200000});
        geometric_.addAtom(Elements::P, {4.800000, -0.020000, 0.000000});
        geometric_.addAtom(Elements::F, {4.800000, 1.180000, 0.000000});
        geometric_.addAtom(Elements::F, {3.865000, -0.560000, 0.000000});
        geometric_.addAtom(Elements::F, {5.735000, -0.560000, 0.000000});
        geometric_.addAtom(Elements::F, {4.800000, -0.020000, -1.200000});
        geometric_.addAtom(Elements::F, {4.800000, -0.020000, 1.200000});
        geometric_.addAtom(Elements::P, {-3.000000, -4.300000, 0.000000});
        geometric_.addAtom(Elements::F, {-3.000000, -3.100000, 0.000000});
        geometric_.addAtom(Elements::F, {-4.280000, -4.300000, 0.000000});
        geometric_.addAtom(Elements::F, {-1.720000, -4.300000, 0.000000});
        geometric_.addAtom(Elements::P, {3.000000, -4.000000, 0.000000});
        geometric_.addAtom(Elements::F, {3.000000, -2.800000, 0.000000});
        geometric_.addAtom(Elements::F, {3.000000, -5.200000, 0.000000});
        geometric_.addAtom(Elements::F, {1.720000, -4.000000, 0.000000});
        geometric_.addAtom(Elements::F, {4.280000, -4.000000, 0.000000});
        geometric_.addMissingBonds();
    }
    return geometric_;
}

// Return benzene test species
const Species &benzeneSpecies()
{
    static Species benzene_;
    if (benzene_.nAtoms() == 0)
    {
        benzene_.setName("Benzene");
        benzene_.addAtom(Elements::C, {-1.390000, 0.000000, 0.000000});
        benzene_.addAtom(Elements::C, {-0.695000, 1.203775, 0.000000});
        benzene_.addAtom(Elements::C, {0.695000, 1.203775, 0.000000});
        benzene_.addAtom(Elements::C, {1.390000, 0.000000, 0.000000});
        benzene_.addAtom(Elements::C, {0.695000, -1.203775, 0.000000});
        benzene_.addAtom(Elements::C, {-0.695000, -1.203775, 0.000000});
        benzene_.addAtom(Elements::H, {1.195000, 2.069801, 0.000000});
        benzene_.addAtom(Elements::H, {2.390000, 0.000000, 0.000000});
        benzene_.addAtom(Elements::H, {1.195000, -2.069801, 0.000000});
        benzene_.addAtom(Elements::H, {-1.195000, -2.069801, 0.000000});
        benzene_.addAtom(Elements::H, {-2.390000, 0.000000, 0.000000});
        benzene_.addAtom(Elements::H, {-1.195000, 2.069801, 0.000000});
        benzene_.addMissingBonds();
    }
    return benzene_;
}

} // namespace UnitTest
