// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#define _USE_MATH_DEFINES

#include "nodes/bragg.h"

BraggNode::BraggNode(Graph *parentGraph)
    : Node(parentGraph), braggReflectionHistory_(
                             [this]()
                             {
                                 BraggReflectionVector v;
                                 v.values().resize(this->braggReflections_->values().size());
                                 return v;
                             })
{
    // Inputs
    addInput<Configuration *>("Configuration", "Set target configuration for the node", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});
    addInput<PartialSet *>("UnweightedSQ", "Unweighted partials for target configuration", unweightedSQ_);

    // Outputs
    addOutput<PartialSet *>("UnweightedSQ", "Unweighted partials for target configuration", unweightedSQ_);

    // Options
    addOption<Number>("QMin", "Minimum Q value for Bragg calculation", qMin_);
    addOption<Number>("QMax", "Maximum Q value for Bragg calculation", qMax_);
    addOption<Number>("QDelta", "Resolution (bin width) in Q space to use when calculating Bragg reflections", qDelta_);
    addOption<Vector3i>("Multiplicity", "Bragg intensity scaling factor accounting for number of repeat units in Configuration",
                        multiplicity_);
    addOption<std::optional<Number>>("Averaging", "Number of historical data sets to combine into final reflection data",
                                     averagingLength_)
        ->setFlags({ParameterBase::ClearData});
    addOption<Averaging::AveragingScheme>("AveragingScheme", "Weighting scheme to use when averaging reflection data",
                                          averagingScheme_);
    addOption<Function1DWrapper>("BraggQBroadening", "Broadening function to apply to Bragg reflections when generating S(Q)",
                                 braggQBroadening_);
    addOption<bool>("SaveReflections", "Whether to save Bragg reflection data to disk", saveReflections_);

    // Serialisables
    addSerialisable("weightedGR", braggReflections_);
}

std::string_view BraggNode::type() const { return "Bragg"; }

std::string_view BraggNode::summary() const { return "Calculates the Bragg reflections over a specified Q-range."; }

