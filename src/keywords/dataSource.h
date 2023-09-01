// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"
#include "io/import/dataBase.h"
#include "keywords/dataSourceBase.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "templates/optionalRef.h"
#include <optional>
#include <vector>

// Keyword managing data sources
// Template arguements: data class (Data1D, Data2D ...), add data function type
template <class DataType, class DataFormat> class DataSourceKeyword : public DataSourceKeywordBase
{
    /*
     * Data
     */
    private:
    // Format object for the data
    DataFormat format_;

    /*
     * Arguments
     */
    private:
    // End keyword for the dataSource keyword
    const std::string endKeyword_;

    public:
    explicit DataSourceKeyword(std::shared_ptr<DataPair> &dataSources, std::string_view endKeyword);
    ~DataSourceKeyword() override = default;

    // Return minimum number of arguments accepted
    int minArguments() const override { return 0; };
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override { return std::nullopt; };
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override
    {
        int dataCounter = 0;
        while (!parser.eofOrBlank())
        {
            // Only allows maximum of two data sources per keyword
            if (dataCounter >= 2)
            {
                break;
            }

            DataPair dataPair;

            // If data is internal
            if (DissolveSys::sameString(parser.argsv(startArg), internalKeyword()))
            {
                DataSource internalData{Internal};
                // Has the first item already been initialised?
                dataCounter == 0 ? dataPair.first = internalData : dataPair.second = internalData;
                dataCounter++;
            }
            // If data is external
            else if (DissolveSys::sameString(parser.argsv(startArg), externalKwd_))
            {
                DataSource externalData{External};
                // Read the supplied arguements
                if (externalData.data()->read(parser, startArg + 1, fmt::format("End{}", externalKeyword()), coreData) !=
                    FileAndFormat::ReadResult::Success)
                {
                    return false;
                }

                // Has the first item already been initialised?
                dataCounter == 0 ? dataPair.first = externalData : dataPair.second = externalData;
                dataCounter++;
            }
            else
            {
                return Messenger::error("Unsupported data location '{}' provided to keyword '{}'\n", parser.argsv(startArg),
                                        name());
            }

            // Read the next line
            if (parser.getArgsDelim() != LineParser::Success)
            {
                return false;
            }

            // Is this the end of the block?
            if (DissolveSys::sameString(parser.argsv(0), endKeyword_))
            {
                break;
            }
        }

        // Add the data to data vector
        dataSources_.get()->push_back(dataPair);

        return true;
    }

    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override
    {
        for (auto &[dataPair] : *dataSources_.get())
        {
            if (!parser.writeLineF("{}{}\n", prefix, keywordName))
                return false;

            // Write source: internal/external
            if (!parser.writeLineF("{}{}  ", prefix, dataSourceTypes().keyword(dataPair.first.dataSourceType())))
                return false;

            // If data is internal
            if (dataPair.first.dataSourceType() == Internal)
            {
                if (!parser.writeLineF("'{}'\n", dataPair.first.data<std::string>()))
                    return false;
            }
            // If data is external
            else
            {
                // Write filename and format
                if (!dataPair.first.data<FileAndFormat>()->writeFilenameAndFormat(parser, prefix))
                    return false;
                // Write extra keywords
                if (!dataPair.first.data<FileAndFormat>()->writeBlock(parser, prefix)) // Express as a serialisable value
                    return false;
                // End the block
                if (!parser.writeLineF("End{}", externalKeyword()))
                    return false;
            }

            if (!dataPair.second)
            {
                continue;
            }

            // Second data source (optional)

            // Write source: internal/external
            if (!parser.writeLineF("{}{}  ", prefix, dataSourceTypes().keyword(dataPair.second.dataSourceType())))
                return false;

            // If data is internal
            if (dataPair.second.dataSourceType() == Internal)
            {
                if (!parser.writeLineF("'{}'\n", dataPair.second.data<std::string>()))
                    return false;
            }
            // If data is external
            else
            {
                // Write filename and format
                if (!dataPair.second.data<FileAndFormat>()->writeFilenameAndFormat(parser, prefix))
                    return false;
                // Write extra keywords
                if (!dataPair.second.data<FileAndFormat>()->writeBlock(parser, prefix)) // Express as a serialisable value
                    return false;
                // End the block
                if (!parser.writeLineF("End{}", externalKeyword()))
                    return false;
            }

            if (!parser.writeLineF("{}End{}\n", prefix, keywordName))
                return false;
        }

        return true;
    }
    // Express as a serialisable value
    SerialisedValue serialise() const override { return {{"type", "x"}, {"parameters", "y"}}; }
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override {}
};
