// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atomType.h"
#include "main/dissolve.h"
#include "math/gaussFit.h"
#include "math/mathFunc.h"
#include "math/poissonFit.h"
#include "nodes/configuration.h"
#include "nodes/dissolve.h"
#include "nodes/epsr.h"
#include "nodes/neutronSQ.h"
#include "nodes/sq/sq.h"
#include "templates/algorithms.h"

// Return enum option info for ExpansionFunctionType
EnumOptions<EPSRNode::ExpansionFunctionType> EPSRNode::expansionFunctionTypes()
{
    return EnumOptions<ExpansionFunctionType>("ExpansionFunctionType", {{EPSRNode::PoissonExpansionFunction, "Poisson"},
                                                                        {EPSRNode::GaussianExpansionFunction, "Gaussian"}});
}
EnumOptions<EPSRNode::ExpansionFunctionType> getEnumOptions(EPSRNode::ExpansionFunctionType)
{
    return EPSRNode::expansionFunctionTypes();
}

// Returns the target unweighted SQ
EPSRNode::TargetCorrelationDataMap EPSRNode::findTargetCorrelationData()
{
    TargetCorrelationDataMap result{};

    auto &allNodes = dissolveGraph()->nodes();
    auto filterOn = !namedWeights_.value().empty();
    for (const auto &[nodeName, nodePtr] : allNodes)
    {
        // Filter by name (if any names set)
        if (filterOn)
        {
            auto it = std::find(targetNames_.begin(), targetNames_.end(), nodeName);
            if (it == targetNames_.end())
                continue;
        }

        // Attempt casting to NeutronSQNode
        auto neutronSQNode = dynamic_cast<NeutronSQNode *>(nodePtr.get());

        // If NeutronSQNode cast is not null, check configuration matches and add to target data
        if (neutronSQNode && neutronSQNode->sourceConfiguration() == targetConfiguration_)
        {
            appendTarget(result, nodePtr.get(), NeutronSQ, namedWeights_.value());
            continue;
        }

        // Attempt casting to XRaySQNode
        auto xRaySQNode = dynamic_cast<XRaySQNode *>(nodePtr.get());

        // If XRaySQNode cast is not null, check configuration matches and add to target data
        if (xRaySQNode && xRaySQNode->sourceConfiguration() == targetConfiguration_)
        {
            appendTarget(result, nodePtr.get(), XRaySQ, namedWeights_.value());
            continue;
        }

        // Attempt casting to SQNode
        auto sqNode = dynamic_cast<SQNode *>(nodePtr.get());

        // If SQNode cast is not null, check configuration matches and add to target data
        if (sqNode && sqNode->sourceConfiguration() == targetConfiguration_)
        {
            appendTarget(result, nodePtr.get(), SQ, namedWeights_.value());
            continue;
        }
    }

    // Logging
    std::map<CorrelationNodeType, std::string> foundNodeNames{{NeutronSQ, ""}, {XRaySQ, ""}, {SQ, ""}};
    for (auto &[type, s] : foundNodeNames)
        for (int i = 0; i < result[type].size(); i++)
        {
            auto &[node, weight] = result[type][i];
            s += std::format("'{}' weight={}{}", node->name(), weight, (i + 1) == result[type].size() ? "" : ", ");
        }

    message(
        "EPSR found {} NeutronSQ nodes{}, {} XRaySQ nodes{}, and {} SQ node{}for refinement.\nThe common configuration is '{}'",
        result[NeutronSQ].size(), result[NeutronSQ].size() > 0 ? std::format(" ({}) ", foundNodeNames[NeutronSQ]) : "",
        result[XRaySQ].size(), result[XRaySQ].size() > 0 ? std::format(" ({}) ", foundNodeNames[XRaySQ]) : "",
        result[SQ].size(), result[SQ].size() > 0 ? std::format(" ({}) ", foundNodeNames[SQ]) : "",
        std::string(targetConfiguration_->name()));

    return result;
}

// Returns the target correlation data
inline void EPSRNode::appendTarget(EPSRNode::TargetCorrelationDataMap &data, Node *node, EPSRNode::CorrelationNodeType nodeType,
                                   std::vector<std::pair<std::string, double>> inputWeights)
{
    auto nodeTypeMap = data.find(nodeType);

    if (nodeTypeMap == data.end())
        data.insert({nodeType, {{node, 1.0}}});
    else
        data[nodeType].push_back({node, 1.0});

    auto &assignedWeight = data[nodeType].back().second;

    // Update target weight
    for (const auto &[targetName, weight] : inputWeights)
        if (targetName == node->name())
        {
            assignedWeight *= weight;
            break;
        }
}

// Return target process data for a given node
const EPSRNode::TargetProcessData &EPSRNode::targetProcessData(Node *target) { return targetProcessData_[target]; }

// Return current scattering matrix
const ScatteringMatrix &EPSRNode::scatteringMatrix() const { return *scatteringMatrix_; }

