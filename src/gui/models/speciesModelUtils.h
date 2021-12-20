#include "classes/speciesintra.h"
#include "templates/algorithms.h"
#include <QString>
#include <iostream>
#include <numeric>
#include <vector>

// Populate the parameters of a SpeciesIntra with a comma separated
// string of values
template <class Intra> bool splitParameters(const QString &params, Intra &destination)
{
    if (destination.masterTerm())
        return false;

    std::vector<std::string> terms(destination.parameters().size());
    if (splitString(params.toStdString(), terms.begin(), terms.size()) != terms.size())
        return false;
    for (int i = 0; i < terms.size(); ++i)
        destination.setParameter(i, std::stod(terms[i]));
    return true;
}
