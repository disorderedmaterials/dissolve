// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "classes/moleculeSet.h"
#include "nodes/node.h"

// Insert Node
class InsertNode : public Node
{
    public:
    InsertNode(Graph *parentGraph);
    ~InsertNode() override = default;

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
     */
    public:
    // Box Action Style
    enum class BoxActionStyle
    {
        None,        /* Box geometry / volume will remain unchanged */
        AddVolume,   /* Increase Box volume to accommodate new species, according to supplied density */
        ScaleVolume, /* Scale current Box volume to give, after addition of the current species, the supplied density */
    };
    // Return enum option info for BoxActionStyle
    static EnumOptions<BoxActionStyle> boxActionStyles();

    private:
    // Target configuration to insert into
    Configuration *configuration_{nullptr};
    // AtomTypes owned by the node
    const std::vector<std::shared_ptr<AtomType>> *atomTypes_{nullptr};
    // Species to be added (if no MoleculeSet is given)
    const Species *species_{nullptr};
    // MoleculeSet to be added (if no Species is given)
    const MoleculeSet *moleculeSet_{nullptr};
    // The default box action if none is specified
    static constexpr BoxActionStyle defaultBoxAction_ = BoxActionStyle::AddVolume;
    // Action to take on the Box geometry / volume on addition of the species
    BoxActionStyle boxAction_{defaultBoxAction_};
    // Target density when adding molecules (if adjusting box size)
    Number density_{1.0};
    // Units for the specified density value
    Units::DensityUnits densityUnits_{Units::GramsPerCentimetreCubedUnits};
    // Population of molecules to add
    Number population_{100};
    // The default scaling settings
    static constexpr bool defaultScale_{true};
    // Flags controlling box axis scaling
    bool scaleA_{defaultScale_}, scaleB_{defaultScale_}, scaleC_{defaultScale_};
    // Whether to rotate molecules on insertion
    bool rotate_{true};

    /*
     * Functions
     */
    private:
    // Add volume the configuration's box to accommodate specified atoms / mass
    void addVolume(int nAtomsToAdd, double massToAdd) const;
    // Scale the configuration's box volume to accommodate specified atoms / mass
    void scaleVolume(int nAtomsToAdd, double massToAdd) const;
    // Get population totals to be added from specified MoleculeSet
    std::tuple<int, int, double> getPopulationTotals(int population, const MoleculeSet &molecules) const;

    /*
     * Processing
     */
    public:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
