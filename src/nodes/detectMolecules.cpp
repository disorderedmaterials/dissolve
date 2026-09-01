// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/detectMolecules.h"
#include "classes/empiricalFormula.h"
#include "classes/fragment.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include <algorithm>
#include <format>
#include <iostream>

DetectMoleculesNode::DetectMoleculesNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Structure>("Structure", "Input structure", inputStructure_);
}

/*
 * Definition
 */

std::string_view DetectMoleculesNode::type() const { return "DetectMolecules"; }

std::string_view DetectMoleculesNode::summary() const { return "Detect molecular instances within a structure"; }

/*
 * Data
 */

// Return detected structures
const std::map<std::string, Structure> &DetectMoleculesNode::detectedStructures() const { return detectedStructures_; }

/*
 * Processing
 */

// Duplicate specified atoms (from indices) and their bonds, returning a new structure (including the unit cell)
Structure DetectMoleculesNode::copyAtomsAndBonds(const std::vector<int> &inputStructureAtomIndices) const
{
    Structure structure;

    // Copy fragment atoms, forming a map of the original indices to the new atom in the structure
    std::map<int, StructureAtom *> originalIndexMap;
    for (auto fragAtomIndex : inputStructureAtomIndices)
    {
        const auto fragmentAtom = inputStructure_.atom(fragAtomIndex);
        originalIndexMap[fragAtomIndex] = structure.addAtom(fragmentAtom->Z(), fragmentAtom->r(), fragmentAtom->q());
    }

    // Copy bond information - since our fragment is by definition a bound fragment, we copy all bonds on each atom
    for (auto originalAtomIndex : inputStructureAtomIndices)
    {
        const auto fragmentAtom = inputStructure_.atom(originalAtomIndex);
        for (const auto bond : fragmentAtom->bonds())
        {
            // Add a bond between the new atoms in the detected structure (as long as it doesn't already exist)
            if (!structure.hasBond(originalIndexMap[bond->i()->index()], originalIndexMap[bond->j()->index()]))
                structure.addBond(originalIndexMap[bond->i()->index()], originalIndexMap[bond->j()->index()]);
        }
    }

    return structure;
}

// Get all fragments in the structure
std::map<int, std::vector<std::vector<int>>> DetectMoleculesNode::getFragments() const
{
    std::map<int, std::vector<std::vector<int>>> map;
    std::set<int> atomsInFragments;

    for (auto i = 0; i < inputStructure_.nAtoms(); ++i)
    {
        // Skip this atom if it has already been detected within a fragment
        if (atomsInFragments.contains(i))
            continue;

        // Get the fragment containing this atom index
        auto fragmentIndices = Fragment<StructureAtom, Bond<StructureAtom>>::get(inputStructure_.atoms(), i);

        // Map fragment size to fragment indices
        const auto size = fragmentIndices.size();
        auto &targetFragments = map[size];
        targetFragments.push_back(fragmentIndices);

        // Merge in new indices
        atomsInFragments.merge(std::set<int>(fragmentIndices.begin(), fragmentIndices.end()));
    }

    return map;
}

// Get coordinates of specified atoms of the input structure
std::vector<Vector3> DetectMoleculesNode::getAtomCoordinates(const std::vector<int> &inputStructureAtomIndices) const
{
    std::vector<Vector3> r(inputStructureAtomIndices.size());
    std::ranges::transform(inputStructureAtomIndices, r.begin(), [&](auto index) { return inputStructure_.atom(index)->r(); });
    return r;
}

