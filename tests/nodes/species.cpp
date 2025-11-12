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
    auto option = s->findOption("Data");
    ASSERT_NE(option, nullptr);

    auto species = option->get<std::shared_ptr<Species>>();
    species->addAtom(Elements::O, {0.156743, -0.152502, 0.317676}, -0.82);
    species->addAtom(Elements::H, {-0.257130, 0.637822, 0.198948}, 0.41);
    species->addAtom(Elements::H, {0.100387, -0.485320, -0.516624}, 0.41);
    species->addBond(0, 1);
    species->addBond(0, 2);
    species->addAngle(1, 0, 2);

    ASSERT_EQ(s->run(), NodeConstants::ProcessResult::Unchanged);
    auto result = s->findOutput("Species")->get<const Species *>();

    ASSERT_EQ(result->atoms().size(), 3);
    ASSERT_EQ(result->atom(0).charge(), -0.82);
    ASSERT_EQ(result->atom(1).r().x, -0.257130);

    ASSERT_EQ(result->bonds().size(), 2);
    ASSERT_EQ(result->angles().size(), 1);
}

}; // namespace UnitTest
