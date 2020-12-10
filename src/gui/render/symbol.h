// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include <QString>

// Forward Declarations
/* none */

// Symbols
class SymbolData
{
    public:
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
    // Unicode Character
    QChar character;
    // Short name of symbol
    QString name;
    // Symbol description
    QString description;

    /*
     * Singleton
     */
    public:
    // Static list of symbols
    static SymbolData symbols[];
    // Return named symbol
    static SymbolData *symbol(std::string_view name);
};
