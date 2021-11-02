// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/neutronweights.h"
#include "classes/species.h"
#include "io/export/data1d.h"
#include "main/dissolve.h"
#include "math/filters.h"
#include "math/ft.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"

// Run set-up stage
bool NeutronSQModule::setUp(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Load and set up reference data (if a file/format was given)
     */
    if (referenceFQ_.hasFilename())
    {
        // Load the data
        Data1D referenceData;
        if (!referenceFQ_.importData(referenceData, &procPool))
        {
            Messenger::error("Failed to load reference data '{}'.\n", referenceFQ_.filename());
            return false;
        }

        // Get dependent modules
        if (!sourceSQ_)
            return Messenger::error("A source SQ module must be provided.\n");
        auto *rdfModule = sourceSQ_->sourceRDF();
        if (!rdfModule)
            return Messenger::error("A source RDF module (in the SQ module) must be provided.\n");

        // Remove normalisation_ factor from data
        if (referenceNormalisation_ != StructureFactors::NoNormalisation)
        {
            // We need the neutron weights in order to do the normalisation_
            NeutronWeights weights;
            calculateWeights(rdfModule, weights);

            // Remove normalisation_ of data
            if (referenceNormalisation_ == StructureFactors::AverageOfSquaresNormalisation)
            {
                referenceData *= weights.boundCoherentAverageOfSquares();
                Messenger::print("NeutronSQ: Removed <b>**2 normalisation_ from reference data ('{}'), factor = {}.\n",
                                 uniqueName(), weights.boundCoherentAverageOfSquares());
            }
            else if (referenceNormalisation_ == StructureFactors::SquareOfAverageNormalisation)
            {
                referenceData *= weights.boundCoherentSquareOfAverage();
                Messenger::print("NeutronSQ: Removed <b**2> normalisation_ from reference data ('{}'), factor = {}.\n",
                                 uniqueName(), weights.boundCoherentSquareOfAverage());
            }
        }

        // Get Q-range and window function to use for transformation of F(Q) to G(r)
        auto ftQMin = keywords_.hasBeenSet("ReferenceFTQMin") ? referenceFTQMin_ : 0.0;
        auto ftQMax = keywords_.hasBeenSet("ReferenceFTQMax") ? referenceFTQMax_ : referenceData.xAxis().back() + 1.0;
        if (referenceWindowFunction_ == WindowFunction::Form::None)
            Messenger::print("No window function will be applied in Fourier transform of reference data to g(r).");
        else
            Messenger::print("Window function to be applied in Fourier transform of reference data is {}.",
                             WindowFunction::forms().keyword(referenceWindowFunction_));

        // Store the reference data in processing
        referenceData.setTag(uniqueName());
        auto &storedData =
            dissolve.processingModuleData().realise<Data1D>("ReferenceData", uniqueName(), GenericItem::ProtectedFlag);
        storedData = referenceData;

        // Calculate and store the FT of the reference data in processing
        referenceData.setTag(uniqueName());
        auto &storedDataFT =
            dissolve.processingModuleData().realise<Data1D>("ReferenceDataFT", uniqueName(), GenericItem::ProtectedFlag);
        storedDataFT = referenceData;
        Filters::trim(storedDataFT, ftQMin, ftQMax);
        auto rho = rdfModule->effectiveDensity();
        Messenger::print("Effective atomic density used in Fourier transform of reference data is {} atoms/Angstrom3.\n", rho);
        Fourier::sineFT(storedDataFT, 1.0 / (2.0 * PI * PI * rho), referenceFTDeltaR_, referenceFTDeltaR_, 30.0,
                        WindowFunction(referenceWindowFunction_));

        // Save data?
        if (saveReference_)
        {
            if (procPool.isMaster())
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}-ReferenceData.q", uniqueName()));
                if (!exportFormat.exportData(storedData))
                    return procPool.decideFalse();
                Data1DExportFileFormat exportFormatFT(fmt::format("{}-ReferenceData.r", uniqueName()));
                if (!exportFormatFT.exportData(storedDataFT))
                    return procPool.decideFalse();
                procPool.decideTrue();
            }
            else if (!procPool.decision())
                return false;
        }
    }

    return true;
}

