// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/typeDefs.h"
#include "templates/flags.h"
#include <functional>
#include <optional>

class SelectionProcessor
{

    public:
    // Selection rule flags
    enum SelectionRuleFlags
    {
        IndexALTIndexB,        /* Index A less than Index B */
        IndexAGTIndexB,        /* Index A greater than Index B */
        IndexBLTIndexC,        /* Index B less than Index C */
        IndexBGTIndexC,        /* Index B greater than Index C */
        MoleculeAEQMoleculeB,  /* Molecule A equal to Molecule B */
        MoleculeBEQMoleculeC,  /* Molecule B equal to Molecule C */
        MoleculeANEQMoleculeB, /* Molecule A not equal to Molecule B */
        MoleculeBNEQMoleculeC, /* Molecule B not equal to Molecule C */
    };

    public:
    SelectionProcessor() = default;
    ~SelectionProcessor() = default;

    private:
    int nASelections_{0}, nAAvailable_{0}, nACumulative_{0};
    int nBSelections_{0}, nBAvailable_{0}, nBCumulative_{0};
    int nCSelections_{0}, nCAvailable_{0}, nCCumulative_{0};

    public:
    int nASelections() const;
    int nAAvailable() const;
    int nACumulative() const;
    int nBSelections() const;
    int nBAvailable() const;
    int nBCumulative() const;
    int nCSelections() const;
    int nCAvailable() const;
    int nCCumulative() const;

    public:
    bool
    process(const Analyser::SiteVector &sitesA, std::optional<Analyser::SiteVector &> sitesB = std::nullopt,
            std::optional<Analyser::SiteVector &> sitesC = std::nullopt, const Flags<SelectionRuleFlags> &flags = {},
            std::function<bool(const Site *, std::optional<const Site *>, std::optional<const Site *>)> processFunction = {});

    private:
    void reset();
};