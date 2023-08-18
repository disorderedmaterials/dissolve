// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/import/forces.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class MDModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(MDModuleTest, BenzeneRestart)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/md-benzene.txt"));
    ASSERT_NO_THROW(systemTest.loadRestart("dissolve/md-benzene.8.restart"));
    ASSERT_TRUE(systemTest.dissolve().iterate(2));

    systemTest.checkVec3Vector("Forces01//Bulk//Forces", {"dissolve/md-benzene.10.forces", ForceImportFileFormat::ForceImportFormat::Simple}, 1.0e-4);
}
} // namespace UnitTest
