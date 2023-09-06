#include "keywords/dataSource.h"

DataSourceKeywordBase::DataSourceKeywordBase(std::vector<DataSourceKeywordBase::DataPair> &dataSources,
                                             std::string_view endKeyword)
    : KeywordBase(typeid(this)), dataSources_(dataSources), endKeyword_(endKeyword)
{
}
// Returns reference to module data
std::vector<DataSourceKeywordBase::DataPair> &DataSourceKeywordBase::dataSources() { return dataSources_; }