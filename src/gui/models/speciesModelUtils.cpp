#include "gui/models/speciesModelUtils.h"
#include <QStringList>
#include <algorithm>
#include <vector>

QString joinParameters(const SpeciesIntra &source)
{
    std::vector<QString> vec(source.parameters().size());
    std::transform(source.parameters().begin(), source.parameters().end(), vec.begin(),
		   [](const double value) { return QString::number(value); });
    QStringList terms(vec.begin(), vec.end());
    return terms.join(",");
}

bool splitParameters(const QString &params, SpeciesIntra &destination)
{
    if (destination.masterParameters())
	return false;
    auto terms = params.split(",");
    if (terms.size() != destination.parameters().size())
	return false;
    for (int i = 0; i < terms.size(); ++i)
	destination.setParameter(i, terms[i].toDouble());
    return true;
}
