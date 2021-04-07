// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/render/symbol.h"
#include "base/sysfunc.h"
#include <algorithm>

namespace SymbolData
{

// Return symbol vector
const std::vector<std::tuple<Symbol, QString, QString, QString>> &symbols()
{
    static std::vector<std::tuple<Symbol, QString, QString, QString>> symbolData = {
        {MultiplySymbol, "mult", u8"\u00D7", "Multiplication Sign"},
        {DivisionSymbol, "div", u8"\u00F7", "Division Sign"},
        {AngstromSymbol, "angstrom", u8"\u212B", "Angstrom"},
        {DegreeSymbol, "degree", u8"\u00B0", "Degree"},
        {AlphaSymbol, "alpha", u8"\u03B1", "Greek Small Letter Alpha"},
        {BetaSymbol, "beta", u8"\u03B2", "Greek Small Letter Beta"},
        {GammaSymbol, "gamma", u8"\u03B3", "Greek Small Letter Gamma"},
        {DeltaSymbol, "delta", u8"\u03B4", "Greek Small Letter Delta"},
        {EpsilonSymbol, "epsilon", u8"\u03B5", "Greek Small Letter Epsilon"},
        {ZetaSymbol, "zeta", u8"\u03B6", "Greek Small Letter Zeta"},
        {EtaSymbol, "eta", u8"\u03B7", "Greek Small Letter Eta"},
        {ThetaSymbol, "theta", u8"\u03B8", "Greek Small Letter Theta"},
        {IotaSymbol, "iota", u8"\u03B9", "Greek Small Letter Iota"},
        {KappaSymbol, "kappa", u8"\u03BA", "Greek Small Letter Kappa"},
        {LambdaSymbol, "lambda", u8"\u03BB", "Greek Small Letter Lambda"},
        {MuSymbol, "mu", u8"\u03BC", "Greek Small Letter Mu"},
        {NuSymbol, "nu", u8"\u03BD", "Greek Small Letter Nu"},
        {XiSymbol, "xi", u8"\u03BE", "Greek Small Letter Xi"},
        {OmicronSymbol, "omicron", u8"\u03BF", "Greek Small Letter Omicron"},
        {PiSymbol, "pi", u8"\u03C0", "Greek Small Letter Pi"},
        {RhoSymbol, "rho", u8"\u03C1", "Greek Small Letter Rho"},
        {SigmaSymbol, "sigma", u8"\u03C3", "Greek Small Letter Sigma"},
        {TauSymbol, "tau", u8"\u03C4", "Greek Small Letter Tau"},
        {UpsilonSymbol, "upsilon", u8"\u03C5", "Greek Small Letter Upsilon"},
        {PhiSymbol, "phi", u8"\u03C6", "Greek Small Letter Phi"},
        {ChiSymbol, "chi", u8"\u03C7", "Greek Small Letter Chi"},
        {PsiSymbol, "psi", u8"\u03C8", "Greek Small Letter Psi"},
        {OmegaSymbol, "omega", u8"\u03C9", "Greek Small Letter Omega"},
        {CapitalAlphaSymbol, "Alpha", u8"\u0391", "Greek Capital Letter Alpha"},
        {CapitalBetaSymbol, "Beta", u8"\u0392", "Greek Capital Letter Beta"},
        {CapitalGammaSymbol, "Gamma", u8"\u0393", "Greek Capital Letter Gamma"},
        {CapitalDeltaSymbol, "Delta", u8"\u0394", "Greek Capital Letter Delta"},
        {CapitalEpsilonSymbol, "Epsilon", u8"\u0395", "Greek Capital Letter Epsilon"},
        {CapitalZetaSymbol, "Zeta", u8"\u0396", "Greek Capital Letter Zeta"},
        {CapitalEtaSymbol, "Eta", u8"\u0397", "Greek Capital Letter Eta"},
        {CapitalThetaSymbol, "Theta", u8"\u0398", "Greek Capital Letter Theta"},
        {CapitalIotaSymbol, "Iota", u8"\u0399", "Greek Capital Letter Iota"},
        {CapitalKappaSymbol, "Kappa", u8"\u039A", "Greek Capital Letter Kappa"},
        {CapitalLambdaSymbol, "Lambda", u8"\u039B", "Greek Capital Letter Lambda"},
        {CapitalMuSymbol, "Mu", u8"\u039C", "Greek Capital Letter Mu"},
        {CapitalNuSymbol, "Nu", u8"\u039D", "Greek Capital Letter Nu"},
        {CapitalXiSymbol, "Xi", u8"\u039E", "Greek Capital Letter Xi"},
        {CapitalOmicronSymbol, "Omicron", u8"\u039F", "Greek Capital Letter Omicron"},
        {CapitalPiSymbol, "Pi", u8"\u03A0", "Greek Capital Letter Pi"},
        {CapitalRhoSymbol, "Rho", u8"\u03A1", "Greek Capital Letter Rho"},
        {CapitalSigmaSymbol, "Sigma", u8"\u03A3", "Greek Capital Letter Sigma"},
        {CapitalTauSymbol, "Tau", u8"\u03A4", "Greek Capital Letter Tau"},
        {CapitalUpsilonSymbol, "Upsilon", u8"\u03A5", "Greek Capital Letter Upsilon"},
        {CapitalPhiSymbol, "Phi", u8"\u03A6", "Greek Capital Letter Phi"},
        {CapitalChiSymbol, "Chi", u8"\u03A7", "Greek Capital Letter Chi"},
        {CapitalPsiSymbol, "Psi", u8"\u03A8", "Greek Capital Letter Psi"},
        {CapitalOmegaSymbol, "Omega", u8"\u03A9", "Greek Capital Letter Omega"}};

    return symbolData;
}

// Return enumeration for named symbol
Symbol symbol(std::string_view name)
{
    auto it =
        std::find_if(symbols().begin(), symbols().end(), [&name](auto &sym) { return name == qPrintable(std::get<1>(sym)); });

    if (it == symbols().end())
        return nSymbols;

    return std::get<0>(*it);
}

// Return symbol string
QString symbol(Symbol sym) { return std::get<2>(symbols()[sym]); }

// Return symbol description
QString description(Symbol sym) { return std::get<3>(symbols()[sym]); }

// Return first symbol whose description contains the search text
Symbol firstDescriptionMatch(QString text)
{
    auto it = std::find_if(symbols().begin(), symbols().end(), [&text](auto &sym) { return std::get<3>(sym).contains(text); });
    if (it == symbols().end())
        return nSymbols;

    return std::get<0>(*it);
}

}; // namespace SymbolData
