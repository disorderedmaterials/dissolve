// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "classes/atom.h"
#include "data/elements.h"
#include "math/vector3.h"
#include "templates/optionalRef.h"
#include <vector>

// Forward Declarations
class AtomType;
class SpeciesAngle;
class SpeciesBond;
class SpeciesImproper;
class SpeciesTorsion;

// SpeciesAtom Definition
class SpeciesAtom : public Atom<const SpeciesBond>
{
    public:
    SpeciesAtom(Species *parent);

    /*
     * Properties
     */
    private:
    // Parent Species
    Species *parent_{nullptr};
    // Assigned AtomType
    const AtomType *atomType_{nullptr};

    public:
    // Return parent Species
    Species *parent() const;
    // Set AtomType of Atom
    void setAtomType(const AtomType *at);
    // Return AtomType of Atom
    const AtomType *atomType() const;

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
    // Vector of angles which this atom participates in
    std::vector<const SpeciesAngle *> angles_;
    // Vector of torsions which this atom participates in
    std::vector<const SpeciesTorsion *> torsions_;
    // Vector of torsions which this atom participates in
    std::vector<const SpeciesImproper *> impropers_;
    // Vector of Atoms with scaled or excluded interactions
    std::vector<std::pair<const SpeciesAtom *, ScaledInteractionDefinition>> scaledInteractions_;

    public:
    // Add specified Angle to Atom
    void addAngle(const SpeciesAngle *angle);
    // Return angles the atom is involved in
    const std::vector<const SpeciesAngle *> &angles() const;
    // Add specified SpeciesTorsion to Atom
    void addTorsion(const SpeciesTorsion *torsion);
    // Return torsions the atom is involved in
    const std::vector<const SpeciesTorsion *> &torsions() const;
    // Add specified SpeciesImproper to Atom
    void addImproper(const SpeciesImproper *improper);
    // Return impropers the atom is involved in
    const std::vector<const SpeciesImproper *> &impropers() const;
    // Set all scaled intramolecular interactions
    void setScaledInteractions();
    // Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
    ScaledInteractionDefinition scaling(const SpeciesAtom *j) const;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
};
