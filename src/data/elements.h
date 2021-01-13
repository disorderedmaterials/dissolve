// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/array.h"
#include "templates/list.h"
#include "templates/reflist.h"

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

// Create array of Lists, with array size equal to number of elements defined
template <class T> void createElementListArray(Array<List<T>> &listArray)
{
    /*
     * Create the array, and set all Lists to only disown their items on destruction, rather than deleting them.
     * Need to do this otherwise each datum will be destructed twice - once from the List<T> destructor, and once
     * again from the destruction of the static array.
     */
    listArray.initialise(Elements::nElements);
    for (auto n = 0; n < Elements::nElements; ++n)
        listArray[n].setDisownOnDestruction(true);
}

}; // namespace Elements
