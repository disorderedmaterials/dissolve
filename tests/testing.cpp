// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "tests/testing.h"
#include "classes/species.h"
#include "kernels/energy.h"
#include "kernels/force.h"
#include "nodes/importXYData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
/*
 * Data Test Functions
 */

// Test simple double
[[nodiscard]] testing::AssertionResult testDouble(std::string_view quantity, double A, double B, double threshold)
{
    auto delta = fabs(A - B);
    if (delta <= threshold)
        return testing::AssertionSuccess();

    return testing::AssertionFailure() << std::format("{} values {} and {} differ by {} which exceeds the threshold of {}",
                                                      quantity, A, B, delta, threshold);
}
// Test sampled double
[[nodiscard]] testing::AssertionResult testSampledDouble(std::string_view quantity, SampledDouble A, double B, double threshold)
{
    return testDouble(quantity, A.value(), B, threshold);
}
// Test Data1D
[[nodiscard]] testing::AssertionResult testData1D(const Data1D &dataA, std::string_view nameA, const Data1D &dataB,
                                                  std::string_view nameB, double threshold, Error::ErrorType errorType)
{
    // Generate the error estimate and compare against the threshold value
    auto error = Error::error(errorType, dataA, dataB).error;
    if (std::isnan(error) || error > threshold)
        return testing::AssertionFailure() << std::format(
                   "Data '{}' has error of {} with data '{}' h exceeds the threshold of {}\n", nameA, error, nameB, threshold);

    return testing::AssertionSuccess();
}
[[nodiscard]] testing::AssertionResult testData1D(const Data1D &dataA, std::string_view nameA, std::string filePath,
                                                  int xColumn, int yColumn, double threshold, Error::ErrorType errorType)
{
    Data1D dataB;
    if (!ImportXYDataNode::read(dataB, filePath, xColumn, yColumn))
    {
        testing::AssertionFailure() << std::format("Failed to read data from '{}'\n", filePath);
    }

    return testData1D(dataA, nameA, dataB, filePath, threshold, errorType);
}
// Test Data2D
[[nodiscard]] testing::AssertionResult testData2D(const Data2D &dataA, std::string_view nameA, const Data2D &dataB,
                                                  std::string_view nameB, double threshold, Error::ErrorType errorType)
{
    // Generate the error estimate and compare against the threshold value
    auto error = Error::error(errorType, dataA.values().linearArray(), dataB.values().linearArray()).error;
    if (std::isnan(error) || error > threshold)
        return testing::AssertionFailure() << std::format(
                   "Data '{}' has error of {} with data '{}' which exceeds the threshold of {}", nameA, error, nameB,
                   threshold);

    return testing::AssertionSuccess();
}
// Test Data3D
[[nodiscard]] testing::AssertionResult testData3D(const Data3D &dataA, std::string_view nameA, const Data3D &dataB,
                                                  std::string_view nameB, double threshold, Error::ErrorType errorType)
{
    // Generate the error estimate and compare against the threshold value
    auto error = Error::error(errorType, dataA.values().linearArray(), dataB.values().linearArray()).error;
    if (std::isnan(error) || error > threshold)
        return testing::AssertionFailure() << std::format(
                   "Internal data '{}' has error of {} with external data '{}' which exceeds the threshold of {}", nameA, error,
                   nameB, threshold);

    return testing::AssertionSuccess();
}
// Test Vec3 data
[[nodiscard]] testing::AssertionResult testVec3(std::string_view quantity, const Vector3 &A, const Vector3 &B, double tolerance)
{
    auto C = A - B;
    if (fabs(C.x) > tolerance || fabs(C.x) > tolerance || fabs(C.x) > tolerance)
        return testing::AssertionFailure() << std::format(
                   "{} differs by ({},{},{}) which exceeds the per-component threshold of {}", quantity, C.x, C.y, C.z,
                   tolerance);

    return testing::AssertionSuccess();
}
// Test species atom type
void testSpeciesAtomType(Species *sp, const std::map<int, std::string> &namesById)
{
    for (auto &[atomIndex, atomTypeName] : namesById)
    {
        ASSERT_TRUE(atomIndex >= 0 && atomIndex < sp->nAtoms());
        auto &spAtom = sp->atom(atomIndex);
        auto at = spAtom.atomType();
        ASSERT_TRUE(at);
        EXPECT_EQ(at->name(), atomTypeName);
    }
}
// Test interaction parameters
template <class Intra>
void checkIntramolecularTerms(const std::string &termInfo, const InteractionPotential<Intra> &expectedParams,
                              const InteractionPotential<Intra> &actualParams, double tolerance)
{
    Messenger::print("Testing intramolecular interaction: {}...\n", termInfo);
    EXPECT_EQ(Intra::forms().keyword(actualParams.form()), Intra::forms().keyword(expectedParams.form()));
    EXPECT_EQ(actualParams.nParameters(), expectedParams.nParameters());
    for (auto &&[current, expected] : zip(actualParams.parameters(), expectedParams.parameters()))
        EXPECT_NEAR(current, expected, tolerance);
}
// Test species bond term
void testSpeciesIntramolecular(Species *sp, std::vector<int> atoms, const InteractionPotential<BondFunctions> &expectedParams,
                               double tolerance)
{
    ASSERT_TRUE(atoms.size() == 2);
    const auto &b = sp->getBond(&sp->atoms()[atoms[0]], &sp->atoms()[atoms[1]]);
    if (!b)
        throw(std::runtime_error(std::format("No bond {} exists in species '{}'.\n", joinStrings(atoms, "-"), sp->name())));
    checkIntramolecularTerms(std::format("bond {}", joinStrings(atoms, "-")), expectedParams, b->get().interactionPotential(),
                             tolerance);
}
// Test species angle term
void testSpeciesIntramolecular(Species *sp, std::vector<int> atoms, const InteractionPotential<AngleFunctions> &expectedParams,
                               double tolerance)
{
    ASSERT_TRUE(atoms.size() == 3);
    const auto &a = sp->getAngle(&sp->atoms()[atoms[0]], &sp->atoms()[atoms[1]], &sp->atoms()[atoms[2]]);
    if (!a)
        throw(std::runtime_error(std::format("No angle {} exists in species '{}'.\n", joinStrings(atoms, "-"), sp->name())));
    checkIntramolecularTerms(std::format("angle {}", joinStrings(atoms, "-")), expectedParams, a->get().interactionPotential(),
                             tolerance);
}
// Test species torsion / improper term
void testSpeciesIntramolecular(Species *sp, std::vector<int> atoms,
                               const InteractionPotential<TorsionFunctions> &expectedParams, double tolerance)
{
    ASSERT_TRUE(atoms.size() == 4);
    const auto &t =
        sp->getTorsion(&sp->atoms()[atoms[0]], &sp->atoms()[atoms[1]], &sp->atoms()[atoms[2]], &sp->atoms()[atoms[3]]);
    const auto &i =
        sp->getImproper(&sp->atoms()[atoms[0]], &sp->atoms()[atoms[1]], &sp->atoms()[atoms[2]], &sp->atoms()[atoms[3]]);
    if (!t && !i)
        throw(std::runtime_error(
            std::format("No torsion or improper {} exists in species '{}'.\n", joinStrings(atoms, "-"), sp->name())));
    else if (t)
        checkIntramolecularTerms(std::format("torsion {}", joinStrings(atoms, "-")), expectedParams,
                                 t->get().interactionPotential(), tolerance);
    else
        checkIntramolecularTerms(std::format("improper {}", joinStrings(atoms, "-")), expectedParams,
                                 i->get().interactionPotential(), tolerance);
}
// Test consistency between the two supplied double-keyed Data1D maps
bool testDoubleKeyedMap(std::string_view mapContents, const DoubleKeyedMap<Data1D> &mapA, const DoubleKeyedMap<Data1D> &mapB,
                        double testThreshold)
{
    // Check map sizes
    if (mapA.size() != mapB.size())
    {
        std::cout << std::format("Maps containing {} data are of dissimilar size (A = {}, B = {})\n", mapContents, mapA.size(),
                                 mapB.size());
        return false;
    }

    // Check individual data
    for (auto &[key, dataA] : mapA)
    {
        // Find same-keyed data in mapB
        if (mapB.contains(key))
        {
            auto errorReport = Error::percent(dataA, mapB.get(key));
            std::cout << Error::errorReportString(errorReport) << std::endl;
            std::cout << std::format("{} '{}' in map B has {} error of {:7.3f}{} with data in map A and is "
                                     "{} (threshold is {:6.3f}%)\n\n",
                                     mapContents, key, Error::errorTypes().keyword(errorReport.errorType), errorReport.error,
                                     errorReport.errorType == Error::ErrorType::PercentError ? "%" : "",
                                     errorReport.error <= testThreshold ? "OK" : "NOT OK", testThreshold);
            if (errorReport.error > testThreshold)
                return false;
        }
        else
        {
            std::cout << std::format("{} '{}' is present in map A but not in map B.\n", mapContents, key);
            return false;
        }
    }

    return true;
}
// Test consistency, and error, between supplied partial sets
bool testPartialSet(const PartialSet &setA, const PartialSet &setB, double testThreshold)
{
    // Full partials
    if (!testDoubleKeyedMap("Full Partials", setA.partials(), setB.partials(), testThreshold))
        return false;

    // Bound partials
    if (!testDoubleKeyedMap("Bound Partials", setA.boundPartials(), setB.boundPartials(), testThreshold))
        return false;

    // Unbound partials
    if (!testDoubleKeyedMap("Unbound Partials", setA.unboundPartials(), setB.unboundPartials(), testThreshold))
        return false;

    // Total
    auto errorReport = Error::percent(setA.total(), setB.total());
    std::cout << Error::errorReportString(errorReport) << std::endl;
    std::cout << std::format(
        "Total in set B has {} error of {:7.3f}{} with data in set A and is {} (threshold is {:6.3f}%)\n\n",
        Error::errorTypes().keyword(errorReport.errorType), errorReport.error,
        errorReport.errorType == Error::ErrorType::PercentError ? "%" : "",
        errorReport.error <= testThreshold ? "OK" : "NOT OK", testThreshold);
    if (errorReport.error > testThreshold)
        return false;

    return true;
}

