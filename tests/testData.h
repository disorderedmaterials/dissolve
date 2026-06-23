// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/coreData.h"
#include "classes/species.h"
#include "kernels/energy.h"
#include "kernels/force.h"
#include "main/dissolve.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/error.h"
#include "math/mathFunc.h"
#include "math/sampledData1D.h"
#include "math/sampledDouble.h"
#include "math/sampledVector.h"
#include "nodes/energy.h"
#include "nodes/graph.h"
#include "nodes/importXYData.h"
#include "nodes/serialisableData.h"
#include "nodes/species.h"
#include <gtest/gtest.h>

namespace UnitTest
{

// Flags that can modify how a test is setUp.  This can be useful
// for masking tests that are known to be failing, but cannot be
// resolved at this juncture.  The indices must be unique powers
// of two in order for masks to be composable.
//
// Eventually, once we move to C++23, this can be replaced by a
// std::bitset, but bitset isn't constexpr until then.
enum TestFlags
{
    TomlFailure = 1, // tests where the TOML testing is known to fail
};

// Custom Macros
// See https://stackoverflow.com/questions/42956538

// Wrap a code block with try-catch, handle exceptions thrown, print them into EXCEPT_STREAM and rethrow.
// clang-format off
#define PRINT_AND_RETHROW(CODE_BLOCK, EXCEPT_STREAM) try{CODE_BLOCK;}catch(const std::exception& ex){ EXCEPT_STREAM << "std::exception thrown: " << ex.what() << std::endl; throw;  }catch(...){ EXCEPT_STREAM << "unknown structure thrown" << std::endl; throw;}
// clang-format on

// Wrap a code block with try-catch, handle exceptions thrown, print them into std::cerr and rethrow.
#define PRINT_STDERR_AND_RETHROW(CODE_BLOCK) PRINT_AND_RETHROW(CODE_BLOCK, std::cerr)
#define EXPECT_NO_THROW_VERBOSE(CODE_BLOCK) EXPECT_NO_THROW(PRINT_STDERR_AND_RETHROW(CODE_BLOCK))
#define ASSERT_NO_THROW_VERBOSE(CODE_BLOCK) ASSERT_NO_THROW(PRINT_STDERR_AND_RETHROW(CODE_BLOCK))

/*
 * Helper Functions
 */

// Helper function for comparing TOML values with context, but without insisting on a specific ordering of fields.
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

// Serialise A and deserialise into B
template <class T> void tomlRoundTrip(T &a, T &b)
{
    SerialisedValue serialised;
    auto s = std::make_shared<SerialisableClass<T>>("data", a);
    ASSERT_NO_THROW(serialised = s->serialise());

    auto d = std::make_shared<SerialisableClass<T>>("data", b);
    ASSERT_NO_THROW(d->deserialise(serialised));
}

/*
 * System Test Class
 */

class DissolveSystemTest
{
    public:
    DissolveSystemTest() : dissolve_(coreData_) { dissolve_.setRestartFileFrequency(0); };

    /*
     * Dissolve & CoreData
     */
    private:
    CoreData coreData_;
    Dissolve dissolve_;
    // Whether to perform rewrite checks on setUp
    bool rewriteCheck_{true};
    // Function to execute to perform additional setup prior to prepare()
    std::function<void(Dissolve &D, CoreData &C)> additionalSetUp_;

    public:
    // Return the Dissolve object
    Dissolve &dissolve() { return dissolve_; }
    // Return the CoreData object
    CoreData &coreData() { return coreData_; }

