// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/benchmark/benchmark.h"

// Return type of module
std::string_view BenchmarkModule::type() const { return "Benchmark"; }

// Return category for module
std::string_view BenchmarkModule::category() const { return "Checks / Tests"; }

// Return brief description of module
std::string_view BenchmarkModule::brief() const { return "Perform benchmarking on a variety of functions"; }

// Return the number of Configuration targets this Module requires
int BenchmarkModule::nRequiredTargets() const { return Module::OneOrMoreTargets; }
