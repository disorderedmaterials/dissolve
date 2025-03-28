// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "base/cbor.h"
#include "base/messenger.h"

// Convert a serialed Value to its CBOR representation
std::vector<uint8_t> toCBOR(const SerialisedValue &node)
{
    std::vector<uint8_t> result;

    switch (node.type())
    {
        case toml::value_t::integer:
        {
            int64_t number = node.as_integer();
            if (number >= 0)
            {
                result.push_back(0x1b);
            }
            else
            {
                result.push_back(0x3b);
                number *= -1;
            }
            char *ptr = reinterpret_cast<char *>(&number);
            std::copy(ptr, ptr + sizeof(number), std::back_inserter(result));
            break;
        }
        case toml::value_t::boolean:
        {
            if (node.as_boolean())
                result.push_back(0xE0 + 21);
            else
                result.push_back(0xE0 + 20);
            break;
        }
        case toml::value_t::array:
        case toml::value_t::empty:
        case toml::value_t::floating:
        case toml::value_t::string:
        case toml::value_t::table:
        case toml::value_t::local_date:
        case toml::value_t::local_time:
        case toml::value_t::local_datetime:
        case toml::value_t::offset_datetime:
            Messenger::exception("TODO: TOML type {}", (int)node.type());
    }
    return result;
}

// Parse a CBOR representation of a serialised value
std::tuple<SerialisedValue, std::ranges::subrange<std::vector<uint8_t>::iterator>>
fromCBOR(std::ranges::subrange<std::vector<uint8_t>::iterator> bytes)
{
    SerialisedValue result;
    if (bytes.begin() == bytes.end())
        return {result, bytes};
    switch ((uint8_t)*bytes.begin())
    {
        case 0x1b:
        {
            int64_t output;
            bytes.advance(1);
            std::copy(bytes.begin(), bytes.begin() + sizeof(output), reinterpret_cast<char *>(&output));
            result = output;
            bytes.advance(sizeof(output));
            break;
        }
        case 0x3b:
        {
            int64_t output;
            bytes.advance(1);
            std::copy(bytes.begin(), bytes.begin() + sizeof(output), reinterpret_cast<char *>(&output));
            result = -1 * output;
            bytes.advance(sizeof(output));
            break;
        }
        case 0xE0 + 20:
            bytes.advance(1);
            result = false;
            break;
        case 0xE0 + 21:
            bytes.advance(1);
            result = true;
            break;
        default:
            Messenger::exception("Unknown type code {:x}", (int8_t)*bytes.begin());
    }
    return {result, bytes};
}