// Check consistency between production, molecular, and test energies, returning production values
Kernel::EnergyResult testEnergyConsistency(const std::unique_ptr<EnergyKernel> &kernel, double testThreshold)
{
    // Calculate production energies (fully optimised)
    auto productionEnergy = kernel->totalEnergy();

    // Calculate baseline test energies (simple double-loop, PBC always)
    auto testEnergy = kernel->totalEnergySimple();

    // Calculate molecule-centric energy
    auto molecularPPEnergy = kernel->totalMoleculePairPotentialEnergy();

    // Compare basic energies with production value
    EXPECT_NEAR(testEnergy.pairPotential.interMolecular, productionEnergy.pairPotential.interMolecular, testThreshold);
    EXPECT_NEAR(testEnergy.pairPotential.intraMolecular, productionEnergy.pairPotential.intraMolecular, testThreshold);
    EXPECT_NEAR(testEnergy.geometry.total(), productionEnergy.geometry.total(), testThreshold);

    // Compare basic energies with molecule-based values
    EXPECT_NEAR(testEnergy.pairPotential.total(), molecularPPEnergy.total(), testThreshold);
    EXPECT_NEAR(testEnergy.pairPotential.interMolecular, molecularPPEnergy.interMolecular, testThreshold);

    // Compare molecule-based energies with production values
    EXPECT_NEAR(molecularPPEnergy.total(), productionEnergy.pairPotential.total(), testThreshold);
    EXPECT_NEAR(molecularPPEnergy.interMolecular, productionEnergy.pairPotential.interMolecular, testThreshold);

    return productionEnergy;
}

