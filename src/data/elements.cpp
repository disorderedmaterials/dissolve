/*
    *** Element Data
    *** src/data/elements.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "data/elements.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include <ctype.h>

/*
 * Element
 */

Element::Element(int Z, std::string_view name, std::string_view symbol, int group)
    : Z_(Z), name_{name}, symbol_{symbol}, group_(group)
{
}

/*
 * Element Information
 */

// Return atomic number (Z)
int Element::Z() const { return Z_; }

// Return whether the element is unknown
bool Element::isUnknown() const { return Z_ == 0; }

// Return name of element
std::string_view Element::name() const { return name_; }

// Return symbol of element
std::string_view Element::symbol() const { return symbol_; }

// Return group for element
int Element::group() const { return group_; }

/*
 * Elements Helper Class
 */

// Instantiate / return array of element data
Element *Elements::elements()
{
    // Basic element data
    static Element elementData[] = {
        {0, "Unknown", "XX", 0},        {1, "Hydrogen", "H", 1},        {2, "Helium", "He", 18},
        {3, "Lithium", "Li", 1},        {4, "Beryllium", "Be", 2},      {5, "Boron", "B", 13},
        {6, "Carbon", "C", 14},         {7, "Nitrogen", "N", 15},       {8, "Oxygen", "O", 16},
        {9, "Fluorine", "F", 17},       {10, "Neon", "Ne", 18},         {11, "Sodium", "Na", 1},
        {12, "Magnesium", "Mg", 2},     {13, "Aluminium", "Al", 13},    {14, "Silicon", "Si", 14},
        {15, "Phosphorus", "P", 15},    {16, "Sulfur", "S", 16},        {17, "Chlorine", "Cl", 17},
        {18, "Argon", "Ar", 18},        {19, "Potassium", "K", 1},      {20, "Calcium", "Ca", 2},
        {21, "Scandium", "Sc", 3},      {22, "Titanium", "Ti", 4},      {23, "Vanadium", "V", 5},
        {24, "Chromium", "Cr", 6},      {25, "Manganese", "Mn", 7},     {26, "Iron", "Fe", 8},
        {27, "Cobalt", "Co", 9},        {28, "Nickel", "Ni", 10},       {29, "Copper", "Cu", 11},
        {30, "Zinc", "Zn", 12},         {31, "Gallium", "Ga", 13},      {32, "Germanium", "Ge", 14},
        {33, "Arsenic", "As", 15},      {34, "Selenium", "Se", 16},     {35, "Bromine", "Br", 17},
        {36, "Krypton", "Kr", 18},      {37, "Rubidium", "Rb", 1},      {38, "Strontium", "Sr", 2},
        {39, "Yttrium", "Y", 3},        {40, "Zirconium", "Zr", 4},     {41, "Niobium", "Nb", 5},
        {42, "Molybdenum", "Mo", 6},    {43, "Technetium", "Tc", 7},    {44, "Ruthenium", "Ru", 8},
        {45, "Rhodium", "Rh", 9},       {46, "Palladium", "Pd", 10},    {47, "Silver", "Ag", 11},
        {48, "Cadmium", "Cd", 12},      {49, "Indium", "In", 13},       {50, "Tin", "Sn", 14},
        {51, "Antimony", "Sb", 15},     {52, "Tellurium", "Te", 16},    {53, "Iodine", "I", 17},
        {54, "Xenon", "Xe", 18},        {55, "Caesium", "Cs", 1},       {56, "Barium", "Ba", 2},
        {57, "Lanthanum", "La", 98},    {58, "Cerium", "Ce", 98},       {59, "Praesodymium", "Pr", 98},
        {60, "Neodymium", "Nd", 98},    {61, "Prometheum", "Pm", 98},   {62, "Samarium", "Sm", 98},
        {63, "Europium", "Eu", 98},     {64, "Gadolinium", "Gd", 98},   {65, "Terbium", "Tb", 98},
        {66, "Dysprosium", "Dy", 98},   {67, "Holmium", "Ho", 98},      {68, "Erbium", "Er", 98},
        {69, "Thulium", "Tm", 98},      {70, "Ytterbium", "Yb", 98},    {71, "Lutetium", "Lu", 3},
        {72, "Hafnium", "Hf", 4},       {73, "Tantalum", "Ta", 5},      {74, "Tungsten", "W", 6},
        {75, "Rhenium", "Re", 7},       {76, "Osmium", "Os", 8},        {77, "Iridium", "Ir", 9},
        {78, "Platinum", "Pt", 10},     {79, "Gold", "Au", 11},         {80, "Mercury", "Hg", 12},
        {81, "Thallium", "Tl", 13},     {82, "Lead", "Pb", 14},         {83, "Bismuth", "Bi", 15},
        {84, "Polonium", "Po", 16},     {85, "Astatine", "At", 17},     {86, "Radon", "Rn", 18},
        {87, "Francium", "Fr", 1},      {88, "Radium", "Ra", 2},        {89, "Actinium", "Ac", 99},
        {90, "Thorium", "Th", 99},      {91, "Protactinium", "Pa", 99}, {92, "Uranium", "U", 99},
        {93, "Neptunium", "Np", 99},    {94, "Plutonium", "Pu", 99},    {95, "Americium", "Am", 99},
        {96, "Curium", "Cm", 99},       {97, "Berkelium", "Bk", 99},    {98, "Californium", "Cf", 99},
        {99, "Einsteinium", "Es", 99},  {100, "Fermium", "Fm", 99},     {101, "Mendelevium", "Md", 99},
        {102, "Nobelium", "No", 99},    {103, "Lawrencium", "Lr", 3},   {104, "Rutherfordium", "Rf", 4},
        {105, "Dubnium", "Db", 5},      {106, "Seaborgium", "Sg", 6},   {107, "Bohrium", "Bh", 7},
        {108, "Hassium", "Hs", 8},      {109, "Meitnerium", "Mt", 9},   {110, "Darmstadtium", "Ds", 10},
        {111, "Roentgenium", "Rg", 11}, {112, "Copernicium", "Cn", 12}, {113, "Nihonium", "Nh", 13},
        {114, "Flerovium", "Fl", 14},   {115, "Moscovium", "Mc", 15},   {116, "Livermorium", "Lv", 16},
        {117, "Tennessine", "Ts", 17},  {118, "Oganesson", "Og", 18}};

    return elementData;
}

