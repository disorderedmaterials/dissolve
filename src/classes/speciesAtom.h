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
class CoreData;
class SpeciesAngle;
class SpeciesBond;
class SpeciesImproper;
class SpeciesTorsion;

// SpeciesAtom Definition
class SpeciesAtom : public Atom<SpeciesBond>
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
    // Return 'user' index (1->N)
    int userIndex() const;

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
    std::vector<std::reference_wrapper<SpeciesAngle>> angles_;
    // Vector of torsions which this atom participates in
    std::vector<std::reference_wrapper<SpeciesTorsion>> torsions_;
    // Vector of torsions which this atom participates in
    std::vector<std::reference_wrapper<SpeciesImproper>> impropers_;
    // Vector of Atoms with scaled or excluded interactions
    std::vector<std::pair<const SpeciesAtom *, ScaledInteractionDefinition>> scaledInteractions_;

    public:
    // Add specified Angle to Atom
    void addAngle(SpeciesAngle &angle);
    // Return angles the atom is involved in
    const std::vector<std::reference_wrapper<SpeciesAngle>> &angles() const;
    // Add specified SpeciesTorsion to Atom
    void addTorsion(SpeciesTorsion &torsion);
    // Return torsions the atom is involved in
    const std::vector<std::reference_wrapper<SpeciesTorsion>> &torsions() const;
    // Add specified SpeciesImproper to Atom
    void addImproper(SpeciesImproper &improper);
    // Return impropers the atom is involved in
    const std::vector<std::reference_wrapper<SpeciesImproper>> &impropers() const;
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

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