// Run main processing
NodeConstants::ProcessResult BraggNode::process()
{
    auto qMin = qMin_.asDouble();
    auto qMax = qMax_.asDouble();
    auto qDelta = qDelta_.asDouble();

    if (!braggReflections_)
        braggReflections_.emplace();

    const auto nTypes = targetConfiguration_->atomTypePopulations().size();

    if (!braggPartials_)
    {
        // Create the triangular array
        braggPartials_.emplace(nTypes, nTypes, true);

        // Generate empty Data1D over the Q range specified, setting bin centres
        Data1D temp;
        auto q = 0.5 * qDelta;
        while (q <= qMax)
        {
            temp.addPoint(q, 0.0);
            q += qDelta;
        }

        // Set up Data1D array with our empty data
        std::fill((*braggPartials_).begin(), (*braggPartials_).end(), temp);
    }

    // Print argument/parameter summary
    message("Calculating Bragg S(Q) over {} < Q < {} Angstroms**-1 using bin size of {} Angstroms**-1.\n", qMin, qMax, qDelta);
    message("Multiplicity is ({} {} {}).\n", multiplicity_.x, multiplicity_.y, multiplicity_.z);
    if (averagingLength_)
        message("Reflections will be averaged over {} sets (scheme = {}).\n", averagingLength_.value().asInteger(),
                Averaging::averagingSchemes().keyword(averagingScheme_));
    else
        message("No averaging of reflections will be performed.\n");
    message("Multiplicity of unit cell in source configuration is [{} {} {}].\n", multiplicity_.x, multiplicity_.y,
            multiplicity_.z);
    if (unweightedSQ_)
    {
        if (braggQBroadening_.form() == Functions1D::Form::None)
            Messenger::print("No additional broadening will be applied to calculated Bragg S(Q).");
        else
            Messenger::print("Broadening to be applied in calculated Bragg S(Q) is {} ({}).",
                             Functions1D::forms().keyword(braggQBroadening_.form()), braggQBroadening_.parameterSummary());
    }
    message("\n");

    // Store unit cell information
    const auto unitCellVolume = targetConfiguration_->box()->volume() / (multiplicity_.x * multiplicity_.y * multiplicity_.z);

    // Calculate Bragg vectors and intensities for the current Configuration
    if (!calculateBraggTerms())
        return NodeConstants::ProcessResult::Failed;

    // Perform averaging of reflections data if requested
    if (averagingLength_)
        (*braggReflections_) = braggReflectionHistory_.push(*braggReflections_, averagingLength_.value().asInteger());

    // Form partial and total reflection functions
    formReflectionFunctions();

    // Save reflection data?
    if (saveReflections_)
    {
        // Open a file and save the basic reflection data
        LineParser braggParser;
        if (!braggParser.openOutput(std::format("{}-Reflections.txt", name())))
            return NodeConstants::ProcessResult::Failed;
        braggParser.writeLineF("#   ID      Q     h k l     mult    Intensity(0,0)\n");
        auto count = 0;
        for (const auto &reflxn : braggReflections_->values())
        {
            if (!braggParser.writeLineF("{:6d}  {:10.6f} {} {} {} {:8d}  {:10.6e}\n", ++count, reflxn.q(), reflxn.hkl().x,
                                        reflxn.hkl().y, reflxn.hkl().z, reflxn.nKVectors(), reflxn.intensity(0, 0)))
                return NodeConstants::ProcessResult::Failed;
        }
        braggParser.closeFiles();

        // Save intensity data
        auto types = targetConfiguration_->atomTypePopulations();
        auto success = for_each_pair_early(
            types,
            [&](int i, const auto &popI, int j, const auto &popJ) -> EarlyReturn<bool>
            {
                LineParser intensityParser;
                if (!intensityParser.openOutput(std::format("{}-{}-{}.txt", name(), popI.first->name(), popJ.first->name())))
                    return false;
                intensityParser.writeLineF("#     Q      Intensity({},{})\n", popI.first->name(), popJ.first->name());
                for (const auto &reflxn : braggReflections_->values())
                    if (!intensityParser.writeLineF("{:10.6f}  {:10.6e}\n", reflxn.q(), reflxn.intensity(i, j)))
                        return false;
                intensityParser.closeFiles();

                return EarlyReturn<bool>::Continue;
            });
        if (!success.value_or(true))
            return NodeConstants::ProcessResult::Failed;
    }

    if (unweightedSQ_)
    {
        const auto nReflections = braggReflections_->values().size();
        const auto braggQMax = braggReflections_->values().at(nReflections - 1).q();
        message("Found reflections data (nReflections = {}, Q(last) = {} Angstroms**-1).\n", nReflections, braggQMax);

        // Prepare a temporary object for the Bragg partials
        auto typeFractions = unweightedSQ_->atomTypeFractions();
        // braggPartials.initialise(typeFractions.size(), typeFractions.size(), true);
        for (auto &partial : *braggPartials_)
            partial.initialise(unweightedSQ_->partials().begin()->second);

        // For each partial in our S(Q) array, calculate the broadened Bragg function and blend it
        auto success = for_each_pair_early(typeFractions,
                                           [&](auto indexI, auto &popI, int indexJ, auto &popJ) -> EarlyReturn<bool>
                                           {
                                               // Grab relevant partial and loop over reflections
                                               auto &partial = (*braggPartials_)[{indexI, indexJ}];
                                               for (const auto &reflxn : braggReflections_->values())
                                               {
                                                   const auto intensity = reflxn.intensity(indexI, indexJ);
                                                   for (auto &&[q, by] : zip(partial.xAxis(), partial.values()))
                                                       by += braggQBroadening_.y(q - reflxn.q(), q) * intensity *
                                                             braggQBroadening_.normalisation(q) / (reflxn.q() * q);
                                               }

                                               return EarlyReturn<bool>::Continue;
                                           });
        if (success && !success.value())
            return NodeConstants::ProcessResult::Failed;

        // Finalise partials
        for (auto &partial : *braggPartials_)
            std::transform(partial.values().begin(), partial.values().end(), partial.values().begin(),
                           [unitCellVolume](auto &val) { return val * 2.0 * pow(M_PI, 2) / unitCellVolume; });

        // Remove self-scattering level from partials between the same atom type and remove normalisation from atomic fractions
        dissolve::for_each_pair(ParallelPolicies::par, typeFractions,
                                [&](auto indexI, auto &popI, int indexJ, auto &popJ)
                                {
                                    // Subtract self-scattering level if types are equivalent
                                    if (indexI == indexJ)
                                        (*braggPartials_)[{indexI, indexJ}] -= popI.second;

                                    // Remove atomic fraction normalisation
                                    (*braggPartials_)[{indexI, indexJ}] /= popI.second * popJ.second;
                                });

        // Blend the bound/unbound and Bragg partials at the higher Q limit
        dissolve::for_each_pair(ParallelPolicies::par, typeFractions,
                                [&](auto indexI, auto &popI, int indexJ, auto &popJ)
                                {
                                    // Note: Intramolecular broadening will not be applied to bound terms within the
                                    // calculated Bragg scattering
                                    auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};

                                    auto &bound = unweightedSQ_->boundPartials().get(key);
                                    auto &unbound = unweightedSQ_->unboundPartials().get(key);
                                    auto &partial = unweightedSQ_->partials().get(key);
                                    auto &bragg = (*braggPartials_)[{indexI, indexJ}];

                                    for (auto n = 0; n < bound.nValues(); ++n)
                                    {
                                        const auto q = bound.xAxis(n);
                                        if (q <= braggQMax)
                                        {
                                            bound.value(n) = 0.0;
                                            unbound.value(n) = bragg.value(n);
                                            partial.value(n) = bragg.value(n);
                                        }
                                    }
                                });

        // Re-form the total function
        unweightedSQ_->formTotals(true);
    }

    return NodeConstants::ProcessResult::Success;
}

