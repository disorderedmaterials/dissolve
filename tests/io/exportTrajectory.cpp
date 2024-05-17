// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "data/elements.h"
#include "io/export/trajectory.h"
#include "tests/testData.h"
#include <fstream>
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class ExportTrajectoryTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

std::string exportFile(DissolveSystemTest &systemTest, std::string outfile,
                       TrajectoryExportFileFormat::TrajectoryExportFormat format)
{
    std::string inputFile = "dissolve/input/angle.txt";

    systemTest.setUp(inputFile);

    auto *cfg = systemTest.coreData().configuration(0);

    auto output_path = fmt::format("{}/{}", DissolveSys::beforeLastChar(inputFile, '/'), outfile);
    TrajectoryExportFileFormat exporter(output_path, format);
    EXPECT_TRUE(exporter.exportData(cfg));

    return output_path;
}

TEST_F(ExportTrajectoryTest, XYZ)
{
    auto output_path = exportFile(systemTest, "TestOutput_exportTrajectory.basic.xyz",
                                  TrajectoryExportFileFormat::TrajectoryExportFormat::XYZ);
    auto *cfg = systemTest.coreData().configuration(0);

    std::ifstream result(output_path);
    ASSERT_TRUE(result.is_open());

    // Size Header
    int size;
    result >> size;
    EXPECT_EQ(size, cfg->nAtoms());

    // Configuration Header
    std::string name, at;
    int version;
    result >> name >> at >> version;
    EXPECT_EQ(name, cfg->name());
    EXPECT_EQ(at, "@");
    EXPECT_EQ(version, cfg->contentsVersion());

    // Line by line analysis
    for (auto atom : cfg->atoms())
    {
        std::string elem;
        double x, y, z;
        result >> elem >> x >> y >> z;
        EXPECT_EQ(elem, Elements::symbol(atom.speciesAtom()->Z()));
        EXPECT_NEAR(atom.x(), x, 1e-9);
        EXPECT_NEAR(atom.y(), y, 1e-9);
        EXPECT_NEAR(atom.z(), z, 1e-9);
        break;
    }
}

TEST_F(ExportTrajectoryTest, XYZExport)
{
    auto output_path = exportFile(systemTest, "TestOutput_exportTrajectory.extended.xyz",
                                  TrajectoryExportFileFormat::TrajectoryExportFormat::XYZExtended);
    auto *cfg = systemTest.coreData().configuration(0);

    std::ifstream result(output_path);
    ASSERT_TRUE(result.is_open());

    // Size Header
    int size;
    result >> size;
    EXPECT_EQ(size, cfg->nAtoms());

    // Configuration Header
    std::string name, at;
    int version;
    result >> name >> at >> version;
    EXPECT_EQ(name, cfg->name());
    EXPECT_EQ(at, "@");
    EXPECT_EQ(version, cfg->contentsVersion());

    // Line by line analysis
    for (auto atom : cfg->atoms())
    {
        std::string elem, atomType;
        double x, y, z;
        int index;
        result >> elem >> x >> y >> z >> index >> atomType;
        EXPECT_EQ(elem, Elements::symbol(atom.speciesAtom()->Z()));
        EXPECT_NEAR(atom.x(), x, 1e-9);
        EXPECT_NEAR(atom.y(), y, 1e-9);
        EXPECT_NEAR(atom.z(), z, 1e-9);
        EXPECT_EQ(atomType, atom.speciesAtom()->atomType()->name());
        EXPECT_EQ(index, atom.localTypeIndex());
        break;
    }
}

} // namespace UnitTest
