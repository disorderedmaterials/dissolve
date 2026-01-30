// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/rangedVector3.h"

RangedVector3::RangedVector3(const Vector3 &value) : value_(value) {}

RangedVector3::RangedVector3(const Vector3 &value, std::optional<Vector3> min, std::optional<Vector3> max)
    : min_(min), max_(max)
{
    set(value);
}

RangedVector3::operator Vector3() const { return value_; }

/*
 * Data
 */

// Impose limit on vector
Vector3 RangedVector3::limit(const Vector3 &v) const
{
    Vector3 result = v;
    if (hasLowerBound())
    {
        result.x = result.x < min_->x ? min_->x : result.x;
        result.y = result.y < min_->y ? min_->y : result.y;
        result.z = result.z < min_->z ? min_->z : result.z;
    }
    if (hasUpperBound())
    {
        result.x = result.x > max_->x ? max_->x : result.x;
        result.y = result.y > max_->y ? max_->y : result.y;
        result.z = result.z > max_->z ? max_->z : result.z;
    }
    return result;
}

// Set from vector
void RangedVector3::set(const Vector3 &v) { value_ = limit(v); }

// Return whether the number has lower bound
bool RangedVector3::hasLowerBound() const { return min_.has_value(); }

// Return whether the number has upper bound
bool RangedVector3::hasUpperBound() const { return max_.has_value(); }

// Return whether the number has bounds
bool RangedVector3::isBounded() const { return hasLowerBound() && hasUpperBound(); }

// Return optional min
std::optional<Vector3> RangedVector3::min() const { return min_; }

// Return optional max
std::optional<Vector3> RangedVector3::max() const { return max_; }

// Return x component of vector
double RangedVector3::x() const { return value_.x; }

// Return y component of vector
double RangedVector3::y() const { return value_.y; }

// Return z component of vector
double RangedVector3::z() const { return value_.z; }

// Return current value
Vector3 RangedVector3::value() const { return value_; }

/*
 * Serialisable
 */

// Express as a serialisable value
void RangedVector3::serialise(std::string tag, SerialisedValue &target) const { value_.serialise(tag, target); }

// Read values from a serialisable value
void RangedVector3::deserialise(const SerialisedValue &node) { value_.deserialise(node); }
