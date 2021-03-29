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
    ~NETARootNode() = default;

    /*
     * Modifiers
     */
    private:
    // Number of bonds value
    int nBondsValue_;
    // Number of bonds value comparison operator
    NETANode::ComparisonOperator nBondsValueOperator_;
    // Number of hydrogens value
    int nHydrogensValue_;
    // Number of hydrogens value comparison operator
    NETANode::ComparisonOperator nHydrogensValueOperator_;

    public:
    // Available modifiers
    enum class NETARootModifier
    {
        NBonds,    /* 'nbonds' - Specifies number of bonds (default = -1) */
        NHydrogens /* 'nh' - Specifies number of hydrogens (default = -1) */
    };
    // Return enum options for NETARootModifiers
    static EnumOptions<NETARootNode::NETARootModifier> modifiers();
    // Return whether the specified modifier is valid for this node
    bool isValidModifier(std::string_view s) const override;
    // Set value and comparator for specified modifier
    bool setModifier(std::string_view modifier, ComparisonOperator op, int value) override;

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
