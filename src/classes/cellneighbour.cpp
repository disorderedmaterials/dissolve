// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "classes/cellneighbour.h"
#include <cstdlib>

CellNeighbour::CellNeighbour()
{
    cell_ = nullptr;
    useMim_ = false;
}

CellNeighbour::~CellNeighbour() {}

// Set cell and mim flag
void CellNeighbour::set(Cell *cell, bool useMim)
{
    cell_ = cell;
    useMim_ = useMim;
}

// Return referenced cell
const Cell *CellNeighbour::cell() const { return cell_; }

// Return whether mim should be applied
bool CellNeighbour::useMim() { return useMim_; }
