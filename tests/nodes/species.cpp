// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/species.h"
#include "nodes/constants.h"
#include "nodes/dissolve.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class SpeciesNodeTest : public ::testing::Test
{
    public:
    SpeciesNodeTest() : dissolve_(coreData_), root_(dissolve_) {}

    protected:
    // We need a CoreData and Dissolve definition to properly
    // instantiate DissolveGraph at present.
    CoreData coreData_;
    Dissolve dissolve_;
    DissolveGraph root_;
};

TEST_F(SpeciesNodeTest, Creation)
{
    auto s = dynamic_cast<SpeciesNode *>(root_.createNode("Species", "s"));
    ASSERT_EQ(s->name(), "s");
    auto option = s->findOption("Definition");
    ASSERT_NE(option, nullptr);
    option->set<std::string>(R"(
name="Water"
[[atoms]]
index=1
z="O"
r=[ 0.156743, -0.152502, 0.317676 ]
charge=-0.82
[[atoms]]
index=2
z="H"
r=[-0.257130, 0.637822, 0.198948]
charge=0.41
[[atoms]]
index=3
z="H"
r=[0.100387, -0.485320, -0.516624]
charge=0.41
[[bonds]]
i=1
j=2
form="Harmonic"
parameters=[4431.53, 1.012]
[[bonds]]
i=1
j=3
form="Harmonic"
parameters=[4431.53, 1.012]
[[angles]]
i=2
j=1
k=3
form="Harmonic"
parameters=[317.5656, 113.24]
)");
    ASSERT_EQ(s->run(), NodeConstants::ProcessResult::Success);
    auto result = s->findOutput("Species")->get<const Species *>();

    ASSERT_EQ(result->atoms().size(), 3);
    ASSERT_EQ(result->atom(0).charge(), -0.82);
    ASSERT_EQ(result->atom(1).r().x, -0.257130);

    ASSERT_EQ(result->bonds().size(), 2);
    ASSERT_EQ(result->angles().size(), 1);
}

}; // namespace UnitTest
