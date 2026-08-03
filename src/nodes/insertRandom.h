// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "math/vector3.h"
#include "nodes/node.h"

// Forward Declarations
class Configuration;

class InsertRandomNode : public Node
{
    public:
    InsertRandomNode(Graph *parentGraph);
    ~InsertRandomNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    public:
    // Box Scaling Type
    enum class BoxScalingType
    {
        None,         /* Box geometry / volume will remain unchanged */
        AddVolume,    /* Scale Box volume to accommodate new species at the supplied density */
        FixedDensity, /* Scale current Box volume to give, after addition of the current species, the supplied overall density
                       */
    };
    // Return enum option info for BoxScalingType
    static EnumOptions<BoxScalingType> boxScalingType();

    private:
    // Source Species
    const Species *species_;
    // Target configuration to insert into
    Configuration *configuration_{nullptr};
    // The default box action if none is specified
    static constexpr BoxScalingType defaultBoxScaling_ = BoxScalingType::AddVolume;
    // Box scaling type
    BoxScalingType boxScaling_{defaultBoxScaling_};
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
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