// Calculate unweighted Bragg scattering for specified Configuration
bool BraggNode::calculateBraggTerms()
{
    auto &braggReflections = braggReflections_->values();

    // Realise the arrays from the Configuration
    std::vector<KVector> braggKVectors;
    Array2D<double> braggAtomVectorXCos;
    Array2D<double> braggAtomVectorYCos;
    Array2D<double> braggAtomVectorZCos;
    Array2D<double> braggAtomVectorXSin;
    Array2D<double> braggAtomVectorYSin;
    Array2D<double> braggAtomVectorZSin;
    Vector3i braggMaximumHKL;

    // Grab some useful values
    const auto *box = targetConfiguration_->box();
    auto nTypes = targetConfiguration_->atomTypePopulations().size();
    auto nAtoms = targetConfiguration_->nAtoms(AtomConstants::Presence::Physical);
    auto &atoms = targetConfiguration_->atoms();

    // Set up reciprocal axes and lengths - take those from the Box and scale based on the multiplicity
    auto rAxes = box->reciprocalAxes();
    rAxes.columnMultiply(0, multiplicity_.x);
    rAxes.columnMultiply(1, multiplicity_.y);
    rAxes.columnMultiply(2, multiplicity_.z);
    auto rLengths = box->reciprocalAxisLengths();
    rLengths.x *= multiplicity_.x;
    rLengths.y *= multiplicity_.y;
    rLengths.z *= multiplicity_.z;
    message("Reciprocal axes and lengths (accounting for multiplicity) are:\n");
    message("	r(x) = {:e} {:e} {:e} ({:e})\n", rAxes.columnAsVec3(0).x, rAxes.columnAsVec3(0).y, rAxes.columnAsVec3(0).z,
            rLengths.x);
    message("	r(y) = {:e} {:e} {:e} ({:e})\n", rAxes.columnAsVec3(1).x, rAxes.columnAsVec3(1).y, rAxes.columnAsVec3(1).z,
            rLengths.y);
    message("	r(z) = {:e} {:e} {:e} ({:e})\n", rAxes.columnAsVec3(2).x, rAxes.columnAsVec3(2).y, rAxes.columnAsVec3(2).z,
            rLengths.z);

    int n, m, h, k, l, kAbs, lAbs;
    double *cosTermsH, *sinTermsH, *cosTermsK, *sinTermsK, *cosTermsL, *sinTermsL;
    double hkCos, hkSin, hklCos, hklSin;

    // Create a timer
    Timer timer;

    // Calculate k-vectors within specified Q range
    double mag, magSq;
    int braggIndex;
    timer.start();
    if (braggKVectors.empty())
    {
        message("Performing initial set up of Bragg arrays...\n");
        timer.start();
        auto qMax = qMax_.asDouble(), qMin = qMin_.asDouble(), qDelta = qDelta_.asDouble();
        auto qMaxSq = qMax * qMax, qMinSQ = qMin * qMin;
        auto nBraggBins = qMax / qDelta + 1;

        // Determine extents of hkl indices to use
        braggMaximumHKL.x = qMax / rLengths.x;
        braggMaximumHKL.y = qMax / rLengths.y;
        braggMaximumHKL.z = qMax / rLengths.z;

        // Clear old arrays, and set a suitable reservation for the k-vectors array
        braggReflections.clear();
        braggReflections.resize(nBraggBins, BraggReflection());
        braggKVectors.clear();
        braggKVectors.reserve(braggMaximumHKL.x * braggMaximumHKL.y * braggMaximumHKL.z);

        // Create temporary 3D array for k-vectors, and linear array for Bragg reflections
        OffsetArray3D<KVector> tempKVectors(0, braggMaximumHKL.x, -braggMaximumHKL.y, braggMaximumHKL.y, -braggMaximumHKL.z,
                                            braggMaximumHKL.z);

        // Initialise Bragg reflections - Q values reflect the centre-bins of the reflection.
        auto q = 0.5 * qDelta;
        for (auto &reflxn : braggReflections)
        {
            reflxn.initialise(q, -1, nTypes);
            q += qDelta;
        }
        Vector3 kVec, v;
        for (h = 0; h <= braggMaximumHKL.x; ++h)
        {
            kVec.x = h;
            for (k = -braggMaximumHKL.y; k <= braggMaximumHKL.y; ++k)
            {
                kVec.y = k;
                for (l = -braggMaximumHKL.z; l <= braggMaximumHKL.z; ++l)
                {
                    if ((h == 0) && (k == 0) && (l == 0))
                        continue;

                    kVec.z = l;
                    v = rAxes * kVec;

                    // Calculate magnitude of this k vector
                    magSq = v.magnitudeSq();
                    if ((magSq >= qMinSQ) && (magSq <= qMaxSq))
                    {
                        mag = sqrt(magSq);

                        // Calculate index of associated Bragg reflection in the reflections array
                        braggIndex = int(mag / qDelta);

                        // Point this (h,k,l) value to this Bragg reflection
                        tempKVectors[{h, k, l}].initialise(h, k, l, braggIndex, nTypes);

                        // Note in the reflection that we have found another (h,k,l) that contributes to it
                        braggReflections[braggIndex].addKVectors(1);
                        braggReflections[braggIndex].setHKL(h, k, l);
                    }
                }
            }
        }

        // Renumber reflections, assigning an index only if there are KVectors associated with it
        braggIndex = 0;
        for (auto &reflxn : braggReflections)
            if (reflxn.nKVectors() > 0)
                reflxn.setIndex(braggIndex++);

        // Collapse KVectors into a linear list, excluding any that weren't initialised
        std::copy_if(tempKVectors.begin(), tempKVectors.end(), std::back_inserter(braggKVectors),
                     [](auto &kv) { return kv.braggReflectionIndex() != -1; });

        // Set new indices for BraggReflections in our new KVectors
        for (auto &kv : braggKVectors)
            kv.setBraggReflectionIndex(braggReflections[kv.braggReflectionIndex()].index());

        // Prune BraggReflections array, putting them into a sequential vector that will reflect their new indexing
        braggReflections.erase(std::remove_if(braggReflections.begin(), braggReflections.end(),
                                              [](const auto &rflxn) { return rflxn.nKVectors() == 0; }),
                               braggReflections.end());

        message("Bragg calculation spans {} k-vectors (max HKL = [{} {} {}]) over {} <= Q <= {} ({} elapsed).\n",
                braggKVectors.size(), braggMaximumHKL.x, braggMaximumHKL.y, braggMaximumHKL.z, qMin, qMax,
                timer.elapsedTimeString());
        message("{} unique Bragg reflections found using a Q resolution of {} Angstroms**-1.\n", braggReflections.size(),
                qDelta);

        // Create atom working arrays
        braggAtomVectorXCos.initialise(nAtoms, braggMaximumHKL.x + 1);
        braggAtomVectorYCos.initialise(nAtoms, braggMaximumHKL.y + 1);
        braggAtomVectorZCos.initialise(nAtoms, braggMaximumHKL.z + 1);
        braggAtomVectorXSin.initialise(nAtoms, 2 * braggMaximumHKL.x + 1);
        braggAtomVectorYSin.initialise(nAtoms, 2 * braggMaximumHKL.y + 1);
        braggAtomVectorZSin.initialise(nAtoms, 2 * braggMaximumHKL.z + 1);
    }

    // Precalculate cos/sin terms for atoms
    timer.stop();
    timer.zero();
    timer.start();
    Vector3 v, rI;
    for (n = 0; n < nAtoms; ++n)
    {
        // Skip unphysical atoms
        if (!atoms[n].isPresence(AtomConstants::Presence::Physical))
            continue;

        // Calculate reciprocal lattice atom coordinates
        // TODO CHECK Test this in a non-cubic system!
        v = atoms[n].r();
        rI.x = v.x * rAxes[0] + v.y * rAxes[1] + v.z * rAxes[2];
        rI.y = v.x * rAxes[3] + v.y * rAxes[4] + v.z * rAxes[5];
        rI.z = v.x * rAxes[6] + v.y * rAxes[7] + v.z * rAxes[8];

        // Grab pointers to cos/sin arrays for atom
        cosTermsH = braggAtomVectorXCos.pointerAt(n, 0);
        cosTermsK = braggAtomVectorYCos.pointerAt(n, 0);
        cosTermsL = braggAtomVectorZCos.pointerAt(n, 0);
        sinTermsH = braggAtomVectorXSin.pointerAt(n, braggMaximumHKL.x);
        sinTermsK = braggAtomVectorYSin.pointerAt(n, braggMaximumHKL.y);
        sinTermsL = braggAtomVectorZSin.pointerAt(n, braggMaximumHKL.z);

        // Initialise zeroth and first terms
        cosTermsH[0] = 1.0;
        cosTermsK[0] = 1.0;
        cosTermsL[0] = 1.0;
        sinTermsH[0] = 0.0;
        sinTermsK[0] = 0.0;
        sinTermsL[0] = 0.0;
        cosTermsH[1] = cos(rI.x);
        cosTermsK[1] = cos(rI.y);
        cosTermsL[1] = cos(rI.z);
        sinTermsH[1] = sin(rI.x);
        sinTermsK[1] = sin(rI.y);
        sinTermsL[1] = sin(rI.z);
        sinTermsH[-1] = -sinTermsH[1];
        sinTermsK[-1] = -sinTermsK[1];
        sinTermsL[-1] = -sinTermsL[1];

        // Generate H terms via power expansion
        for (m = 2; m <= braggMaximumHKL.x; ++m)
        {
            cosTermsH[m] = cosTermsH[1] * cosTermsH[m - 1] - sinTermsH[1] * sinTermsH[m - 1];
            sinTermsH[m] = cosTermsH[1] * sinTermsH[m - 1] + sinTermsH[1] * cosTermsH[m - 1];
            sinTermsH[-m] = -sinTermsH[m];
        }
        // Generate K terms via power expansion
        for (m = 2; m <= braggMaximumHKL.y; ++m)
        {
            cosTermsK[m] = cosTermsK[1] * cosTermsK[m - 1] - sinTermsK[1] * sinTermsK[m - 1];
            sinTermsK[m] = cosTermsK[1] * sinTermsK[m - 1] + sinTermsK[1] * cosTermsK[m - 1];
            sinTermsK[-m] = -sinTermsK[m];
        }
        // Generate L terms via power expansion
        for (m = 2; m <= braggMaximumHKL.z; ++m)
        {
            cosTermsL[m] = cosTermsL[1] * cosTermsL[m - 1] - sinTermsL[1] * sinTermsL[m - 1];
            sinTermsL[m] = cosTermsL[1] * sinTermsL[m - 1] + sinTermsL[1] * cosTermsL[m - 1];
            sinTermsL[-m] = -sinTermsL[m];
        }
    }
    timer.stop();
    message("Calculated atomic cos/sin terms ({} elapsed)\n", timer.totalTimeString());

    // Calculate k-vector contributions
    int localTypeIndex;

    // Zero k-vector cos/sin contributions
    std::for_each(braggKVectors.begin(), braggKVectors.end(), [](auto &kvec) { kvec.zeroCosSinTerms(); });

    // Loop over atoms
    timer.start();
    for (n = 0; n < nAtoms; ++n)
    {
        // Skip unphysical atoms
        if (!atoms[n].isPresence(AtomConstants::Presence::Physical))
            continue;

        // Grab localTypeIndex and array pointers for this atom
        localTypeIndex = atoms[n].atomTypeIndex();

        cosTermsH = braggAtomVectorXCos.pointerAt(n, 0);
        cosTermsK = braggAtomVectorYCos.pointerAt(n, 0);
        cosTermsL = braggAtomVectorZCos.pointerAt(n, 0);
        sinTermsH = braggAtomVectorXSin.pointerAt(n, braggMaximumHKL.x);
        sinTermsK = braggAtomVectorYSin.pointerAt(n, braggMaximumHKL.y);
        sinTermsL = braggAtomVectorZSin.pointerAt(n, braggMaximumHKL.z);

        // Loop over k-vectors
        for (auto &kvec : braggKVectors)
        {
            // Grab h, k, and l indices from KVector
            h = kvec.h();
            k = kvec.k();
            l = kvec.l();
            kAbs = abs(k);
            lAbs = abs(l);

            // Calculate complex product from atomic cos/sin terms
            hkCos = cosTermsH[h] * cosTermsK[kAbs] - sinTermsH[h] * sinTermsK[k];
            hkSin = cosTermsH[h] * sinTermsK[k] + sinTermsH[h] * cosTermsK[kAbs];
            hklCos = hkCos * cosTermsL[lAbs] - hkSin * sinTermsL[l];
            hklSin = hkCos * sinTermsL[l] + hkSin * cosTermsL[lAbs];

            // Sum contribution into the k-vector's cos/sin arrays
            kvec.addCosTerm(localTypeIndex, hklCos);
            kvec.addSinTerm(localTypeIndex, hklSin);
        }
    }
    timer.stop();
    message("Calculated atomic contributions to k-vectors ({} elapsed)\n", timer.totalTimeString());

    // Zero Bragg reflection intensities
    std::for_each(braggReflections.begin(), braggReflections.end(), [](auto &reflxn) { reflxn.reset(); });

    // Calculate intensities for individual KVectors - this will be automatically summed into the corresponding
    // BraggReflection
    std::for_each(braggKVectors.begin(), braggKVectors.end(),
                  [&braggReflections](auto &kvec) { kvec.calculateIntensities(braggReflections); });

    // Normalise intensities against number of atoms and unit cell multiplicity
    const auto divisor = 1.0 / (nAtoms * multiplicity_.x * multiplicity_.y * multiplicity_.z);
    std::for_each(braggReflections.begin(), braggReflections.end(), [divisor](auto &reflxn) { reflxn *= divisor; });

    return true;
}

