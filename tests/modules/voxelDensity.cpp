// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/elements.h"
#include "classes/speciesAtom.h"
#include "module/types.h"
#include "modules/voxelDensity/voxelDensity.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
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

    cfg.createBox({8, 8, 8}, {90.0, 90.0, 90.0});
    
    std::vector<SpeciesAtom> speciesAtoms(static_cast<int>std::pow(8, 3));

    auto addToAtoms = [&cfg](auto &atom)
    {
        auto molecule = std::make_shared<Molecule>();
        atom.set(Element::Element::He);
        molecule.addAtom(atom);
        cfg.addAtom(atom, molecule);
    }

    int at = 0;
    for (double i=0.5; i < length; i++)
    {
        for (double j=0.5; j < length; j++)
        {
            for (double k=0.5; k < length; k++) { addToAtoms(speciesAtoms[at++]); }
        }
    }

    auto moduleLayer = coreData.addProcessingLayer();
    auto module = moduleLayer.append(coreData, ModuleTypes::ModuleType::VoxelDensity, coreData.configurations());
    auto densitySet = module.keywords().set("Density", N);
    module.setEnumeration<EnumOptionsKeyword<VoxelDensityModule::TargetPropertyType>("TargetProperty", target);
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

}