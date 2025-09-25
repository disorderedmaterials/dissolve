// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "classes/speciesParticle.h"
#include "data/elements.h"
#include "math/vector3.h"
#include "templates/optionalRef.h"
#include <map>
#include <memory>
#include <vector>

// Forward Declarations
class AtomType;

// SpeciesAtom Definition
class SpeciesAtom : public SpeciesParticle
{
    public:
    SpeciesAtom() = default;
    ~SpeciesAtom() = default;
    SpeciesAtom(SpeciesAtom &source) = delete;
    SpeciesAtom(SpeciesAtom &&source) noexcept;
    SpeciesAtom &operator=(const SpeciesAtom &source) = delete;
    SpeciesAtom &operator=(SpeciesAtom &&source) noexcept;

    private:
    // Move all data from source to this
    void move(SpeciesAtom &source);

    /*
     * Properties
     */
    public:
    // Presence
    enum class Presence
    {
        Phantom = -1,
        Physical,
        Any
    };

    private:
    // Atomic element
    Elements::Element Z_{Elements::Unknown};
    // Charge (if contained in file)
    double charge_{0.0};
    // Assigned AtomType
    std::shared_ptr<AtomType> atomType_{nullptr};
    // Presence of atom
    Presence presence_{Presence::Physical};

    public:
    // Set basic properties
    void set(Elements::Element Z, double rx, double ry, double rz, double q = 0.0);
    void set(Elements::Element Z, const Vector3 &r, double q = 0.0);
    // Set atomic element
    void setZ(Elements::Element Z);
    // Return atomic element
    Elements::Element Z() const;
    // Return whether the atom is of the presence specified
    bool isPresence(SpeciesAtom::Presence presence) const;
    // Set charge of Atom
    void setCharge(double charge);
    // Return charge of Atom
    double charge() const;
    // Set AtomType of Atom
    void setAtomType(const std::shared_ptr<AtomType> &at);
    // Return AtomType of Atom
    std::shared_ptr<AtomType> atomType() const;
    // Return type name for particle
    std::string_view typeName() const override;
    // Return presence of atom
    Presence presence() const;

    /*
     * Intramolecular Information
     */
    public:
    // Scaled interaction types
    enum class ScaledInteraction
    {
        Excluded,
        Scaled,
        NotScaled
    };
    using ScaledInteractionDefinition = std::tuple<ScaledInteraction, double, double>;

    private:
    // Vector of Atoms with scaled or excluded interactions
    std::vector<std::pair<const SpeciesAtom *, ScaledInteractionDefinition>> scaledInteractions_;

    public:
    // Set all scaled intramolecular interactions
    void setScaledInteractions();
    // Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
    ScaledInteractionDefinition scaling(const SpeciesAtom *j) const;

    /*
     * Atom Environment Helpers
     */
    public:
    // Atom Geometry enum
    enum class AtomGeometry
    {
        Unknown,
        Unbound,
        Terminal,
        Linear,
        TShape,
        TrigonalPlanar,
        Tetrahedral,
        SquarePlanar,
        TrigonalBipyramidal,
        Octahedral
    };
    // Return EnumOptions for AtomGeometry
    static EnumOptions<AtomGeometry> geometries();
    // Calculate and return the geometry of this atom
    AtomGeometry geometry() const;
    // Return whether the geometry of this atom matches that specified
    bool isGeometry(AtomGeometry geom) const;
    // Calculate and return the geometry of the specified SpeciesAtom
    static AtomGeometry geometry(const SpeciesAtom *i);
    // Return whether the specified SpeciesAtom exists in the specified geometry
    static bool isGeometry(const SpeciesAtom *i, AtomGeometry geom);
    // Guess and return oxidation state for the specified SpeciesAtom
    static int guessOxidationState(const SpeciesAtom *i);

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, CoreData &coreData) override;
};
