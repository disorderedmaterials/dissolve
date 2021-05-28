// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#pragma once

#include "classes/cell.h"
#include <algorithm>
#include <vector>

struct CellNeighbour
{
    CellNeighbour(Cell *m, Cell *n, bool mim) : master_(m), neighbour_(n), requiresMIM_(mim) {}
    Cell *master_ = nullptr;
    Cell *neighbour_ = nullptr;
    bool requiresMIM_ = false;

    bool operator==(const CellNeighbour &other) const
    {
        if (this->master_ == other.master_ && this->neighbour_ == other.neighbour_)
            return true;
        else if (this->neighbour_ == other.master_ && this->master_ == other.neighbour_)
            return true;
        else
            return false;
    }
};

class CellNeighbourPairs
{
    public:
    // Try to insert a neighbour pair - Returning true if succesfull.
    // If the pair is already present it will return false and the pair will not be added.
    bool insert(const CellNeighbour &neighbour) { return addNeighbour(neighbour); };
    bool insert(Cell *master, Cell *other, bool mimRequired)
    {
        return addNeighbour(CellNeighbour(master, other, mimRequired));
    };
    const std::vector<CellNeighbour> &neighbours() const { return neighbours_; }
    std::vector<CellNeighbour> &neighbours() { return neighbours_; }

    private:
    std::vector<CellNeighbour> neighbours_;
    bool addNeighbour(CellNeighbour neighbour)
    {
        // At the moment we find if a pair is already present through a linear seearch.
        // Searching could be optimised using a set, but would cost more memory.
        auto it = std::find(neighbours_.begin(), neighbours_.end(), neighbour);
        if (it == neighbours_.end())
        {
            neighbours_.push_back(std::move(neighbour));
            return true;
        }
        return false;
    }
};