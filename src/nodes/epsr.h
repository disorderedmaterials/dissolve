// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "classes/scatteringMatrix.h"
#include "math/data1D.h"
#include "math/range.h"
#include "nodes/node.h"
#include "nodes/number.h"
#include "templates/doubleKeyedMap.h"
#include <optional>
#include <tuple>

// Forward Declarations
class AtomType;
class PartialSet;

class EPSRNamedTargetWeights : public Serialisable
{
    public:
    EPSRNamedTargetWeights() = default;
    EPSRNamedTargetWeights(const EPSRNamedTargetWeights &other);
    ~EPSRNamedTargetWeights() = default;

    private:
    // Target weights
    std::map<std::string, double> weights_;

    public:
    // Return named weights vector
    std::vector<std::pair<std::string, double>> value();

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};

// EPSR Node
class EPSRNode : public Node
{
    public:
    enum CorrelationNodeType
    {
        NeutronSQ,
        XRaySQ,
        SQ
    };
    using TargetWeights = std::vector<std::pair<Node *, double>>;
    using TargetCorrelationDataMap = std::map<EPSRNode::CorrelationNodeType, TargetWeights>;

    private:
    struct TargetProcessData
    {
        TargetProcessData() = default;
        Data1D errors, differenceData, deltaFQ, deltaFQFit, simulatedFR, totalRFactor;
        std::vector<double> fitCoefficients;
    };
    using TargetProcessDataMap = std::map<Node *, TargetProcessData>;

    public:
    EPSRNode(Graph *parentGraph);
    ~EPSRNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    public:
    enum ExpansionFunctionType
    {
        PoissonExpansionFunction,  /* Fit difference functions using Poisson (power exponential) functions */
        GaussianExpansionFunction, /* Fit difference functions using Gaussian functions */
        nExpansionFunctionTypes
    };
    // Return enum option info for ExpansionFunctionType
    static EnumOptions<ExpansionFunctionType> expansionFunctionTypes();

    private:
    // Target configurations
    Configuration *targetConfiguration_{nullptr};
    // Limit of magnitude of additional potential for any one pair potential
    Number eReq_{3.0};
    // Expansion function type to use for potential fits
    EPSRNode::ExpansionFunctionType expansionFunction_{EPSRNode::PoissonExpansionFunction};
    // Confidence factor
    Number feedback_{0.9};
    // Scattering matrix
    std::optional<ScatteringMatrix> scatteringMatrix_;
    // Maximum Q value over which to generate potentials from total scattering data
    Number qMax_{30.0};
    // Minimum Q value over which to generate potentials from total scattering data
    Number qMin_{0.5};
    // Factor used when adding fluctuation coefficients to pair potentials
    Number weighting_{1.0};
    // Width for Gaussian function in reciprocal space
    Number gSigma1_{0.1};
    // Width for Gaussian function in real space
    Number gSigma2_{0.2};
    // Vector storing atom pairs and associated potentials
    std::vector<std::tuple<const AtomType *, const AtomType *, Data1D>> empiricalPotentials_;
    // Frequency at which to apply generated perturbations to interatomic potentials
    std::optional<Number> modifyPotential_{1};
    // Whether to apply this node's generated potentials to the global pair potentials
    bool applyPotentials_{true};
    // Number of coefficients used to define the empirical potential
    std::optional<Number> nCoeffP_;
    // Number of steps for refining the potential
    std::optional<Number> nPItSs_{1000};
    // Overwrite potentials each time rather than summing them
    bool overwritePotentials_{false};
    // Width for Poisson functions in reciprocal space (N.B. this is psigma2 in EPSR)
    Number pSigma1_{0.01};
    // Width for Poisson functions in real space
    Number pSigma2_{0.01};
    // Radius at which potential truncation goes to zero (-1.0 to use pair potential maximum range)
    std::optional<Number> rMaxPT_;
    // Radius at which potential truncation begins (-1.0 to set to 2.0 Angstroms under rmaxpt)
    std::optional<Number> rMinPT_;
    // Degree of smoothing to apply to fluctuation coefficients before summation into potential
    std::optional<Number> fluctuationSmoothing_;
    // Target Modules containing data to refine against
    std::vector<std::string> targetNames_;
    // Weightings for targets (if not 1.0)
    EPSRNamedTargetWeights namedWeights_;
    // Total r-factor
    std::optional<Data1D> totalRFactor_;
    // Empirical potential coefficients
    std::optional<Array2D<std::vector<double>>> potentialCoefficients_;
    // S(Q) from completed scattering matrix
    std::optional<DoubleKeyedMap<Data1D>> estimatedSQ_;
    // Estimated g(r) from estimated S(Q)
    std::optional<DoubleKeyedMap<Data1D>> estimatedGR_;
    // Additonal potentials stored for reference
    std::optional<DoubleKeyedMap<Data1D>> additionalPotentials_;
    // Phi magnitude array
    std::optional<Data1D> phiArray_;
    // Container for process data for individual target nodes
    TargetProcessDataMap targetProcessData_;
    // Ranges to calculate rFactor over
    std::vector<Range> ranges_;
    // Run count
    std::optional<int> runCount_;

    public:
    // Return target process data for a given node
    const TargetProcessData &targetProcessData(Node *target);
    // Return scattering matrix
    const ScatteringMatrix &scatteringMatrix() const;
    // Return additional potential for a given atom type pair
    const Data1D &additionalPotential(std::string i, std::string j) const;
    // Return estimated S(Q) for a given atom type pair
    const Data1D &estimatedSQ(std::string i, std::string j) const;

    /*
     * Processing
     */
    private:
    // Returns the target correlation data
    TargetCorrelationDataMap findTargetCorrelationData();
    // Returns the target correlation data
    static inline void appendTarget(TargetCorrelationDataMap &data, Node *node, EPSRNode::CorrelationNodeType nodeType,
                                    std::vector<std::pair<std::string, double>> inputWeights);

    public:
    // Set whether to apply this node's generated potentials to the global pair potentials
    void setApplyPotentials(bool b);

    private:
    // Create / update delta S(Q) information
    void updateDeltaSQ(OptionalReferenceWrapper<const PartialSet> unweightedSQ = std::nullopt,
                       OptionalReferenceWrapper<const DoubleKeyedMap<Data1D>> optCalculatedSQ = std::nullopt,
                       OptionalReferenceWrapper<const DoubleKeyedMap<Data1D>> optEstimatedSQ = std::nullopt);
    // Create / retrieve arrays for storage of empirical potential coefficients
    Array2D<std::vector<double>> &potentialCoefficients(const int nAtomTypes, std::optional<Number> ncoeffp = std::nullopt);
    // Generate empirical potentials from current coefficients
    bool generateEmpiricalPotentials(const std::vector<const AtomType *> &atomTypes, double rho, std::optional<Number> ncoeffp,
                                     double rminpt, double rmaxpt, double sigma1, double sigma2);
    // Generate and return single empirical potential function
    Data1D generateEmpiricalPotentialFunction(int nAtomTypes, int i, int j, int n);
    // Calculate absolute energy of empirical potentials
    double absEnergyEP(const std::vector<const AtomType *> &atomTypes);
    // Truncate the supplied data
    void truncate(Data1D &data, double rMin, double rMax);
    // Return vector of empirical potentials
    std::vector<std::tuple<const AtomType *, const AtomType *, Data1D>> empiricalPotentials();

    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};

EnumOptions<EPSRNode::ExpansionFunctionType> getEnumOptions(EPSRNode::ExpansionFunctionType);