// Form partial and total reflection functions from calculated reflection data
bool BraggNode::formReflectionFunctions()
{
    auto &braggReflections = braggReflections_->values();
    auto &braggPartials = braggPartials_.value();
    const auto nReflections = braggReflections.size();

    Data1D braggTotal;
    braggTotal.clear();

    // Zero Bragg partials
    for (auto &partial : braggPartials)
        std::fill(partial.values().begin(), partial.values().end(), 0.0);

    // Loop over pairs of atom types, adding in contributions from our calculated BraggReflections
    double qCentre;
    int bin;
    auto types = targetConfiguration_->atomTypePopulations();
    dissolve::for_each_pair(ParallelPolicies::seq, types,
                            [&](int typeI, auto &popI, int typeJ, auto &popJ)
                            {
                                // Retrieve partial container and make sure its tag is set
                                auto &partial = braggPartials[{typeI, typeJ}];
                                partial.setTag(std::format("{}-{}", popI.first->name(), popJ.first->name()));

                                // Loop over defined Bragg reflections
                                for (auto n = 0; n < nReflections; ++n)
                                {
                                    // Get q value and intensity of reflection
                                    qCentre = braggReflections.at(n).q();
                                    bin = qCentre / qDelta_.asDouble();

                                    partial.value(bin) += braggReflections.at(n).intensity(typeI, typeJ);
                                }

                                // Add this partial into the total function, accounting for doubling of partials between unlike
                                // atom types
                                braggTotal += partial;
                                if (typeI != typeJ)
                                    braggTotal += partial;
                            });

    return true;
}

