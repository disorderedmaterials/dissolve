// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include <string>

// Resolvable
template <class T> class Resolvable
{
    public:
    Resolvable() = default;
    Resolvable(T raw)
        requires(std::is_pointer_v<T>)
        : raw_(raw), name_(raw->name())
    {
    }
    Resolvable(std::string_view name) : raw_(nullptr), name_(name) {}

    private:
    // Object (if resolved)
    T raw_{nullptr};
    // Name of object (for resolution)
    std::string name_;

    public:
    // Return raw pointer to object
    T raw() const { return raw_; }
    // Return object name, preferring that direct from the raw pointer if defined
    std::string_view name() const { return raw_ ? raw_->name() : name_; }
    // Resolve object
    void resolve(T raw) { raw_ = raw; }
};
