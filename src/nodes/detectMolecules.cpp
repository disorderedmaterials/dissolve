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
 * Processing
 */

// Copy atom and bond information from one structure to another
Structure &DetectMoleculesNode::copyStructureAtomsAndBonds(Structure &target, const std::vector<int> fragmentAtomIndices) const
{
    // Copy fragment atoms, forming a map of the original indices to the new atom in the structure
    std::map<int, StructureAtom *> originalIndexMap;
    for (auto fragAtomIndex : fragmentAtomIndices)
    {
        const auto fragmentAtom = inputStructure_.atom(fragAtomIndex);
        originalIndexMap[fragAtomIndex] = target.addAtom(fragmentAtom->Z(), fragmentAtom->r(), fragmentAtom->q());
    }

    // Copy bond information - since our fragment is by definition a bound fragment, we copy all bonds on each atom
    for (auto fragAtomIndex : fragmentAtomIndices)
    {
        const auto fragmentAtom = inputStructure_.atom(fragAtomIndex);
        for (auto bond : fragmentAtom->bonds())
        {
            // Add a bond between the new atoms in the detected structure (as long as it doesn't already exist)
            if (!target.hasBond(originalIndexMap[bond->i()->index()], originalIndexMap[bond->j()->index()]))
                target.addBond(originalIndexMap[bond->i()->index()], originalIndexMap[bond->j()->index()]);
        }
    }

    return target;
}

// Get all fragments in the structure
std::map<int, DetectMoleculesNode::NETAFragmentVector> DetectMoleculesNode::getFragments() const
{
    std::map<int, NETAFragmentVector> map;
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
        if (!map.contains(size))
            map.emplace(size, NETAFragmentVector{});

        auto &targetFragments = map[size];
        targetFragments.push_back(
            {(size * 2 > inputStructure_.nAtoms()) ? std::optional<NETADefinition>{} : bestNETADefinition(fragmentIndices),
             fragmentIndices});

        // Merge in new indices
        atomsInFragments.merge(std::set<int>(fragmentIndices.begin(), fragmentIndices.end()));
    }

    return map;
}

// Determine best NETA definition for index atoms within a fragment
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
        for (auto &[neta, fragment] : fragments)
        {
            // Make a const copy of the current fragment for later reference
            const auto currentFragment = fragment;

            // Create a provisional structure for the detected fragment
            Structure detectedStructure;
            detectedStructure.createBox(inputStructure_.box().axes());
            std::vector<std::vector<Vector3>> instances;

            if (!neta.has_value())
            {
                // Add instances
                auto &instanceAtoms = instances.emplace_back();
                for (auto &fragmentAtomIndex : currentFragment)
                    instanceAtoms.push_back(inputStructure_.atom(fragmentAtomIndex)->r());

                break;
            }
            else
            {
                // Iterate over all structural atoms, matching their unit cell atoms by NETA
                fragments.erase(
                    std::remove_if(fragments.begin(), fragments.end(),
                                   [this, &instances, &neta, &currentFragment](auto &pair) -> bool
                                   {
                                       auto &[_, fragmentAtomIndices] = pair;

                                       for (const auto &fragmentAtomIndex : fragmentAtomIndices)
                                       {
                                           auto matchedUnitCellAtoms =
                                               neta->matchedPath(this->inputStructure_.atom(fragmentAtomIndex)).set();

                                           if (!matchedUnitCellAtoms.empty())
                                           {
                                               //  Add instances
                                               auto &instanceAtoms = instances.emplace_back();
                                               for (const auto &matchedUnitCellAtom : matchedUnitCellAtoms)
                                                   instanceAtoms.push_back(matchedUnitCellAtom->r());

                                               return true;
                                           }
                                       }

                                       return false;
                                   }),
                    fragments.end());
            }

            if (!instances.empty())
                detectedStructures_.emplace_back(copyStructureAtomsAndBonds(detectedStructure, currentFragment)).instances() =
                    instances;
        }

    message("Detected {} distinct fragment structures:\n\n", detectedStructures_.size());
    message("   ID     N  Species Formula\n");
    auto count = 1;
    for (const auto &structure : detectedStructures_)
        message("  {:3d}  {:4d}  {}\n", count++, structure.instances().size(),
                EmpiricalFormula::formula(structure.atoms(), [](const auto &i) { return i->Z(); }));
    message("");

    /*
     * Dynamic outputs
     */

    // Register dynamic outputs
    for (auto i = 0; i < detectedStructures_.size(); ++i)
    {
        auto val = detectedStructures_[i];
        auto paramName = std::string("DetectedMolecule" + std::format("-{}", i));

        // Check if output already exists - do not add if it does
        if (outputs_.find(paramName) != outputs_.end())
            continue;

        addOutput(paramName, "Detected molecular structure", detectedStructures_[i]);
    }

    return NodeConstants::ProcessResult::Success;
}
