// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/detectMolecules.h"
#include "classes/empiricalFormula.h"
#include "classes/fragment.h"
#include "classes/molecule.h"
#include "classes/species.h"

DetectMoleculesNode::DetectMoleculesNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Structure>("Structure", "Input structure", inputStructure_);
}

std::string_view DetectMoleculesNode::type() const { return "DetectMolecules"; }

std::string_view DetectMoleculesNode::summary() const { return "Detect molecular species within a structure"; }

// Run main processing
NodeConstants::ProcessResult DetectMoleculesNode::process()
{
    detectedStructures_.clear();

    auto fragment = [this](int i)
    { return Fragment<StructureAtom, Bond<StructureAtom>>::get(this->inputStructure_.atoms(), i); };

    // Try selecting within the species from the first atom - if this captures all atoms we have a bound framework...
    if (fragment(0).size() == inputStructure_.nAtoms())
        return error(
            "Can't create molecular definitions since this unit cell appears to be a continuous framework/network. Consider "
            "adjusting the bonding options in order to generate molecular fragments.\n");

    std::vector<bool> atomMask(inputStructure_.nAtoms(), false);

    // Find all molecular species, and their instances
    auto indexIterator = atomMask.begin();
    while (indexIterator != atomMask.end())
    {
        // Select a fragment from the next available index
        auto atomIndex = indexIterator - atomMask.begin();
        auto fragmentIndices = fragment(atomIndex);

        // Create a new structure for the detected fragment
        auto &detectedMolStructure = detectedStructures_.emplace_back();
        detectedMolStructure.createBox(inputStructure_.box().axes());

        // Copy fragment atoms, forming a map of the original indices to the new atom in the structure
        std::map<int, StructureAtom *> originalIndexMap;
        for (auto fragAtomIndex : fragmentIndices)
        {
            const auto fragmentAtom = inputStructure_.atom(fragAtomIndex);
            originalIndexMap[fragAtomIndex] =
                detectedMolStructure.addAtom(fragmentAtom->Z(), fragmentAtom->r(), fragmentAtom->q());
            std::cout << std::format("New atom added to structure: {}  {}\n", fragAtomIndex,
                                     Elements::symbol(fragmentAtom->Z()));
        }

        // Copy bond information - since our fragment is by definition a bound fragment, we copy all bonds on each atom
        for (auto fragAtomIndex : fragmentIndices)
        {
            const auto fragmentAtom = inputStructure_.atom(fragAtomIndex);
            for (auto bond : fragmentAtom->bonds())
            {
                // Add a bond between the new atoms in the detected structure (as long as it doesn't already exist)
                if (!detectedMolStructure.hasBond(originalIndexMap[bond->i()->index()], originalIndexMap[bond->j()->index()]))
                    detectedMolStructure.addBond(originalIndexMap[bond->i()->index()], originalIndexMap[bond->j()->index()]);
            }
        }

        // Find instances of this fragment. For large fragments that represent > 50% of the remaining atoms we don't even
        // attempt to create a NETA definition etc. For cases such as framework species this will speed up detection no end.
        if (fragmentIndices.size() * 2 > inputStructure_.nAtoms())
        {
            // Create an instance of the current fragment
            auto &symmetryAtomCoords = detectedMolStructure.instances().emplace_back();

            for (auto i = 0; i < detectedMolStructure.nAtoms(); ++i)
            {
                auto &r = symmetryAtomCoords.emplace_back();
                r = detectedMolStructure.atom(i)->r();
                atomMask[fragmentIndices[i]] = true;
            }
        }
        else
        {
            // Determine the best NETA definition describing the fragment
            auto &&[bestNETA, rootAtoms] = bestNETADefinition(detectedMolStructure);
            auto best = bestNETADefinitions_.emplace(std::make_unique<NETADefinition>(bestNETA), rootAtoms);

            if (rootAtoms.empty())
                return error("Couldn't generate molecular partitioning for CIF - no suitable NETA definition for the "
                             "fragment could be determined.\n");

            // Find instances of this fragment
            detectedMolStructure.instances() =
                getInstances(detectedMolStructure, atomMask, *best.first->first, best.first->second);

            if (detectedMolStructure.instances().empty())
                return error("No instances found for the detected molecular structure");
        }

        // Search for the next valid starting index
        indexIterator = std::find(std::next(indexIterator), atomMask.end(), false);
    }

    message("Detected {} distinct fragment structures:\n\n", detectedStructures_.size());
    message("   ID     N  Species Formula\n");
    auto count = 1;
    for (const auto &s : detectedStructures_)
        message("  {:3d}  {:4d}  {}\n", count++, s.instances().size(),
                EmpiricalFormula::formula(s.atoms(), [](const auto &i) { return i->Z(); }));
    message("");

    return NodeConstants::ProcessResult::Success;
}

