// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/serialiser.h"
#include "base/messenger.h"

namespace Serialisable
{
void serialiseOnto(int a, std::string tag, SerialisedValue &target) { target[tag] = a; }
void serialiseOnto(double a, std::string tag, SerialisedValue &target) { target[tag] = a; }
void serialiseOnto(std::string a, std::string tag, SerialisedValue &target) { target[tag] = a; }
} // namespace Serialisable

namespace Deserialisable
{
void deserialiseOnto(int &a, const SerialisedValue &target) { a = target.as_integer(); }
void deserialiseOnto(long &a, const SerialisedValue &target) { a = target.as_integer(); }
void deserialiseOnto(float &a, const SerialisedValue &target) { a = target.as_floating(); }
void deserialiseOnto(double &a, const SerialisedValue &target) { a = target.as_floating(); }
void deserialiseOnto(std::string &a, const SerialisedValue &target) { a = target.as_string(); }
} // namespace Deserialisable
