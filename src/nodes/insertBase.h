// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "nodes/node.h"

// Forward Declarations
class MoleculeSet;

// Base class for all Insert-type nodes
class InsertNodeBase : public Node
{
    public:
    InsertNodeBase(Graph *parentGraph);
    ~InsertNodeBase() override = default;

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

    protected:
    // Target configuration to insert into
    Configuration *configuration_;
    // The default box action if none is specified
    static constexpr BoxActionStyle defaultBoxAction_ = BoxActionStyle::AddVolume;
    // Action to take on the Box geometry / volume on addition of the species
    BoxActionStyle boxAction_{defaultBoxAction_};
    // Target density when adding molecules (if adjusting box size)
    Number density_{1.0};
    Units::DensityUnits densityUnits_{Units::GramsPerCentimetreCubedUnits};
    // Population of molecules to add
    Number population_{1.0};
    // The default scaling settings
    static constexpr bool defaultScale_{true};
    // Flags controlling box axis scaling
    bool scaleA_{defaultScale_}, scaleB_{defaultScale_}, scaleC_{defaultScale_};
    // Whether to rotate molecules on insertion
    bool rotate_{true};

    /*
     * Common Functions
     */
    protected:
    // Get population totals to be added from specified MoleculeSet
    std::tuple<int, int, double> getPopulationTotals(int population, const MoleculeSet &molecules) const;
    // Adjust or set box volume ready for addition
    void adjustBoxVolume(Configuration *cfg, int nAtomsToAdd, double massToAdd) const;
};
