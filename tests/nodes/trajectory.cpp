// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "io/import/trajectory.h"
#include "io/export/trajectory.h"
#include "nodes/constants.h"
#include "nodes/exportTrajectory.h"
#include "nodes/importConfigurationTrajectory.h"
#include "tests/graphData.h"
#include "tests/tempFile.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <memory>

namespace UnitTest
{
TEST(TrajectoryNodesTest, RoundTrip)
{
    const std::string importFile = "dlpoly/water267-analysis/water-267-298K.xyz";
    auto exportFile = TempFile("water-267-298K_copy.xyz");
    auto inputFormat = TrajectoryImportFileFormat::TrajectoryImportFormat::XYZ;
    auto exportFormat = TrajectoryExportFileFormat::TrajectoryExportFormat::XYZ;

    TestGraph testGraph_;
    testGraph_.createConfiguration("Box", {{createWaterDLPoly, 267}}, 0.1);

    // Create an import configuration trajectory node
    auto trajectoryImport =
        dynamic_cast<ImportConfigurationTrajectoryNode *>(testGraph_.createNode("ImportConfigurationTrajectory"));
    ASSERT_TRUE(trajectoryImport);
    ASSERT_TRUE(trajectoryImport->setOption<std::string>("FilePath", importFile));
    ASSERT_TRUE(trajectoryImport->setOption<TrajectoryImportFileFormat::TrajectoryImportFormat>("FileFormat", inputFormat));
    ASSERT_TRUE(testGraph_.addEdge({"Insert-Water", "Configuration", "ImportConfigurationTrajectory", "Configuration"}));

    // Create an export configuration trajectory node
    auto trajectoryExport = dynamic_cast<ExportTrajectoryNode *>(testGraph_.createNode("ExportTrajectory"));
    ASSERT_TRUE(trajectoryExport);
    ASSERT_TRUE(trajectoryExport->setOption<std::string>("FilePath", exportFile));
    ASSERT_TRUE(trajectoryExport->setOption<TrajectoryExportFileFormat::TrajectoryExportFormat>("FileFormat", exportFormat));
    ASSERT_TRUE(testGraph_.addEdge({"ImportConfigurationTrajectory", "Configuration", "ExportTrajectory", "Configuration"}));

    ASSERT_EQ(trajectoryExport->run(), NodeConstants::ProcessResult::Success);
}
} // namespace UnitTest
