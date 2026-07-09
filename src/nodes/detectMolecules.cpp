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

/*
 * Definition
 */

std::string_view DetectMoleculesNode::type() const { return "DetectMolecules"; }

std::string_view DetectMoleculesNode::summary() const { return "Detect molecular instances within a structure"; }

/*
 * Processing
 */

// Run main processing
NodeConstants::ProcessResult DetectMoleculesNode::process()
{
    detectedStructures_.clear();

    // Unfold structure
    inputStructure_.unFold();

    // Return all discovered molecular fragment index vectors
    auto fragmentMap = findMolecularFragments(inputStructure_);

    // Define lambda for capturing any molecular instances we detect
    auto appendInstances = [&](std::vector<std::vector<Vector3>> &instances, Structure &detectedMolecularStructure,
                               const std::vector<int> &fragment) -> bool
    {
        if (!instances.empty())
        {
            detectedStructures_.emplace_back(copyStructureAtomsAndBonds(inputStructure_, detectedMolecularStructure, fragment))
                .instances() = instances;

            return true;
        }
        return false;
    };

    // Try selecting within the fragment from the first atom - if this captures all atoms we have a bound framework...
    if (fragmentMap.contains(inputStructure_.nAtoms()))
        return error(
            "Can't create molecular definitions since this unit cell appears to be a continuous framework/network. Consider "
            "adjusting the bonding options in order to generate molecular fragments.\n");

    std::set<const StructureAtom *> atomMask;

    for (const auto &[size, fragments] : fragmentMap)
        for (const auto &fragment : fragments)
        {
            // Create a provisional structure for the detected fragment
            Structure detectedStructure;
            detectedStructure.createBox(inputStructure_.box().axes());
            std::vector<std::vector<Vector3>> instances;

            // Get fragment atoms
            auto fragmentAtoms = getFragmentAtoms(inputStructure_, fragment);

            // Remove fragments that are larger than 50 % of the structure
            if (size * 2 > inputStructure_.nAtoms())
            {
                addInstance(instances.emplace_back(), fragmentAtoms);

                // Mask these fragment atoms
                for (const auto &unmasked : fragmentAtoms)
                    atomMask.insert(unmasked);

                appendInstances(instances, detectedStructure, fragment);

                break;
            }

            for (const auto &fragmentAtom : fragmentAtoms)
            {
                if (atomMask.contains(fragmentAtom))
                    continue;

                /*
                 * Best NETA definition
                 */

                // Set up the return value and bind its contents
                NETADefinition bestNETA;
                std::vector<StructureAtom *> rootAtoms;

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
                std::vector<StructureAtom *> currentRootAtoms;
                for (auto fragmentAtomIndex : fragment)
                {
                    const auto fragmentAtom = inputStructure_.atom(fragmentAtomIndex);
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

                // Get all atoms belonging to fragments from the same fragment size group
                auto fragmentSizeGroupAtoms = getFragmentAtoms(inputStructure_, fragments);

                // Iterate over all structural atoms, matching their unit cell atoms by NETA
                std::vector<std::set<const AtomBase *>> matchedUnitCellAtomSets;
                for (const auto &fragmentAtom : fragmentSizeGroupAtoms)
                {
                    if (atomMask.contains(fragmentAtom))
                        continue;

                    auto matchedPath = neta.matchedPath(fragmentAtom).set();
                    if (!matchedPath.empty())
                    {
                        auto set = matchedUnitCellAtomSets.emplace_back(matchedPath);

                        // Mask the current matched fragment atom
                        for (const auto &matchedAtom : set)
                            atomMask.insert(static_cast<const StructureAtom *>(matchedAtom));
                    }
                }

                // Loop over matched unit cell atoms, retrieving instances
                for (const auto &matchedUnitCellAtoms : matchedUnitCellAtomSets)
                {
                    if (matchedUnitCellAtoms.empty())
                        continue;

                    addInstance(instances.emplace_back(), matchedUnitCellAtoms);
                }
            }

            appendInstances(instances, detectedStructure, fragment);
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
    for (int i = 0; i < detectedStructures_.size(); i++)
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

/*
 * Helpers
 */

// Copy atom and bond information from one structure to another
Structure &DetectMoleculesNode::copyStructureAtomsAndBonds(const Structure &source, Structure &target,
                                                           const std::vector<int> fragmentAtomIndices)
{
    // Copy fragment atoms, forming a map of the original indices to the new atom in the structure
    std::map<int, StructureAtom *> originalIndexMap;
    for (auto fragAtomIndex : fragmentAtomIndices)
    {
        const auto fragmentAtom = source.atom(fragAtomIndex);
        originalIndexMap[fragAtomIndex] = target.addAtom(fragmentAtom->Z(), fragmentAtom->r(), fragmentAtom->q());
        std::cout << std::format("New atom added to structure: {}  {}\n", fragAtomIndex, Elements::symbol(fragmentAtom->Z()));
    }

    // Copy bond information - since our fragment is by definition a bound fragment, we copy all bonds on each atom
    for (auto fragAtomIndex : fragmentAtomIndices)
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

// Add fragment molecular instance
void DetectMoleculesNode::addInstance(std::vector<Vector3> &targetInstance, const AtomCollection &instanceFragmentAtoms)
{
    std::visit(
        [&](const auto &atoms)
        {
            for (const AtomBase *atom : atoms)
                targetInstance.push_back(atom->r());
        },
        instanceFragmentAtoms);
}

// Get fragment atoms from either a single set of fragment indices, or in its overloaded form, a vector of fragments
std::vector<const StructureAtom *> DetectMoleculesNode::getFragmentAtoms(const Structure &structure,
                                                                         const std::vector<int> &fragmentIndices)
{
    std::vector<const StructureAtom *> fragmentAtoms;
    for (const auto &fragmentAtomIndex : fragmentIndices)
        fragmentAtoms.push_back(structure.atom(int(fragmentAtomIndex)));

    return fragmentAtoms;
}

// Get fragment atoms from either a single set of fragment indices, or in its overloaded form, a vector of fragments
std::vector<const StructureAtom *> DetectMoleculesNode::getFragmentAtoms(const Structure &structure,
                                                                         const FragmentVector &fragmentIndices)
{
    std::vector<int> indices;
    std::size_t newSize = 0;
    for (const auto &v : fragmentIndices)
        ++newSize;
    indices.reserve(newSize);
    for (const auto &v : fragmentIndices)
        indices.insert(indices.end(), v.begin(), v.end());

    return getFragmentAtoms(structure, indices);
}

// Find all molecular fragments
std::map<int, DetectMoleculesNode::FragmentVector> DetectMoleculesNode::findMolecularFragments(const Structure &structure)
{
    std::map<int, FragmentVector> map;

    auto fragment = [structure](int i) { return Fragment<StructureAtom, Bond<StructureAtom>>::get(structure.atoms(), i); };

    for (int i = 0; i < structure.nAtoms(); i++)
    {
        auto element = fragment(i);
        const int size = element.size();
        if (!map.contains(size))
            map.emplace(size, FragmentVector{});
        auto &targetFragments = map[size];
        targetFragments.push_back(element);
    }

    return map;
}