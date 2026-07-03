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
[[nodiscard]] testing::AssertionResult testSpeciesAtomType(Species *sp, const std::map<int, std::string> &namesById)
{
    for (auto &[atomIndex, atomTypeName] : namesById)
    {
        if (atomIndex < 0 || atomIndex >= sp->nAtoms())
            return testing::AssertionFailure()
                   << std::format("Atom index out of range - {} in a species of {} atoms", atomIndex, sp->nAtoms());

        auto &spAtom = sp->atom(atomIndex);
        auto at = spAtom.atomType();
        if (!at)
            return testing::AssertionFailure()
                   << std::format("Atom index out of range - {} in a species of {} atoms", atomIndex, sp->nAtoms());

        if (at->name() != atomTypeName)
            return testing::AssertionFailure() << std::format("Atom type name ({}) for index {} does not match expected ({})",
                                                              at->name(), atomIndex, atomTypeName);
    }

    return testing::AssertionSuccess();
}
// Test interaction parameters
template <class Intra>
[[nodiscard]] testing::AssertionResult
checkIntramolecularTerms(const std::string &term, const InteractionPotential<Intra> &expectedParams,
                         const InteractionPotential<Intra> &actualParams, double tolerance)
{
    if (Intra::forms().keyword(actualParams.form()) != Intra::forms().keyword(expectedParams.form()))
        return testing::AssertionFailure() << std::format("{} has wrong form - {} vs the expected {}", term,
                                                          Intra::forms().keyword(actualParams.form()),
                                                          Intra::forms().keyword(expectedParams.form()));
    if (actualParams.nParameters() != expectedParams.nParameters())
        return testing::AssertionFailure() << std::format("{} has wrong parameter count - {} vs the expected {}", term,
                                                          actualParams.nParameters(), expectedParams.nParameters());
    for (auto &&[current, expected] : zip(actualParams.parameters(), expectedParams.parameters()))
        if (fabs(current - expected) > tolerance)
            return testing::AssertionFailure()
                   << std::format("{} has wrong parameter value - {} vs the expected {}", term, current, expected);

    return testing::AssertionSuccess();
}
// Test species bond term
[[nodiscard]] testing::AssertionResult testSpeciesBond(OptionalReferenceWrapper<const SpeciesBond> optBond,
                                                       const InteractionPotential<BondFunctions> &expectedParams,
                                                       double tolerance)
{
    if (!optBond)
        return testing::AssertionFailure() << "No bond provided to test";
    auto &b = (*optBond).get();
    return checkIntramolecularTerms(std::format("Bond {} in species '{}'",
                                                joinStrings(b.atoms(), "-", [](const auto &atom) { return atom->index(); }),
                                                b.parent()->name()),
                                    expectedParams, b.interactionPotential(), tolerance);
}
// Test species angle term
[[nodiscard]] testing::AssertionResult testSpeciesAngle(OptionalReferenceWrapper<const SpeciesAngle> optAngle,
                                                        const InteractionPotential<AngleFunctions> &expectedParams,
                                                        double tolerance)
{
    if (!optAngle)
        return testing::AssertionFailure() << "No angle provided to test";
    auto &a = (*optAngle).get();
    printf("SP = %p\n", a.parent());
    return checkIntramolecularTerms(std::format("Angle {} in species '{}'",
                                                joinStrings(a.atoms(), "-", [](const auto &atom) { return atom->index(); }),
                                                a.parent()->name()),
                                    expectedParams, a.interactionPotential(), tolerance);
}
// Test species torsion term
[[nodiscard]] testing::AssertionResult testSpeciesTorsion(OptionalReferenceWrapper<const SpeciesTorsion> optTorsion,
                                                          const InteractionPotential<TorsionFunctions> &expectedParams,
                                                          double tolerance)
{
    if (!optTorsion)
        return testing::AssertionFailure() << "No torsion provided to test";
    auto &t = (*optTorsion).get();
    return checkIntramolecularTerms(std::format("Torsion {} in species '{}'",
                                                joinStrings(t.atoms(), "-", [](const auto &atom) { return atom->index(); }),
                                                t.parent()->name()),
                                    expectedParams, t.interactionPotential(), tolerance);
}
// Test species improper term
[[nodiscard]] testing::AssertionResult testSpeciesImproper(OptionalReferenceWrapper<const SpeciesImproper> optImproper,
                                                           const InteractionPotential<TorsionFunctions> &expectedParams,
                                                           double tolerance)
{
    if (!optImproper)
        return testing::AssertionFailure() << "No improper provided to test";
    auto &i = (*optImproper).get();
    return checkIntramolecularTerms(std::format("Improper {} in species '{}'",
                                                joinStrings(i.atoms(), "-", [](const auto &atom) { return atom->index(); }),
                                                i.parent()->name()),
                                    expectedParams, i.interactionPotential(), tolerance);
}
// Test consistency between the two supplied double-keyed Data1D maps
[[nodiscard]] testing::AssertionResult testDoubleKeyedMap(std::string_view mapContents, const DoubleKeyedMap<Data1D> &mapA,
                                                          const DoubleKeyedMap<Data1D> &mapB, double testThreshold)
{
    // Check map sizes
    if (mapA.size() != mapB.size())
        return testing::AssertionFailure() << std::format("Maps containing {} data are of dissimilar size (A = {}, B = {})\n",
                                                          mapContents, mapA.size(), mapB.size());

    // Check individual data
    for (auto &[key, dataA] : mapA)
    {
        // Find same-keyed data in mapB
        if (mapB.contains(key))
        {
            auto errorReport = Error::percent(dataA, mapB.get(key));

            if (errorReport.error > testThreshold)
                return testing::AssertionFailure() << std::format(
                           "{} '{}' in map B has {} error of {}{} with data in map A which exceeds the threshold of {}",
                           mapContents, key, Error::errorTypes().keyword(errorReport.errorType), errorReport.error,
                           errorReport.errorType == Error::ErrorType::PercentError ? "%" : "", testThreshold);
        }
        else
        {
            return testing::AssertionFailure()
                   << std::format("{} '{}' is present in map A but not in map B.\n", mapContents, key);
        }
    }

    return testing::AssertionSuccess();
}
// Test consistency between production, molecular, and test energies, returning production values
[[nodiscard]] testing::AssertionResult testEnergyConsistency(const std::unique_ptr<EnergyKernel> &kernel, double threshold,
                                                             Kernel::EnergyResult &productionEnergy)
{
    // Calculate production energies (fully optimised)
    productionEnergy = kernel->totalEnergy();

    // Calculate baseline test energies (simple double-loop, PBC always)
    auto testEnergy = kernel->totalEnergySimple();

    // Calculate molecule-centric energy
    auto molecularPPEnergy = kernel->totalMoleculePairPotentialEnergy();

    // Compare basic energies with production value
    if (fabs(testEnergy.pairPotential.interMolecular - productionEnergy.pairPotential.interMolecular) > threshold)
        return testing::AssertionFailure() << std::format(
                   "Production interMolecular pairPotential value of {} differs by {} from "
                   "test energy of {} which exceeds threshold of {}",
                   productionEnergy.pairPotential.interMolecular,
                   testEnergy.pairPotential.interMolecular - productionEnergy.pairPotential.interMolecular,
                   testEnergy.pairPotential.interMolecular, threshold);
    if (fabs(testEnergy.pairPotential.intraMolecular - productionEnergy.pairPotential.intraMolecular) > threshold)
        return testing::AssertionFailure() << std::format(
                   "Production intraMolecular pairPotential value of {} differs by {} from "
                   "test energy of {} which exceeds threshold of {}",
                   productionEnergy.pairPotential.intraMolecular,
                   testEnergy.pairPotential.intraMolecular - productionEnergy.pairPotential.intraMolecular,
                   testEnergy.pairPotential.intraMolecular, threshold);
    if (fabs(testEnergy.geometry.total() - productionEnergy.geometry.total()) > threshold)
        return testing::AssertionFailure() << std::format(
                   "Production total geometry value of {} differs by {} from test energy of {} which exceeds threshold of {}",
                   productionEnergy.geometry.total(), testEnergy.geometry.total() - productionEnergy.geometry.total(),
                   testEnergy.geometry.total(), threshold);

    // Compare basic energies with molecule-based values
    if (fabs(testEnergy.pairPotential.total() - molecularPPEnergy.total()) > threshold)
        return testing::AssertionFailure() << std::format("Molecular total pairPotential value of {} differs by {} from test "
                                                          "energy of {} which exceeds threshold of {}",
                                                          molecularPPEnergy.total(),
                                                          testEnergy.pairPotential.total() - molecularPPEnergy.total(),
                                                          testEnergy.total(), threshold);
    if (fabs(testEnergy.pairPotential.interMolecular - molecularPPEnergy.interMolecular) > threshold)
        return testing::AssertionFailure() << std::format(
                   "Molecular interMolecular pairPotential value of {} differs by {} from test energy of {} which exceeds "
                   "threshold of {}",
                   molecularPPEnergy.interMolecular, testEnergy.pairPotential.interMolecular - molecularPPEnergy.interMolecular,
                   testEnergy.pairPotential.interMolecular, threshold);

    // Compare molecule-based energies with production values
    if (fabs(molecularPPEnergy.total() - productionEnergy.pairPotential.total()) > threshold)
        return testing::AssertionFailure() << std::format(
                   "Production total pairPotential value of {} differs by {} from molecular "
                   "energy of {} which exceeds threshold of {}",
                   molecularPPEnergy.total() - productionEnergy.pairPotential.total(), productionEnergy.pairPotential.total(),
                   molecularPPEnergy.total(), threshold);
    if (fabs(molecularPPEnergy.interMolecular - productionEnergy.pairPotential.interMolecular) > threshold)
        return testing::AssertionFailure() << std::format(
                   "Production interMolecular pairPotential value of {} differs by {} from "
                   "molecular energy of {} which exceeds threshold of {}",
                   productionEnergy.pairPotential.interMolecular,
                   molecularPPEnergy.interMolecular - productionEnergy.pairPotential.interMolecular,
                   molecularPPEnergy.interMolecular, threshold);

    return testing::AssertionSuccess();
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
