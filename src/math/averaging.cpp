/*
    *** Averaging
    *** src/math/averaging.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "math/averaging.h"

// Return enum option info for AveragingScheme
EnumOptions<Averaging::AveragingScheme> &Averaging::averagingSchemes()
{
    static EnumOptionsList AveragingSchemeOptions = EnumOptionsList()
                                                    << EnumOption(Averaging::LinearAveraging, "Linear")
                                                    << EnumOption(Averaging::ExponentialAveraging, "Exponential");

    static EnumOptions<Averaging::AveragingScheme> options("AveragingScheme", AveragingSchemeOptions,
                                                           Averaging::LinearAveraging);

    return options;
}
