// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/add.h"
#include "generator/box.h"
#include "generator/generator.h"
#include "generator/parameters.h"
#include "generator/select.h"
#include "keywords/node.h"
#include <gtest/gtest.h>
#include <string>

namespace UnitTest
{
TEST(GeneratorTest, Scope)
{
    Generator generator;

    // Select A
    auto selectA = generator.createRootNode<SelectGeneratorNode>("A");
    auto forEachA = selectA->branch()->get();
    EXPECT_TRUE(generator.rootSequence().check());

    // Select B
    auto selectB = forEachA.create<SelectGeneratorNode>("B");
    auto forEachB = selectB->branch()->get();
    EXPECT_TRUE(generator.rootSequence().check());

    // Select C
    auto selectC = generator.createRootNode<SelectGeneratorNode>("C");

    // Node external to generator
    auto selectExternal = std::make_shared<SelectGeneratorNode>();
    selectExternal->setName("X");

    // Valid node keyword argument (in scope)
    EXPECT_TRUE(selectB->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectGeneratorNode>{selectA}));
    EXPECT_TRUE(selectC->keywords().set("ReferenceSite", selectA));

    // Invalid node arguments (out of scope)
    // -- Out of scope (node appears subsequently in sequence)
    EXPECT_FALSE(selectA->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectGeneratorNode>{selectB}));
    // -- Out of scope (node is in a branch in a previous node)
    EXPECT_FALSE(selectC->keywords().set("ReferenceSite", selectB));
    // -- Out of scope (not in same generator)
    EXPECT_FALSE(selectB->keywords().set("ExcludeSameMolecule", ConstNodeVector<SelectGeneratorNode>{selectExternal}));

    // Check removal of bad node references
    // -- Confirm current keyword data
    auto keywordNode = selectC->keywords()
                           .get<std::shared_ptr<const SelectGeneratorNode>, NodeKeyword<SelectGeneratorNode>>("ReferenceSite")
                           .value();
    EXPECT_EQ(keywordNode, selectA);
    // -- Move 'C' so it is before 'A'
    std::swap(generator.rootSequence().sequence()[0], generator.rootSequence().sequence()[1]);
    // -- Validate generator and re-check keyword data
    generator.rootSequence().validateNodeKeywords();
    keywordNode = selectC->keywords()
                      .get<std::shared_ptr<const SelectGeneratorNode>, NodeKeyword<SelectGeneratorNode>>("ReferenceSite")
                      .value();
    EXPECT_EQ(keywordNode, nullptr);
}

TEST(GeneratorTest, Parameters)
{
    Generator generator;

    // Parameters (Small Things)
    auto smallThings = generator.createRootNode<ParametersGeneratorNode>("Small");
    smallThings->addParameter("Egg", 1);
    smallThings->addParameter("Marble", -5);
    smallThings->addParameter("Pin", 11.22);

    // Add (Small)
    auto smallHole = generator.createRootNode<AddGeneratorNode>("SmallHole");

    // Parameters (Big Things)
    auto largeThings = generator.createRootNode<ParametersGeneratorNode>("Large");
    largeThings->addParameter("Bus", 1);
    largeThings->addParameter("MeaningOfLife", 2.0);
    largeThings->addParameter("Family", 30);

    // Add (Big Things)
    auto bigHole = generator.createRootNode<AddGeneratorNode>("BigHole");

    EXPECT_TRUE(generator.rootSequence().check());

    // Check number of available parameters to each Add node
    auto smallVars = smallHole->getParametersInScope();
    EXPECT_EQ(smallVars.size(), 3);
    auto bigVars = bigHole->getParametersInScope();
    EXPECT_EQ(bigVars.size(), 6);

    // Set an equation in A via a NodeValueProxy
    NodeValueProxy simple("20.0");
    NodeValueProxy eggPinFromSmall("1 + Egg * Pin"), eggPinFromBig("1 + Egg * Pin");
    NodeValueProxy eggBus("Egg / Bus"), busMeaning("Bus + MeaningOfLife");
    EXPECT_TRUE(smallHole->keywords().set("Population", simple));
    EXPECT_TRUE(smallHole->keywords().set("Population", eggPinFromSmall));
    EXPECT_TRUE(smallHole->keywords().set("Population", eggPinFromBig));
    EXPECT_FALSE(smallHole->keywords().set("Population", eggBus));
    EXPECT_FALSE(smallHole->keywords().set("Population", busMeaning));
    EXPECT_TRUE(bigHole->keywords().set("Population", eggPinFromSmall));
    EXPECT_TRUE(bigHole->keywords().set("Population", eggPinFromBig));
    EXPECT_TRUE(bigHole->keywords().set("Population", eggBus));
    EXPECT_TRUE(bigHole->keywords().set("Population", busMeaning));
}

} // namespace UnitTest
