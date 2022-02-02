// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

// Enum Broadcast Vessel
template <class E> class EnumCast
{
    /*
     * Template-only class that takes reference to an enum and allows the integer conversion to be passed by reference to
     * the broadcast routines. Before destruction, the integerValue_ that was subject to broadcast is cast back into the
     * original enum ref.
     */
    public:
    EnumCast(E &originalEnum) : originalEnum_(originalEnum) { integerValue_ = originalEnum_; }
    ~EnumCast()
    {
        // Cast integer variable back into enum
        originalEnum_ = (E)integerValue_;
    }
    operator int &() { return integerValue_; }

    private:
    // Original enum object
    E &originalEnum_;
    // Integer conversion of enum
    int integerValue_;
};