    /*
     * Checks
     */
    public:
    // Test simple double
    [[nodiscard]] static bool checkDouble(std::string_view quantity, double A, double B, double threshold)
    {
        auto delta = fabs(A - B);
        auto isOK = delta <= threshold;
        Messenger::print("Reference {} delta with correct value is {:15.9e} and is {} (threshold is {:10.3e})\n", quantity,
                         delta, isOK ? "OK" : "NOT OK", threshold);
        return isOK;
    }
    // Test sampled double
    [[nodiscard]] static bool checkSampledDouble(std::string_view quantity, SampledDouble A, double B, double threshold)
    {
        return checkDouble(quantity, A.value(), B, threshold);
    }
    // Test sampled double
    [[nodiscard]] bool checkSampledDouble(std::string_view quantity, std::string_view tag, double B, double threshold)
    {
        // Locate the target reference data
        const auto &A = dissolve_.processingModuleData().retrieve<SampledDouble>(tag);

        return checkDouble(quantity, A.value(), B, threshold);
    }
    // Test Data1D
    [[nodiscard]] static bool checkData1D(const Data1D &dataA, std::string_view nameA, const Data1D &dataB,
                                          std::string_view nameB, double tolerance = 5.0e-3,
                                          Error::ErrorType errorType = Error::ErrorType::EuclideanError)
    {
        // Generate the error estimate and compare against the threshold value
        auto error = Error::error(errorType, dataA, dataB).error;
        auto notOK = std::isnan(error) || error > tolerance;
        Messenger::print("Data '{}' has error of {:7.3e} with data '{}' and is {} (threshold is {:6.3e}).\n", nameA, error,
                         nameB, notOK ? "NOT OK" : "OK", tolerance);
        return !notOK;
    }
    [[nodiscard]] static bool checkData1D(const Data1D &dataA, std::string_view nameA, std::string filePath, int xColumn,
                                          int yColumn, double tolerance = 5.0e-3,
                                          Error::ErrorType errorType = Error::ErrorType::EuclideanError)
    {
        Data1D dataB;
        if (!ImportXYDataNode::read(dataB, filePath, xColumn, yColumn))
        {
            std::cout << std::format("Failed to read data from '{}'\n", filePath);
            return false;
        }

        return checkData1D(dataA, nameA, dataB, filePath, tolerance, errorType);
    }
    // Test Data2D
    [[nodiscard]] static bool checkData2D(const Data2D &dataA, std::string_view nameA, const Data2D &dataB,
                                          std::string_view nameB, double tolerance = 5.0e-3,
                                          Error::ErrorType errorType = Error::ErrorType::EuclideanError)
    {
        // Generate the error estimate and compare against the threshold value
        auto error = Error::error(errorType, dataA.values().linearArray(), dataB.values().linearArray()).error;
        auto notOK = std::isnan(error) || error > tolerance;
        Messenger::print("Data '{}' has error of {:7.3f} with data '{}' and is {} (threshold is {:6.3e})\n\n", nameA, error,
                         nameB, notOK ? "NOT OK" : "OK", tolerance);

        return !notOK;
    }
    // Test Data3D
    [[nodiscard]] static bool checkData3D(const Data3D &dataA, std::string_view nameA, const Data3D &dataB,
                                          std::string_view nameB, double tolerance = 5.0e-3,
                                          Error::ErrorType errorType = Error::ErrorType::EuclideanError)
    {
        // Generate the error estimate and compare against the threshold value
        auto error = Error::error(errorType, dataA.values().linearArray(), dataB.values().linearArray()).error;
        auto notOK = std::isnan(error) || error > tolerance;
        Messenger::print("Internal data '{}' has error of {:7.3f} with external data '{}' and is {} (threshold is {:6.3e})\n\n",
                         nameA, error, nameB, notOK ? "NOT OK" : "OK", tolerance);

        return !notOK;
    }
    // Test SampledVector data
    [[nodiscard]] bool checkSampledVector(std::string_view tag, const std::vector<double> &referenceData,
                                          double tolerance = 5.0e-3,
                                          Error::ErrorType errorType = Error::ErrorType::EuclideanError)
    {
        // Locate the target reference data
        auto optData = dissolve_.processingModuleData().search<const SampledVector>(tag);
        if (!optData)
            throw(std::runtime_error(std::format("No data with tag '{}' exists.\n", tag)));
        const auto &data = optData->get();

        // Generate the error estimate and compare against the threshold value
        auto error = Error::error(errorType, data.values(), referenceData).error;
        auto notOK = std::isnan(error) || error > tolerance;
        Messenger::print("Target data '{}' has error of {:7.3e} with reference data and is {} (threshold is {:6.3e})\n\n", tag,
                         error, notOK ? "NOT OK" : "OK", tolerance);
        return !notOK;
    }
    // Test Vec3 data
    static void checkVec3(const Vector3 &A, const Vector3 &B, double tolerance = 1.0e-6)
    {
        EXPECT_NEAR(A.x, B.x, tolerance);
        EXPECT_NEAR(A.y, B.y, tolerance);
        EXPECT_NEAR(A.z, B.z, tolerance);
    }
    // Test Vec3 vector data
    static void checkVec3Vector(const std::vector<Vector3> &A, const std::vector<Vector3> &B, double tolerance = 1.0e-6)
    {
        ASSERT_EQ(A.size(), B.size());
        for (auto n = 0; n < A.size(); ++n)
            checkVec3(A[n], B[n], tolerance);
    }
    // Test species atom type
    static void checkSpeciesAtomType(Species *sp, const std::map<int, std::string> &namesById)
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
                                  const InteractionPotential<Intra> &actualParams, double tolerance = 1.0e-6)
    {
        Messenger::print("Testing intramolecular interaction: {}...\n", termInfo);
        EXPECT_EQ(Intra::forms().keyword(actualParams.form()), Intra::forms().keyword(expectedParams.form()));
        EXPECT_EQ(actualParams.nParameters(), expectedParams.nParameters());
        for (auto &&[current, expected] : zip(actualParams.parameters(), expectedParams.parameters()))
            EXPECT_NEAR(current, expected, tolerance);
    }
    // Test species bond term
    void checkSpeciesIntramolecular(Species *sp, std::vector<int> atoms,
                                    const InteractionPotential<BondFunctions> &expectedParams, double tolerance = 1.0e-6)
    {
        ASSERT_TRUE(atoms.size() == 2);
        const auto &b = sp->getBond(&sp->atoms()[atoms[0]], &sp->atoms()[atoms[1]]);
        if (!b)
            throw(std::runtime_error(std::format("No bond {} exists in species '{}'.\n", joinStrings(atoms, "-"), sp->name())));
        checkIntramolecularTerms(std::format("bond {}", joinStrings(atoms, "-")), expectedParams,
                                 b->get().interactionPotential(), tolerance);
    }
    // Test species angle term
    void checkSpeciesIntramolecular(Species *sp, std::vector<int> atoms,
                                    const InteractionPotential<AngleFunctions> &expectedParams, double tolerance = 1.0e-6)
    {
        ASSERT_TRUE(atoms.size() == 3);
        const auto &a = sp->getAngle(&sp->atoms()[atoms[0]], &sp->atoms()[atoms[1]], &sp->atoms()[atoms[2]]);
        if (!a)
            throw(
                std::runtime_error(std::format("No angle {} exists in species '{}'.\n", joinStrings(atoms, "-"), sp->name())));
        checkIntramolecularTerms(std::format("angle {}", joinStrings(atoms, "-")), expectedParams,
                                 a->get().interactionPotential(), tolerance);
    }
    // Test species torsion / improper term
    void checkSpeciesIntramolecular(Species *sp, std::vector<int> atoms,
                                    const InteractionPotential<TorsionFunctions> &expectedParams, double tolerance = 1.0e-6)
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
    static bool checkDoubleKeyedMap(std::string_view mapContents, const DoubleKeyedMap<Data1D> &mapA,
                                    const DoubleKeyedMap<Data1D> &mapB, double testThreshold)
    {
        // Check map sizes
        if (mapA.size() != mapB.size())
        {
            std::cout << std::format("Maps containing {} data are of dissimilar size (A = {}, B = {})\n", mapContents,
                                     mapA.size(), mapB.size());
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
                                         mapContents, key, Error::errorTypes().keyword(errorReport.errorType),
                                         errorReport.error, errorReport.errorType == Error::ErrorType::PercentError ? "%" : "",
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
    static bool checkPartialSet(const PartialSet &setA, const PartialSet &setB, double testThreshold)
    {
        // Full partials
        if (!checkDoubleKeyedMap("Full Partials", setA.partials(), setB.partials(), testThreshold))
            return false;

        // Bound partials
        if (!checkDoubleKeyedMap("Bound Partials", setA.boundPartials(), setB.boundPartials(), testThreshold))
            return false;

        // Unbound partials
        if (!checkDoubleKeyedMap("Unbound Partials", setA.unboundPartials(), setB.unboundPartials(), testThreshold))
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
};

// Check consistency between production, molecular, and test energies, returning production values
Kernel::EnergyResult checkEnergyConsistency(const std::unique_ptr<EnergyKernel> &kernel, double testThreshold = 1.0e-6)
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
void checkForceConsistency(const std::unique_ptr<ForceKernel> &kernel, std::vector<Vector3> &ppForces,
                           std::vector<Vector3> &geomForces, Flags<Kernel::CalculationFlags> flags = {},
                           double ppMaxDeviation = 1.0e-2, double geomMaxDeviation = 1.0e-6)
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
void checkReferenceForceConsistency(const std::vector<Vector3> &ppForces, const std::vector<Vector3> &geomForces,
                                    const std::vector<Vector3> &referenceForces, double maxDeviation = 1.0e-3)
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
} // namespace UnitTest
