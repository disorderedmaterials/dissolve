// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "math/matrix3.h"
#include <memory>
#include <optional>

// Forward Declarations
class Molecule;
class SpeciesBead;

// Bead Definition
class Bead
{
    public:
    Bead(const SpeciesBead *parent = nullptr, std::optional<int> uniqueBeadIndex = {},
         std::shared_ptr<const Molecule> molecule = nullptr, Vec3<double> origin = Vec3<double>());
    ~Bead() = default;
    Bead &operator=(const Bead &source) = default;
    Bead(const Bead &source) = default;
    Bead(Bead &&source) = default;

    /*
     * Bead Definition
     */
    protected:
    // Bead definition used to generate bead
    const SpeciesBead *parent_;
    // Unique bead index in the parent
    std::optional<int> uniqueBeadIndex_;
    // Bead origin
    Vec3<double> origin_;
    // Molecule to which bead is related (if relevant)
    std::shared_ptr<const Molecule> molecule_;

    public:
    // Return the parent
    const SpeciesBead *parent() const;
    // Return the unique bead index in the parent
    std::optional<int> uniqueBeadIndex() const;
    // Return bead origin
    const Vec3<double> &origin() const;
    // Return Molecule to which bead is related (if relevant)
    std::shared_ptr<const Molecule> molecule() const;
    // Return whether local axes are present
    virtual bool hasAxes() const;
    // Return local axes
    virtual const Matrix3 &axes() const;
};

// Oriented Bead Definition
class OrientedBead : public Bead
{
    public:
    OrientedBead(const SpeciesBead *parent = nullptr, std::optional<int> uniqueBeadIndex = {},
                 std::shared_ptr<const Molecule> molecule = nullptr, Vec3<double> origin = Vec3<double>(),
                 Vec3<double> xAxis = Vec3<double>(), Vec3<double> yAxis = Vec3<double>(), Vec3<double> zAxis = Vec3<double>());
    OrientedBead &operator=(const OrientedBead &source) = default;
    OrientedBead(const OrientedBead &source) = default;
    OrientedBead(OrientedBead &&source) = default;

    /*
     * Bead Definition
     */
    private:
    // Local axes
    Matrix3 axes_;

    public:
    // Axis Enum
    enum BeadAxis
    {
        XAxis = 0,
        YAxis = 1,
        ZAxis = 2
    };
    // Return enum options for BeadAxis
    static EnumOptions<OrientedBead::BeadAxis> beadAxis();
    // Return whether local axes are present
    bool hasAxes() const override;
    // Return local axes
    const Matrix3 &axes() const override;
    // Rotate about axis
    void rotate(double angle, OrientedBead::BeadAxis axis);
};
