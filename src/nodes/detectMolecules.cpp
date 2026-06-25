// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/detectMolecules.h"
#include "classes/empiricalFormula.h"
#include "classes/fragment.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include <algorithm>
#include <format>

DetectMoleculesNode::DetectMoleculesNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Structure>("Structure", "Input structure", inputStructure_);
}

std::string_view DetectMoleculesNode::type() const { return "DetectMolecules"; }

std::string_view DetectMoleculesNode::summary() const { return "Detect molecular species within a structure"; }

std::vector<std::vector<int>> DetectMoleculesNode::findMolecularFragments(const Structure &structure) const
{
    std::vector<std::vector<int>> fragments;

    auto fragment = [structure](int i) { return Fragment<StructureAtom, Bond<StructureAtom>>::get(structure.atoms(), i); };

    for (int i = 0; i < structure.nAtoms(); i++)
        fragments.emplace_back(fragment(i));

    return fragments;
}

// Run main processing
NodeConstants::ProcessResult DetectMoleculesNode::process()
{
    detectedStructures_.clear();

    // Return all discovered molecular fragment index vectors
    auto allFragmentIndices = findMolecularFragments(inputStructure_);

    // Try selecting within the species from the first atom - if this captures all atoms we have a bound framework...
    if (allFragmentIndices[0].size() == inputStructure_.nAtoms())
        return error(
            "Can't create molecular definitions since this unit cell appears to be a continuous framework/network. Consider "
            "adjusting the bonding options in order to generate molecular fragments.\n");

    std::set<const StructureAtom *> atomMask;

    for (int i = 0; i < inputStructure_.nAtoms(); i++)
    {
        const auto structureAtom = inputStructure_.atom(i);
        if (atomMask.contains(structureAtom))
            continue;

        auto fragmentIndices = allFragmentIndices.at(i);

        auto largeFragment = fragmentIndices.size() * 2 > inputStructure_.nAtoms();

        // Find instances of this fragment.
        for (auto fragAtomIndex : fragmentIndices)
        {
            const auto fragmentAtom = inputStructure_.atom(fragAtomIndex);

            // Create a provisional structure for the detected fragment
            Structure detectedMolStructure;
            detectedMolStructure.createBox(inputStructure_.box().axes());
            std::vector<std::vector<Vector3>> instances;

            // Skip NETA definition creation for large fragments
            if (largeFragment)
            {
                if (atomMask.contains(fragmentAtom))
                    continue;

                auto &instance = instances.emplace_back();
                for (auto fragAtomIndex : fragmentIndices)
                {
                    auto fragmentAtom = inputStructure_.atom(fragAtomIndex);
                    instance.push_back(fragmentAtom->r());
                    atomMask.insert(fragmentAtom);

                    // Remove fragments with this size
                    std::erase_if(allFragmentIndices, [&fragmentIndices](const auto &fragment)
                                  { return fragment.size() == fragmentIndices.size(); });
                }
            }
            else
            {
                /*
                 * Best NETA definition
                 */

                // Set up the return value and bind its contents
                NETADefinition bestNETA;
                std::vector<StructureAtom *> rootAtoms;

                // Maintain a set of atoms matched by any NETA description we generate
                std::set<StructureAtom *> alreadyMatched;

                // Skip this atom?
                if (alreadyMatched.find(fragmentAtom) != alreadyMatched.end())
                    continue;

                // Create a NETA definition with this atom as the root
                NETADefinition neta;
                neta.create(static_cast<AtomBase *>(fragmentAtom), std::nullopt,
                            Flags<NETADefinition::NETACreationFlags>(NETADefinition::NETACreationFlags::ExplicitHydrogens,
                                                                     NETADefinition::NETACreationFlags::IncludeRootElement));

                // Apply this match over the whole species
                std::vector<StructureAtom *> currentRootAtoms;
                for (auto fragAtomIndex : fragmentIndices)
                {
                    const auto fragmentAtom = inputStructure_.atom(fragAtomIndex);
                    if (neta.matches(fragmentAtom))
                    {
                        currentRootAtoms.push_back(fragmentAtom);
                        alreadyMatched.insert(fragmentAtom);
                    }
                }

                // Is this a better description?
                auto better = false;
                if (rootAtoms.empty() || currentRootAtoms.size() < rootAtoms.size())
                    better = true;
                else if (currentRootAtoms.size() == rootAtoms.size())
                {
                    // Replace the current match if there are more bonds on the current atom.
                    if (fragmentAtom->nBonds() > rootAtoms.front()->nBonds())
                        better = true;
                }

                if (better)
                {
                    bestNETA = neta;
                    rootAtoms = currentRootAtoms;
                }

                /*
                 * Get instances
                 */

                // Iterate over all structural atoms, matching their unit cell atoms by NETA
                std::vector<std::set<const AtomBase *>> matchedUnitCellAtomSets;
                for (const auto &structureAtom : inputStructure_.atoms())
                {
                    if (atomMask.contains(structureAtom.get()))
                        continue;

                    auto matchedPath = neta.matchedPath(structureAtom.get()).set();
                    if (!matchedPath.empty())
                    {
                        auto set = matchedUnitCellAtomSets.emplace_back(matchedPath);
                        for (const auto &matchedAtom : set)
                            atomMask.insert(static_cast<const StructureAtom *>(matchedAtom));
                    }
                }

                // Loop over matched unit cell atoms, retrieving instances
                for (const auto &matchedUnitCellAtoms : matchedUnitCellAtomSets)
                {
                    if (matchedUnitCellAtoms.empty())
                        continue;

                    auto &instance = instances.emplace_back();
                    for (const auto &matchedAtom : matchedUnitCellAtoms)
                        instance.push_back(matchedAtom->r());
                }
            }

            // Detect structures that have instances
            if (!instances.empty())
                detectedStructures_
                    .emplace_back(copyStructureAtomsAndBonds(inputStructure_, detectedMolStructure, fragmentIndices))
                    .instances() = instances;
        }
    }

    // Unfold all detected structures
    for (auto &structure : detectedStructures_)
        structure.unFold();

    message("Detected {} distinct fragment structures:\n\n", detectedStructures_.size());
    message("   ID     N  Species Formula\n");
    auto count = 1;
    for (const auto &structure : detectedStructures_)
        message("  {:3d}  {:4d}  {}\n", count++, structure.instances().size(),
                EmpiricalFormula::formula(structure.atoms(), [](const auto &i) { return i->Z(); }));
    message("");

    return NodeConstants::ProcessResult::Success;
}