// Return additional potential for a given atom type pair
const Data1D &EPSRNode::additionalPotential(std::string i, std::string j) const { return (*additionalPotentials_)[{i, j}]; }

// Return estimated S(Q) for a given atom type pair
const Data1D &EPSRNode::estimatedSQ(std::string i, std::string j) const { return (*estimatedSQ_)[{i, j}]; }

// Set whether to apply this node's generated potentials to the global pair potentials
void EPSRNode::setApplyPotentials(bool b) { applyPotentials_ = b; }

// Create / update delta S(Q) information
void EPSRNode::updateDeltaSQ(OptionalReferenceWrapper<const PartialSet> unweightedSQ,
                             OptionalReferenceWrapper<const DoubleKeyedMap<Data1D>> optCalculatedSQ,
                             OptionalReferenceWrapper<const DoubleKeyedMap<Data1D>> optEstimatedSQ)
{
    // Find the relevant data if we were not provided them
    if (!optCalculatedSQ)
        return;
    if (!optEstimatedSQ)
        return;

    auto calculatedSQ = optCalculatedSQ->get(), estimatedSQ = optEstimatedSQ->get();

    assert(calculatedSQ.size() == estimatedSQ.size());

    // Realise the DeltaSQ array
    DoubleKeyedMap<Data1D> deltaSQ;
    deltaSQ.clear();

    for (auto &[key, calcSQ] : calculatedSQ)
    {
        deltaSQ[key] = estimatedSQ[key];
        Interpolator::addInterpolated(calcSQ, deltaSQ[key], -1.0);

        // Copy the tag
        deltaSQ[key].setTag(calcSQ.tag());
    }
}

// Create / retrieve arrays for storage of empirical potential coefficients
Array2D<std::vector<double>> &EPSRNode::potentialCoefficients(const int nAtomTypes, std::optional<Number> ncoeffp)
{
    if (!potentialCoefficients_)
        potentialCoefficients_.emplace();

    auto &coefficients = *potentialCoefficients_;

    auto arrayNCoeffP = (coefficients.nRows() && coefficients.nColumns() ? coefficients[{0, 0}].size() : 0);
    if ((coefficients.nRows() != nAtomTypes) || (coefficients.nColumns() != nAtomTypes) ||
        (ncoeffp && ncoeffp.value().asInteger() != arrayNCoeffP))
    {
        coefficients.initialise(nAtomTypes, nAtomTypes, true);
        for (auto &n : coefficients)
        {
            n.clear();
            if (ncoeffp && ncoeffp.value().asInteger() > 0)
                n.resize(ncoeffp.value().asInteger(), 0);
        }
    }

    return coefficients;
}

// Generate empirical potentials from current coefficients
bool EPSRNode::generateEmpiricalPotentials(const std::vector<const AtomType *> &atomTypes, double averagedRho,
                                           std::optional<Number> ncoeffp, double rminpt, double rmaxpt, double sigma1,
                                           double sigma2)
{
    // Get coefficients array
    Array2D<std::vector<double>> &coefficients = potentialCoefficients(atomTypes.size(), ncoeffp);

    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int i, auto at1, int j, auto at2)
                            {
                                auto &potCoeff = coefficients[{i, j}];

                                // Regenerate empirical potential from the stored coefficients
                                Data1D ep;
                                if (expansionFunction_ == EPSRNode::GaussianExpansionFunction)
                                {
                                    // Construct our fitting object and generate the potential using it
                                    GaussFit generator(ep);
                                    generator.set(rmaxpt, potCoeff, sigma1);
                                    ep = generator.approximation(FunctionSpace::RealSpace, 1.0, 0.0, PairPotential::delta(),
                                                                 PairPotential::range(), sigma2 / sigma1);
                                }
                                else if (expansionFunction_ == EPSRNode::PoissonExpansionFunction)
                                {
                                    // Construct our fitting object and generate the potential using it
                                    // We pass 1.0/rho as the factor to PossonFit::approximation() - this is the factor of rho
                                    // not present in our denominator
                                    PoissonFit generator(ep);
                                    generator.set(FunctionSpace::ReciprocalSpace, rmaxpt, potCoeff, sigma1, sigma2);
                                    ep = generator.approximation(FunctionSpace::RealSpace, 1.0 / averagedRho, 0.0,
                                                                 PairPotential::delta(), PairPotential::range());
                                }

                                // Multiply by truncation function
                                truncate(ep, rminpt, rmaxpt);

                                // Put potentials in vector
                                empiricalPotentials_.emplace_back(at1, at2, ep);

                                if (!additionalPotentials_)
                                    additionalPotentials_.emplace();

                                additionalPotentials_->set({at1->name(), at2->name()}, ep);

                                // Apply potentials?
                                if (applyPotentials_)
                                {
                                    // Grab pointer to the relevant pair potential (if it exists)
                                    auto pp = pairPotential(at1, at2);
                                    pp.setAdditionalPotential(ep);
                                }
                            });

    return true;
}

