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
         std::shared_ptr<const Molecule> molecule = nullptr, Vec3<double> origin = Vec3<double>());
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
    // Site origin
    Vec3<double> origin_;
    // Molecule to which site is related (if relevant)
    std::shared_ptr<const Molecule> molecule_;

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
    virtual bool hasAxes() const;
    // Return local axes
    virtual const Matrix3 &axes() const;
};

// Oriented Site Definition
class OrientedSite : public Site
{
    public:
    OrientedSite(const SpeciesSite *parent = nullptr, std::optional<int> uniqueSiteIndex = {},
                 std::shared_ptr<const Molecule> molecule = nullptr, Vec3<double> origin = Vec3<double>(),
                 Vec3<double> xAxis = Vec3<double>(), Vec3<double> yAxis = Vec3<double>(), Vec3<double> zAxis = Vec3<double>());
    OrientedSite &operator=(const OrientedSite &source) = default;
    OrientedSite(const OrientedSite &source) = default;
    OrientedSite(OrientedSite &&source) = default;

    /*
     * Site Definition
     */
    private:
    // Local axes
    Matrix3 axes_;

    public:
    // Axis Enum
    enum SiteAxis
    {
        XAxis = 0,
        YAxis = 1,
        ZAxis = 2
    };
    // Return enum options for SiteAxis
    static EnumOptions<OrientedSite::SiteAxis> siteAxis();
    // Return whether local axes are present
    bool hasAxes() const override;
    // Return local axes
    const Matrix3 &axes() const override;
    // Rotate about axis
    void rotate(double angle, OrientedSite::SiteAxis axis);
};