// Return Element with corresponding Z
Element &Elements::element(int Z)
{
    if ((Z < 0) || (Z > nElements()))
    {
        Messenger::error("Element with Z={} is out of range!\n", Z);
        return elements()[0];
    }

    return elements()[Z];
}

// Return Element with corresponding symbol
Element &Elements::element(std::string_view symbol)
{
    std::string cleaned;
    auto nDigits = 0;
    for (const auto c : symbol)
    {
        if (c == ' ')
            continue;
        else if (std::isalpha(c))
            cleaned += c;
        else if (std::isdigit(c))
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
        if ((Z < 0) || (Z > nElements()))
            return elements()[0];
        else
            return elements()[Z];
    }
    else
        for (int n = 0; n < nElements(); n++)
            if (cleaned == elements()[n].symbol())
                return elements()[n];

    return elements()[0];
}

// Return pointer to Element with corresponding Z
Element *Elements::elementPointer(int Z) { return &element(Z); }

// Return pointer to Element with corresponding symbol
Element *Elements::elementPointer(std::string_view symbol) { return &element(symbol); }

// Return name of element with specified Z
std::string_view Elements::name(int Z) { return element(Z).name(); }

// Return symbol of element with specified Z
std::string_view Elements::symbol(int Z) { return element(Z).symbol(); }

// Return group for element with specified Z
int Elements::group(int Z) { return element(Z).group(); }

/*
 * ElementReference
 */

ElementReference::ElementReference(int z) : element_(Elements::element(z)) {}

ElementReference::~ElementReference() {}

// Return referenced element
const Element &ElementReference::element() const { return element_; }

// Return atomic number (Z)
int ElementReference::Z() const { return element_.Z(); }

// Return name of element
std::string_view ElementReference::name() const { return element_.name(); }

// Return symbol of element
std::string_view ElementReference::symbol() const { return element_.symbol(); }
