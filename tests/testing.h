// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "classes/partialSet.h"
#include "kernels/common.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/error.h"
#include "nodes/dissolve.h"
#include "nodes/insert.h"
#include <gtest/gtest.h>

// Forward Declarations
class GRNode;
class IterableGraph;
class SpeciesNode;
class NeutronSQNode;
class SQNode;
class XRaySQNode;

namespace UnitTest
{
/*
 * Utility Functions
 */

// Save the specified graph in Mermaid format to a file named after the unit test
void exportMermaidGraph(Graph &graph);

/*
 * Test Graph Framework
 */

// Basic object setup for any Graph-based test
class TestGraph : public DissolveGraph
{
    public:
    TestGraph() : currentGraph_(this)
    {
        setEcho(true);
        PairPotential::setChargeSource(PairPotential::ChargeSource::Automatic);
    }
    ~TestGraph() { exportMermaidGraph(*this); }

    public:
    // Container for data 1D import filename and whether or not it is a histogram
    struct Data1DImportFileFormat
    {
        std::string filename;
        bool histogram;

        Data1DImportFileFormat(std::string filename = "", bool histogram = false) : filename(filename), histogram(histogram) {}
    };

    private:
    // Current graph target
    Graph *currentGraph_{nullptr};
    // Most recently appended node in the current graph
    Node *head_{nullptr};

    /*
     * Graph Creation Helpers
     */
    public:
    // Returns pointer to current top node in graph
    Node *fetchHead() const;
    // Returns the name of the current head node in the graph
    std::string fetchHeadName() const;
    // Returns reference to current top node in graph, cast to the known node type
    template <class NodeType> NodeType *head() const { return static_cast<NodeType *>(head_); }
    // Append new node to the graph
    Node *appendNode(const std::string &nodeType, const std::optional<std::string> &name = {});
    // Create species insertion node chain
    Node *createAndInsertSpecies(Node *cfgSourceNode, std::string cfgSourceOutput,
                                 const std::vector<std::pair<std::string, int>> &species, double rho,
                                 Units::DensityUnits rhoUnits = Units::DensityUnits::AtomsPerAngstromUnits,
                                 InsertNode::BoxActionStyle boxActionStyle = InsertNode::BoxActionStyle::AddVolume);

