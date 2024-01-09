// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <string>
#include <tuple>
#include <vector>

// Symbols
namespace SymbolData
{
// Symbol
enum Symbol
{
    MultiplySymbol,
    DivisionSymbol,
    AngstromSymbol,
    DegreeSymbol,
    AlphaSymbol,
    BetaSymbol,
    GammaSymbol,
    DeltaSymbol,
    EpsilonSymbol,
    ZetaSymbol,
    EtaSymbol,
    ThetaSymbol,
    IotaSymbol,
    KappaSymbol,
    LambdaSymbol,
    MuSymbol,
    NuSymbol,
    XiSymbol,
    OmicronSymbol,
    PiSymbol,
    RhoSymbol,
    SigmaSymbol,
    TauSymbol,
    UpsilonSymbol,
    PhiSymbol,
    ChiSymbol,
    PsiSymbol,
    OmegaSymbol,
    CapitalAlphaSymbol,
    CapitalBetaSymbol,
    CapitalGammaSymbol,
    CapitalDeltaSymbol,
    CapitalEpsilonSymbol,
    CapitalZetaSymbol,
    CapitalEtaSymbol,
    CapitalThetaSymbol,
    CapitalIotaSymbol,
    CapitalKappaSymbol,
    CapitalLambdaSymbol,
    CapitalMuSymbol,
    CapitalNuSymbol,
    CapitalXiSymbol,
    CapitalOmicronSymbol,
    CapitalPiSymbol,
    CapitalRhoSymbol,
    CapitalSigmaSymbol,
    CapitalTauSymbol,
    CapitalUpsilonSymbol,
    CapitalPhiSymbol,
    CapitalChiSymbol,
    CapitalPsiSymbol,
    CapitalOmegaSymbol,
    nSymbols
};

// Return symbol vector
const std::vector<std::tuple<Symbol, std::string, std::string, std::string>> &symbols();
// Return enumeration for named symbol
Symbol symbol(std::string_view name);
// Return symbol string
std::string_view symbol(Symbol sym);
// Return symbol description
std::string_view description(Symbol sym);
// Return first symbol whose description contains the search text
Symbol firstDescriptionMatch(std::string_view text);
}; // namespace SymbolData
