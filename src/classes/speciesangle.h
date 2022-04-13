// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "classes/speciesintra.h"

#include <map>
#include <toml11/toml.hpp>
#include <vector>

// Forward Declarations
class SpeciesAtom;
class Species;

// Angle functional forms
class AngleFunctions
{
    public:
    enum class Form
    {
        None,
        Harmonic,
        Cosine,
        Cos2
    };
    // Return enum options for form
    static EnumOptions<Form> forms();
    // Return parameters for specified form
    static const std::vector<std::string> &parameters(Form form);
    // Return nth parameter for the given form
    static std::string parameter(Form form, int n);
    // Return index of parameter in the given form
    static std::optional<int> parameterIndex(Form form, std::string_view name);
};

// SpeciesAngle Definition
class SpeciesAngle : public SpeciesIntra<SpeciesAngle, AngleFunctions>
{
    public:
    SpeciesAngle();
    SpeciesAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k);
    ~SpeciesAngle() override = default;
    SpeciesAngle(SpeciesAngle &source);
    SpeciesAngle(SpeciesAngle &&source) noexcept;
    SpeciesAngle &operator=(const SpeciesAngle &source);
    SpeciesAngle &operator=(SpeciesAngle &&source) noexcept;

    /*
     * SpeciesAtom Information
     */
    private:
    // First SpeciesAtom in interaction
    SpeciesAtom *i_{nullptr};
    // Second (central) SpeciesAtom in interaction
    SpeciesAtom *j_{nullptr};
    // Third SpeciesAtom in interaction
    SpeciesAtom *k_{nullptr};

    private:
    // Detach from current atoms
    void detach();

    public:
    // Assign the three atoms in the angle
    void assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k);
    // Rewrite SpeciesAtom pointer
    void switchAtom(const SpeciesAtom *oldPtr, SpeciesAtom *newPtr);
    // Return first SpeciesAtom
    SpeciesAtom *i() const;
    // Return second (central) SpeciesAtom
    SpeciesAtom *j() const;
    // Return third SpeciesAtom
    SpeciesAtom *k() const;
    // Return vector of involved atoms
    std::vector<const SpeciesAtom *> atoms() const override;
    // Return index (in parent Species) of first SpeciesAtom
    int indexI() const;
    // Return index (in parent Species) of second (central) SpeciesAtom
    int indexJ() const;
    // Return index (in parent Species) of third SpeciesAtom
    int indexK() const;
    // Return index (in parent Species) of nth SpeciesAtom
    int index(int n) const;
    // Return whether SpeciesAtom match those specified
    bool matches(const SpeciesAtom *i, const SpeciesAtom *j, const SpeciesAtom *k) const;
    // Return whether all atoms in the interaction are currently selected
    bool isSelected() const;

    /*
     * Interaction Parameters
     */
    public:
    // Return fundamental frequency for the interaction
    double fundamentalFrequency(double reducedMass) const override;
    // Return energy for specified angle
    double energy(double angleInDegrees) const;
    // Return force multiplier for specified angle
    double force(double angleInDegrees) const;

    toml::basic_value<toml::discard_comments, std::map, std::vector> serialize();
    void deserialize(toml::value node, std::vector<SpeciesAtom> &atoms);
};

// MasterAngle Definition
class MasterAngle : public SpeciesAngle
{
    public:
    explicit MasterAngle(std::string_view name) : SpeciesAngle(), name_{name} {};

    /*
     * Identifying Name
     */
    private:
    // Identifying name
    std::string name_;

    public:
    // Set identifying name
    void setName(std::string_view name) override { name_ = name; }
    // Return identifying name
    std::string_view name() const override { return name_; };
};
