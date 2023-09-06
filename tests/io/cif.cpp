// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/import/cif.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(ImportCIFTest, Parse)
{
    // Test files
    auto cifPath = "cif/";
    std::vector<std::string> cifs = {"1517789.cif", "1557470.cif", "1557599.cif", "7705246.cif",
                                     "9000004.cif", "9000095.cif", "9000418.cif"};

    for (auto &cif : cifs)
    {
        CIFImport cifImporter;
        EXPECT_TRUE(cifImporter.read(cifPath + cif));
    }
}
} // namespace UnitTest
