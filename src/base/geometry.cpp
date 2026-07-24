// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/geometry.h"

Geometry::Geometry()
{
    indices_[0] = -1;
    indices_[1] = -1;
    indices_[2] = -1;
    indices_[3] = -1;
    value_ = 0.0;
}

/*
 * Data
 */

// Set reference value and indices
void Geometry::set(double value, int a, int b, int c, int d)
{
    indices_[0] = a;
    indices_[1] = b;
    indices_[2] = c;
    indices_[3] = d;
    value_ = value;
}

// Return reference value
double Geometry::value() const { return value_; }

// Return index value
int Geometry::indices(int i) const { return indices_[i]; }

bool Geometry::operator==(const Geometry &rhs) const { return value_ == rhs.value_ && indices_ == rhs.indices_; }
bool Geometry::operator!=(const Geometry &rhs) const { return !(rhs == *this); }

namespace Serialisable
{
void serialiseOnto(const Geometry::GeometryType &e, std::string tag, SerialisedValue &node)
{
    switch (e)
    {
        case Geometry::GeometryType::AngleType:
            node["tag"] = "angle";
        case Geometry::GeometryType::DistanceType:
            node["tag"] = "distance";
        case Geometry::GeometryType::TorsionType:
            node["tag"] = "torsion";
        default:
            throw std::runtime_error("Unhandled geometry type - can't convert to TOML value.\n");
    }
}
}; // namespace Serialisable

namespace Deserialisable
{
void deserialiseOnto(Geometry::GeometryType &e, const SerialisedValue &target)
{
    auto typeString = target.as_string();
    if (typeString == "angle")
        e = Geometry::GeometryType::AngleType;
    else if (typeString == "distance")
        e = Geometry::GeometryType::DistanceType;
    else if (typeString == "torsion")
        e = Geometry::GeometryType::TorsionType;
    else
        throw toml::type_error(
            std::format("Unhandled geometry type '{}' - can't convert from TOML value.\n", std::string(typeString)),
            target.location());
}
} // namespace Deserialisable
