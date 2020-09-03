/*
    *** Benchmark Module - Definition
    *** src/modules/benchmark/definition.cpp
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

#include "modules/benchmark/benchmark.h"

// Return type of module
std::string_view BenchmarkModule::type() const { return "Benchmark"; }

// Return category for module
std::string_view BenchmarkModule::category() const { return "NO CATEGORY ASSIGNED"; }

// Return brief description of module
std::string_view BenchmarkModule::brief() const { return "Perform benchmarking on a variety of functions"; }

// Return the number of Configuration targets this Module requires
int BenchmarkModule::nRequiredTargets() const { return Module::OneOrMoreTargets; }
