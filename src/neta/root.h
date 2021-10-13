// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesatom.h"
#include "neta/node.h"
#include <vector>

// Forward Declarations
class Element;
class ForcefieldAtomType;
class NETADefinition;

// NETA Root Node
class NETARootNode : public NETANode
{
    public:
    NETARootNode(NETADefinition *parent);
    ~NETARootNode() override = default;

    /*
     * Options
     */
    private:
    // Atom Geometry
    SpeciesAtom::AtomGeometry geometry_;
    // Geometry comparison operator
    NETANode::ComparisonOperator geometryOperator_;

    public:
    // Available options
    enum class NETARootOption
    {
        Geometry /* 'geometry' - Specifies the required geometry about the atom (default = none) */
    };
    // Return enum options for NETARootOptions
    static EnumOptions<NETARootNode::NETARootOption> options();
    // Return whether the specified option is valid for this node
    bool isValidOption(std::string_view s) const override;
    // Set value and comparator for specified option
    bool setOption(std::string_view option, ComparisonOperator op, std::string_view value) override;

    /*
     * Scoring
     */
    public:
    // Evaluate the node and return its score
    int score(const SpeciesAtom *i, std::vector<const SpeciesAtom *> &matchPath) const override;
};