// Determine best NETA definition for supplied fragment atoms
NETADefinition DetectMoleculesNode::bestNETADefinition(const std::vector<int> &fragmentIndices) const
{
    // Find the best NETA definition for this fragment
    NETADefinition bestNETA;
    std::vector<const StructureAtom *> rootAtoms;

    for (const auto &idx : fragmentIndices)
    {
        auto fragmentAtom = inputStructure_.atom(idx);

        // Maintain a set of atoms matched by any NETA description we generate
        std::set<const StructureAtom *> alreadyMatched;

        // Skip this atom?
        if (alreadyMatched.find(fragmentAtom) != alreadyMatched.end())
            continue;

        // Create a NETA definition with this atom as the root
        NETADefinition neta;
        neta.create(static_cast<const AtomBase *>(fragmentAtom), std::nullopt,
                    Flags<NETADefinition::NETACreationFlags>(NETADefinition::NETACreationFlags::ExplicitHydrogens,
                                                             NETADefinition::NETACreationFlags::IncludeRootElement));

        // Apply this match over the whole fragment
        std::vector<const StructureAtom *> currentRootAtoms;
        for (auto idx : fragmentIndices)
        {
            auto fragmentAtom = inputStructure_.atom(idx);

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
    }

    return bestNETA;
}

// Use the supplied NETA definition on the provided fragment, returning the first match
NETAMatchedGroup DetectMoleculesNode::matchFragment(const NETADefinition &neta, const std::vector<int> &fragmentAtoms) const
{
    for (auto index : fragmentAtoms)
    {
        auto matchedGroup = neta.matchedPath(inputStructure_.atom(index));
        if (!matchedGroup.matched().empty())
            return matchedGroup;
    }

    return {};
}

// Run main processing
NodeConstants::ProcessResult DetectMoleculesNode::process()
{
    detectedStructures_.clear();

    // Unfold structure
    inputStructure_.unFold();

    // Return all discovered molecular fragment index vectors
    auto fragmentMap = getFragments();

    // Check for a single, bound framework fragment
    if (fragmentMap.contains(inputStructure_.nAtoms()))
        return error(
            "Can't create molecular definitions since this unit cell appears to be a continuous framework/network. Consider "
            "adjusting the bonding options in order to generate molecular fragments.\n");

    for (auto &[_, fragments] : fragmentMap)
    {
        // If there is a single fragment for this size, no NETA is required and we can just store it
        if (fragments.size() == 1)
        {
            auto structure = copyAtomsAndBonds(fragments.front());
            structure.instances().push_back(getAtomCoordinates(fragments.front()));

            detectedStructures_[EmpiricalFormula::formula(structure.atoms(), [](const auto &i) { return i->Z(); })] = structure;

            continue;
        }

        // Loop over fragments of this size
        while (!fragments.empty())
        {
            // Get frontmost fragment and create the best NETA definition for it
            const auto &currentFragment = fragments.front();
            auto neta = bestNETADefinition(currentFragment);

            // Apply the NETA match back over the fragement in order to get the matched atom ordering, and create a structure
            auto netaMatch = matchFragment(neta, currentFragment);
            std::vector<int> netaOrdering(netaMatch.matched().size());
            std::ranges::transform(netaMatch.matched(), netaOrdering.begin(), [](auto atom) { return atom->index(); });

            // Create a provisional structure for the current fragment, using indices in the order matched by NETA
            auto detectedStructure = copyAtomsAndBonds(netaOrdering);
            detectedStructure.box() = Box(inputStructure_.box().axes());

            // Find, copy as instances, and then erase all fragments that match the current NETA
            fragments.erase(std::remove_if(fragments.begin(), fragments.end(),
                                           [&](auto &fragment) -> bool
                                           {
                                               // Attempt to match this fragment
                                               auto fragmentMatch = matchFragment(neta, fragment);
                                               if (fragmentMatch.matched().empty())
                                                   return false;

                                               std::cout << "Fragment:\n";
                                               // Store this match as an instance
                                               auto &instanceAtoms = detectedStructure.instances().emplace_back();
                                               for (const auto fragmentAtom : fragmentMatch.matched())
                                               {
                                                   std::cout << std::format(
                                                       "{} @ {} {} {}\n", Elements::symbol(fragmentAtom->Z()),
                                                       fragmentAtom->r().x, fragmentAtom->r().y, fragmentAtom->r().z);
                                                   instanceAtoms.push_back(fragmentAtom->r());
                                               }

                                               return true;
                                           }),
                            fragments.end());

            // Store the detected structure (under a unique name)
            auto name = DissolveSys::uniqueName(
                EmpiricalFormula::formula(detectedStructure.atoms(), [](const auto &i) { return i->Z(); }), detectedStructures_,
                [](const auto &pair) { return pair.first; });
            detectedStructures_[name] = detectedStructure;
        }
    }

    message("Detected {} distinct fragment structures:\n\n", detectedStructures_.size());
    message("   ID     N  Name / Empirical Formula\n");
    auto count = 1;
    for (const auto &[name, structure] : detectedStructures_)
        message("  {:3d}  {:4d}  {}\n", count++, structure.instances().size(), name);
    message("");

    // Register dynamic outputs
    for (auto &[name, structure] : detectedStructures_)
    {
        // Update output data if it already exists, otherwise create new
        auto outputIt = outputs_.find(name);
        if (outputIt == outputs_.end())
            addOutput(name, "Detected molecular structure", structure);
        else
        {
            // TODO Need to update the underlying ParameterBase here, but how?
        }
    }

    return NodeConstants::ProcessResult::Success;
}
