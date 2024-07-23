// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/voxelDensity/voxelDensity.h"
#include "classes/speciesAtom.h"
#include "data/elements.h"
#include "module/types.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
#define BOX_LENGTH 8
#define NUM_ATOMS static_cast<int> std::pow(BOX_LENGTH, 3)

class VoxelDensityModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

// Set up the module processing for a simple Helium box
void setProcessing(CoreData &coreData, VoxelDensityModule::TargetPropertyType target, int N)
{
    coreData.clear();

    auto cgf = systemTest.coreData().addConfiguration();

    cfg.createBox({BOX_LENGTH, BOX_LENGTH, BOX_LENGTH}, {90.0, 90.0, 90.0});

    std::vector<SpeciesAtom> speciesAtoms(NUM_ATOMS);

    auto addToAtoms =
        [&cfg](auto &atom, const auto &r)
    {
        auto molecule = std::make_shared<Molecule>();
        atom.set(Element::Element::He);
        molecule.addAtom(atom);
        cfg.addAtom(atom, molecule, Vec3<double>(r[0], r[1], r[2]));
    }

    int at = 0;
    for (double i = 0.5; i < BOX_LENGTH; i++)
    {
        for (double j = 0.5; j < BOX_LENGTH; j++)
        {
            for (double k = 0.5; k < BOX_LENGTH; k++)
            {
                addToAtoms(speciesAtoms[at++], {i, j, k});
            }
        }
    }

    auto moduleLayer = coreData.addProcessingLayer();
    auto module = moduleLayer.append(coreData, ModuleTypes::ModuleType::VoxelDensity, coreData.configurations());
    auto densitySet = module.keywords().set("Density", N);
    module.keywords().setEnumeration("TargetProperty", target);
}

TEST_F(VoxelDensityModuleTest, HeliumBoxAtomicMass)
{
    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::Mass, 8);
    systemTest.dissolve().iterate(20);
    // Assertions

    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::Mass, 4);
    systemTest.dissolve().iterate(20);
    // Assertions

    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::Mass, 2);
    systemTest.dissolve().iterate(20);
    // Assertions

    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::Mass, 1);
    systemTest.dissolve().iterate(20);
    // Assertions
}

TEST_F(VoxelDensityModuleTest, HeliumBoxAtomicNumber)
{
    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::AtomicNumber, 8);
    systemTest.dissolve().iterate(20);
    // Assertions

    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::AtomicNumber, 4);
    systemTest.dissolve().iterate(20);
    // Assertions

    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::AtomicNumber, 2);
    systemTest.dissolve().iterate(20);
    // Assertions

    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::AtomicNumber, 1);
    systemTest.dissolve().iterate(20);
    // Assertions
}

TEST_F(VoxelDensityModuleTest, HeliumBoxScatteringLengthDensity)
{
    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 8);
    systemTest.dissolve().iterate(20);
    // Assertions

    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 4);
    systemTest.dissolve().iterate(20);
    // Assertions

    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 2);
    systemTest.dissolve().iterate(20);
    // Assertions

    setProcessing(systemTest.coreData(), VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 1);
    systemTest.dissolve().iterate(20);
    // Assertions
}

} // namespace UnitTest