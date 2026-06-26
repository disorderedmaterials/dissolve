// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/parserLibrary.h"
#include <format>

namespace Parsers
{

// A parser that accepts an integer greater than or equal to zero
Parser<int> natural()
{
    auto result = digits();
    return result.map(
        [](const auto terms) -> int
        {
            int total = 0;
            for (auto term : terms)
                total = 10 * total + (term - '0');
            return total;
        });
}

// Take an optional minus sign and a whole number to create an integer
int nat2int(std::optional<std::string_view> minus, int number)
{
    if (minus)
        return -number;
    return number;
}

// A parser that accepts an integer
Parser<int> integer() { return (maybe("-"_p) & natural()).apply(nat2int); }

// Take the numbers before the decimal, some optional digits after the period, and an optional exponent, and return a double.
double nat2dbl(std::optional<std::string_view> minus, std::string_view front, std::optional<std::string_view> decimals,
               std::optional<std::tuple<std::optional<::std::string_view>, std::string_view>> exponent)
{
    auto basic = std::format("{}{}", minus.value_or(""), front);
    if (decimals)
        basic += std::format(".{}", *decimals);
    if (exponent)
        basic += std::format("e{}{}", std::get<0>(*exponent).value_or(""), std::get<1>(*exponent));
    return std::stod(basic);
}

// A parser that accepts a real, floating point number
Parser<double> real()
{
    auto result = maybe("-"_p) & digits() & maybe("." >> digits()) & maybe(("e"_p | "E"_p) >> maybe("-"_p | "+"_p) & digits());
    return result.apply(nat2dbl);
}

// A parser that accepts a 3-vector of floating point numbers
Parser<Vector3> vector3()
{
    return (real() & spaces() >> real() & spaces() >> real())
        .apply([](double x, double y, double z) { return Vector3(x, y, z); });
}

// A parser that accepts a 3x3 matrix of floating point numbers
Parser<Matrix3> matrix3()
{
    return (maybe(inlineSpaces()) >> vector3() << spaces() & maybe(inlineSpaces()) >> vector3() << spaces() &
            maybe(inlineSpaces()) >> vector3() << spaces())
        .apply([](const auto m1, const auto m2, const auto m3) { return Matrix3(m1, m2, m3); });
}

} // namespace Parsers
