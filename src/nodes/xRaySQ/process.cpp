// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#define _USE_MATH_DEFINES
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/xRayWeights.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "math/filters.h"
#include "math/ft.h"
#include "nodes/xRaySQ/xRaySQ.h"
#include "templates/algorithms.h"

// Run main processing
NodeConstants::ProcessResult XRaySQNode::process()
{
    /*
     * Calculate x-ray structure factors from existing g(r) data
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     * Partial calculation routines called by this routine are parallel.
     */

    // Print argument/parameter summary
    Messenger::print("XRaySQ: Form factors to use are '{}'.\n", XRayFormFactors::xRayFormFactorData().keyword(formFactors_));
    if (normaliseTo_ == StructureFactors::NoNormalisation)
        Messenger::print("XRaySQ: No normalisation will be applied to total F(Q).\n");
    else if (normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation)
        Messenger::print("XRaySQ: Total F(Q) will be normalised to <b**2>");
    else if (normaliseTo_ == StructureFactors::SquareOfAverageNormalisation)
        Messenger::print("XRaySQ: Total F(Q) will be normalised to <b>**2");
    if (referenceWindowFunction_ == WindowFunction::Form::None)
        Messenger::print("XRaySQ: No window function will be applied when calculating representative g(r) from S(Q).");
    else
        Messenger::print("XRaySQ: Window function to be applied when calculating representative g(r) from S(Q) is {}.",
                         WindowFunction::forms().keyword(referenceWindowFunction_));
    if (saveFormFactors_)
        Messenger::print("XRaySQ: Combined form factor weightings for atomtype pairs will be saved.\n");
    if (saveSQ_)
        Messenger::print("XRaySQ: Weighted partial S(Q) and total F(Q) will be saved.\n");
    if (saveGR_)
        Messenger::print("XRaySQ: Weighted partial g(r) and total G(r) will be saved.\n");
    if (saveRepresentativeGR_)
        Messenger::print("XRaySQ: Representative G(r) will be saved.\n");
    Messenger::print("\n");

    /*
     * Transform UnweightedSQ from provided SQ data into WeightedSQ.
     */

    // Set up the weighted SQ storage if needed
    if (!weightedSQ_)
    {
        weightedSQ_.emplace();
        weightedSQ_.value().initialise(*unweightedSQ_);
    }

    // Set up weighted GR storage if we need it
    if (!weightedGR_)
    {
        weightedGR_.emplace();
        weightedGR_.value().initialise(*unweightedGR_);
    }

    // Construct weights matrix
    if (!weights_.setUp(unweightedSQ_->realSpeciesPopulations(), formFactors_))
        return error("Failed to setup xRay weights.");

    // Set up reference FQ and GR
    if (referenceFQ_ and !setReferenceData())
        return NodeConstants::ProcessResult::Failed;

    Messenger::print("Weights matrix:\n\n");
    weights_.print();

    // Calculate weighted S(Q)
    calculateWeightedSQ(*unweightedSQ_, *weightedSQ_, weights_, normaliseTo_);

    if (saveFormFactors_)
    {
        // TODO This will be cleaned up once XRayWeights moves to DoubleKeyedMap.
        KeyedVector<const AtomType *, int> typeVector;
        for (auto &[resolvableSpecies, _] : unweightedSQ_->realSpeciesPopulations())
            for (auto &[atomType, _] : resolvableSpecies.raw()->atomTypePopulations())
                typeVector[atomType] = 1;

        auto result = for_each_pair_early(typeVector,
                                          [&](int indexI, auto &popI, int indexJ, auto &popJ) -> EarlyReturn<bool>
                                          {
                                              DoubleKeyedMapKey key{popI.first->name(), popJ.first->name()};

                                              if (indexI == indexJ)
                                              {
                                                  Data1D atomicData = unweightedSQ_->partials().get(key);
                                                  atomicData.values() = weights_.formFactor(popI.first, atomicData.xAxis());
                                                  Data1DExportFileFormat exportFormat(
                                                      std::format("{}-{}.form", name(), popI.first->name()));
                                                  if (!exportFormat.exportData(atomicData))
                                                      return false;
                                              }

                                              Data1D ffData = unweightedSQ_->partials().get(key);
                                              ffData.values() = weights_.weight(popI.first, popJ.first, ffData.xAxis());
                                              Data1DExportFileFormat exportFormat(
                                                  std::format("{}-{}-{}.form", name(), popI.first->name(), popJ.first->name()));
                                              if (!exportFormat.exportData(ffData))
                                                  return false;

                                              return EarlyReturn<bool>::Continue;
                                          });

        if (!result.value_or(true))
        {
            Messenger::error("Failed to save form factor data.");
            return NodeConstants::ProcessResult::Failed;
        }
    }

    /*
     * Transform UnweightedGR from underlying RDF data into WeightedGR.
     */

    // Calculate weighted g(r)
    calculateWeightedGR(*unweightedGR_, *weightedGR_, weights_, normaliseTo_);

    // Calculate representative total g(r) from FT of calculated F(Q)
    representativeGR_ = weightedSQ_->total();
    auto ftQMax = 0.0;
    if (referenceFTQMax_)
        ftQMax = referenceFTQMax_.value();
    else if (referenceFQ_)
    {
        // Take FT max Q limit from reference data
        ftQMax = referenceFQ_->xAxis().back();
    }
    else
        ftQMax = weightedSQ_->total().xAxis().back();

    Filters::trim(representativeGR_, referenceFTQMin_.value_or(0.0), ftQMax);
    auto rMin = weightedGR_->total().xAxis().front();
    auto rMax = weightedGR_->total().xAxis().back();
    auto rho = unweightedGR_->effectiveDensity();

    Fourier::sineFT(representativeGR_, 1.0 / (2.0 * M_PI * M_PI * rho), rMin, 0.05, rMax,
                    WindowFunction(referenceWindowFunction_));

    // Save data if requested
    if (saveRepresentativeGR_)
    {
        Data1DExportFileFormat exportFormat(std::format("{}-weighted-total.gr.broad", name()));
        if (!exportFormat.exportData(representativeGR_))
            return NodeConstants::ProcessResult::Failed;
    }

    return NodeConstants::ProcessResult::Success;
}