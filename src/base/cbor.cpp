// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "base/cbor.h"
#include "base/messenger.h"
#include <bit>

// Push a value onto a buffer
template <typename T> void ontoBuffer(T value, std::vector<uint8_t> &buf)
{
    auto value_representation = std::bit_cast<std::array<uint8_t, sizeof(T)>>(value);
    if constexpr (std::endian::native == std::endian::big)
        std::copy(value_representation.begin(), value_representation.end(), std::back_inserter(buf));
    else
        std::copy(value_representation.rbegin(), value_representation.rend(), std::back_inserter(buf));
}

// Pull a value from a buffer
template <typename T> T fromBuffer(std::ranges::subrange<std::vector<uint8_t>::iterator> &buf)
{
    T output;
    if constexpr (std::endian::native == std::endian::big)
        std::copy(buf.begin(), buf.begin() + sizeof(output), reinterpret_cast<uint8_t *>(&output));
    else
        std::reverse_copy(buf.begin(), buf.begin() + sizeof(output), reinterpret_cast<uint8_t *>(&output));
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
        {
            std::string str = node.as_string();
            result.push_back(0x7b);
            uint64_t len = str.size();
            ontoBuffer(len, result);
            std::copy(str.begin(), str.end(), std::back_inserter(result));
            break;
        }
        case toml::value_t::array:
        {
            result.push_back(0x9b);
            uint64_t len = node.as_array().size();
            ontoBuffer(len, result);
            for (auto n : node.as_array())
            {
                auto element = toCBOR(n);
                std::copy(element.begin(), element.end(), std::back_inserter(result));
            }
            break;
        }
        case toml::value_t::table:
        {
            result.push_back(0xbb);
            uint64_t len = node.as_table().end() - node.as_table().begin();
            ontoBuffer(len, result);
            for (auto [k, v] : node.as_table())
            {
                auto key = toCBOR(k);
                auto value = toCBOR(v);
                std::copy(key.begin(), key.end(), std::back_inserter(result));
                std::copy(value.begin(), value.end(), std::back_inserter(result));
            }
            break;
        }
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
        case 0x7b: // String
        {
            bytes.advance(1);
            auto len = fromBuffer<uint64_t>(bytes);
            std::string str;
            str.reserve(len);
            std::copy(bytes.begin(), bytes.begin() + len, std::back_inserter(str));
            result = str;
            bytes.advance(len);
            break;
        }
        case 0x9b: // Array
        {
            std::vector<SerialisedValue> buf;
            bytes.advance(1);
            auto len = fromBuffer<uint64_t>(bytes);
            for (int i = 0; i < len; ++i)
            {
                auto [elem, rest] = fromCBOR(bytes);
                bytes = rest;
                buf.push_back(elem);
            }
            result = buf;
            break;
        }
        case 0xbb: // Map
        {
            SerialisedValue map;
            bytes.advance(1);
            auto len = fromBuffer<uint64_t>(bytes);
            for (int i = 0; i < len; ++i)
            {
                auto [key, rest] = fromCBOR(bytes);
                auto [value, remainder] = fromCBOR(rest);
                bytes = remainder;
                map[key.as_string()] = value;
            }
            result = map;
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
