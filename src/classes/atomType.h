// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "classes/atomConstants.h"
#include "classes/interactionPotential.h"
#include "classes/shortRangeFunctions.h"
#include "data/elements.h"
#include <map>
#include <memory>
#include <vector>

// AtomType Definition
class AtomType : public Serialisable, public std::enable_shared_from_this<AtomType>
{
    public:
    AtomType(Elements::Element Z = Elements::Unknown);
    AtomType(std::string_view name);
    AtomType(Elements::Element Z, std::string_view name);
    ~AtomType() = default;

    /*
     * Character
     */
    private:
    // Name
    std::string name_{"XX"};
    // Associated chemical element
    Elements::Element Z_;
    // Whether the atom type is exchangeable
    bool exchangeable_{false};

    public:
    // Set name of AtomType
    void setName(std::string_view name);
    // Return name of AtomType
    std::string_view name() const;
    // Set chemical element
    void setZ(Elements::Element Z);
    // Return chemical element
    Elements::Element Z() const;
    // Set whether the atom type is exchangeable
    void setExchangeable(bool exchangeable);
    // Return whether the atom type is exchangeable
    bool isExchangeable() const;

    /*
     * Interaction Parameters
     */
    private:
    // Short-range interaction potential
    InteractionPotential<ShortRangeFunctions> interactionPotential_;
    // Atomic charge
    double charge_{0.0};

    public:
    // Return short-range interaction potential
    InteractionPotential<ShortRangeFunctions> &interactionPotential();
    const InteractionPotential<ShortRangeFunctions> &interactionPotential() const;
    // Set atomic charge
    void setCharge(double q);
    // Return atomic charge
    double charge() const;
    // Return whether our parameters are the same as those provided
    bool sameParametersAs(const AtomType *other, bool checkCharge = false) const;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(SerialisedValue node);
};
