#include "dataSource.h"
#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"

template <typename T, typename F>
DataSourceKeyword<T, F>::DataSourceKeyword(F &&addData, std::string_view endKeyword)
    : KeywordBase(typeid(this)), addData_(addData), endKeyword_(endKeyword_)
{
    // Get the correct import object to use
    if (std::is_same_v<T, Data1D>)
    {
        format_ = std::make_unique<Data1DImportFileFormat>();
    }
    else if (std::is_same_v<T, Data2D>)
    {
        format_ = std::make_unique<Data2DImportFileFormat>();
    }
    else if (std::is_same_v<T, Data3D>)
    {
        format_ = std::make_unique<Data3DImportFileFormat>();
    }
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
template <typename T, typename F> int DataSourceKeyword<T, F>::minArguments() const { return 0; }

// Return maximum number of arguments accepted
template <typename T, typename F> std::optional<int> DataSourceKeyword<T, F>::maxArguments() const { return std::nullopt; }

// Deserialise from supplied LineParser, starting at given argument offset
template <typename T, typename F>
bool DataSourceKeyword<T, F>::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Vector of pairs to hold source location and reference to data objects
    std::vector<std::pair<std::string_view, std::reference_wrapper<T>>> dataSources;
    // Vector to hold unique_ptrs to data objects
    std::vector<T> dataVector;

    while (!parser.eofOrBlank())
    {
        std::string_view source;
        T data;

        // If data is internal
        if (DissolveSys::sameString(parser.argsv(startArg), internalKwd_))
        {
            data.setTag(parser.argsv(startArg + 1));
            source = internalKwd_;
        }
        // If data is external
        else if (DissolveSys::sameString(parser.argsv(startArg), externalKwd_))
        {
            // Read the supplied arguements
            if (format_->read(parser, startArg + 1, externalEndKwd_, coreData) != FileAndFormat::ReadResult::Success)
            {
                return false;
            }
            // Import the data
            if (!format_->importData(data, parser.processPool()))
            {
                return false;
            }
            // Set tag to filename
            data.setTag(parser.argsv(startArg + 2));

            source = externalKwd_;
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

        // Add the data to the vectors
        dataVector.push_back(data);
        dataSources.push_back(std::make_pair(source, std::reference_wrapper<T>(data)));
    }

    // Once finished reading the arguments, add the data to the module
    if (!addData_(dataVector))
    {
        Messenger::error("Failed to add data sources supplied for keyword '{}'", name());
        return false;
    }

    data_.push_back(dataSources);

    return true;
}

// Serialise data to specified LineParser
template <typename T, typename F>
bool DataSourceKeyword<T, F>::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{

    for (auto &dataVec : data_)
    {
        if (!parser.writeLineF("{}End{}\n", prefix, keywordName))
            return false;

        for (auto &[source, data] : dataVec)
        {
            // Write source: internal/external
            if (!parser.writeLineF("{}{}  ", prefix, source))
                return false;

            // If data is internal
            if (DissolveSys::sameString(internalKwd_, source))
            {
                if (!parser.writeLineF("'{}'\n", data.get().tag()))
                    return false;
            }
            // If data is external
            else
            {
                // Write filename and format
                format_->setFilename(data.get().tag());
                if (!format_->writeFilenameAndFormat(parser, prefix))
                    return false;
                // Write extra keywords
                if (!format_->writeBlock(parser, prefix)) // Express as a serialisable value
                    return false;
                // End the block
                if (!parser.writeLineF("End{}", source))
                    return false;
            }
        }

        if (!parser.writeLineF("{}End{}\n", prefix, keywordName))
            return false;
    }

    return true;
}

// Express as a serialisable value
template <typename T, typename F> SerialisedValue DataSourceKeyword<T, F>::serialise() const { return SerialisedValue(); }

// Read values from a serialisable value
template <typename T, typename F>
void DataSourceKeyword<T, F>::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
}

// Explicit instantiation of allowed template types
template class DataSourceKeyword<Data1D, std::function<bool(std::vector<Data1D>)>>;
template class DataSourceKeyword<Data2D, std::function<bool(std::vector<Data2D>)>>;
template class DataSourceKeyword<Data3D, std::function<bool(std::vector<Data3D>)>>;
