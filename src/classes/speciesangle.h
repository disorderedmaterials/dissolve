// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "classes/speciesintra.h"

// Forward Declarations
class SpeciesAtom;
class Species;

// SpeciesAngle Definition
class SpeciesAngle : public SpeciesIntra
{
    public:
    SpeciesAngle(SpeciesAtom *i = nullptr, SpeciesAtom *j = nullptr, SpeciesAtom *k = nullptr);
    ~SpeciesAngle() = default;
    SpeciesAngle(SpeciesAngle &source);
    SpeciesAngle(SpeciesAngle &&source);
    SpeciesAngle &operator=(SpeciesAngle &source);
    SpeciesAngle &operator=(SpeciesAngle &&source);

    /*
     * SpeciesAtom Information
     */
    private:
    // First SpeciesAtom in interaction
    SpeciesAtom *i_;
    // Second (central) SpeciesAtom in interaction
    SpeciesAtom *j_;
    // Third SpeciesAtom in interaction
    SpeciesAtom *k_;

    private:
    // Assign the three atoms in the angle
    void assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k);
    // Detach from current atoms
    void detach();

    public:
    // Return first SpeciesAtom
    SpeciesAtom *i() const;
    // Return second (central) SpeciesAtom
    SpeciesAtom *j() const;
    // Return third SpeciesAtom
    SpeciesAtom *k() const;
    // Return index (in parent Species) of first SpeciesAtom
    int indexI() const;
    // Return index (in parent Species) of second (central) SpeciesAtom
    int indexJ() const;
    // Return index (in parent Species) of third SpeciesAtom
    int indexK() const;
    // Return index (in parent Species) of nth SpeciesAtom
    int index(int n) const;
    // Return whether SpeciesAtom match those specified
    bool matches(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k) const;
    // Return whether all atoms in the interaction are currently selected
    bool isSelected() const;

    /*
     * Interaction Parameters
     */
    public:
    // Angle functional forms
    enum AngleFunction
    {
        NoForm,
        HarmonicForm,
        CosineForm,
        Cos2Form
    };
    // Return enum options for AngleFunction
    static EnumOptions<AngleFunction> angleFunctions();

    public:
    // Set up any necessary parameters
    void setUp();
    // Return fundamental frequency for the interaction
    double fundamentalFrequency(double reducedMass) const;
    // Return type of this interaction
    SpeciesIntra::InteractionType type() const;
    // Return energy for specified angle
    double energy(double angleInDegrees) const;
    // Return force multiplier for specified angle
    double force(double angleInDegrees) const;
};
