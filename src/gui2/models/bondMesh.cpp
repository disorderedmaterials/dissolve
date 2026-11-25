// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/bondMesh.h"

const Configuration *BondMesh::configuration() const { return config_; }

void BondMesh::setConfiguration(const Configuration *config) { config_ = config; }
