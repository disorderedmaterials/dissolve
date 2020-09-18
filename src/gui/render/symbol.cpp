/*
    *** Symbol Definitions
    *** src/gui/render/symbol.cpp
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

#include "gui/render/symbol.h"

// Static list of symbols
SymbolData SymbolData::symbols[] = {{0x00D7, "mult", "Multiplication Sign"},
                                    {0x00F7, "div", "Division Sign"},
                                    {0x212B, "angstrom", "Angstrom"},
                                    {0x00B0, "degree", "Degree"},
                                    {0x03B1, "alpha", "Greek Small Letter Alpha"},
                                    {0x03B2, "beta", "Greek Small Letter Beta"},
                                    {0x03B3, "gamma", "Greek Small Letter Gamma"},
                                    {0x03B4, "delta", "Greek Small Letter Delta"},
                                    {0x03B5, "epsilon", "Greek Small Letter Epsilon"},
                                    {0x03B6, "zeta", "Greek Small Letter Zeta"},
                                    {0x03B7, "eta", "Greek Small Letter Eta"},
                                    {0x03B8, "theta", "Greek Small Letter Theta"},
                                    {0x03B9, "iota", "Greek Small Letter Iota"},
                                    {0x03BA, "kappa", "Greek Small Letter Kappa"},
                                    {0x03BB, "lambda", "Greek Small Letter Lambda"},
                                    {0x03BC, "mu", "Greek Small Letter Mu"},
                                    {0x03BD, "nu", "Greek Small Letter Nu"},
                                    {0x03BE, "xi", "Greek Small Letter Xi"},
                                    {0x03BF, "omicron", "Greek Small Letter Omicron"},
                                    {0x03C0, "pi", "Greek Small Letter Pi"},
                                    {0x03C1, "rho", "Greek Small Letter Rho"},
                                    {0x03C3, "sigma", "Greek Small Letter Sigma"},
                                    {0x03C4, "tau", "Greek Small Letter Tau"},
                                    {0x03C5, "upsilon", "Greek Small Letter Upsilon"},
                                    {0x03C6, "phi", "Greek Small Letter Phi"},
                                    {0x03C7, "chi", "Greek Small Letter Chi"},
                                    {0x03C8, "psi", "Greek Small Letter Psi"},
                                    {0x03C9, "omega", "Greek Small Letter Omega"},
                                    {0x0391, "Alpha", "Greek Capital Letter Alpha"},
                                    {0x0392, "Beta	", "Greek Capital Letter Beta"},
                                    {0x0393, "Gamma", "Greek Capital Letter Gamma"},
                                    {0x0394, "Delta", "Greek Capital Letter Delta"},
                                    {0x0395, "Epsilon", "Greek Capital Letter Epsilon"},
                                    {0x0396, "Zeta", "Greek Capital Letter Zeta"},
                                    {0x0397, "Eta", "Greek Capital Letter Eta"},
                                    {0x0398, "Theta", "Greek Capital Letter Theta"},
                                    {0x0399, "Iota", "Greek Capital Letter Iota"},
                                    {0x039A, "Kappa", "Greek Capital Letter Kappa"},
                                    {0x039B, "Lambda", "Greek Capital Letter Lambda"},
                                    {0x039C, "Mu", "Greek Capital Letter Mu"},
                                    {0x039D, "Nu", "Greek Capital Letter Nu"},
                                    {0x039E, "Xi", "Greek Capital Letter Xi"},
                                    {0x039F, "Omicron", "Greek Capital Letter Omicron"},
                                    {0x03A0, "Pi", "Greek Capital Letter Pi"},
                                    {0x03A1, "Rho", "Greek Capital Letter Rho"},
                                    {0x03A3, "Sigma", "Greek Capital Letter Sigma"},
                                    {0x03A4, "Tau", "Greek Capital Letter Tau"},
                                    {0x03A5, "Upsilon", "Greek Capital Letter Upsilon"},
                                    {0x03A6, "Phi", "Greek Capital Letter Phi"},
                                    {0x03A7, "Chi", "Greek Capital Letter Chi"},
                                    {0x03A8, "Psi", "Greek Capital Letter Psi"},
                                    {0x03A9, "Omega", "Greek Capital Letter Omega"}};

// Return named symbol
SymbolData *SymbolData::symbol(QString name)
{
    for (int n = 0; n < nSymbols; ++n)
        if (name == symbols[n].name)
            return &symbols[n];

    return nullptr;
}