// Check consistency between production and test forces
void testForceConsistency(const std::unique_ptr<ForceKernel> &kernel, std::vector<Vector3> &ppForces,
                          std::vector<Vector3> &geomForces, Flags<Kernel::CalculationFlags> flags, double ppMaxDeviation,
                          double geomMaxDeviation)
{
    // Calculate production forces (fully optimised)
    kernel->totalForces(ppForces, geomForces, flags);

    // Calculate baseline test forces (simple double-loop, PBC always)
    std::vector<Vector3> ppTestForces, geomTestForces;
    kernel->totalForcesSimple(ppTestForces, geomTestForces, flags);

    // Pair potential forces
    if (!(flags.isSet(Kernel::CalculationFlags::ExcludeInterMolecularPairPotential) &&
          flags.isSet(Kernel::CalculationFlags::ExcludeIntraMolecularPairPotential)))
        for (auto &&[pairPotentialTestForce, pairPotentialProductionForce] : zip(ppTestForces, ppForces))
        {
            EXPECT_NEAR(pairPotentialProductionForce.x, pairPotentialTestForce.x, ppMaxDeviation);
            EXPECT_NEAR(pairPotentialProductionForce.y, pairPotentialTestForce.y, ppMaxDeviation);
            EXPECT_NEAR(pairPotentialProductionForce.z, pairPotentialTestForce.z, ppMaxDeviation);
        }

    // Geometric forces
    if (flags.isNotSet(Kernel::CalculationFlags::ExcludeGeometric))
        for (auto &&[geometryTestForce, geometryProductionForce] : zip(geomTestForces, geomForces))
        {
            EXPECT_NEAR(geometryProductionForce.x, geometryTestForce.x, geomMaxDeviation);
            EXPECT_NEAR(geometryProductionForce.y, geometryTestForce.y, geomMaxDeviation);
            EXPECT_NEAR(geometryProductionForce.z, geometryTestForce.z, geomMaxDeviation);
        }
}