// Run main processing
bool NeutronSQModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Calculate neutron structure factors from existing S(Q) data
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     * Partial calculation routines called by this routine are parallel.
     */

    if (!sourceSQ_)
        return Messenger::error("A source SQ module must be provided.\n");
    const auto *rdfModule = sourceSQ_->sourceRDF();
    if (!rdfModule)
        return Messenger::error("A source RDF module (in the SQ module) must be provided.\n");

    // Print argument/parameter summary
    Messenger::print("NeutronSQ: Source unweighted S(Q) will be taken from module '{}'.\n", sourceSQ_->uniqueName());
    if (referenceWindowFunction_ == WindowFunction::Form::None)
        Messenger::print("No window function will be applied when calculating representative g(r) from S(Q).");
    else
        Messenger::print("Window function to be applied when calculating representative g(r) from S(Q) is {}.",
                         WindowFunction::forms().keyword(referenceWindowFunction_));
    if (normalisation_ == StructureFactors::NoNormalisation)
        Messenger::print("NeutronSQ: No normalisation will be applied to total F(Q).\n");
    else if (normalisation_ == StructureFactors::AverageOfSquaresNormalisation)
        Messenger::print("NeutronSQ: Total F(Q) will be normalised to <b>**2");
    else if (normalisation_ == StructureFactors::SquareOfAverageNormalisation)
        Messenger::print("NeutronSQ: Total F(Q) will be normalised to <b**2>");
    if (saveSQ_)
        Messenger::print("NeutronSQ: Weighted partial S(Q) and total F(Q) will be saved.\n");
    if (saveGR_)
        Messenger::print("NeutronSQ: Weighted partial g(r) and total G(r) will be saved.\n");
    if (saveRepresentativeGR_)
        Messenger::print("NeutronSQ: Representative G(r) will be saved.\n");
    Messenger::print("\n");

    /*
     * Transform UnweightedSQ from provided SQ data into WeightedSQ.
     */

    // Get unweighted S(Q) from the specified SQMOdule
    if (!dissolve.processingModuleData().contains("UnweightedSQ", sourceSQ_->uniqueName()))
        return Messenger::error("Couldn't locate unweighted S(Q) data from the SQModule '{}'.\n", sourceSQ_->uniqueName());
    const auto &unweightedSQ = dissolve.processingModuleData().value<PartialSet>("UnweightedSQ", sourceSQ_->uniqueName());

    // Calculate and store weights
    auto &weights =
        dissolve.processingModuleData().realise<NeutronWeights>("FullWeights", uniqueName_, GenericItem::InRestartFileFlag);
    calculateWeights(rdfModule, weights);

    // Create, print, and store weights
    Messenger::print("Isotopologue and isotope composition:\n\n");
    weights.createFromIsotopologues(exchangeable_);
    weights.print();

    // Does a PartialSet for the weighted S(Q) already exist for this Configuration?
    auto [weightedSQ, wSQstatus] =
        dissolve.processingModuleData().realiseIf<PartialSet>("WeightedSQ", uniqueName_, GenericItem::InRestartFileFlag);
    if (wSQstatus == GenericItem::ItemStatus::Created)
        weightedSQ.setUpPartials(unweightedSQ.atomTypeMix());

    // Calculate weighted S(Q)
    calculateWeightedSQ(unweightedSQ, weightedSQ, weights, normalisation_);

    // Save data if requested
    if (saveSQ_ && (!MPIRunMaster(procPool, weightedSQ.save(uniqueName_, "WeightedSQ", "sq", "Q, 1/Angstroms"))))
        return false;

    /*
     * Transform UnweightedGR from underlying RDF data into WeightedGR.
     */

    // Get summed unweighted g(r) from the RDFMOdule
    if (!dissolve.processingModuleData().contains("UnweightedGR", rdfModule->uniqueName()))
        return Messenger::error("Couldn't locate summed unweighted g(r) data.\n");
    const auto &unweightedGR = dissolve.processingModuleData().value<PartialSet>("UnweightedGR", rdfModule->uniqueName());

    // Create/retrieve PartialSet for summed weighted g(r)
    auto [weightedGR, wGRstatus] =
        dissolve.processingModuleData().realiseIf<PartialSet>("WeightedGR", uniqueName_, GenericItem::InRestartFileFlag);
    if (wGRstatus == GenericItem::ItemStatus::Created)
        weightedGR.setUpPartials(unweightedGR.atomTypeMix());

    // Calculate weighted g(r)
    calculateWeightedGR(unweightedGR, weightedGR, weights, normalisation_);

    // Save data if requested
    if (saveGR_ && (!MPIRunMaster(procPool, weightedGR.save(uniqueName_, "WeightedGR", "gr", "r, Angstroms"))))
        return false;

    // Calculate representative total g(r) from FT of calculated F(Q)
    auto &repGR =
        dissolve.processingModuleData().realise<Data1D>("RepresentativeTotalGR", uniqueName_, GenericItem::InRestartFileFlag);
    repGR = weightedSQ.total();
    auto rMin = weightedGR.total().xAxis().front();
    auto rMax = weightedGR.total().xAxis().back();
    auto rho = rdfModule->effectiveDensity();
    Fourier::sineFT(repGR, 1.0 / (2.0 * PI * PI * rho), rMin, 0.05, rMax, WindowFunction(referenceWindowFunction_));

    // Save data if requested
    if (saveRepresentativeGR_)
    {
        if (procPool.isMaster())
        {
            Data1DExportFileFormat exportFormat(fmt::format("{}-weighted-total.gr.broad", uniqueName_));
            if (exportFormat.exportData(repGR))
                procPool.decideTrue();
            else
                procPool.decideFalse();
        }
        else if (!procPool.decision())
            return false;
    }

    return true;
}
