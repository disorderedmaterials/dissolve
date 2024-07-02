// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/bead.h"

#include "base/messenger.h"
#include <utility>

/*
 * Bead
 */

Bead::Bead(const SpeciesBead *parent, std::optional<int> uniqueBeadIndex, std::shared_ptr<const Molecule> molecule,
           Vec3<double> origin)
{
    parent_ = parent;
    uniqueBeadIndex_ = uniqueBeadIndex;
    molecule_ = std::move(molecule);
    origin_ = origin;
}

// Return the parent
const SpeciesBead *Bead::parent() const { return parent_; }

// Return the unique bead index in the parent
std::optional<int> Bead::uniqueBeadIndex() const { return uniqueBeadIndex_; }

// Return bead origin
const Vec3<double> &Bead::origin() const { return origin_; }

// Return Molecule to which bead is related (if relevant)
std::shared_ptr<const Molecule> Bead::molecule() const { return molecule_; }

// Return whether local axes are present
bool Bead::hasAxes() const { return false; }

// Return local axes
const Matrix3 &Bead::axes() const
{
    static Matrix3 dummy;
    Messenger::warn("Returning empty axes for this Bead, since it doesn't have any.\n");
    return dummy;
}

/*
 * Oriented Bead
 */

OrientedBead::OrientedBead(const SpeciesBead *parent, std::optional<int> uniqueBeadIndex,
                           std::shared_ptr<const Molecule> molecule, Vec3<double> origin, Vec3<double> xAxis,
                           Vec3<double> yAxis, Vec3<double> zAxis)
    : Bead(parent, uniqueBeadIndex, std::move(molecule), origin)
{
    axes_.setColumn(0, xAxis);
    axes_.setColumn(1, yAxis);
    axes_.setColumn(2, zAxis);
}

// Return whether local axes are present
bool OrientedBead::hasAxes() const { return true; }

// Return enum options for BeadAxis
EnumOptions<OrientedBead::BeadAxis> OrientedBead::beadAxis()
{
    return EnumOptions<OrientedBead::BeadAxis>(
        "BeadAxis", {{OrientedBead::XAxis, "X"}, {OrientedBead::YAxis, "Y"}, {OrientedBead::ZAxis, "Z"}});
}

// Return local axes
const Matrix3 &OrientedBead::axes() const { return axes_; }
