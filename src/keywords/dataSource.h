// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "templates/optionalRef.h"
#include <optional>
#include <vector>

// Foward declaration
class DataSource;

// Keyword managing data sources
template <typename T, typename F> class DataSourceKeyword : public KeywordBase
{
    public:
    enum class SourceType
    {
        Internal,
        External
    };

    explicit DataSourceKeyword(F addData);
    ~DataSourceKeyword() override = default;

    /*
     * Data
     */
    private:
    F addData;
    // Size of data container
    int containerSize_;
    SourceType sourceType_;

    public:
    // Return reference to data
    Data1D &data();
    Data2D &data();
    Data3D &data();
    const Data1D &data() const;
    const Data2D &data() const;
    const Data3D &data() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
    // Has not changed from initial value
    bool isDefault() const override;
};
