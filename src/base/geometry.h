// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <fmt/format.h>
#include <map>
#include <toml11/toml.hpp>

// Geometry Definition
class Geometry
{
    public:
    Geometry();
    ~Geometry() = default;
    // Type of geometry data passed
    enum GeometryType
    {
        AngleType,
        DistanceType,
        TorsionType
    };

    /*
     * Data
     */
    private:
    // Reference value
    double value_;
    // Array for indices
    int indices_[4];

    public:
    // Set reference value and indices
    void set(double value, int a, int b, int c = -1, int d = -1);
    // Return reference value
    double value() const;
    // Return specified index
    int indices(int i) const;
    bool operator==(const Geometry &rhs) const;
    bool operator!=(const Geometry &rhs) const;
};

// TOML Conversion
namespace toml
{
template <> struct from<Geometry::GeometryType>
{
    static Geometry::GeometryType from_toml(const toml::value &node)
    {
        auto typeString = node.as_string();
        if (typeString == "angle")
            return Geometry::GeometryType::AngleType;
        else if (typeString == "distance")
            return Geometry::GeometryType::DistanceType;
        else if (typeString == "torsion")
            return Geometry::GeometryType::TorsionType;
        else
            throw toml::type_error(
                fmt::format("Unhandled geometry type '{}' - can't convert from TOML value.\n", std::string(typeString)),
                node.location());
    }
};

template <> struct into<Geometry::GeometryType>
{
    static toml::basic_value<toml::preserve_comments> into_toml(const Geometry::GeometryType &e)
    {
        switch (e)
        {
            case Geometry::GeometryType::AngleType:
                return "angle";
            case Geometry::GeometryType::DistanceType:
                return "distance";
            case Geometry::GeometryType::TorsionType:
                return "torsion";
            default:
                throw std::runtime_error("Unhandled geometry type - can't convert to TOML value.\n");
        }
    }
};
} // namespace toml
