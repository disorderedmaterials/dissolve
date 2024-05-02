// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/version.h"

VersionCounter::VersionCounter() { version_ = 0; }

/*
 * Version Information
 */

// Reset version counter to zero
void VersionCounter::zero() { version_ = 0; }

/*
 * Operators
 */

// Automatic conversion to integer
VersionCounter::operator int() const { return version_; }

// Prefix increment
int VersionCounter::operator++() { return (++version_); }
