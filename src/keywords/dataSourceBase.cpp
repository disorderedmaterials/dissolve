#include "keywords/dataSource.h"

DataSourceKeywordBase::DataSourceKeywordBase(std::vector<DataSourceKeywordBase::DataPair> &dataSources,
                                             std::string_view endKeyword)
    : KeywordBase(typeid(this)), dataSources_(dataSources), endKeyword_(endKeyword)
{
}

std::vector<DataSourceKeywordBase::DataPair> &DataSourceKeywordBase::dataSources() { return dataSources_; }

std::string DataSourceKeywordBase::externalKeyword() { return DataSource::dataSourceTypes().keyword(DataSource::External); }

std::string DataSourceKeywordBase::internalKeyword() { return DataSource::dataSourceTypes().keyword(DataSource::Internal); }