/*
 * Helpers
 */

// Determine the best NETA definition for the supplied species
std::tuple<NETADefinition, std::vector<StructureAtom *>> DetectMoleculesNode::bestNETADefinition(const Structure &structure)
{
    // Set up the return value and bind its contents
    std::tuple<NETADefinition, std::vector<StructureAtom *>> result{NETADefinition(), {}};
    auto &&[bestNETA, rootAtoms] = result;

    // Maintain a set of atoms matched by any NETA description we generate
    std::set<StructureAtom *> alreadyMatched;

    // Loop over species atoms
    for (auto &i : structure.atoms())
    {
        // Skip this atom?
        if (alreadyMatched.find(i.get()) != alreadyMatched.end())
            continue;

        // Create a NETA definition with this atom as the root
        NETADefinition neta;
        neta.create(static_cast<AtomBase *>(i.get()), std::nullopt,
                    Flags<NETADefinition::NETACreationFlags>(NETADefinition::NETACreationFlags::ExplicitHydrogens,
                                                             NETADefinition::NETACreationFlags::IncludeRootElement));

        // Apply this match over the whole species
        std::vector<StructureAtom *> currentRootAtoms;
        for (auto &j : structure.atoms())
        {
            if (neta.matches(j.get()))
            {
                currentRootAtoms.push_back(j.get());
                alreadyMatched.insert(j.get());
            }
        }

        // Is this a better description?
        auto better = false;
        if (rootAtoms.empty() || currentRootAtoms.size() < rootAtoms.size())
            better = true;
        else if (currentRootAtoms.size() == rootAtoms.size())
        {
            // Replace the current match if there are more bonds on the current atom.
            if (i->nBonds() > rootAtoms.front()->nBonds())
                better = true;
        }

        if (better)
        {
            bestNETA = neta;
            rootAtoms = currentRootAtoms;
        }
    }

    return result;
}

// Get instances for the supplied species from the cleaned unit cell
std::vector<std::vector<Vector3>> DetectMoleculesNode::getInstances(const Structure &referenceStructure,
                                                                    std::vector<bool> &atomMask, const NETADefinition &neta,
                                                                    const std::vector<StructureAtom *> &referenceRootAtoms)
{
    if (referenceRootAtoms.empty() || !neta.isValid())
        return {};

    // Loop over atoms in the unit cell - we'll mark any that we select as an instance so we speed things up and avoid
    // duplicates
    const auto &structureAtoms = inputStructure_.atoms();
    std::vector<std::vector<Vector3>> instances;
    auto atomIndexIterator = std::find(atomMask.begin(), atomMask.end(), false);
    while (atomIndexIterator != atomMask.end())
    {
        // Try to match this atom / fragment
        const auto atomIndex = atomIndexIterator - atomMask.begin();
        auto &atom = structureAtoms[atomIndex];
        auto matchedUnitCellAtoms = neta.matchedPath(atom.get()).set();
        if (matchedUnitCellAtoms.empty())
        {
            atomIndexIterator = std::find(std::next(atomIndexIterator), atomMask.end(), false);
            continue;
        }

        // Found a fragment that matches the NETA description - we now create a temporary instance Species which will contain
        // the selected fragment atoms, reassembled into a molecule (i.e. unfolded) and with bonding applied / calculated.
        // We need to copy the unit cell from the crystal so we detect bonds properly.
        Structure instanceStructure;
        instanceStructure.createBox(referenceStructure.box().axisLengths(), referenceStructure.box().axisAngles());
        StructureAtom *rootAtom = nullptr;
        // -- Create structure atoms from those matched in the unit cell by the NETA description.
        for (auto &matchedAtom : matchedUnitCellAtoms)
        {
            auto *newAtom = instanceStructure.addAtom(matchedAtom->Z(), matchedAtom->r(), 0.0);

            // Store the structure atom which corresponds to the root atom (when we find it)
            if (matchedAtom == atom.get())
                rootAtom = newAtom;
        }
        // -- Store the local root atom so we can access its coordinates for the origin translation
        auto &instanceStructureRootAtom = rootAtom;

        for (auto &&[matchedAtom, instanceMolAtom] : zip(matchedUnitCellAtoms, instanceStructure.atoms()))
        {
            instanceMolAtom->setR(matchedAtom->r());
            atomMask[matchedAtom->index()] = true;
        }

        /*
         * Now, we have a root match atom on the current instance and a vector of possible matching sites on the reference
         * species (in referenceRootAtoms). For each of the referenceRootAtoms, try to incrementally select along bonds using
         * basic NETA connectivity.
         */

        // Generate basic NETA descriptions for each atom in the reference and candidate species
        std::map<const StructureAtom *, NETADefinition> referenceAtomNETA;
        for (auto &structureAtom : referenceStructure.atoms())
            referenceAtomNETA[structureAtom.get()] =
                NETADefinition(structureAtom.get(), 1, {NETADefinition::NETACreationFlags::IncludeRootElement});

        std::map<const StructureAtom *, const StructureAtom *> matchMap;
        for (const auto *referenceRootAtom : referenceRootAtoms)
        {
            // The root atom is the starting point
            matchMap = matchAtom(referenceRootAtom, instanceStructureRootAtom, referenceAtomNETA, {});
            if (!matchMap.empty())
                break;
        }

        // Result?
        if (matchMap.empty())
        {
            error("Failed to match connectivity of an instance to the reference molecule.\n");
            return {};
        }
        else if (matchMap.size() != referenceStructure.nAtoms())
        {
            error("Internal error - failed to match connectivity of all atoms within an instance to the reference molecule.\n");
            return {};
        }

        // Create the final instance
        auto &instance = instances.emplace_back();
        for (const auto &[refSpeciesAtom, instanceStructureAtom] : matchMap)
            instance.push_back(instanceStructureAtom->r());

        // Find the next available atom
        atomIndexIterator = std::find(std::next(atomIndexIterator), atomMask.end(), false);
    }

    return instances;
}

