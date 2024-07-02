// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/beadReference.h"

BeadReference::BeadReference(BeadStack &beadStack, std::string_view name) : beadStack_(beadStack), name_{name} {}

// Return referenced stack of beads
BeadStack &BeadReference::beadStack() const { return beadStack_; }

// Return name of this bead stack (in the context of an Analyser)
std::string_view BeadReference::name() const { return name_; }
