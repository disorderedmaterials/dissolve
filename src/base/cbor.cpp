// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "base/cbor.h"
#include "base/messenger.h"

// Push a value onto a buffer
template <typename T> void ontoBuffer(T value, std::vector<uint8_t> &buf)
{
    uint8_t *ptr = reinterpret_cast<uint8_t *>(&value);
    std::copy(ptr, ptr + sizeof(value), std::back_inserter(buf));
}

// Pull a value from a buffer
template <typename T> T fromBuffer(std::ranges::subrange<std::vector<uint8_t>::iterator> &buf)
{
    T output;
    std::copy(buf.begin(), buf.begin() + sizeof(output), reinterpret_cast<uint8_t *>(&output));
    buf.advance(sizeof(output));
    return output;
}

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
            ontoBuffer(number, result);
            break;
        }
        case toml::value_t::boolean:
        {
            if (node.as_boolean())
                result.push_back(0xF5);
            else
                result.push_back(0xF4);
            break;
        }
        case toml::value_t::floating:
        {
            double number = node.as_floating();
            result.push_back(0xFB);

            ontoBuffer(number, result);
            break;
        }
        case toml::value_t::string:
        case toml::value_t::array:
        case toml::value_t::table:
        case toml::value_t::empty:
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
        case 0x1b: // Positive Int
        {
            bytes.advance(1);
            result = fromBuffer<int64_t>(bytes);
            break;
        }
        case 0x3b: // Negative Int
        {
            bytes.advance(1);
            result = -1 * fromBuffer<int64_t>(bytes);
            break;
        }
        case 0xF4: // Boolean False
            bytes.advance(1);
            result = false;
            break;
        case 0xF5: // Boolean True
            bytes.advance(1);
            result = true;
            break;
        case 0xFB: // Float
        {
            bytes.advance(1);
            result = fromBuffer<double>(bytes);
            break;
        }
        default:
            Messenger::exception("Unknown type code {:x}", (int8_t)*bytes.begin());
    }
    return {result, bytes};
}