/*
 * Helpers
 */

// Copy atom and bond information from one structure to another
Structure &DetectMoleculesNode::copyStructureAtomsAndBonds(const Structure &source, Structure &target,
                                                           const std::vector<int> fragmentIndices)
{
    // Copy fragment atoms, forming a map of the original indices to the new atom in the structure
    std::map<int, StructureAtom *> originalIndexMap;
    for (auto fragAtomIndex : fragmentIndices)
    {
        const auto fragmentAtom = source.atom(fragAtomIndex);
        originalIndexMap[fragAtomIndex] = target.addAtom(fragmentAtom->Z(), fragmentAtom->r(), fragmentAtom->q());
        std::cout << std::format("New atom added to structure: {}  {}\n", fragAtomIndex, Elements::symbol(fragmentAtom->Z()));
    }

    // Copy bond information - since our fragment is by definition a bound fragment, we copy all bonds on each atom
    for (auto fragAtomIndex : fragmentIndices)
    {
        const auto fragmentAtom = source.atom(fragAtomIndex);
        for (auto bond : fragmentAtom->bonds())
        {
            // Add a bond between the new atoms in the detected structure (as long as it doesn't already exist)
            if (!target.hasBond(originalIndexMap[bond->i()->index()], originalIndexMap[bond->j()->index()]))
                target.addBond(originalIndexMap[bond->i()->index()], originalIndexMap[bond->j()->index()]);
        }
    }

    return target;
}

/*
 * Getters
 */

// Output structures
const std::vector<Structure> &DetectMoleculesNode::detectedStructures() const { return detectedStructures_; }