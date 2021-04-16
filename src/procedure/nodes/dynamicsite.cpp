// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/dynamicsite.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/molecule.h"
#include "classes/site.h"
#include "classes/species.h"
#include "data/elements.h"
#include "keywords/types.h"
#include "procedure/nodes/select.h"

DynamicSiteProcedureNode::DynamicSiteProcedureNode(SelectProcedureNode *parent)
    : ProcedureNode(ProcedureNode::NodeType::DynamicSite)
{
    parent_ = parent;

    keywords_.add("Definition", new AtomTypeRefListKeyword(atomTypes_), "AtomType",
                  "Define one or more AtomTypes to include in this site");
    keywords_.add("Definition", new ElementVectorKeyword(elements_), "Element",
                  "Define one or more Elements to include in this site");
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool DynamicSiteProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

// Return whether a name for the node must be provided
bool DynamicSiteProcedureNode::mustBeNamed() const { return false; }

/*
 * Site Generation
 */

// Generate sites from the specified Molecule
void DynamicSiteProcedureNode::generateSites(std::shared_ptr<const Molecule> molecule)
{
    // Loop over Atoms in the Molecule
    for (auto n = 0; n < molecule->nAtoms(); ++n)
    {
        // If the element is listed in our target elements list, add this atom as a site
        if (std::find(elements_.begin(), elements_.end(), molecule->atom(n)->speciesAtom()->Z()) != elements_.end())
        {
            generatedSites_.add(Site(molecule, molecule->atom(n)->r()));
            continue;
        }

        // If the Atom's AtomType is listed in our target AtomType list, add this atom as a site
        auto it = std::find(atomTypes_.begin(), atomTypes_.end(), molecule->atom(n)->speciesAtom()->atomType());
        if (atomTypes_.end() != it)
        {
            generatedSites_.add(Site(molecule, molecule->atom(n)->r()));
            continue;
        }
    }
}

// Return Array of generated sites
const Array<Site> &DynamicSiteProcedureNode::generatedSites() const { return generatedSites_; }

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
bool DynamicSiteProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                       GenericList &targetList)
{
    // Clear our current list of sites
    generatedSites_.clear();

    // Grab exclusion lists and any specific Molecule parent
    const auto &excludedMolecules = parent_->excludedMolecules();
    std::shared_ptr<const Molecule> moleculeParent = parent_->sameMoleculeMolecule();

    /*
     * We'll loop over all Molecules in the Configuration, rather than all Atoms, since there are useful exclusions we can
     * make based on the parent Molecule. If, however, a sameMolecule_ is defined then we can simply grab this Molecule and
     * do the checks within it, rather than looping. Both use the local function generateSites(std::shared_ptr<Molecule>) in
     * order to extract site information.
     */

    if (moleculeParent)
        generateSites(moleculeParent);
    else
    {
        // Loop over Molecules in the target Configuration
        std::deque<std::shared_ptr<Molecule>> &molecules = cfg->molecules();
        for (auto molecule : molecules)
        {
            // Check Molecule exclusions
            if (std::find(excludedMolecules.begin(), excludedMolecules.end(), molecule) != excludedMolecules.end())
                continue;

            // All OK, so generate sites
            generateSites(molecule);
        }
    }

    return true;
}
