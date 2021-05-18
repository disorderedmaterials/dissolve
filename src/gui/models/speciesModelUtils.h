#include "classes/speciesintra.h"
#include <QString>

// Join the parameters values of a
QString joinParameters(const SpeciesIntra &source);

bool splitParameters(const QString &params, SpeciesIntra &destination);
