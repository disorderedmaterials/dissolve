// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/epsr.h"
#include "base/sysFunc.h"
#include "classes/partialSet.h"
#include "classes/scatteringMatrix.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "math/filters.h"
#include "math/ft.h"
#include "math/gaussFit.h"
#include "math/mathFunc.h"
#include "math/poissonFit.h"
#include "nodes/neutronSQ.h"
#include "nodes/sq.h"
#include "nodes/xRaySQ.h"
#include "templates/algorithms.h"
#include <functional>

EPSRNode::EPSRNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the node", targetConfiguration_);

    // Options
    addOption("Targets", "Add specified Node (and it's Reference data) as a refinement target", targetNames_);
    addOption("TargetWeights", "Set relative weighting for specific node targets as they enter into the scattering matrix",
              namedWeights_);
    addOption("EReq", "Limit of magnitude of additional potential for any one pair potential", eReq_);
    addOption("Feedback", "Confidence factor", feedback_);
    addOption("ModifyPotential", "Frequency at which to apply generated perturbations to interatomic potentials",
              modifyPotential_);
    addOption("QMin", "Minimum Q value over which to generate potentials from total scattering data", qMin_);
    addOption("QMax", "Maximum Q value over which to generate potentials from total scattering data", qMax_);
    addOption("Weighting", "Factor used when adding fluctuation coefficients to pair potentials", weighting_);
    addOption("RFactorRanges", "Ranges over which to calculate RFactors", ranges_);
    addOption("ExpansionFunction", "Form of expansion function to use when fitting difference data", expansionFunction_);
    addOption("GSigma1", "Width for Gaussian function in reciprocal space", gSigma1_);
    addOption("GSigma2", "Width for Gaussian function in real space", gSigma2_);
    addOption("NCoeffP", "Number of coefficients used to define the empirical potential", nCoeffP_);
    addOption("NPItSs", "Number of iterations when refining fits to delta functions", nPItSs_);
    addOption("PSigma1", "Width for Poisson functions in reciprocal space (N.B. this is psigma2 in EPSR)", pSigma1_);
    addOption("PSigma2", "Width for Poisson functions in real space", pSigma2_);
    addOption("RMaxPT", "Radius at which potential truncation goes to zero", rMaxPT_);
    addOption("RMinPT", "Radius at which potential truncation begins (or Auto for 2 Angstroms under rmaxpt)", rMinPT_);
    addOption("Smoothing", "Smoothing to apply to fluctuation coefficients before summation into potential",
              fluctuationSmoothing_);
    addOption("OverwritePotentials", "Overwrite potentials each time rather than summing them", overwritePotentials_);

    // Serialisables
    // addSerialisable("potentialCoefficients", potentialCoefficients_);
    addSerialisable("estimatedSQ", estimatedSQ_);
    addSerialisable("estimatedGR", estimatedGR_);
    addSerialisable("additionalPotentials", additionalPotentials_);
    addSerialisable("phiArray", phiArray_);
}

// Return type of the node
std::string_view EPSRNode::type() const { return "EPSR"; }

// Return short summary of the node's purpose
std::string_view EPSRNode::summary() const { return "Refine interatomic potentials according to the methodology of Soper"; }

