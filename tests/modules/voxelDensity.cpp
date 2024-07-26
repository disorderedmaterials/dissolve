// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/voxelDensity/voxelDensity.h"
#include "classes/atom.h"
#include "classes/speciesAtom.h"
#include "data/elements.h"
#include "keywords/integer.h"
#include "module/types.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
#define BOX_LENGTH 8
#define NUM_ATOMS static_cast<int>(std::pow(BOX_LENGTH, 3))
#define MASS_HELIUM 4.002602
#define Z_HELIUM 2
#define SCATTERING_LENGTH_DENSITY_HELIUM 4.0026

class HeliumBox
{
    public:
    HeliumBox(CoreData &coreData) : coreData_(coreData) {}
    ~HeliumBox() { reset(); }

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
        auto densitySet = module->keywords().set("NumberOfPoints", numPoints);
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
    const auto &data8Bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//Mass/A^3")->get();
    ASSERT_TRUE(data8Bin.nNonZeroValues().size() == 1);
    auto max8Bin = data8Bin.maxValueAt();
    ASSERT_NEAR(max8Bin.first, MASS_HELIUM);
    ASSERT_NEAR(max8Bin.second, std::pow(8, 3));

    box.setProcessing(VoxelDensityModule::TargetPropertyType::Mass, 4);
    systemTest.dissolve().iterate(20);
    const auto &data4Bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//Mass/A^3")->get();
    ASSERT_TRUE(data4Bin.nNonZeroValues().size() == 1);
    auto max4Bin = data4Bin.maxValueAt();
    ASSERT_NEAR(max4Bin.first, MASS_HELIUM);
    ASSERT_NEAR(max4Bin.second, std::pow(4, 3));

    box.setProcessing(VoxelDensityModule::TargetPropertyType::Mass, 2);
    systemTest.dissolve().iterate(20);
    const auto &data2Bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//Mas/A^3")->get();
    ASSERT_TRUE(data2Bin.nNonZeroValues().size() == 1);
    auto max2Bin = data2Bin.maxValueAt();
    ASSERT_NEAR(max2Bin.first, MASS_HELIUM);
    ASSERT_NEAR(max2Bin.second, std::pow(2, 3));

    box.setProcessing(VoxelDensityModule::TargetPropertyType::Mass, 1);
    systemTest.dissolve().iterate(20);
    const auto &data1Bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//Mass/A^3")->get();
    ASSERT_TRUE(data1Bin.nNonZeroValues().size() == 1);
    auto max1Bin = data1Bin.maxValueAt();
    ASSERT_NEAR(max1Bin.first, MASS_HELIUM);
    ASSERT_NEAR(max1Bin.second, 1);
}

TEST_F(VoxelDensityModuleTest, HeliumBoxAtomicNumber)
{
    HeliumBox box(systemTest.coreData());

    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 8);
    systemTest.dissolve().iterate(20);
    const auto &data8Bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//AtomicNumber/A^3")->get();
    ASSERT_TRUE(data8Bin.nNonZeroValues().size() == 1);
    auto max8Bin = data8Bin.maxValueAt();
    ASSERT_NEAR(max8Bin.first, Z_HELIUM);
    ASSERT_NEAR(max8Bin.second, std::pow(8, 3));

    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 4);
    systemTest.dissolve().iterate(20);
    const auto &data4bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//AtomicNumber/A^3")->get();
    ASSERT_TRUE(data4Bin.nNonZeroValues().size() == 1);
    auto max4Bin = data4Bin.maxValueAt();
    ASSERT_NEAR(max4Bin.first, Z_HELIUM);
    ASSERT_NEAR(max4Bin.second, std::pow(4, 3));

    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 2);
    systemTest.dissolve().iterate(20);
    const auto &data2bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//AtomicNumber/A^3")->get();
    ASSERT_TRUE(data2Bin.nNonZeroValues().size() == 1);
    auto max2Bin = data2Bin.maxValueAt();
    ASSERT_NEAR(max2Bin.first, Z_HELIUM);
    ASSERT_NEAR(max2Bin.second, std::pow(2, 3));

    box.setProcessing(VoxelDensityModule::TargetPropertyType::AtomicNumber, 1);
    systemTest.dissolve().iterate(20);
    const auto &data1bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//Data1D//AtomicNumber/A^3")->get();
    ASSERT_TRUE(data1Bin.nNonZeroValues().size() == 1);
    auto max8Bin = data1Bin.maxValueAt();
    ASSERT_NEAR(max1Bin.first, Z_HELIUM);
    ASSERT_NEAR(max1Bin.second, 1);
}

TEST_F(VoxelDensityModuleTest, HeliumBoxScatteringLengthDensity)
{
    HeliumBox box(systemTest.coreData());

    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 8);
    systemTest.dissolve().iterate(20);
    const auto &data8bin = systemTest.dissolve()
                               .processingModuleData()
                               .search<const Data1D>("VoxelDensity//Data1D//ScatteringLengthDensity/A^3")
                               ->get();
    ASSERT_TRUE(data8Bin.nNonZeroValues().size() == 1);
    auto max8Bin = data8Bin.maxValueAt();
    ASSERT_NEAR(max8Bin.first, SCATTERING_LENGTH_DENSITY_HELIUM);
    ASSERT_NEAR(max8Bin.second, std::pow(8, 3));

    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 4);
    systemTest.dissolve().iterate(20);
    const auto &data4bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//ScatteringLengthDensity/A^3")->get();
    ASSERT_TRUE(data4Bin.nNonZeroValues().size() == 1);
    auto max4Bin = data4Bin.maxValueAt();
    ASSERT_NEAR(max4Bin.first, SCATTERING_LENGTH_DENSITY_HELIUM);
    ASSERT_NEAR(max4Bin.second, std::pow(4, 3));

    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 2);
    systemTest.dissolve().iterate(20);
    const auto &data2bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//ScatteringLengthDensity/A^3")->get();
    ASSERT_TRUE(data2Bin.nNonZeroValues().size() == 1);
    auto max2Bin = data2Bin.maxValueAt();
    ASSERT_NEAR(max2Bin.first, SCATTERING_LENGTH_DENSITY_HELIUM);
    ASSERT_NEAR(max2Bin.second, std::pow(2, 3));

    box.setProcessing(VoxelDensityModule::TargetPropertyType::ScatteringLengthDensity, 1);
    systemTest.dissolve().iterate(20);
    const auto &data1bin =
        systemTest.dissolve().processingModuleData().search<const Data1D>("VoxelDensity//ScatteringLengthDensity/A^3")->get();
    ASSERT_TRUE(data1Bin.nNonZeroValues().size() == 1);
    auto max1Bin = data1Bin.maxValueAt();
    ASSERT_NEAR(max1Bin.first, SCATTERING_LENGTH_DENSITY_HELIUM);
    ASSERT_NEAR(max1Bin.second, 1);
}

TEST_F(VoxelDensityModuleTest, Water)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/voxelDensity-water.txt"));
    auto module = systemTest.getModule<VoxelDensityModule>("AtomicMass");
    ASSERT_TRUE(module->keywords().getInt("NumberOfPoints") == 1);
}

} // namespace UnitTest