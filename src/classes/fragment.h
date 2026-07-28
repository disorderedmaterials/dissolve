// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include <vector>

// Fragment
template <class AtomClass, class BondClass> class Fragment
{
    private:
    // Recursively add atoms along any path from the specified one, ignoring the bond(s) provided
    static void getIndicesRecursive(const std::vector<AtomClass> &atoms, std::vector<int> &indices, int index,
                                    const std::vector<const BondClass *> &exclusions)
    {
        // Loop over bonds on indexed atom
        indices.emplace_back(index);
        const auto &i = atoms.at(index);
        for (const auto *bond : i.bonds())
        {
            // Is this either of the excluded bonds?
            if (std::ranges::find(exclusions, bond) != exclusions.end())
                continue;

            // Get the partner atom in the bond and select it (if it is not selected already)
            auto *j = bond->partner(&i);
            if (std::find(indices.begin(), indices.end(), j->index()) == indices.end())
                getIndicesRecursive(atoms, indices, j->index(), exclusions);
        }
    }
    static void getIndicesRecursive(const std::vector<std::unique_ptr<AtomClass>> &atoms, std::vector<int> &indices, int index,
                                    const std::vector<const BondClass *> &exclusions)
    {
        // Loop over bonds on indexed atom
        indices.emplace_back(index);
        const auto i = atoms.at(index).get();
        for (const auto *bond : i->bonds())
        {
            // Is this either of the excluded bonds?
            if (std::ranges::find(exclusions, bond) != exclusions.end())
                continue;

            // Get the partner atom in the bond and select it (if it is not selected already)
            auto j = bond->partner(i);
            if (std::find(indices.begin(), indices.end(), j->index()) == indices.end())
                getIndicesRecursive(atoms, indices, j->index(), exclusions);
        }
    }

    public:
    // Return the fragment (vector of indices) containing the specified atom
    static std::vector<int> get(const std::vector<AtomClass> &atoms, int startIndex,
                                const std::vector<const BondClass *> &exclusions = {})
    {
        std::vector<int> indices;
        getIndicesRecursive(atoms, indices, startIndex, exclusions);
        return indices;
    }
    static std::vector<int> get(const std::vector<std::unique_ptr<AtomClass>> &atoms, int startIndex,
                                const std::vector<const BondClass *> &exclusions = {})
    {
        std::vector<int> indices;
        getIndicesRecursive(atoms, indices, startIndex, exclusions);
        return indices;
    }
};