// Generate and return single empirical potential function
Data1D EPSRNode::generateEmpiricalPotentialFunction(int nAtomTypes, int i, int j, int n)
{
    // EPSR constants
    const auto mcoeff = 200;

    // Calculate some values if they were not provided
    auto rmaxpt = rMaxPT_ ? rMaxPT_.value() : PairPotential::range();
    auto rminpt = rMinPT_ ? rMinPT_.value() : rmaxpt - 2.0;
    nCoeffP_ = nCoeffP_ <= 0 ? std::min(int(10.0 * rmaxpt.asDouble() + 0.0001), mcoeff) : nCoeffP_;

    // Get coefficients array
    auto &coefficients = potentialCoefficients(nAtomTypes);
    auto &potCoeff = coefficients[{i, j}];

    // Regenerate empirical potential from the stored coefficients
    Data1D result;
    if (expansionFunction_ == EPSRNode::GaussianExpansionFunction)
    {
        // Construct our fitting object and generate the potential using it
        GaussFit generator(result);
        generator.set(rmaxpt.asDouble(), potCoeff, gSigma1_.asDouble());
        result = generator.singleFunction(n, FunctionSpace::RealSpace, 1.0, 0.0, PairPotential::delta(), PairPotential::range(),
                                          gSigma2_.asDouble() / gSigma1_.asDouble());
    }
    else if (expansionFunction_ == EPSRNode::PoissonExpansionFunction)
    {
        // Construct our fitting object and generate the potential using it
        PoissonFit generator(result);
        generator.set(FunctionSpace::ReciprocalSpace, rmaxpt.asDouble(), potCoeff, pSigma1_.asDouble(), pSigma2_.asDouble());
        result =
            generator.singleFunction(n, FunctionSpace::RealSpace, 1.0, 0.0, PairPotential::delta(), PairPotential::range());
    }

    // Multiply by truncation function
    truncate(result, rminpt.asDouble(), rmaxpt.asDouble());

    return result;
}

// Calculate absolute energy of empirical potentials
double EPSRNode::absEnergyEP(const std::vector<const AtomType *> &atomTypes)
{
    /*
     * Routine from EPSR25.
     * Loop over sets of empirical potential coefficients and find the maximal/minimal values.
     * Return the largest range we find.
     */

    // Get coefficients array
    auto &coefficients = potentialCoefficients(atomTypes.size());
    if (coefficients.empty())
        return 0.0;

    auto absEnergyEP = 0.0;

    auto unaryOp = [&](auto pair)
    {
        auto [i, j] = pair;
        auto &potCoeff = coefficients[{i, j}];

        auto cMin = potCoeff.empty() ? 0.0 : *std::min_element(potCoeff.begin(), potCoeff.end());
        auto cMax = potCoeff.empty() ? 0.0 : *std::max_element(potCoeff.begin(), potCoeff.end());

        auto range = cMax - cMin;
        if (range > absEnergyEP)
            absEnergyEP = range;

        // Output information
        message("  abs_energy_ep>    {:4} {:4} {:12.6f}\n", atomTypes[i]->name(), atomTypes[j]->name(), range);
    };

    PairIterator pairs(atomTypes.size());
    dissolve::for_each(ParallelPolicies::seq, pairs.begin(), pairs.end(), unaryOp);

    return absEnergyEP;
}

// Truncate the supplied data
void EPSRNode::truncate(Data1D &data, double rMin, double rMax)
{
    // Replicates the EPSR25 truncate(xx,rminpt,rmaxpt) function applied over a whole dataset.
    double x;
    auto &y = data.values();
    const auto decay = rMax - rMin;
    for (auto n = 0; n < data.nValues(); ++n)
    {
        x = data.xAxis(n);

        if (x >= rMax)
            y[n] = 0.0;
        else if (x > rMin)
            y[n] *= 0.5 * (1.0 + cos(((x - rMin) * M_PI) / decay));
    }
}

std::vector<std::tuple<const AtomType *, const AtomType *, Data1D>> EPSRNode::empiricalPotentials()
{
    return empiricalPotentials_;
}

EPSRNamedTargetWeights::EPSRNamedTargetWeights(const EPSRNamedTargetWeights &other) : weights_(other.weights_) {}

// Return reflections vector
std::vector<std::pair<std::string, double>> EPSRNamedTargetWeights::value()
{
    std::vector<std::pair<std::string, double>> weights;
    for (const auto &[key, value] : weights_)
        weights.push_back({key, value});

    return weights;
}

// Express as a serialisable value
void EPSRNamedTargetWeights::serialise(std::string tag, SerialisedValue &target) const
{
    Serialisable::fromMap(weights_, tag, target);
}

// Read values from a serialisable value
void EPSRNamedTargetWeights::deserialise(const SerialisedValue &node)
{
    weights_.clear();
    return Serialisable::toMap(node,
                               [&](const auto &key, const auto &value)
                               {
                                   std::pair<std::string, double> mapping(std::string(key), 1.0);
                                   weights_.insert(mapping);
                               });
}
