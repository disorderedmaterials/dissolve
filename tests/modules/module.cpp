// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "generator/copy.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/siteRDF/siteRDF.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class ModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(ModuleTest, ConfigurationTargets)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/siteRDF-water.txt"));

    // Perform a few iterations
    const auto nIterations = 5;
    ASSERT_TRUE(systemTest.iterateRestart(nIterations));

    // Check the processing module data
    EXPECT_EQ(systemTest.dissolve().processingModuleData().version("Histo-AB", "RDF(COM-COM)"), nIterations - 1);

    // Get the COM-COM SiteRDF module
    auto *siteRDF = systemTest.getModule<SiteRDFModule>("RDF(COM-COM)");

    // Try to run the siteRDF module again
    ModuleContext context(systemTest.dissolve().worldPool(), systemTest.dissolve());
    EXPECT_EQ(siteRDF->executeProcessing(context), Module::ExecutionResult::NotExecuted);

    // Double-check the processing module data
    EXPECT_EQ(systemTest.dissolve().processingModuleData().version("Histo-AB", "RDF(COM-COM)"), nIterations - 1);

    // Switch the configuration for a copy of the original
    Configuration cfg;
    cfg.generator().createRootNode<CopyGeneratorNode>("CopyConfig", systemTest.dissolve().coreData().configuration(0));
    cfg.generate({systemTest.dissolve()});
    siteRDF->keywords().set("Configuration", &cfg);

    // Try to run the module again - it should, but all processing module data for it should be cleared
    EXPECT_EQ(siteRDF->executeProcessing(context), Module::ExecutionResult::Success);
    EXPECT_EQ(systemTest.dissolve().processingModuleData().version("Histo-AB", "RDF(COM-COM)"), 0);

    // Now remove the configuration target completely - module shouldn't run, but data should remain
    siteRDF->keywords().objectNoLongerValid(&cfg);
    EXPECT_EQ(siteRDF->executeProcessing(context), Module::ExecutionResult::Failed);
    EXPECT_TRUE(systemTest.dissolve().processingModuleData().contains("Histo-AB", "RDF(COM-COM)"));
}

} // namespace UnitTest
