#include "classes/speciesintra.h"
#include <QString>
#include <iostream>
#include <numeric>
#include <vector>

// Populate the parameters of a SpeciesIntra with a comma separated
// string of values
bool splitParameters(const QString &params, SpeciesIntra &destination);
