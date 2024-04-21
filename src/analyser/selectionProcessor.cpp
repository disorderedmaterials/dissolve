// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/selectionProcessor.h"
#include "classes/site.h"
#include "templates/algorithms.h"
#include "templates/combinable.h"
#include <tuple>
#include <vector>

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

bool SelectionProcessor::processMultithreaded(
    const Analyser::SiteVector &sitesA, std::optional<Analyser::SiteVector> sitesB, std::optional<Analyser::SiteVector> sitesC,
    const Flags<SelectionRuleFlags> &flags,
    std::function<bool(const Site *, std::optional<const Site *>, std::optional<const Site *>)> processFunction)
{
    // First, create a vector containing the site tuples for processing
    std::vector<std::tuple<const Site *, std::optional<const Site *>, std::optional<const Site *>>> siteTuples;
    Messenger::print("Selecting site tuples for processing based on provided rules...\n");
    for (const auto &[siteA, indexA] : sitesA)
    {
        // if (sitesB.has_value())
        // {
        for (const auto &[siteB, indexB] : sitesB.value())
        {

            // if (flags.isSet(IndexALTIndexB) && indexA >= indexB)
            //     continue;
            // if (flags.isSet(IndexAGTIndexB) && indexA <= indexB)
            //     continue;
            // if (flags.isSet(MoleculeAEQMoleculeB) && siteA->molecule() != siteB->molecule())
            // continue;
            if (flags.isSet(MoleculeANEQMoleculeB) && siteA->molecule() == siteB->molecule())
                continue;

            // if (sitesC.has_value())
            // {
            //     for (const auto &[siteC, indexC] : sitesC.value())
            //     {

            //         if (flags.isSet(IndexBLTIndexC) && indexB >= indexC)
            //             continue;
            //         if (flags.isSet(IndexBGTIndexC) && indexB <= indexC)
            //             continue;
            //         if (flags.isSet(MoleculeBEQMoleculeC) && siteB->molecule() != siteC->molecule())
            //             continue;
            //         if (flags.isSet(MoleculeBNEQMoleculeC) && siteB->molecule() == siteC->molecule())
            //             continue;

            //         siteTuples.push_back(std::make_tuple(siteA, siteB, siteC));
            //     }
            // }
            // else
            // {
            siteTuples.push_back(std::make_tuple(siteA, siteB, std::nullopt));
            // }
        }
        // }
        // else
        // {
        //     siteTuples.push_back(std::make_tuple(siteA, std::nullopt, std::nullopt));
        // }
    }
    Messenger::print("Selected {} site tuples for processing.\n", siteTuples.size());

    // Now we need to create a combinable

    using SiteTuple = std::tuple<const Site *, std::optional<const Site *>, std::optional<const Site *>>;

    // dissolve::CombinableFunctor<std::vector<SiteTuple>> combinableSiteTuples(
    //     [&]() -> std::vector<SiteTuple>
    //     {
    //         auto vec = std::vector<SiteTuple>();
    //         vec.reserve(siteTuples.size());
    //         return vec;
    //     });

    // TODO:
    // - Implement combinable for site tuples? is this why the below doesn't work??

    // dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(0),
    //                    dissolve::counting_iterator<int>(siteTuples.size()),
    //                    [&](int index)
    //                    {
    //                        auto *siteA = std::get<0>(siteTuples[index]);
    //                        std::optional<const Site *> siteB = std::get<1>(siteTuples[index]);
    //                        std::optional<const Site *> siteC = std::nullopt;
    //                        //    if (std::get<1>(siteTuples[index]).has_value())
    //                        //    {
    //                        //        siteB = std::get<1>(siteTuples[index]);
    //                        //    }
    //                        //    if (std::get<2>(siteTuples[index]).has_value())
    //                        //    {
    //                        //        siteC = std::get<2>(siteTuples[index]);
    //                        //    }
    //                        processFunction(siteA, siteB, std::nullopt);
    //                    });

    for (auto &siteTuple : siteTuples)
    {
        auto *siteA = std::get<0>(siteTuple);
        std::optional<const Site *> siteB = std::get<1>(siteTuple);
        std::optional<const Site *> siteC = std::nullopt;
        processFunction(siteA, siteB, siteC);
    }

    // auto createCombinable =
    //     [&]() -> std::vector<std::tuple<const Site *, std::optional<const Site *>, std::optional<const Site *>>>
    // {
    //     auto vec = std::vector<std::tuple<const Site *, std::optional<const Site *>, std::optional<const Site *>>>();
    //     vec.reserve(siteTuples.size());
    //     return vec;
    // };

    // dissolve::CombinableContainer<
    //     std::vector<std::tuple<const Site *, std::optional<const Site *>, std::optional<const Site *>>>>
    //     combinableSiteTuples(siteTuples, createCombinable);

    // dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(0),
    //                    dissolve::counting_iterator<int>(siteTuples.size()),
    //                    [&](int index)
    //                    {
    //                        auto &siteTuplesLocal = combinableSiteTuples.local();
    //                        auto *siteA = std::get<0>(siteTuples[index]);
    //                        std::optional<const Site *> siteB = std::nullopt;
    //                        std::optional<const Site *> siteC = std::nullopt;
    //                        if (std::get<1>(siteTuplesLocal[index]).has_value())
    //                        {
    //                            siteB = std::get<1>(siteTuplesLocal[index]);
    //                        }
    //                        if (std::get<2>(siteTuplesLocal[index]).has_value())
    //                        {
    //                            siteC = std::get<2>(siteTuplesLocal[index]);
    //                        }
    //                        processFunction(siteA, siteB, siteC);
    //                    });

    // combinableSiteTuples.finalize();

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