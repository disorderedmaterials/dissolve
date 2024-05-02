// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <bitset>

// Flag Set
template <class EnumClass> class Flags
{
    public:
    Flags() : flags_{0} {};
    ~Flags() = default;
    explicit Flags(int flagMask) : flags_(flagMask) {}
    Flags(EnumClass flag) : flags_{0} { flags_.set(flag); }
    Flags(const std::initializer_list<EnumClass> flags) : flags_{0}
    {
        for (auto &flag : flags)
            flags_.set(flag);
    }
    template <typename... Args> Flags(EnumClass flag, Args... flags) : flags_{0} { setFlags(flag, flags...); }
    Flags(const Flags &source) { flags_ = source.flags_; }
    Flags &operator=(int flagMask)
    {
        flags_ = flagMask;
        return *this;
    }
    void operator+=(EnumClass flag) { flags_.set(flag); }
    void operator+=(int flagMask) { flags_ |= flagMask; }
    void operator-=(EnumClass flag) { flags_.reset(flag); }
    void operator|=(int flagMask) { flags_ |= flagMask; }
    operator int() const { return flags_.to_ulong(); }

    private:
    // Maximum bitset size
    static constexpr int maxFlagsSize_ = 32;
    // Bitset of flags
    std::bitset<maxFlagsSize_> flags_;

    public:
    // Set specified flag
    void setFlag(EnumClass flag) { flags_.set(flag); }
    // Unset specified flag
    void removeFlag(EnumClass flag) { flags_.reset(flag); }
    // Set state of flag
    void setState(EnumClass flag, bool state) { state ? flags_.set(flag) : flags_.reset(flag); }
    // Set a pack of flags
    template <typename... Args> void setFlags(Args... flags) { (setFlag(flags), ...); }
    // Return whether any flags are set
    bool anySet() const { return flags_.any(); }
    // Return true if the specified flag is set
    bool isSet(EnumClass flag) const { return flags_.test(flag); }
    // Return true if the specified flag is set, or if none are set at all
    bool isSetOrNone(EnumClass flag) const { return flags_.none() || flags_.test(flag); }
};
