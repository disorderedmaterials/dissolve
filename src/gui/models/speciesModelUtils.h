#include "classes/speciesintra.h"
#include <QString>

// Join the parameters into a comma separated string
QString joinParameters(const SpeciesIntra &source);

// Populate the parameters of a SpeciesIntra with a comma separated
// string of values
bool splitParameters(const QString &params, SpeciesIntra &destination);
