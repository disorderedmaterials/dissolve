// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/empiricalFormula.h"
#include "io/import/species.h"
#include "nodes/cifLoader.h"
#include "nodes/cifMolecularSpecies.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <string>

namespace UnitTest
{
class CIFNodeTest : public ::testing::Test
{
    public:
    CIFNodeTest() = default;
    ~CIFNodeTest() = default;

    protected:
    TestGraph testGraph_;
    const std::string delimiter_{".cif"};
    const std::string path_{"cif/"};

    public:
    // Molecular species information
    using MolecularSpeciesInfo = std::tuple<std::string, int, int>;
    // Create CIF graph
    void createGraph(std::string filename)
    {
        auto name = cifNameFromFile(filename);
        EXPECT_TRUE(testGraph_.nextNode("CIFLoader", name));
        testGraph_.fetchHead()->setOption("FilePath", path_ + filename);
        EXPECT_TRUE(testGraph_.nextNode("CIFBondingOptions", name + "//BondingOptions"));
        EXPECT_TRUE(testGraph_.nextNode("CIFRemoveAtomic", name + "//RemoveAtomic"));
        EXPECT_TRUE(testGraph_.nextNode("CIFRemoveWater", name + "//RemoveWater"));
        EXPECT_TRUE(testGraph_.nextNode("CIFStructureCleanup", name + "//StructureCleanup"));
        EXPECT_TRUE(testGraph_.nextNode("CIFMolecularSpecies", name + "//MolecularSpecies"));
        testGraph_.addEdge({name, "CIFContext", name + "//BondingOptions", "CIFContext"});
        testGraph_.addEdge({name + "//BondingOptions", "CIFContext", name + "//RemoveAtomic", "CIFContext"});
        testGraph_.addEdge({name + "//RemoveAtomic", "CIFContext", name + "//RemoveWater", "CIFContext"});
        testGraph_.addEdge({name + "//RemoveWater", "CIFContext", name + "//StructureCleanup", "CIFContext"});
        testGraph_.addEdge({name + "//StructureCleanup", "CIFContext", name + "//MolecularSpecies", "CIFContext"});
    }
    // Determine CIF node name from filename
    std::string cifNameFromFile(std::string filename)
    {
        auto name = filename.substr(0, filename.find(delimiter_));
        return name;
    }
    // Retrieve CIF context by filename
    CIFLoaderNode::CIFContext *getContextByFileName(std::string filename)
    {
        auto name = cifNameFromFile(filename);
        auto node = testGraph_.findNode(name);
        auto context = node->getOutputValue<CIFLoaderNode::CIFContext *>("CIFContext");
        return context;
    }
    // Test Box definition
    void testBox(const Configuration *cfg, const Vector3 &lengths, const Vector3 &angles, int nAtoms)
    {
        ASSERT_TRUE(cfg);
        EXPECT_EQ(cfg->nAtoms(), nAtoms);
        EXPECT_NEAR(cfg->box()->axisLengths().x, lengths.x, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisLengths().y, lengths.y, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisLengths().z, lengths.z, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisAngles().x, angles.x, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisAngles().y, angles.y, 1.0e-6);
        EXPECT_NEAR(cfg->box()->axisAngles().z, angles.z, 1.0e-6);
    }
    // Test molecular species information provided
    void testMolecularSpecies(const CIFMolecularSpecies &molSp, const MolecularSpeciesInfo &info)
    {
        EXPECT_EQ(molSp.species()->name(), std::get<0>(info));
        EXPECT_EQ(molSp.instances().size(), std::get<1>(info));
        EXPECT_EQ(molSp.species()->nAtoms(), std::get<2>(info));
    }
    // Check instance consistency with reference coordinates
    void testInstanceConsistency(const CIFMolecularSpecies &molSp, const Species &referenceCoordinates)
    {
        // Get the box from the reference species
        const auto *box = referenceCoordinates.box();

        // Loop over instances and ensure their stored atoms overlap exactly with one in the reference system
        for (const auto &instance : molSp.instances())
        {
            for (auto &&[instanceAtom, speciesAtom] : zip(instance.localAtoms(), molSp.species()->atoms()))
            {
                // Locate the atom in the reference system at the instance atom coordinates
                auto instanceR = instanceAtom.r();
                auto spAtomIt = std::find_if(referenceCoordinates.atoms().begin(), referenceCoordinates.atoms().end(),
                                             [box, instanceR](const auto &refAtom)
                                             { return box->minimumDistance(refAtom.r(), instanceR) < 0.01; });
                std::cout << std::format("{}  {} {} {}", Elements::symbol(speciesAtom.Z()), instanceAtom.r().x,
                                         instanceAtom.r().y, instanceAtom.r().z)
                          << std::endl;
                ASSERT_NE(spAtomIt, referenceCoordinates.atoms().end());
                EXPECT_EQ(spAtomIt->Z(), speciesAtom.Z());
            }
        }
    }
};

TEST_F(CIFNodeTest, Parse)
{
    // Test files
    std::vector<std::string> cifs = {"1557470.cif", "1557599.cif", "7705246.cif", "9000004.cif", "9000095.cif", "9000418.cif"};

    for (auto &cif : cifs)
    {
        createGraph(cif);
        ASSERT_EQ(testGraph_.findNode(cifNameFromFile(cif))->run(), NodeConstants::ProcessResult::Success);
    }
}

TEST_F(CIFNodeTest, NaCl)
{
    // Load the CIF file
    auto cif = "NaCl-1000041.cif";
    createGraph(cif);
    auto loaderNode = testGraph_.findNode(cifNameFromFile(cif));
    ASSERT_EQ(loaderNode->run(), NodeConstants::ProcessResult::Success);

    auto cifContext = getContextByFileName(cif);
    ASSERT_TRUE(cifContext);
    EXPECT_TRUE(cifContext->generate());

    // Check basic info
    auto molecularSpeciesNode = testGraph_.findNode(cifNameFromFile(cif) + "//MolecularSpecies");
    ASSERT_EQ(molecularSpeciesNode->run(), NodeConstants::ProcessResult::Success);

    EXPECT_EQ(cifContext->spaceGroup(), SpaceGroups::SpaceGroup_225);
    constexpr double A = 5.62;
    testBox(molecularSpeciesNode->getOutputValue<Configuration *>("SupercellConfiguration"), {A, A, A}, {90, 90, 90}, 8);

    // Calculating bonding is the default, but this gives a continuous framework...
    EXPECT_EQ(molecularSpeciesNode->getOutputValue<std::vector<CIFMolecularSpecies>>("DetectedMolecularSpecies").size(), 0);

    // Get molecular species
    auto bondingNode = testGraph_.findNode(cifNameFromFile(cif) + "//BondingOptions");
    bondingNode->setOption("UseCIFBondingDefinitions", true);
    testGraph_.setUpdateRequired();
    ASSERT_EQ(molecularSpeciesNode->run(), NodeConstants::ProcessResult::Success);

    auto molecularSpecies = molecularSpeciesNode->getOutputValue<std::vector<CIFMolecularSpecies>>("DetectedMolecularSpecies");

    EXPECT_EQ(molecularSpecies.size(), 2);
    testMolecularSpecies(molecularSpecies.at(0), {"Na", 4, 1});
    std::vector<Vector3> R = {{0.0, 0.0, 0.0}, {0.0, A / 2, A / 2}, {A / 2, 0.0, A / 2}, {A / 2, A / 2, 0.0}};
    for (auto &&[instance, r2] : zip(molecularSpecies.at(0).instances(), R))
        DissolveSystemTest::checkVec3(instance.localAtoms()[0].r(), r2);
    testMolecularSpecies(molecularSpecies.at(1), {"Cl", 4, 1});
    for (auto &&[instance, r2] : zip(molecularSpecies.at(1).instances(), R))
        DissolveSystemTest::checkVec3(instance.localAtoms()[0].r(), (r2 - A / 2).abs());

    // 2x2x2 supercell
    molecularSpeciesNode->setOption<Vector3i>("SupercellRepeat", {2, 2, 2});
    testGraph_.dissolveGraph()->setUpdateRequired();
    ASSERT_EQ(molecularSpeciesNode->run(), NodeConstants::ProcessResult::Success);
    testBox(molecularSpeciesNode->getOutputValue<Configuration *>("SupercellConfiguration"), {A * 2, A * 2, A * 2},
            {90, 90, 90}, 8 * 8);
}

TEST_F(CIFNodeTest, NaClO3)
{
    // Load the CIF file
    auto cif = "NaClO3-1010057.cif";
    createGraph(cif);
    ASSERT_EQ(testGraph_.findNode(cifNameFromFile(cif))->run(), NodeConstants::ProcessResult::Success);

    auto cifContext = getContextByFileName(cif);
    ASSERT_TRUE(cifContext);
    EXPECT_TRUE(cifContext->generate());

    // Check basic info
    auto molecularSpeciesNode = testGraph_.findNode(cifNameFromFile(cif) + "//MolecularSpecies");
    ASSERT_EQ(molecularSpeciesNode->run(), NodeConstants::ProcessResult::Success);

    EXPECT_EQ(cifContext->spaceGroup(), SpaceGroups::SpaceGroup_198);
    constexpr double A = 6.55;
    testBox(molecularSpeciesNode->getOutputValue<Configuration *>("SupercellConfiguration"), {A, A, A}, {90, 90, 90}, 20);

    // Turn off automatic bond calculation - there are no bonding defs in the CIF, so we expect species for each atomic
    // component (4 Na, 4 Cl, and 12 O)
    auto bondingNode = testGraph_.findNode(cifNameFromFile(cif) + "//BondingOptions");
    bondingNode->setOption("UseCIFBondingDefinitions", true);
    testGraph_.setUpdateRequired();
    ASSERT_EQ(molecularSpeciesNode->run(), NodeConstants::ProcessResult::Success);

    auto cifMolsA = molecularSpeciesNode->getOutputValue<std::vector<CIFMolecularSpecies>>("DetectedMolecularSpecies");
    ASSERT_EQ(cifMolsA.size(), 3);
    testMolecularSpecies(cifMolsA.at(0), {"Na", 4, 1});
    testMolecularSpecies(cifMolsA.at(1), {"Cl", 4, 1});
    testMolecularSpecies(cifMolsA.at(2), {"O", 12, 1});

    // Calculate bonding ourselves to get the correct species
    bondingNode->setOption("UseCIFBondingDefinitions", false);
    testGraph_.setUpdateRequired();
    ASSERT_EQ(molecularSpeciesNode->run(), NodeConstants::ProcessResult::Success);
    auto cifMolsB = molecularSpeciesNode->getOutputValue<std::vector<CIFMolecularSpecies>>("DetectedMolecularSpecies");
    ASSERT_EQ(cifMolsB.size(), 2);
    testMolecularSpecies(cifMolsB.at(0), {"Na", 4, 1});
    testMolecularSpecies(cifMolsB.at(1), {"ClO3", 4, 4});
}

TEST_F(CIFNodeTest, CuBTC)
{
    // Load the CIF file
    auto cif = "CuBTC-7108574.cif";
    createGraph(cif);
    ASSERT_EQ(testGraph_.findNode(cifNameFromFile(cif))->run(), NodeConstants::ProcessResult::Success);

    auto cifContext = getContextByFileName(cif);
    ASSERT_TRUE(cifContext);
    EXPECT_TRUE(cifContext->generate());

    // Check basic info
    auto molecularSpeciesNode = testGraph_.findNode(cifNameFromFile(cif) + "//MolecularSpecies");
    ASSERT_EQ(molecularSpeciesNode->run(), NodeConstants::ProcessResult::Success);

    EXPECT_EQ(cifContext->spaceGroup(), SpaceGroups::SpaceGroup_225);
    constexpr auto A = 26.3336;
    testBox(molecularSpeciesNode->getOutputValue<Configuration *>("SupercellConfiguration"), {A, A, A}, {90, 90, 90}, 672);

    // 16 basic formula units per unit cell
    constexpr auto N = 16;

    // Check basic formula (which includes bound water oxygens - with no H - at this point) and using O group
    EmpiricalFormula::EmpiricalFormulaMap cellFormulaH = {
        {Elements::Cu, 3 * N}, {Elements::C, 18 * N}, {Elements::H, 6 * N}, {Elements::O, 15 * N}};
    EXPECT_EQ(
        EmpiricalFormula::formula(molecularSpeciesNode->getOutputValue<Configuration *>("SupercellConfiguration")->atoms(),
                                  [](const auto &i) { return i.speciesAtom()->Z(); }),
        EmpiricalFormula::formula(cellFormulaH));
    auto cifMolsA = molecularSpeciesNode->getOutputValue<std::vector<CIFMolecularSpecies>>("DetectedMolecularSpecies");
    EXPECT_EQ(cifMolsA.size(), 2);

    // Change active assemblies to get amine-substituted structure
    EmpiricalFormula::EmpiricalFormulaMap cellFormulaNH2 = cellFormulaH;
    cellFormulaNH2[Elements::N] = 6 * N;
    cellFormulaNH2[Elements::H] *= 2;
    EXPECT_TRUE(testGraph_.nextNode("SetCIFAtomGroupActivity", cifNameFromFile(cif) + "//AtomGroupA1"));
    testGraph_.fetchHead()->setOption("Assembly", std::string("A"));
    testGraph_.fetchHead()->setOption("AtomGroup", std::string("1"));
    testGraph_.fetchHead()->setOption("SetActive", false);
    EXPECT_TRUE(testGraph_.nextNode("SetCIFAtomGroupActivity", cifNameFromFile(cif) + "//AtomGroupB2"));
    testGraph_.fetchHead()->setOption("Assembly", std::string("B"));
    testGraph_.fetchHead()->setOption("AtomGroup", std::string("2"));
    testGraph_.fetchHead()->setOption("SetActive", true);
    EXPECT_TRUE(testGraph_.nextNode("SetCIFAtomGroupActivity", cifNameFromFile(cif) + "//AtomGroupC2"));
    testGraph_.fetchHead()->setOption("Assembly", std::string("C"));
    testGraph_.fetchHead()->setOption("AtomGroup", std::string("2"));
    testGraph_.fetchHead()->setOption("SetActive", true);
    testGraph_.removeEdge(
        {cifNameFromFile(cif) + "//StructureCleanup", "CIFContext", std::string(molecularSpeciesNode->name()), "CIFContext"});
    testGraph_.addEdge(
        {cifNameFromFile(cif) + "//StructureCleanup", "CIFContext", cifNameFromFile(cif) + "//AtomGroupA1", "CIFContext"});
    testGraph_.addEdge(
        {cifNameFromFile(cif) + "//AtomGroupA1", "CIFContext", cifNameFromFile(cif) + "//AtomGroupB2", "CIFContext"});
    testGraph_.addEdge(
        {cifNameFromFile(cif) + "//AtomGroupB2", "CIFContext", cifNameFromFile(cif) + "//AtomGroupC2", "CIFContext"});
    testGraph_.addEdge(
        {cifNameFromFile(cif) + "//AtomGroupC2", "CIFContext", std::string(molecularSpeciesNode->name()), "CIFContext"});
    testGraph_.setUpdateRequired();
    ASSERT_EQ(molecularSpeciesNode->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(
        EmpiricalFormula::formula(molecularSpeciesNode->getOutputValue<Configuration *>("SupercellConfiguration")->atoms(),
                                  [](const auto &i) { return i.speciesAtom()->Z(); }),
        EmpiricalFormula::formula(cellFormulaNH2));

    // Remove those free oxygens so we just have a framework
    auto removeAtomicsNode = testGraph_.findNode(cifNameFromFile(cif) + "//RemoveAtomic");
    removeAtomicsNode->setOption("RemoveAtomics", true);
    testGraph_.setUpdateRequired();
    ASSERT_EQ(molecularSpeciesNode->run(), NodeConstants::ProcessResult::Success);
    auto cifMolsB = molecularSpeciesNode->getOutputValue<std::vector<CIFMolecularSpecies>>("DetectedMolecularSpecies");
    EXPECT_EQ(cifMolsB.size(), 0);
}

TEST_F(CIFNodeTest, MoleculeOrdering)
{
    const auto cifFiles = {"molecule-test-simple-ordered.cif", "molecule-test-simple-unordered.cif",
                           "molecule-test-simple-unordered-rotated.cif"};
    for (auto cifFile : cifFiles)
    {
        // Load the CIF file
        createGraph(cifFile);
        ASSERT_EQ(testGraph_.findNode(cifNameFromFile(cifFile))->run(), NodeConstants::ProcessResult::Success);

        auto cifContext = getContextByFileName(cifFile);
        ASSERT_TRUE(cifContext);
        EXPECT_TRUE(cifContext->generate());

        auto molecularSpeciesNode = testGraph_.findNode(cifNameFromFile(cifFile) + "//MolecularSpecies");
        ASSERT_EQ(molecularSpeciesNode->run(), NodeConstants::ProcessResult::Success);

        auto molecularSpecies =
            molecularSpeciesNode->getOutputValue<std::vector<CIFMolecularSpecies>>("DetectedMolecularSpecies");
        EXPECT_EQ(molecularSpecies.size(), 1);

        auto &cifMolecule = molecularSpecies.front();
        EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {
            {Elements::Cl, 1}, {Elements::O, 1}, {Elements::C, 1}, {Elements::H, 3}};
        testMolecularSpecies(cifMolecule, {EmpiricalFormula::formula(moleculeFormula), 6, 6});

        auto &unitCellSpecies = static_cast<CIFMolecularSpeciesNode *>(molecularSpeciesNode)->cleanedUnitCellSpecies();
        testInstanceConsistency(cifMolecule, unitCellSpecies);
    }
}

TEST_F(CIFNodeTest, BigMoleculeOrdering)
{
    const auto cifFile = "Bisphen_n_arenes_1517789.cif";
    createGraph(cifFile);
    ASSERT_EQ(testGraph_.findNode(cifNameFromFile(cifFile))->run(), NodeConstants::ProcessResult::Success);

    auto cifContext = getContextByFileName(cifFile);
    ASSERT_TRUE(cifContext);
    EXPECT_TRUE(cifContext->generate());

    auto molecularSpeciesNode = testGraph_.findNode(cifNameFromFile(cifFile) + "//MolecularSpecies");
    ASSERT_EQ(molecularSpeciesNode->run(), NodeConstants::ProcessResult::Success);

    auto molecularSpecies = molecularSpeciesNode->getOutputValue<std::vector<CIFMolecularSpecies>>("DetectedMolecularSpecies");
    EXPECT_EQ(molecularSpecies.size(), 1);

    auto &cifMolecule = molecularSpecies.front();
    EmpiricalFormula::EmpiricalFormulaMap moleculeFormula = {{Elements::O, 6}, {Elements::C, 51}, {Elements::H, 54}};
    testMolecularSpecies(cifMolecule, {EmpiricalFormula::formula(moleculeFormula), 4, 111});

    auto &unitCellSpecies = static_cast<CIFMolecularSpeciesNode *>(molecularSpeciesNode)->cleanedUnitCellSpecies();
    testInstanceConsistency(cifMolecule, unitCellSpecies);
}

} // namespace UnitTest