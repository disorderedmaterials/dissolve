// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/constants.h"
#include "nodes/exportXYZTrajectory.h"
#include "nodes/importXYZTrajectory.h"
#include "tests/tempFile.h"
#include "tests/testing.h"
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>

namespace UnitTest
{
TEST(TrajectoryNodesTest, RoundTrip)
{
    const std::string importFile = "dlpoly/water267-analysis/water-267-298K.xyz";
    auto exportFile = TempFile("water-267-298K_copy.xyz");

    TestGraph testGraph_;
    testGraph_.createConfiguration("Box", {{"species/water-dlpoly.toml", 267}}, 0.1);

    // Create an import configuration trajectory node
    auto trajectoryImport = dynamic_cast<ImportXYZTrajectoryNode *>(testGraph_.createNode("ImportXYZTrajectory"));
    ASSERT_TRUE(trajectoryImport);
    ASSERT_TRUE(trajectoryImport->setOption<std::string>("FilePath", importFile));
    ASSERT_TRUE(testGraph_.createNode("SetCoordinates"));
    ASSERT_TRUE(testGraph_.addEdge({"Insert-Water", "Configuration", "SetCoordinates", "Configuration"}));
    ASSERT_TRUE(testGraph_.addEdge({"ImportXYZTrajectory", "Structure", "SetCoordinates", "Structure"}));

    // Create an export configuration trajectory node
    auto trajectoryExport = dynamic_cast<ExportXYZTrajectoryNode *>(testGraph_.createNode("ExportXYZTrajectory"));
    ASSERT_TRUE(trajectoryExport);
    ASSERT_TRUE(trajectoryExport->setOption<std::string>("FilePath", exportFile));
    ASSERT_TRUE(trajectoryExport->setOption<bool>("Extended", false));
    ASSERT_TRUE(testGraph_.addEdge({"SetCoordinates", "Configuration", "ExportXYZTrajectory", "Configuration"}));

    ASSERT_EQ(trajectoryExport->run(), NodeConstants::ProcessResult::Success);

    std::ifstream reference{importFile}, actual{exportFile};
    std::string ref, act;

    // Loop through the lines of the output file and check that the
    // trajectories are the same
    int line = 0;
    // Only go through the first 801 lines, since we don't have cell
    // information
    while (line < 801)
    {
        getline(reference, ref);
        getline(actual, act);
        // skip reference rows
        if (act == "801" or act == " @ 3")
        {
            ++line;
            continue;
        }

        // Use stringstream to parse line
        std::istringstream correct(ref), result(act);
        std::string correct_name, result_name;

        correct >> correct_name;
        result >> result_name;
        // Ensure that first letter is correct (so that "O" and "OW"
        // are marked as the same).
        ASSERT_EQ(correct_name[0], result_name[0]);

        // Ensure that the x, y, and z are the same
        double correct_float, result_float;
        for (int i = 0; i < 3; ++i)
        {
            correct >> correct_float;
            result >> result_float;
            ASSERT_EQ(correct_float, result_float) << std::format("line {}, value {}", line, i);
        }
        ++line;
    }
}
} // namespace UnitTest