// Perform processing
NodeConstants::ProcessResult EPSRNode::process()
{
    std::string testDataName;

    // EPSR constants
    const auto mcoeff = 200;

    /*
     * Do we have targets to refine against?
     */

    // Find all correlation data for refinement
    auto correlationData = findTargetCorrelationData();
    auto sqNodes = correlationData.extract(SQ);
    auto &[sqNode, _] = sqNodes.mapped().front();

    if (!sqNode)
    {
        error("EPSR could not find an SQ node corresponding to the input configuration {}", targetConfiguration_->name());
        return NodeConstants::ProcessResult::Failed;
    }

    const auto &unweightedSQ = static_cast<SQNode *>(sqNode)->unweightedSQ();

    // Create target weights
    TargetWeights targets;
    targets.insert(targets.end(), correlationData[NeutronSQ].begin(), correlationData[NeutronSQ].end());
    targets.insert(targets.end(), correlationData[XRaySQ].begin(), correlationData[XRaySQ].end());

    // Default to applying generated potentials - an associated EPSRManager may turn this off in its own setup stage
    applyPotentials_ = true;
    auto rho = targetConfiguration_->atomicDensity().value_or(0.1);

    // Try to calculate the deltaSQ array
    updateDeltaSQ();

    // Clear any existing scattering matrix
    scatteringMatrix_ = std::nullopt;

    // Calculate some values if they were not provided directly
    auto rmaxpt = rMaxPT_ ? rMaxPT_->asDouble() : PairPotential::range();
    auto rminpt = rMinPT_ ? rMinPT_->asDouble() : rmaxpt - 2.0;
    auto ncoeffp = nCoeffP_ ? nCoeffP_->asDouble() : std::min(int(10.0 * rmaxpt + 0.0001), mcoeff);
    auto eReq = eReq_.asDouble();
    auto feedback = feedback_.asDouble();
    auto qMax = qMax_.asDouble();
    auto qMin = qMin_.asDouble();
    auto weighting = weighting_.asDouble();
    auto gSigma1 = gSigma1_.asDouble();
    auto gSigma2 = gSigma2_.asDouble();
    auto pSigma1 = pSigma1_.asDouble();
    auto pSigma2 = pSigma2_.asDouble();
    auto fluctuationSmoothing = std::optional<int>(fluctuationSmoothing_.value_or(0).asInteger());
    auto modifyPotential = std::optional<int>(modifyPotential_.value_or(0).asInteger());

    // TODO DISSOLVE2
    auto iteration = 0;

    message("Feedback factor is {}.\n", feedback);
    for (auto &[targetNode, weight] : targets)
        message("Weight for node data '{}' set to {}.\n", targetNode->name(), weight);

    if (targets.empty())
    {
        error("At least one Node target containing suitable data must be provided.\n");
        return NodeConstants::ProcessResult::Failed;
    }

    if (!targetConfiguration_)
    {
        error("No target configuration is set.\n");
        return NodeConstants::ProcessResult::Failed;
    }

    if (!targetConfiguration_->atomicDensity())
    {
        error("No density available for target configuration '{}'\n", targetConfiguration_->name());
        return NodeConstants::ProcessResult::Failed;
    }

    // Print option summary
    message("{} functions will be used to approximate difference data.\n",
            expansionFunctionTypes().keyword(expansionFunction_));

    message("Number of functions used in approximation is {}, sigma(Q) = {}.\n", ncoeffp, pSigma2);
    if (modifyPotential)
    {
        message("Perturbations to interatomic potentials will be generated with a frequency of {}.\n", *modifyPotential);
        if (applyPotentials_)
            message("Global pair potentials will be modified.\n");
        else
            message("Current potentials will not be modified.\n");
    }
    else
        warn("Fluctuation coefficients and resulting potentials will not be modified.\n");

    message("Range for potential generation is {} < Q < {} Angstroms**-1.\n", qMin, qMax);
    message("Weighting factor used when applying fluctuation coefficients is {}\n", weighting);
    if (fluctuationSmoothing)
        message("Coefficients will be smoothed (average length = 2N+1, N = {})", *fluctuationSmoothing);
    message("\n");

    /*
     * Increase run counter
     */
    if (!runCount_)
        runCount_.emplace(0);

    ++(*runCount_);

    /*
     * EPSR Main
     */

    // Set up storage for the changes to coefficients used to generate the empirical potentials
    const auto atomTypes = targetConfiguration_->atomTypeVector();
    const auto nAtomTypes = atomTypes.size();

    Array3D<double> fluctuationCoefficients(nAtomTypes, nAtomTypes, ncoeffp);
    fluctuationCoefficients = 0.0;

    // Create storage for our summed UnweightedSQ
    DoubleKeyedMap<Data1D> calculatedUnweightedSQ;
    dissolve::for_each_pair(
        ParallelPolicies::par, atomTypes, [&](int indexI, auto atI, int indexJ, auto atJ)
        { calculatedUnweightedSQ[{atI->name(), atJ->name()}].setTag(std::format("{}-{}", atI->name(), atJ->name())); });

    // Get summed weights over all datasets
    const auto totalDataSetWeight =
        std::accumulate(targets.begin(), targets.end(), 0.0,
                        [](const auto acc, const auto &targetWeight) { return acc + targetWeight.second; }) +
        (targetNames_.size() - namedWeights_.value().size());

    // Initialise a scattering matrix if we haven't already
    if (!scatteringMatrix_)
        scatteringMatrix_.emplace(atomTypes);

    // Loop over target data
    auto rFacTot = 0.0;
    std::vector<double> rangedRFacTots(ranges_.size());
    for (auto &targetNodeType : std::views::keys(correlationData))
        for (auto &[targetNode, _] : correlationData[targetNodeType])
        {
            /*
             * Retrieve data for this node
             */
            targetProcessData_.try_emplace(targetNode);

            const auto &weightedSQ = *targetNode->getOutputValue<PartialSet *>("WeightedSQ");

            // Retrieve the ReferenceData
            if (!targetNode->getInputValue<std::optional<Data1D>>("ReferenceData").has_value())
            {
                error("Reference data not found for target '{}'.\n", targetNode->name());
                return NodeConstants::ProcessResult::Failed;
            }
            const auto originalReferenceData = targetNode->getInputValue<std::optional<Data1D>>("ReferenceData").value();

            // Realise the r-factor array and make sure its object name is set
            auto &errors = targetProcessData_[targetNode].errors;

            /*
             * Calculate difference functions and current percentage errors in calculated vs reference target data.
             * Do this over the widest Q-range allowed by both datasets
             */
            auto &differenceData = targetProcessData_[targetNode].differenceData;

            differenceData = weightedSQ.total();
            differenceData *= -1.0;
            Interpolator::addInterpolated(originalReferenceData, differenceData);

            // Zero the difference function for points outside of the reference data range
            auto originalXMin = originalReferenceData.xAxis().front();
            auto originalXMax = originalReferenceData.xAxis().back();
            for (auto &&[x, y] : zip(differenceData.xAxis(), differenceData.values()))
                if (x < originalXMin || x > originalXMax)
                    y = 0.0;

            // Calculate r-factor over fit range and store
            auto trimmedReferenceData = originalReferenceData;
            Filters::trim(trimmedReferenceData, qMin, qMax);
            const auto rFactorReport = Error::rFactor(trimmedReferenceData, weightedSQ.total());
            rFacTot += rFactorReport.error;
            // TODO: Iteration may be hidden in some graph set ups (i.e. EPSR inside a standard sub-graph inside an iterable
            // graph, in which case parent graph is NOT iterable. Consider how we could fix this, perhaps we need all graphs to
            // accept an optional iterator parameter, in case of nesting.
            errors.addPoint(iteration, rFactorReport.error);
            message("Current R-Factor for reference data '{}' is {:.5f}.\n", targetNode->name(), rFactorReport.error);

            // Calculate r-factor over specified ranges_
            for (auto &&[range, rangeTot] : zip(ranges_, rangedRFacTots))
            {
                if (range.minimum() < rFactorReport.firstX || range.maximum() > rFactorReport.lastX)
                {
                    warn("The specified range '{:.5f} to {:.5f}' is outside of the range of data '{}', which exists "
                         "between {:.5f} and {:.5f}",
                         range.minimum(), range.maximum(), targetNode->name(), rFactorReport.firstX, rFactorReport.lastX);
                }
                const auto rangedRFactorError = Error::rFactor(trimmedReferenceData, weightedSQ.total(), range).error;
                rangeTot += rangedRFactorError;
                message("Current R-Factor for reference data '{}' over range {:.5f} to {:.5f} is {:.5f}.\n", targetNode->name(),
                        range.minimum(), range.maximum(), rangedRFactorError);
            }

            /*
             * Generate difference function for fitting, spanning (maximally) only the range requested
             */
            auto &deltaFQ = targetProcessData_[targetNode].deltaFQ;
            auto &deltaFQFit = targetProcessData_[targetNode].deltaFQFit;

            // Copy the original difference data and "invert" it
            deltaFQ = differenceData;
            deltaFQ *= -1.0;
            for (auto &&[x, y] : zip(deltaFQ.xAxis(), deltaFQ.values()))
                if (x < qMin || x > qMax)
                    y = 0.0;

            // Fit a function expansion to the deltaFQ - if the coefficient arrays already exist then re-fit starting from
            // those.
            auto &fitCoefficients = targetProcessData_[targetNode].fitCoefficients;
            auto fitError = 0.0;
            auto nIterations = nPItSs_.value_or(0).asInteger();
            if (expansionFunction_ == EPSRNode::GaussianExpansionFunction)
            {
                // Construct our fitting object
                GaussFit coeffMinimiser(deltaFQ);

                if (fitCoefficients.size() != ncoeffp)
                {
                    warn("Number of terms ({}) in existing FitCoefficients array for target '{}' does "
                         "not match the current number ({}), so will fit from scratch.\n",
                         fitCoefficients.size(), targetNode->name(), ncoeffp);
                    fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, gSigma1, nIterations, 0.01,
                                                                  fluctuationSmoothing);
                }
                else
                    fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, fitCoefficients, gSigma1, nIterations, 0.01,
                                                                  fluctuationSmoothing);

                // Store the new fit coefficients
                fitCoefficients = coeffMinimiser.A();

                deltaFQFit = coeffMinimiser.approximation();
            }
            else if (expansionFunction_ == EPSRNode::PoissonExpansionFunction)
            {
                // Construct our fitting object
                PoissonFit coeffMinimiser(deltaFQ);

                if (fitCoefficients.size() != ncoeffp)
                {
                    warn("Number of terms ({}) in existing FitCoefficients array for target '{}' does "
                         "not match the current number ({}), so will fit from scratch.\n",
                         fitCoefficients.size(), targetNode->name(), ncoeffp);
                    fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, pSigma1, pSigma2, nIterations, 0.01,
                                                                  fluctuationSmoothing);
                }
                else
                    fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, fitCoefficients, pSigma1, pSigma2, nIterations,
                                                                  0.01, fluctuationSmoothing);

                // Store the new fit coefficients
                fitCoefficients = coeffMinimiser.C();

                deltaFQFit = coeffMinimiser.approximation();
            }
            message("Error between delta F(Q) and fit function is {:.2f}%.\n", fitError);

            /*
             * Calculate F(r)
             */

            // Retrieve the storage object
            auto &simulatedFR = targetProcessData_[targetNode].simulatedFR;

            // Copy the total calculated F(Q) and trim to the same range as the experimental data before FT
            simulatedFR = weightedSQ.total();
            Filters::trim(simulatedFR, originalReferenceData);
            Fourier::sineFT(simulatedFR, 1.0 / (2 * M_PI * M_PI * rho), 0.0, 0.03, 30.0,
                            WindowFunction(WindowFunction::Form::Lorch0));

            /*
             * Add the Data to the Scattering Matrix
             */

            // Set overall weighting factor for the dataset
            auto weightIt = std::find_if(targets.begin(), targets.end(),
                                         [targetNode](const auto &targetWeight) { return targetWeight.first == targetNode; });
            auto dataSetWeight =
                feedback * (weightIt == targets.end() ? 1.0 : weightIt->second) * (targets.size() / totalDataSetWeight);

            if (targetNodeType == NeutronSQ)
            {
                const auto weights = dynamic_cast<NeutronSQNode *>(targetNode)->weights();

                // Subtract intramolecular total from the reference data - this will enter into the ScatteringMatrix
                auto refMinusIntra = trimmedReferenceData;
                Interpolator::addInterpolated(weightedSQ.boundTotal(), refMinusIntra, -1.0);

                // Always add absolute data to the scattering matrix - if the calculated data has been normalised, remove this
                // normalisation from the reference data (we assume that the two are consistent)
                auto normType = targetNode->findOption("NormaliseTo")->get<StructureFactors::NormalisationType>();
                if (normType == StructureFactors::AverageOfSquaresNormalisation)
                    refMinusIntra *= weights.boundCoherentAverageOfSquares();
                else if (normType == StructureFactors::SquareOfAverageNormalisation)
                    refMinusIntra *= weights.boundCoherentSquareOfAverage();

                // Set the zero limit on the data (equivalent to EPSR's szeros == 0.0)
                Data1D zeroed;
                zeroed.addPoint(0.0, refMinusIntra.values().front());
                for (auto &&[x, y] : zip(refMinusIntra.xAxis(), refMinusIntra.values()))
                    zeroed.addPoint(x, y);

                scatteringMatrix_->setRow(std::format("Neutron//{}", targetNode->name()), zeroed, weights, dataSetWeight);
            }
            else if (targetNodeType == XRaySQ)
            {
                const auto weights = dynamic_cast<XRaySQNode *>(targetNode)->weights();

                // For X-ray data we always add the reference data normalised to AverageOfSquares in order to give consistency
                // in terms of magnitude with any neutron data. If the calculated data have not been normalised, or were
                // normalised to something else, we correct it before adding.
                auto refMinusIntra = trimmedReferenceData;
                Interpolator::addInterpolated(weightedSQ.boundTotal(), refMinusIntra, -1.0);

                auto normType = targetNode->findOption("NormaliseTo")->get<StructureFactors::NormalisationType>();
                if (normType == StructureFactors::SquareOfAverageNormalisation)
                {
                    // Remove square of average normalisation, and apply average of squares
                    auto bSqOfAv = weights.boundCoherentSquareOfAverage(refMinusIntra.xAxis());
                    auto bAvOfSq = weights.boundCoherentAverageOfSquares(refMinusIntra.xAxis());
                    for (auto &&[val, bOld, bNew] : zip(refMinusIntra.values(), bSqOfAv, bAvOfSq))
                        val *= bOld / bNew;
                }
                else if (normType == StructureFactors::NoNormalisation)
                {
                    auto bbar = weights.boundCoherentAverageOfSquares(refMinusIntra.xAxis());
                    std::transform(refMinusIntra.values().begin(), refMinusIntra.values().end(), bbar.begin(),
                                   refMinusIntra.values().begin(), std::divides<>());
                }

                // Set the zero limit on the data (equivalent to EPSR's szeros == 0.0)
                Data1D zeroed;
                zeroed.addPoint(0.0, refMinusIntra.values().front());
                for (auto &&[x, y] : zip(refMinusIntra.xAxis(), refMinusIntra.values()))
                    zeroed.addPoint(x, y);

                scatteringMatrix_->setRow(std::format("XRay//{}", targetNode->name()), zeroed, weights, dataSetWeight);
            }
            else
            {
                error("Don't know how to add data from a node of type '{}' to the scattering matrix.",
                      (targetNodeType == NeutronSQ ? "NeutronSQ" : "XRaySQ"));
                return NodeConstants::ProcessResult::Failed;
            }

            /*
             * Sum Unweighted S(Q)
             */

            // Add the unweighted from this target to our combined, unweighted S(Q) data
            dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                                    [&](int i, const auto &at1, int j, const auto &at2)
                                    {
                                        const auto &partialIJ = unweightedSQ.unboundPartials().get(at1->name(), at2->name());
                                        Interpolator::addInterpolated(partialIJ,
                                                                      calculatedUnweightedSQ[{at1->name(), at2->name()}],
                                                                      1.0 / targets.size());
                                    });
        }

    // Finalise and store the total r-factor
    rFacTot /= targets.size();
    if (!totalRFactor_)
        totalRFactor_.emplace();
    totalRFactor_->addPoint(iteration, rFacTot);
    message("Current total R-Factor is {:.5f}.\n", rFacTot);
    for (auto &&[range, rangeTot] : zip(ranges_, rangedRFacTots))
    {
        message("Current total R-Factor over range {:.5f} to {:.5f} is {:.5f}.\n", range.minimum(), range.maximum(),
                rangeTot /= targets.size());
    }

    message("\n");

    /*
     * Augment the Scattering Matrix
     */

    // Add a contribution from each interatomic partial S(Q), weighted according to the feedback factor
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int i, auto &atI, int j, auto &atJ)
                            {
                                // Copy and rename the data for clarity
                                auto data = calculatedUnweightedSQ[{atI->name(), atJ->name()}];
                                data.setTag(std::format("Simulated {}-{}", atI->name(), atJ->name()));

                                scatteringMatrix_->setRow(std::format("{}-{}", atI->name(), atJ->name()), data, atI, atJ,
                                                          1.0 - feedback);
                            });

    // Make sure inverse matrices are up-to-date, taking representative Q values from the calculated unweighted S(Q)
    scatteringMatrix_->generateMatrices(calculatedUnweightedSQ.begin()->second.xAxis());
    scatteringMatrix_->print();

    if (Messenger::isVerbose())
    {
        message("\nScattering Matrix Inverse (Q = 0.0):\n");
        scatteringMatrix_->printInverse();

        message("\nIdentity (Ainv * A):\n");
        scatteringMatrix_->matrixProduct().print();
    }

    /*
     * Generate S(Q) from completed scattering matrix
     */
    if (!estimatedSQ_)
        estimatedSQ_.emplace();

    auto &estimatedSQ = *estimatedSQ_;
    estimatedSQ = scatteringMatrix_->generateEstimatedPartials();
    updateDeltaSQ(unweightedSQ, calculatedUnweightedSQ, estimatedSQ);

    /*
     * Calculate estimated g(r) from estimated S(Q)
     */
    if (!estimatedGR_)
        estimatedGR_.emplace();

    auto &estimatedGR = *estimatedGR_;

    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int indexI, auto atI, int indexJ, auto atJ)
                            {
                                DoubleKeyedMapKey key(atI->name(), atJ->name());

                                // Set tag
                                estimatedGR.get(key).setTag(std::format("{}-{}", atI->name(), atJ->name()));

                                // Copy experimental S(Q) and FT it
                                estimatedGR[key] = estimatedSQ.get(key);
                                Fourier::sineFT(estimatedGR[key], 1.0 / (2 * M_PI * M_PI * rho), 0.0, 0.05, 30.0,
                                                WindowFunction(WindowFunction::Form::Lorch0));
                                estimatedGR[key] += 1.0;
                            });
    /*
     * Calculate contribution to potential coefficients.
     * Multiply each coefficient by the associated weight in the inverse scattering matrix.
     * Note: the data were added to the scattering matrix in the order they appear in the targets
     * iterator.
     */
    auto dataIndex = 0;
    for (auto &[targetNode, _] : targets)
    {
        // For this Node, retrieve the coefficients of the fit performed above.
        auto &fitCoefficients = targetProcessData_[targetNode].fitCoefficients;
        // Loop over pair potentials and retrieve the inverse weight from the scattering matrix
        auto columnIndex = 0;
        dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                                [&](int i, auto typeI, int j, auto typeJ)
                                {
                                    auto weight = scatteringMatrix_->qZeroMatrixInverse()[{columnIndex, dataIndex}];

                                    /*
                                     * EPSR assembles the potential coefficients from the deltaFQ fit coefficients as a linear
                                     * combination with the following weighting factors (see circa line 3378 in
                                     * epsr_standalone_rev1.f):
                                     *
                                     * 1. The overall potential factor (potfac) which is typically set to 1.0 in EPSR (or 0.0 to
                                     * disable potential generation)
                                     * 2. A flag controlling whether specific potentials are refined (efacp)
                                     * 3. The value of the inverse scattering matrix for this dataset / potential (cwtpot),
                                     * multiplied by the feedback factor.
                                     */

                                    // In the original EPSR the off-diagonal elements in the inverse matrix have also been
                                    // halved so as not to double-count the i != j terms
                                    if (i != j)
                                        weight *= 0.5;

                                    // Store fluctuation coefficients ready for addition to potential coefficients later on.
                                    auto [begin, end] = fluctuationCoefficients[std::tuple{i, j}];
                                    std::transform(fitCoefficients.begin(), fitCoefficients.end(), begin, begin,
                                                   [weight, feedback, this](auto coeff, auto result)
                                                   { return result + weight * feedback * coeff; });

                                    ++columnIndex;
                                });

        // Increase dataIndex
        ++dataIndex;
    }

    // Generate new empirical potentials
    auto energabs = 0.0;
    if (modifyPotential && (*runCount_ % *modifyPotential == 0))
    {
        // Sum fluctuation coefficients in to the potential coefficients
        auto &coefficients = potentialCoefficients(nAtomTypes, ncoeffp);
        dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                                [&](int i, auto at1, int j, auto at2)
                                {
                                    auto &potCoeff = coefficients[{i, j}];

                                    // Zero potential before adding in fluctuation coefficients?
                                    if (overwritePotentials_)
                                        std::fill(potCoeff.begin(), potCoeff.end(), 0.0);

                                    // Add in fluctuation coefficients
                                    auto [begin, end] = fluctuationCoefficients[std::tuple{i, j}];
                                    std::transform(potCoeff.begin(), potCoeff.end(), begin, potCoeff.begin(),
                                                   [this, weighting](auto pot, auto fluct) { return pot + weighting * fluct; });

                                    // Set first term to zero (following EPSR)
                                    potCoeff[0] = 0.0;
                                });

        // Determine absolute energy of empirical potentials
        energabs = absEnergyEP(atomTypes);

        /*
         * Determine the scaling we will apply to the coefficients (if any)
         * Notes:
         * 	- ereq is actually read in as a variable called 'pressreq', then assigned to another variable
         * 'absolute_energy' in the EPSR25 source.
         * 	- erequnit appears to be set to the value of ereqstep read in from the 'ereqstep' command (clamped to
         * 0.0-1.0).
         */
        auto pressfac = 1.0;
        auto erequnit = 0.0, ereqstep = 0.0;

        if (fabs(eReq) == 0.0)
        {
            pressfac = 0.0;
            energabs = 0.0;
        }
        else if (fabs(energabs) > 0.0)
        {
            if (fabs(energabs) < fabs(erequnit))
                pressfac = fabs(erequnit) / fabs(energabs);
            else
            {
                pressfac = fabs(eReq) / fabs(energabs);
                if ((pressfac > 1.0) && (ereqstep == 0.0))
                    pressfac = 1.0;
            }
        }
        message("  generate_ep>  {}  {}  {}\n", eReq, energabs, pressfac);

        // Scale coefficients
        for (auto &n : coefficients)
            std::transform(n.begin(), n.end(), n.begin(), [pressfac](auto value) { return value * pressfac; });
        energabs *= pressfac;

        // Generate additional potentials from the coefficients
        auto sigma1 = expansionFunction_ == EPSRNode::PoissonExpansionFunction ? pSigma1 : gSigma1;
        auto sigma2 = expansionFunction_ == EPSRNode::PoissonExpansionFunction ? pSigma2 : gSigma2;

        if (!generateEmpiricalPotentials(atomTypes, rho, ncoeffp, rminpt, rmaxpt, sigma1, sigma2))
            return NodeConstants::ProcessResult::Failed;
    }
    else
        energabs = absEnergyEP(atomTypes);

    if (!phiArray_)
        phiArray_.emplace();
    phiArray_->addPoint(iteration, energabs);

    return NodeConstants::ProcessResult::Success;
}
