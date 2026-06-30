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
 * Data Test Functions
 */

// Test simple double
[[nodiscard]] testing::AssertionResult testDouble(std::string_view quantity, double A, double B, double threshold);
// Test sampled double
[[nodiscard]] testing::AssertionResult testSampledDouble(std::string_view quantity, SampledDouble A, double B,
                                                         double threshold);
// Test Data1D
[[nodiscard]] testing::AssertionResult testData1D(const Data1D &dataA, std::string_view nameA, const Data1D &dataB,
                                                  std::string_view nameB, double tolerance = 5.0e-3,
                                                  Error::ErrorType errorType = Error::ErrorType::EuclideanError);
[[nodiscard]] testing::AssertionResult testData1D(const Data1D &dataA, std::string_view nameA, std::string filePath,
                                                  int xColumn, int yColumn, double tolerance = 5.0e-3,
                                                  Error::ErrorType errorType = Error::ErrorType::EuclideanError);
// Test Data2D
[[nodiscard]] testing::AssertionResult testData2D(const Data2D &dataA, std::string_view nameA, const Data2D &dataB,
                                                  std::string_view nameB, double tolerance = 5.0e-3,
                                                  Error::ErrorType errorType = Error::ErrorType::EuclideanError);
// Test Data3D
[[nodiscard]] testing::AssertionResult testData3D(const Data3D &dataA, std::string_view nameA, const Data3D &dataB,
                                                  std::string_view nameB, double tolerance = 5.0e-3,
                                                  Error::ErrorType errorType = Error::ErrorType::EuclideanError);
// Test Vec3 data
void testVec3(const Vector3 &A, const Vector3 &B, double tolerance = 1.0e-6);
// Test Vec3 vector data
void testVec3Vector(const std::vector<Vector3> &A, const std::vector<Vector3> &B, double tolerance = 1.0e-6);
// Test species atom type
void testSpeciesAtomType(Species *sp, const std::map<int, std::string> &namesById);
// Test interaction parameters
template <class Intra>
void checkIntramolecularTerms(const std::string &termInfo, const InteractionPotential<Intra> &expectedParams,
                              const InteractionPotential<Intra> &actualParams, double tolerance = 1.0e-6);
// Test species bond term
void testSpeciesIntramolecular(Species *sp, std::vector<int> atoms, const InteractionPotential<BondFunctions> &expectedParams,
                               double tolerance = 1.0e-6);
// Test species angle term
void testSpeciesIntramolecular(Species *sp, std::vector<int> atoms, const InteractionPotential<AngleFunctions> &expectedParams,
                               double tolerance = 1.0e-6);
// Test species torsion / improper term
void testSpeciesIntramolecular(Species *sp, std::vector<int> atoms,
                               const InteractionPotential<TorsionFunctions> &expectedParams, double tolerance = 1.0e-6);
// Test consistency between the two supplied double-keyed Data1D maps
static bool testDoubleKeyedMap(std::string_view mapContents, const DoubleKeyedMap<Data1D> &mapA,
                               const DoubleKeyedMap<Data1D> &mapB, double testThreshold);
// Test consistency, and error, between supplied partial sets
bool testPartialSet(const PartialSet &setA, const PartialSet &setB, double testThreshold);
// Check consistency between production, molecular, and test energies, returning production values
Kernel::EnergyResult testEnergyConsistency(const std::unique_ptr<EnergyKernel> &kernel, double testThreshold = 1.0e-6);
// Check consistency between production and test forces
void testForceConsistency(const std::unique_ptr<ForceKernel> &kernel, std::vector<Vector3> &ppForces,
                          std::vector<Vector3> &geomForces, Flags<Kernel::CalculationFlags> flags = {},
                          double ppMaxDeviation = 1.0e-2, double geomMaxDeviation = 1.0e-6);
// Check consistency of supplied forces
void testReferenceForceConsistency(const std::vector<Vector3> &ppForces, const std::vector<Vector3> &geomForces,
                                   const std::vector<Vector3> &referenceForces, double maxDeviation = 1.0e-3);

/*
 * TOML
 */

// Compare TOML values with context, but without insisting on a specific ordering of fields
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