    public:
    // Create and return atomic SpeciesNode
    static std::unique_ptr<SpeciesNode> createAtomicSpecies(Elements::Element element,
                                                            InteractionPotential<ShortRangeFunctions> potential = {
                                                                ShortRangeFunctions::Form::Undefined, ""});
    // Create species from TOML file
    static std::unique_ptr<SpeciesNode> loadTOMLSpecies(std::string_view path);
    // Create a species node with structure and forcefield data sources
    SpeciesNode *createSpeciesFromStructureAndForcefield(std::string name, std::string structureNodeType,
                                                         std::string structureFilePath, std::shared_ptr<Forcefield> ff,
                                                         bool calculateBonding = true);
    // Create basic configuration graph, returning the last node
    Node *createConfiguration(std::string name, const std::vector<std::pair<std::string, int>> &species, double rho,
                              Units::DensityUnits rhoUnits = Units::DensityUnits::AtomsPerAngstromUnits);
    // Create basic configuration graph, returning the last node
    Node *createConfiguration(std::string name, const std::vector<std::pair<std::string, int>> &species,
                              const Vector3 &cellLengths, const Vector3 &cellAngles = {90.0, 90.0, 90.0});
    // Append a set coordinates node with a structure import input
    Node *appendSetCoordinates(std::string_view importNodeType, std::string filePath,
                               std::string sourceOutpuName = "Configuration");
    // Create a trajectory iterator subgraph
    IterableGraph *appendTrajectoryIterator(std::string trajectoryImportNodeType, std::string filePath);
    // Append GR and SQ nodes
    std::pair<GRNode *, SQNode *> appendGRSQ(bool noAveraging = false, bool noIntraBroadening = false);
    // Create a NeutronSQ node with optional reference data
    NeutronSQNode *appendNeutronSQ(SQNode *sqNode, std::string name,
                                   const std::vector<std::tuple<std::string, std::string, double>> isotopologues = {},
                                   TestGraph::Data1DImportFileFormat referenceData = TestGraph::Data1DImportFileFormat{});
    // Create an XRaySQ node with optional reference data
    XRaySQNode *appendXRaySQ(SQNode *sqNode, std::string name,
                             TestGraph::Data1DImportFileFormat referenceData = TestGraph::Data1DImportFileFormat{});
};

/*
 * Data Test Functions
 */

// Test simple double
[[nodiscard]] bool checkDouble(std::string_view quantity, double A, double B, double threshold);
// Test sampled double
[[nodiscard]] bool checkSampledDouble(std::string_view quantity, SampledDouble A, double B, double threshold);
// Test Data1D
[[nodiscard]] bool checkData1D(const Data1D &dataA, std::string_view nameA, const Data1D &dataB, std::string_view nameB,
                               double tolerance = 5.0e-3, Error::ErrorType errorType = Error::ErrorType::EuclideanError);
[[nodiscard]] bool checkData1D(const Data1D &dataA, std::string_view nameA, std::string filePath, int xColumn, int yColumn,
                               double tolerance = 5.0e-3, Error::ErrorType errorType = Error::ErrorType::EuclideanError);
// Test Data2D
[[nodiscard]] bool checkData2D(const Data2D &dataA, std::string_view nameA, const Data2D &dataB, std::string_view nameB,
                               double tolerance = 5.0e-3, Error::ErrorType errorType = Error::ErrorType::EuclideanError);
// Test Data3D
[[nodiscard]] bool checkData3D(const Data3D &dataA, std::string_view nameA, const Data3D &dataB, std::string_view nameB,
                               double tolerance = 5.0e-3, Error::ErrorType errorType = Error::ErrorType::EuclideanError);
// Test Vec3 data
void checkVec3(const Vector3 &A, const Vector3 &B, double tolerance = 1.0e-6);
// Test Vec3 vector data
void checkVec3Vector(const std::vector<Vector3> &A, const std::vector<Vector3> &B, double tolerance = 1.0e-6);
// Test species atom type
void checkSpeciesAtomType(Species *sp, const std::map<int, std::string> &namesById);
// Test interaction parameters
template <class Intra>
void checkIntramolecularTerms(const std::string &termInfo, const InteractionPotential<Intra> &expectedParams,
                              const InteractionPotential<Intra> &actualParams, double tolerance = 1.0e-6);
// Test species bond term
void checkSpeciesIntramolecular(Species *sp, std::vector<int> atoms, const InteractionPotential<BondFunctions> &expectedParams,
                                double tolerance = 1.0e-6);
// Test species angle term
void checkSpeciesIntramolecular(Species *sp, std::vector<int> atoms, const InteractionPotential<AngleFunctions> &expectedParams,
                                double tolerance = 1.0e-6);
// Test species torsion / improper term
void checkSpeciesIntramolecular(Species *sp, std::vector<int> atoms,
                                const InteractionPotential<TorsionFunctions> &expectedParams, double tolerance = 1.0e-6);
// Test consistency between the two supplied double-keyed Data1D maps
static bool checkDoubleKeyedMap(std::string_view mapContents, const DoubleKeyedMap<Data1D> &mapA,
                                const DoubleKeyedMap<Data1D> &mapB, double testThreshold);
// Test consistency, and error, between supplied partial sets
bool checkPartialSet(const PartialSet &setA, const PartialSet &setB, double testThreshold);
// Check consistency between production, molecular, and test energies, returning production values
Kernel::EnergyResult checkEnergyConsistency(const std::unique_ptr<EnergyKernel> &kernel, double testThreshold = 1.0e-6);
// Check consistency between production and test forces
void checkForceConsistency(const std::unique_ptr<ForceKernel> &kernel, std::vector<Vector3> &ppForces,
                           std::vector<Vector3> &geomForces, Flags<Kernel::CalculationFlags> flags = {},
                           double ppMaxDeviation = 1.0e-2, double geomMaxDeviation = 1.0e-6);
// Check consistency of supplied forces
void checkReferenceForceConsistency(const std::vector<Vector3> &ppForces, const std::vector<Vector3> &geomForces,
                                    const std::vector<Vector3> &referenceForces, double maxDeviation = 1.0e-3);

/*
 * TOML
 */

// Comparie TOML values with context, but without insisting on a specific ordering of fields
void compareToml(std::string location, SerialisedValue toml, SerialisedValue toml2);
// Perform round-trip serialisation of A into B
template <class T> void tomlRoundTrip(T &a, T &b)
{
    SerialisedValue serialised;
    auto s = std::make_shared<SerialisableClass<T>>("data", a);
    ASSERT_NO_THROW(serialised = s->serialise());

    auto d = std::make_shared<SerialisableClass<T>>("data", b);
    ASSERT_NO_THROW(d->deserialise(serialised));
}
}; // namespace UnitTest