// Re-bin reflection data into supplied arrays
bool BraggNode::reBinReflections()
{
    auto &braggReflections = braggReflections_->values();
    auto &braggPartials = braggPartials_.value();
    const auto nReflections = braggReflections.size();

    const auto nTypes = targetConfiguration_->atomTypePopulations().size();

    // Create a temporary Data1D into which we will generate individual Bragg peak contributions
    const auto qDelta = braggPartials[{0, 0}].xAxis(1) - braggPartials[{0, 0}].xAxis(0);
    const auto nBins = braggPartials[{0, 0}].nValues();
    std::vector<int> nAdded(nBins, 0);

    // Loop over defined Bragg reflections
    // TODO Prune reflections based on intensity to speed-up process?
    int bin;
    for (auto n = 0; n < nReflections; ++n)
    {
        // Get Q bin (in the braggPartials) of the reflection
        bin = int(braggReflections[n].q() / qDelta);
        if ((bin < 0) || (bin >= nBins))
        {
            warn("Reflection {} is at Q = {} Angstroms**-1, which is outside of the current Q range.\n", n,
                 braggReflections.at(n).q());
            continue;
        }

        ++nAdded[bin];

        // Loop over pairs of atom types, binning intensity contributions from this reflection
        auto types = targetConfiguration_->atomTypePopulations();
        int typeI = 0;
        for (auto atd1 = types.begin(); atd1 != types.end(); typeI++, atd1++)
        {
            int typeJ = typeI;
            for (auto atd2 = atd1; atd2 != types.end(); typeJ++, atd2++)
            {
                braggPartials[{typeI, typeJ}].value(bin) += braggReflections.at(n).intensity(typeI, typeJ);
            }
        }
    }

    // Normalise data against number of intensities added to each bin
    for (auto typeI = 0; typeI < nTypes; ++typeI)
    {
        for (auto typeJ = typeI; typeJ < nTypes; ++typeJ)
        {
            for (auto n = 0; n < nBins; ++n)
                if (nAdded[n] > 0)
                    braggPartials[{typeI, typeJ}].value(n) /= nAdded[n];
        }
    }
    return true;
}

// Get reflections data
const std::vector<BraggReflection> &BraggNode::braggReflections() { return braggReflections_->values(); }