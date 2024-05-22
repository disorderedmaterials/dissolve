// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/elements.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include <cassert>
#include <cctype>

namespace Elements
{

/*
 * Element
 */

ElementData::ElementData(Element Z, std::string_view name, std::string_view symbol, int group)
    : Z_(Z), name_{name}, symbol_{symbol}, group_(group)
{
}

/*
 * Element Information
 */

// Return atomic number (Z)
Elements::Element ElementData::Z() const { return Z_; }

// Return whether the element is unknown
bool ElementData::isUnknown() const { return Z_ == 0; }

// Return name of element
std::string_view ElementData::name() const { return name_; }

// Return symbol of element
std::string_view ElementData::symbol() const { return symbol_; }

// Return group for element
int ElementData::group() const { return group_; }

/*
 * Elements Helper Class
 */

// Basic element data
static std::vector<ElementData> elementData_ = {{Elements::Unknown, "Unknown", "XX", 0},
                                                {Elements::H, "Hydrogen", "H", 1},
                                                {Elements::He, "Helium", "He", 18},
                                                {Elements::Li, "Lithium", "Li", 1},
                                                {Elements::Be, "Beryllium", "Be", 2},
                                                {Elements::B, "Boron", "B", 13},
                                                {Elements::C, "Carbon", "C", 14},
                                                {Elements::N, "Nitrogen", "N", 15},
                                                {Elements::O, "Oxygen", "O", 16},
                                                {Elements::F, "Fluorine", "F", 17},
                                                {Elements::Ne, "Neon", "Ne", 18},
                                                {Elements::Na, "Sodium", "Na", 1},
                                                {Elements::Mg, "Magnesium", "Mg", 2},
                                                {Elements::Al, "Aluminium", "Al", 13},
                                                {Elements::Si, "Silicon", "Si", 14},
                                                {Elements::P, "Phosphorus", "P", 15},
                                                {Elements::S, "Sulfur", "S", 16},
                                                {Elements::Cl, "Chlorine", "Cl", 17},
                                                {Elements::Ar, "Argon", "Ar", 18},
                                                {Elements::K, "Potassium", "K", 1},
                                                {Elements::Ca, "Calcium", "Ca", 2},
                                                {Elements::Sc, "Scandium", "Sc", 3},
                                                {Elements::Ti, "Titanium", "Ti", 4},
                                                {Elements::V, "Vanadium", "V", 5},
                                                {Elements::Cr, "Chromium", "Cr", 6},
                                                {Elements::Mn, "Manganese", "Mn", 7},
                                                {Elements::Fe, "Iron", "Fe", 8},
                                                {Elements::Co, "Cobalt", "Co", 9},
                                                {Elements::Ni, "Nickel", "Ni", 10},
                                                {Elements::Cu, "Copper", "Cu", 11},
                                                {Elements::Zn, "Zinc", "Zn", 12},
                                                {Elements::Ga, "Gallium", "Ga", 13},
                                                {Elements::Ge, "Germanium", "Ge", 14},
                                                {Elements::As, "Arsenic", "As", 15},
                                                {Elements::Se, "Selenium", "Se", 16},
                                                {Elements::Br, "Bromine", "Br", 17},
                                                {Elements::Kr, "Krypton", "Kr", 18},
                                                {Elements::Rb, "Rubidium", "Rb", 1},
                                                {Elements::Sr, "Strontium", "Sr", 2},
                                                {Elements::Y, "Yttrium", "Y", 3},
                                                {Elements::Zr, "Zirconium", "Zr", 4},
                                                {Elements::Nb, "Niobium", "Nb", 5},
                                                {Elements::Mo, "Molybdenum", "Mo", 6},
                                                {Elements::Tc, "Technetium", "Tc", 7},
                                                {Elements::Ru, "Ruthenium", "Ru", 8},
                                                {Elements::Rh, "Rhodium", "Rh", 9},
                                                {Elements::Pd, "Palladium", "Pd", 10},
                                                {Elements::Ag, "Silver", "Ag", 11},
                                                {Elements::Cd, "Cadmium", "Cd", 12},
                                                {Elements::In, "Indium", "In", 13},
                                                {Elements::Sn, "Tin", "Sn", 14},
                                                {Elements::Sb, "Antimony", "Sb", 15},
                                                {Elements::Te, "Tellurium", "Te", 16},
                                                {Elements::I, "Iodine", "I", 17},
                                                {Elements::Xe, "Xenon", "Xe", 18},
                                                {Elements::Cs, "Caesium", "Cs", 1},
                                                {Elements::Ba, "Barium", "Ba", 2},
                                                {Elements::La, "Lanthanum", "La", 98},
                                                {Elements::Ce, "Cerium", "Ce", 98},
                                                {Elements::Pr, "Praseodymium", "Pr", 98},
                                                {Elements::Nd, "Neodymium", "Nd", 98},
                                                {Elements::Pm, "Promethium", "Pm", 98},
                                                {Elements::Sm, "Samarium", "Sm", 98},
                                                {Elements::Eu, "Europium", "Eu", 98},
                                                {Elements::Gd, "Gadolinium", "Gd", 98},
                                                {Elements::Tb, "Terbium", "Tb", 98},
                                                {Elements::Dy, "Dysprosium", "Dy", 98},
                                                {Elements::Ho, "Holmium", "Ho", 98},
                                                {Elements::Er, "Erbium", "Er", 98},
                                                {Elements::Tm, "Thulium", "Tm", 98},
                                                {Elements::Yb, "Ytterbium", "Yb", 98},
                                                {Elements::Lu, "Lutetium", "Lu", 3},
                                                {Elements::Hf, "Hafnium", "Hf", 4},
                                                {Elements::Ta, "Tantalum", "Ta", 5},
                                                {Elements::W, "Tungsten", "W", 6},
                                                {Elements::Re, "Rhenium", "Re", 7},
                                                {Elements::Os, "Osmium", "Os", 8},
                                                {Elements::Ir, "Iridium", "Ir", 9},
                                                {Elements::Pt, "Platinum", "Pt", 10},
                                                {Elements::Au, "Gold", "Au", 11},
                                                {Elements::Hg, "Mercury", "Hg", 12},
                                                {Elements::Tl, "Thallium", "Tl", 13},
                                                {Elements::Pb, "Lead", "Pb", 14},
                                                {Elements::Bi, "Bismuth", "Bi", 15},
                                                {Elements::Po, "Polonium", "Po", 16},
                                                {Elements::At, "Astatine", "At", 17},
                                                {Elements::Rn, "Radon", "Rn", 18},
                                                {Elements::Fr, "Francium", "Fr", 1},
                                                {Elements::Ra, "Radium", "Ra", 2},
                                                {Elements::Ac, "Actinium", "Ac", 99},
                                                {Elements::Th, "Thorium", "Th", 99},
                                                {Elements::Pa, "Protactinium", "Pa", 99},
                                                {Elements::U, "Uranium", "U", 99},
                                                {Elements::Np, "Neptunium", "Np", 99},
                                                {Elements::Pu, "Plutonium", "Pu", 99},
                                                {Elements::Am, "Americium", "Am", 99},
                                                {Elements::Cm, "Curium", "Cm", 99},
                                                {Elements::Bk, "Berkelium", "Bk", 99},
                                                {Elements::Cf, "Californium", "Cf", 99},
                                                {Elements::Es, "Einsteinium", "Es", 99},
                                                {Elements::Fm, "Fermium", "Fm", 99},
                                                {Elements::Md, "Mendelevium", "Md", 99},
                                                {Elements::No, "Nobelium", "No", 99},
                                                {Elements::Lr, "Lawrencium", "Lr", 3},
                                                {Elements::Rf, "Rutherfordium", "Rf", 4},
                                                {Elements::Db, "Dubnium", "Db", 5},
                                                {Elements::Sg, "Seaborgium", "Sg", 6},
                                                {Elements::Bh, "Bohrium", "Bh", 7},
                                                {Elements::Hs, "Hassium", "Hs", 8},
                                                {Elements::Mt, "Meitnerium", "Mt", 9},
                                                {Elements::Ds, "Darmstadtium", "Ds", 10},
                                                {Elements::Rg, "Roentgenium", "Rg", 11},
                                                {Elements::Cn, "Copernicium", "Cn", 12},
                                                {Elements::Nh, "Nihonium", "Nh", 13},
                                                {Elements::Fl, "Flerovium", "Fl", 14},
                                                {Elements::Mc, "Moscovium", "Mc", 15},
                                                {Elements::Lv, "Livermorium", "Lv", 16},
                                                {Elements::Ts, "Tennessine", "Ts", 17},
                                                {Elements::Og, "Oganesson", "Og", 18},
                                                {Elements::Art, "Artificial", "Art", 0}

};

// Return Element with corresponding Z
Element element(int Z)
{
    assert((Z >= 0) && (Z < nElements));

    return elementData_[Z].Z();
}

// Return Element with corresponding symbol
Element element(std::string_view symbol)
{
    std::string cleaned;
    auto nDigits = 0;
    for (const auto c : symbol)
    {
        if (c == ' ')
            continue;
        else if (std::isalpha(c))
            cleaned += c;
        else if (std::isdigit(c) && nDigits == cleaned.size())
        {
            cleaned += c;
            ++nDigits;
        }
        else
            break;
    }

    // Pure digit given?
    if (cleaned.size() == nDigits)
    {
        auto Z = std::stoi(cleaned);
        if ((Z < 0) || (Z > nElements))
            return Elements::Unknown;
        else
            return elementData_[Z].Z();
    }
    else
        for (auto n = 0; n < nElements; n++)
            if (DissolveSys::sameString(cleaned, elementData_[n].symbol()))
                return elementData_[n].Z();

    return Elements::Unknown;
}

// Return name of element with specified Z
std::string_view name(Element Z) { return elementData_[Z].name(); }

// Return symbol of element with specified Z
std::string_view symbol(Element Z) { return elementData_[Z].symbol(); }

// Return group for element with specified Z
int group(Element Z) { return elementData_[Z].group(); }

// Return whether the supplied element is metallic
bool isMetallic(Element Z)
{
    const auto grp = group(Z);
    return (grp == 1 && Z != Elements::H) || (grp >= 2 && grp <= 12) || (grp == 13 && Z != Elements::B) ||
           (grp == 14 && Z >= Elements::Sn) || (grp == 15 && Z >= Elements::Bi) || (grp == 16 && Z >= Elements::Po) ||
           (grp == 17 && Z >= Elements::Ts) || (grp == 18 && Z >= Elements::Og);
}

} // namespace Elements
