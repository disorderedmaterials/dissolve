// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors
#pragma once

#include "classes/cell.h"

struct CellNeighbour
{
    CellNeighbour(const Cell &n, bool mim) : neighbour_(n), requiresMIM_(mim) {}
    const Cell &neighbour_;
    bool requiresMIM_ = false;

    bool operator==(const Cell &other) const { return this->neighbour_.index() == other.index(); }
};

struct CellNeighbourPair
{
    CellNeighbourPair(const Cell &m, const Cell &n, bool mim) : master_(m), neighbour_(n), requiresMIM_(mim) {}
    const Cell &master_;
    const Cell &neighbour_;
    bool requiresMIM_ = false;

    bool operator==(const CellNeighbourPair &other) const
    {
        if (&this->master_ == &other.master_ && &this->neighbour_ == &other.neighbour_)
            return true;
        else if (&this->neighbour_ == &other.master_ && &this->master_ == &other.neighbour_)
            return true;
        else
            return false;
    }
};
