// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/selectionProcessor.h"
#include "classes/site.h"

int SelectionProcessor::nASelections() const { return nASelections_; }
int SelectionProcessor::nAAvailable() const { return nAAvailable_; }
int SelectionProcessor::nACumulative() const { return nACumulative_; }
int SelectionProcessor::nBSelections() const { return nBSelections_; }
int SelectionProcessor::nBAvailable() const { return nBAvailable_; }
int SelectionProcessor::nBCumulative() const { return nBCumulative_; }
int SelectionProcessor::nCSelections() const { return nCSelections_; }
int SelectionProcessor::nCAvailable() const { return nCAvailable_; }
int SelectionProcessor::nCCumulative() const { return nCCumulative_; }

// First focus on getting the actual processing working, then we worry about multithreading afterwards.
// TODO: verify selection statistics
// TODO: implement multithreading
// TODO: handle generic rule
bool SelectionProcessor::process(
    const Analyser::SiteVector &sitesA, std::optional<Analyser::SiteVector> sitesB, std::optional<Analyser::SiteVector> sitesC,
    const Flags<SelectionRuleFlags> &flags,
    std::function<bool(const Site *, std::optional<const Site *>, std::optional<const Site *>)> processFunction)
{
    reset();
    nASelections_ = 1;

    // Loop over A sites
    for (const auto &[siteA, indexA] : sitesA)
    {
        ++nAAvailable_;
        ++nACumulative_;

        // Check if B sites are available
        if (sitesB.has_value())
        {
            ++nBSelections_;

            for (const auto &[siteB, indexB] : sitesB.value())
            {

                if (flags.isSet(IndexALTIndexB) && indexA >= indexB)
                    continue;
                if (flags.isSet(IndexAGTIndexB) && indexA <= indexB)
                    continue;
                if (flags.isSet(MoleculeAEQMoleculeB) && siteA->molecule() != siteB->molecule())
                    continue;
                if (flags.isSet(MoleculeANEQMoleculeB) && siteA->molecule() == siteB->molecule())
                    continue;

                ++nBAvailable_;
                ++nBCumulative_;

                // Check if C sites are available
                if (sitesC.has_value())
                {
                    ++nCSelections_;
                    for (const auto &[siteC, indexC] : sitesC.value())
                    {
                        if (flags.isSet(IndexBLTIndexC) && indexB >= indexC)
                            continue;
                        if (flags.isSet(IndexBGTIndexC) && indexB <= indexC)
                            continue;
                        if (flags.isSet(MoleculeBEQMoleculeC) && siteB->molecule() != siteC->molecule())
                            continue;
                        if (flags.isSet(MoleculeBNEQMoleculeC) && siteB->molecule() == siteC->molecule())
                            continue;

                        ++nCAvailable_;
                        ++nCCumulative_;

                        if (!processFunction(siteA, siteB, siteC))
                            return false;
                    }
                }
                else if (!processFunction(siteA, siteB, std::nullopt))
                    return false;
            }
        }
        else if (!processFunction(siteA, std::nullopt, std::nullopt))
            return false;
    }
    return true;
}

void SelectionProcessor::reset()
{
    nASelections_ = 0;
    nAAvailable_ = 0;
    nACumulative_ = 0;
    nBSelections_ = 0;
    nBAvailable_ = 0;
    nBCumulative_ = 0;
    nCSelections_ = 0;
    nCAvailable_ = 0;
    nCCumulative_ = 0;
}