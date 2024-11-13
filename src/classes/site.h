// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "math/matrix3.h"
#include <memory>
#include <optional>

// Forward Declarations
class Molecule;
class SpeciesSite;

// Site Definition
class Site
{
    public:
    Site(const SpeciesSite *parent = nullptr, std::optional<int> uniqueSiteIndex = {},
         std::shared_ptr<const Molecule> molecule = {}, const Vec3<double> &origin = {});
    Site(const SpeciesSite *parent = nullptr, std::optional<int> uniqueSiteIndex = {},
         std::shared_ptr<const Molecule> molecule = {}, const Matrix3 &axes = {}, const Vec3<double> &origin = {});
    ~Site() = default;
    Site &operator=(const Site &source) = default;
    Site(const Site &source) = default;
    Site(Site &&source) = default;

    /*
     * Site Definition
     */
    protected:
    // Site definition used to generate site
    const SpeciesSite *parent_;
    // Unique site index in the parent
    std::optional<int> uniqueSiteIndex_;
    // Molecule to which site is related (if relevant)
    std::shared_ptr<const Molecule> molecule_;
    // Site origin
    Vec3<double> origin_;
    // Local axes
    Matrix3 axes_;
    // Whether local axes have been set / defined
    bool hasAxes_{false};

    public:
    // Axis Enum
    enum SiteAxis
    {
        XAxis = 0,
        YAxis = 1,
        ZAxis = 2
    };
    // Return enum options for SiteAxis
    static EnumOptions<Site::SiteAxis> siteAxis();

    public:
    // Return the parent
    const SpeciesSite *parent() const;
    // Return the unique site index in the parent
    std::optional<int> uniqueSiteIndex() const;
    // Return site origin
    const Vec3<double> &origin() const;
    // Return Molecule to which site is related (if relevant)
    std::shared_ptr<const Molecule> molecule() const;
    // Return whether local axes are present
    bool hasAxes() const;
    // Return local axes
    const Matrix3 &axes() const;
    // Rotate about axis
    void rotate(double angle, Site::SiteAxis axis);
};
