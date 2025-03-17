// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "generator/node.h"
#include "generator/nodeValue.h"

// Forward Declarations
class RegionGeneratorNodeBase;

// Add Node
class AddGeneratorNodeBase : public GeneratorNode
{
    public:
    explicit AddGeneratorNodeBase(GeneratorNode::NodeType nodeType, const NodeValue &population = 0,
                                  const NodeValue &density = 0.1,
                                  Units::DensityUnits densityUnits = Units::AtomsPerAngstromUnits);
    ~AddGeneratorNodeBase() override = default;

    protected:
    // Set up base keywords
    void setUpBaseKeywords();

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Node Data
     */
    public:
    // Box Action Style
    enum class BoxActionStyle
    {
        None,        /* Box geometry / volume will remain unchanged */
        AddVolume,   /* Increase Box volume to accommodate new species, according to supplied density */
        ScaleVolume, /* Scale current Box volume to give, after addition of the current species, the supplied density */
        Set          /* Set the Box geometry to that specified in the Species */
    };
    // Return enum option info for BoxActionStyle
    static EnumOptions<BoxActionStyle> boxActionStyles();
    // Positioning Type
    enum class PositioningType
    {
        Central, /* Position at the centre of the Box */
        Current, /* Use current coordinates */
        Random,  /* Set position randomly */
        Region   /* Set position randomly within a specified region */
    };
    // Return enum option info for PositioningType
    static EnumOptions<PositioningType> positioningTypes();

    protected:
    // The default box action if none is specified
    static constexpr AddGeneratorNodeBase::BoxActionStyle defaultBoxAction_ = AddGeneratorNodeBase::BoxActionStyle::AddVolume;
    // Action to take on the Box geometry / volume on addition of the species
    AddGeneratorNodeBase::BoxActionStyle boxAction_{defaultBoxAction_};
    // Target density when adding molecules
    std::pair<NodeValue, Units::DensityUnits> density_{1.0, Units::GramsPerCentimetreCubedUnits};
    // Population of molecules to add
    NodeValue population_{1.0};
    // The default positioning if none is specified
    static constexpr AddGeneratorNodeBase::PositioningType defaultPositioningType_ =
        AddGeneratorNodeBase::PositioningType::Random;
    // Positioning type for individual molecules
    AddGeneratorNodeBase::PositioningType positioningType_{defaultPositioningType_};
    // Region into which we will add molecules (if any)
    std::shared_ptr<const RegionGeneratorNodeBase> region_{nullptr};
    // The default scaling settings
    static constexpr bool defaultScale_{true};
    // iFlags controlling box axis scaling
    bool scaleA_{defaultScale_}, scaleB_{defaultScale_}, scaleC_{defaultScale_};

    protected:
    // Adjust or set box volume ready for addition
    void adjustBoxVolume(Configuration *cfg, int nCopies, int nAtomsPerCopy, double massPerCopy) const;

    /*
     * Execute
     */
    protected:
    // Prepare any necessary data, ready for execution in the base class
    bool prepareBase(const GeneratorContext &generatorContext);
};
