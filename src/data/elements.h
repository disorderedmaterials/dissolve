// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <string>

namespace Elements
{

// Element Enum
enum Element
{
    Unknown,
    H,
    He,
    Li,
    Be,
    B,
    C,
    N,
    O,
    F,
    Ne,
    Na,
    Mg,
    Al,
    Si,
    P,
    S,
    Cl,
    Ar,
    K,
    Ca,
    Sc,
    Ti,
    V,
    Cr,
    Mn,
    Fe,
    Co,
    Ni,
    Cu,
    Zn,
    Ga,
    Ge,
    As,
    Se,
    Br,
    Kr,
    Rb,
    Sr,
    Y,
    Zr,
    Nb,
    Mo,
    Tc,
    Ru,
    Rh,
    Pd,
    Ag,
    Cd,
    In,
    Sn,
    Sb,
    Te,
    I,
    Xe,
    Cs,
    Ba,
    La,
    Ce,
    Pr,
    Nd,
    Pm,
    Sm,
    Eu,
    Gd,
    Tb,
    Dy,
    Ho,
    Er,
    Tm,
    Yb,
    Lu,
    Hf,
    Ta,
    W,
    Re,
    Os,
    Ir,
    Pt,
    Au,
    Hg,
    Tl,
    Pb,
    Bi,
    Po,
    At,
    Rn,
    Fr,
    Ra,
    Ac,
    Th,
    Pa,
    U,
    Np,
    Pu,
    Am,
    Cm,
    Bk,
    Cf,
    Es,
    Fm,
    Md,
    No,
    Lr,
    Rf,
    Db,
    Sg,
    Bh,
    Hs,
    Mt,
    Ds,
    Rg,
    Cn,
    Nh,
    Fl,
    Mc,
    Lv,
    Ts,
    Og,
    Art,
    nElements
};

// Basic Element Definition (Z, name, symbol)
class ElementData
{
    public:
    ElementData(Element Z, std::string_view name, std::string_view symbol, int group);

    private:
    // Atomic number (Z)
    Element Z_;
    // Element name
    std::string_view name_;
    // Element symbol
    std::string_view symbol_;
    // Group in periodic table
    int group_;

    public:
    // Return atomic number (Z)
    Element Z() const;
    // Return whether the element is unknown
    bool isUnknown() const;
    // Return name of element
    std::string_view name() const;
    // Return symbol of element
    std::string_view symbol() const;
    // Return group for element
    int group() const;
};

// Return enumerated Element with corresponding integer Z
Element element(int Z);
// Return enumerated Element with corresponding symbol
Element element(std::string_view symbol);
// Return name of element with specified Z
std::string_view name(Element Z);
// Return symbol of element with specified Z
std::string_view symbol(Element Z);
// Return group for element with specified Z
int group(Element Z);
// Return whether the supplied element is metallic
bool isMetallic(Element Z);

}; // namespace Elements

// TOML Conversion
namespace toml
{
template <> struct from<Elements::Element>
{
    static Elements::Element from_toml(const toml::value &node) { return Elements::element(toml::get<std::string>(node)); }
};

template <> struct into<Elements::Element>
{
    static toml::basic_value<toml::preserve_comments> into_toml(const Elements::Element &e)
    {
        return Elements::symbol(e).data();
    }
};
} // namespace toml