// Check consistency of supplied forces
void testReferenceForceConsistency(const std::vector<Vector3> &ppForces, const std::vector<Vector3> &geomForces,
                                   const std::vector<Vector3> &referenceForces, double maxDeviation)
{
    ASSERT_TRUE(ppForces.size() == geomForces.size());
    ASSERT_TRUE(ppForces.size() == referenceForces.size());

    for (auto &&[ppForce, geometryForce, referenceForce] : zip(ppForces, geomForces, referenceForces))
    {
        auto calculatedForce = ppForce + geometryForce;
        EXPECT_NEAR(calculatedForce.x, referenceForce.x, maxDeviation);
        EXPECT_NEAR(calculatedForce.y, referenceForce.y, maxDeviation);
        EXPECT_NEAR(calculatedForce.z, referenceForce.z, maxDeviation);
    }
}

/*
 * TOML
 */

// Compare TOML values with context, but without insisting on a specific ordering of fields
void compareToml(std::string location, SerialisedValue toml, SerialisedValue toml2)
{
    if (toml.is_table())
    {
        ASSERT_TRUE(toml2.is_table()) << location;
        for (auto &[k, v] : toml.as_table())
        {
            ASSERT_TRUE(toml2.contains(k)) << location << "." << k << std::endl << "Expected:" << std::endl << toml[k];
            compareToml(std::format("{}.{}", location, k), v, toml2.at(k));
        }
    }
    else if (toml.is_array())
    {
        auto arr = toml.as_array();
        auto arr2 = toml2.as_array();
        ASSERT_EQ(arr.size(), arr2.size()) << location << std::endl << "Expected" << std::endl << toml;
        for (int i = 0; i < arr.size(); ++i)
            compareToml(std::format("{}[{}]", location, i), arr[i], arr2[i]);
    }
    else
    {
        EXPECT_EQ(toml, toml2) << location;
    }
}

/*
 * Utility Functions
 */

// Save the specified graph in Mermaid format to a file named after the unit test
void exportMermaidGraph(Graph &graph)
{
    auto suite = ::testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
    auto name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    std::string filename = std::format("{}-{}.mermaid", suite, name);

    std::ofstream myfile;
    myfile.open(filename);
    myfile << graph;
    myfile.close();
}
}; // namespace UnitTest