// Recursively check NETA description matches between the supplied atoms
std::map<const StructureAtom *, const StructureAtom *>
DetectMoleculesNode::matchAtom(const StructureAtom *referenceAtom, const StructureAtom *instanceAtom,
                               const std::map<const StructureAtom *, NETADefinition> &refNETA,
                               const std::map<const StructureAtom *, const StructureAtom *> &map)
{
    // If the reference atom NETA doesn't match the instance atom we cannot proceed
    if (!refNETA.at(referenceAtom).matches(instanceAtom))
        return {};

    // Check the map to see if we have already associated the reference atom to an instance atom, or if the instance atom
    // is already associated to a different reference atom.
    for (auto &&[mappedRefAtom, mappedInstanceAtom] : map)
    {
        // Found it - double-check to ensure that the current association matches our instance atom. If it does we can return
        // the map as it currently stands. If not we return an empty map to indicate failure.
        if (mappedRefAtom == referenceAtom)
        {
            if (mappedInstanceAtom == instanceAtom)
            {
                return map;
            }
            else
            {
                return {};
            }
        }
        else if (mappedInstanceAtom == instanceAtom)
        {
            return {};
        }
    }

    // Copy the current map, associate our initial pair of atoms and try to extend it
    auto newMap = map;
    newMap[referenceAtom] = instanceAtom;

    // Cycle over bonds on the reference atom and find
    for (const auto &referenceBond : referenceAtom->bonds())
    {
        // Get the reference bond partner
        auto *referenceBondPartner = referenceBond->partner(referenceAtom);

        // Try to find a match over bonds on the instance atom
        std::map<const StructureAtom *, const StructureAtom *> bondResult;
        for (const auto &instanceBond : instanceAtom->bonds())
        {
            // Get the instance bond partner
            auto *instanceBondPartner = instanceBond->partner(instanceAtom);

            // Recurse
            bondResult = matchAtom(referenceBondPartner, instanceBondPartner, refNETA, newMap);
            if (!bondResult.empty())
                break;
        }

        // If we found a suitable match recursing into the bond, store the result into newMap and continue to the next bond.
        // If we didn't find a good match, we return now.
        if (bondResult.empty())
            return {};

        newMap = bondResult;
    }

    // If we get to here then we succeeded, so return the new map
    return newMap;
}

/*
 * Getters
 */

// Output structures
const std::vector<Structure> &DetectMoleculesNode::detectedStructures() const { return detectedStructures_; }