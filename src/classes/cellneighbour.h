// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#pragma once

#include "classes/cell.h"

struct CellNeighbour
{
    CellNeighbour(const Cell &m, const Cell &n, bool mim) : master_(m), neighbour_(n), requiresMIM_(mim) {}
    const Cell &master_;
    const Cell &neighbour_;
    bool requiresMIM_ = false;

    bool operator==(const CellNeighbour &other) const
    {
        if (this->master_.index() == other.master_.index() && this->neighbour_.index() == other.neighbour_.index())
            return true;
        else if (this->neighbour_.index() == other.master_.index() && this->master_.index() == other.neighbour_.index())
            return true;
        else
            return false;
    }
};
