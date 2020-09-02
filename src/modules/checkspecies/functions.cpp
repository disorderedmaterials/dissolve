/*
    *** CheckSpecies Module - Functions
    *** src/modules/checkspecies/functions.cpp
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

#include "modules/checkspecies/checkspecies.h"

// Check supplied parameter vectors for consistency, returning false if any differ by the supplied tolerance
bool CheckSpeciesModule::checkParameters(const std::vector<double> &source, const std::vector<double> &ref,
                                         const double tolerance)
{
    auto result = true;

    for (auto n = 0; n < std::min(source.size(), ref.size()); ++n)
    {
        if (fabs(source.at(n) - ref.at(n)) >= tolerance)
        {
            Messenger::print("  ... parameter {} is incorrect ({:.5e} vs. {:.5e} reference, delta = {:.5e}", n + 1,
                             source.at(n), ref.at(n), fabs(source.at(n) - ref.at(n)));
            result = false;
        }
    }

    if (source.size() != ref.size())
    {
        Messenger::print("  ... number of parameters supplied to check is inconsistent ({} vs. {})", source.size(), ref.size());
        result = false;
    }

    return result;
}
