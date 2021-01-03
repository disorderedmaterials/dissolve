// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coordinateset.h"
#include "base/messenger.h"

CoordinateSet::CoordinateSet() : ListItem<CoordinateSet>() {}

CoordinateSet::~CoordinateSet() {}

/*
 * Coordinates
 */

// Initialise to specified size
void CoordinateSet::initialise(int size) { coordinates_.initialise(size); }

// Return size
int CoordinateSet::size() const { return coordinates_.nItems(); }

// Set specified coordinates
void CoordinateSet::set(int index, Vec3<double> r) { coordinates_.at(index) = r; }

// Return specified coordinates
Vec3<double> CoordinateSet::r(int index) const { return coordinates_.at(index); }

// Return coordinates array
Array<Vec3<double>> &CoordinateSet::coordinates() { return coordinates_; }
