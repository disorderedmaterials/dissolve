#include "dataSource.h"
#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"

template <typename T, typename F>
DataSourceKeyword<T, F>::DataSourceKeyword(F addData, std::string_view endKeyword)
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
    std::vector<T> dataSources;

    while (!parser.eofOrBlank())
    {
        T data;
        // If data is internal
        if (DissolveSys::sameString(parser.argsv(startArg), internalKwd_))
        {
            data.setTag(parser.argsv(startArg + 1));
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
        // Add the data to vector
        dataSources.push_back(data);
    }

    // Once finished reading the arguements, add the data to the module
    if (!addData_(dataSources))
    {
        Messenger::error("Failed to add data sources supplied for keyword '{}'", name());
        return false;
    }

    std::for_each(dataSources.begin(), dataSources.end(), [&](T &d) { data_.push_back(d); });

    return true;
}

// Serialise data to specified LineParser
template <typename T, typename F>
bool DataSourceKeyword<T, F>::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return false;
}

// Express as a serialisable value
template <typename T, typename F> SerialisedValue DataSourceKeyword<T, F>::serialise() const { return SerialisedValue(); }

// Read values from a serialisable value
template <typename T, typename F>
void DataSourceKeyword<T, F>::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
}

// Has not changed from initial value
template <typename T, typename F> bool DataSourceKeyword<T, F>::isDefault() const { return false; }

// Explicit instantiation of allowed template types
template class DataSourceKeyword<Data1D, std::function<bool(std::vector<Data1D>)>>;
template class DataSourceKeyword<Data2D, std::function<bool(std::vector<Data2D>)>>;
template class DataSourceKeyword<Data3D, std::function<bool(std::vector<Data3D>)>>;
