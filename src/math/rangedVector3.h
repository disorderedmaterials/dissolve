// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "math/vector3.h"
#include <optional>

// Ranged Vector3
class RangedVector3 : public Serialisable<>
{
    public:
    RangedVector3() = default;
    RangedVector3(const Vector3 &value);
    RangedVector3(const Vector3 &value, std::optional<Vector3> min, std::optional<Vector3> max = {});
    ~RangedVector3() = default;
    operator Vector3() const;

    /*
     * Data
     */
    protected:
    // Value
    Vector3 value_;
    // Lower bound
    std::optional<Vector3> min_;
    // Upper bound
    std::optional<Vector3> max_;

    private:
    // Impose limit on vector
    Vector3 limit(const Vector3 &v) const;

    public:
    // Set from vector
    void set(const Vector3 &v);
    // Return whether the number has lower bound
    bool hasLowerBound() const;
    // Return whether the number has upper bound
    bool hasUpperBound() const;
    // Return whether the number has bounds
    bool isBounded() const;
    // Return optional min
    std::optional<Vector3> min() const;
    // Return optional max
    std::optional<Vector3> max() const;
    // Return x component of vector
    double x() const;
    // Return y component of vector
    double y() const;
    // Return z component of vector
    double z() const;
    // Return current value
    Vector3 value() const;

    /*
     * Serialisable
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
