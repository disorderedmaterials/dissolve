#include "gui/models/speciesModelUtils.h"

bool splitParameters(const QString &params, SpeciesIntra &destination)
{
    if (destination.masterParameters())
        return false;

    std::vector<std::string> terms(destination.parameters().size());
    if (splitString(params.toStdString(), terms.begin(), terms.size()) != terms.size())
        return false;
    for (int i = 0; i < terms.size(); ++i)
        destination.setParameter(i, std::stod(terms[i]));
    return true;
}
