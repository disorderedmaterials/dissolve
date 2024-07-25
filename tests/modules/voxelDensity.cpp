// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/voxelDensity/voxelDensity.h"
#include "classes/atom.h"
#include "classes/speciesAtom.h"
#include "data/elements.h"
#include "module/types.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
#define BOX_LENGTH 8
#define NUM_ATOMS static_cast<int>(std::pow(BOX_LENGTH, 3))

class HeliumBox
{
    public:
    HeliumBox(CoreData &coreData) : coreData_(coreData) {}

    private:
    CoreData &coreData_;
    int boxLength_{BOX_LENGTH};
    int numAtoms_{NUM_ATOMS};

    std::vector<SpeciesAtom> speciesAtoms_;
    std::vector<std::shared_ptr<Molecule>> molecules_;
    std::vector<Atom> atoms_;

    public:
    void reset()
    {
        coreData_.clear();

        if ((speciesAtoms_.size() == 0) || (molecules_.size() == 0) || (atoms_.size() == 0))
            resizeAll();
    }

    void resizeAll()
    {
        speciesAtoms_.resize(numAtoms_);
        molecules_.resize(numAtoms_, std::make_shared<Molecule>());
        atoms_.resize(numAtoms_);
    }

    void addToAtoms(Configuration *cfg, const SpeciesAtom *spAtom, std::shared_ptr<Molecule> molecule, Atom *atom, double i,
                    double j, double k)
    {
        atom->setSpeciesAtom(spAtom);
        molecule->addAtom(atom);
        cfg->addAtom(spAtom, molecule, Vec3<double>(i, j, k));
    }

    void setProcessing(VoxelDensityModule::TargetPropertyType target, int numPoints)
    {
        reset();

        auto cfg = coreData_.addConfiguration();

        cfg->createBox({(double)boxLength_, (double)boxLength_, (double)boxLength_}, {90.0, 90.0, 90.0});

        int at = 0;
        for (double i = 0.5; i < boxLength_; i++)
        {
            for (double j = 0.5; j < boxLength_; j++)
            {
                for (double k = 0.5; k < boxLength_; k++)
                {
                    speciesAtoms_[at].setZ(Elements::He);
                    addToAtoms(cfg, &speciesAtoms_[at], molecules_[at], &atoms_[at], i, j, k);
                    at++;
                }
            }
        }

        auto moduleLayer = coreData_.addProcessingLayer();
        auto module = moduleLayer->append(coreData_, ModuleTypes::ModuleType::VoxelDensity, coreData_.configurations());
        module->setName("VoxelDensity");
        auto densitySet = module->keywords().set("Density", numPoints);
        module->keywords().setEnumeration("TargetProperty", target);
    }
};

class VoxelDensityModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(VoxelDensityModuleTest, HeliumBoxAtomicMass)
{
    HeliumBox box(systemTest.coreData());

    box.setProcessing(VoxelDensityModule::TargetPropertyType::Mass, 8);
    systemTest.dissolve().iterate(20);
    const auto &data8bin = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//MassData1D")->get();
    // Assertions

    box.setProcessing(VoxelDensityModule::TargetPropertyType::Mass, 4);
    systemTest.dissolve().iterate(20);
    const auto &data4bin = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//MassData1D")->get();
    // Assertions

    box.setProcessing(VoxelDensityModule::TargetPropertyType::Mass, 2);
    systemTest.dissolve().iterate(20);
    const auto &data2bin = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//MassData1D")->get();
    // Assertions

    box.setProcessing(VoxelDensityModule::TargetPropertyType::Mass, 1);
    systemTest.dissolve().iterate(20);
    const auto &data1bin = systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//MassData1D")->get();
    // Assertions
}

TEST_F(VoxelDensityModuleTest, HeliumBoxAtomicNumber)
{
    HeliumBox box(systemTest.coreData());

    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 8);
    systemTest.dissolve().iterate(20);
    const auto &data8bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//AtomicNumberData1D")->get();
    // Assertions

    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 4);
    systemTest.dissolve().iterate(20);
    const auto &data4bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//AtomicNumberData1D")->get();
    // Assertions

    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 2);
    systemTest.dissolve().iterate(20);
    const auto &data2bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//AtomicNumberData1D")->get();
    // Assertions

    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 1);
    systemTest.dissolve().iterate(20);
    const auto &data1bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//AtomicNumberData1D")->get();
    // Assertions
}

TEST_F(VoxelDensityModuleTest, HeliumBoxScatteringLengthDensity)
{
    HeliumBox box(systemTest.coreData());

    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 8);
    systemTest.dissolve().iterate(20);
    const auto &data8bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//ScatteringLengthDensityData1D")->get();
    // Assertions

    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 4);
    systemTest.dissolve().iterate(20);
    const auto &data4bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//ScatteringLengthDensityData1D")->get();
    // Assertions

    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 2);
    systemTest.dissolve().iterate(20);
    const auto &data2bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//ScatteringLengthDensityData1D")->get();
    // Assertions

    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 1);
    systemTest.dissolve().iterate(20);
    const auto &data1bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//ScatteringLengthDensityData1D")->get();
    // Assertions
}

} // namespace UnitTest