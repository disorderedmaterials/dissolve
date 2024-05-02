// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/symbol.h"
#include "base/sysFunc.h"
#include <algorithm>

namespace SymbolData
{

// Return symbol vector
const std::vector<std::tuple<Symbol, std::string, std::string, std::string>> &symbols()
{
    static std::vector<std::tuple<Symbol, std::string, std::string, std::string>> symbolData = {
        {MultiplySymbol, "mult", "\u00D7", "Multiplication Sign"},
        {DivisionSymbol, "div", "\u00F7", "Division Sign"},
        {AngstromSymbol, "angstrom", "\u212B", "Angstrom"},
        {DegreeSymbol, "degree", "\u00B0", "Degree"},
        {AlphaSymbol, "alpha", "\u03B1", "Greek Small Letter Alpha"},
        {BetaSymbol, "beta", "\u03B2", "Greek Small Letter Beta"},
        {GammaSymbol, "gamma", "\u03B3", "Greek Small Letter Gamma"},
        {DeltaSymbol, "delta", "\u03B4", "Greek Small Letter Delta"},
        {EpsilonSymbol, "epsilon", "\u03B5", "Greek Small Letter Epsilon"},
        {ZetaSymbol, "zeta", "\u03B6", "Greek Small Letter Zeta"},
        {EtaSymbol, "eta", "\u03B7", "Greek Small Letter Eta"},
        {ThetaSymbol, "theta", "\u03B8", "Greek Small Letter Theta"},
        {IotaSymbol, "iota", "\u03B9", "Greek Small Letter Iota"},
        {KappaSymbol, "kappa", "\u03BA", "Greek Small Letter Kappa"},
        {LambdaSymbol, "lambda", "\u03BB", "Greek Small Letter Lambda"},
        {MuSymbol, "mu", "\u03BC", "Greek Small Letter Mu"},
        {NuSymbol, "nu", "\u03BD", "Greek Small Letter Nu"},
        {XiSymbol, "xi", "\u03BE", "Greek Small Letter Xi"},
        {OmicronSymbol, "omicron", "\u03BF", "Greek Small Letter Omicron"},
        {PiSymbol, "pi", "\u03C0", "Greek Small Letter Pi"},
        {RhoSymbol, "rho", "\u03C1", "Greek Small Letter Rho"},
        {SigmaSymbol, "sigma", "\u03C3", "Greek Small Letter Sigma"},
        {TauSymbol, "tau", "\u03C4", "Greek Small Letter Tau"},
        {UpsilonSymbol, "upsilon", "\u03C5", "Greek Small Letter Upsilon"},
        {PhiSymbol, "phi", "\u03C6", "Greek Small Letter Phi"},
        {ChiSymbol, "chi", "\u03C7", "Greek Small Letter Chi"},
        {PsiSymbol, "psi", "\u03C8", "Greek Small Letter Psi"},
        {OmegaSymbol, "omega", "\u03C9", "Greek Small Letter Omega"},
        {CapitalAlphaSymbol, "Alpha", "\u0391", "Greek Capital Letter Alpha"},
        {CapitalBetaSymbol, "Beta", "\u0392", "Greek Capital Letter Beta"},
        {CapitalGammaSymbol, "Gamma", "\u0393", "Greek Capital Letter Gamma"},
        {CapitalDeltaSymbol, "Delta", "\u0394", "Greek Capital Letter Delta"},
        {CapitalEpsilonSymbol, "Epsilon", "\u0395", "Greek Capital Letter Epsilon"},
        {CapitalZetaSymbol, "Zeta", "\u0396", "Greek Capital Letter Zeta"},
        {CapitalEtaSymbol, "Eta", "\u0397", "Greek Capital Letter Eta"},
        {CapitalThetaSymbol, "Theta", "\u0398", "Greek Capital Letter Theta"},
        {CapitalIotaSymbol, "Iota", "\u0399", "Greek Capital Letter Iota"},
        {CapitalKappaSymbol, "Kappa", "\u039A", "Greek Capital Letter Kappa"},
        {CapitalLambdaSymbol, "Lambda", "\u039B", "Greek Capital Letter Lambda"},
        {CapitalMuSymbol, "Mu", "\u039C", "Greek Capital Letter Mu"},
        {CapitalNuSymbol, "Nu", "\u039D", "Greek Capital Letter Nu"},
        {CapitalXiSymbol, "Xi", "\u039E", "Greek Capital Letter Xi"},
        {CapitalOmicronSymbol, "Omicron", "\u039F", "Greek Capital Letter Omicron"},
        {CapitalPiSymbol, "Pi", "\u03A0", "Greek Capital Letter Pi"},
        {CapitalRhoSymbol, "Rho", "\u03A1", "Greek Capital Letter Rho"},
        {CapitalSigmaSymbol, "Sigma", "\u03A3", "Greek Capital Letter Sigma"},
        {CapitalTauSymbol, "Tau", "\u03A4", "Greek Capital Letter Tau"},
        {CapitalUpsilonSymbol, "Upsilon", "\u03A5", "Greek Capital Letter Upsilon"},
        {CapitalPhiSymbol, "Phi", "\u03A6", "Greek Capital Letter Phi"},
        {CapitalChiSymbol, "Chi", "\u03A7", "Greek Capital Letter Chi"},
        {CapitalPsiSymbol, "Psi", "\u03A8", "Greek Capital Letter Psi"},
        {CapitalOmegaSymbol, "Omega", "\u03A9", "Greek Capital Letter Omega"}};

    return symbolData;
}

// Return enumeration for named symbol
Symbol symbol(std::string_view name)
{
    auto it = std::find_if(symbols().begin(), symbols().end(),
                           [&name](auto &sym) { return DissolveSys::sameString(std::get<1>(sym), name, true); });

    if (it == symbols().end())
        return nSymbols;

    return std::get<0>(*it);
}

// Return symbol string
std::string_view symbol(Symbol sym) { return std::get<2>(symbols()[sym]); }

// Return symbol description
std::string_view description(Symbol sym) { return std::get<3>(symbols()[sym]); }

// Return first symbol whose description contains the search text
Symbol firstDescriptionMatch(std::string_view text)
{
    auto it = std::find_if(symbols().begin(), symbols().end(),
                           [&text](auto &sym) { return std::get<3>(sym).find(text) != std::string::npos; });
    if (it == symbols().end())
        return nSymbols;

    return std::get<0>(*it);
}

}; // namespace SymbolData
