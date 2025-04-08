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

enum class MajorKey
{
    POS_INT = 0,
    NEG_INT,
    STRING,
    UTF8,
    ARRAY,
    TABLE,
    TAG,
    MISC,
};

enum class MiscMinor
{
    FALSE = 20,
    TRUE = 21,
    // NAN = 23,
    FLOAT = 26,
    DOUBLE = 27,
};

enum class BitCount
{
    Bit8 = 24,
    Bit16 = 25,
    Bit32 = 26,
    Bit64 = 27,
};

struct Header
{
    MajorKey major;
    BitCount minor;
    uint64_t size;
};

// Parse a header file and size information from a file
Header getHeader(std::ranges::subrange<std::vector<uint8_t>::iterator> &buf)
{
    uint64_t size = 0;

    uint8_t head = *buf.begin();
    uint8_t key = (head & 0xE0) >> 5;
    BitCount minor = (BitCount)(head & 0x1F);
    buf.advance(1);

    if (key != 7)
    {
        if (minor < BitCount::Bit8)
            size = (uint8_t)minor;
        else if (minor == BitCount::Bit8)
            size = fromBuffer<uint8_t>(buf);
        else if (minor == BitCount::Bit16)
            size = fromBuffer<uint16_t>(buf);
        else if (minor == BitCount::Bit32)
            size = fromBuffer<uint32_t>(buf);
        else if (minor == BitCount::Bit64)
            size = fromBuffer<uint64_t>(buf);
    }

    return {(MajorKey)key, minor, size};
}

void writeHeader(MajorKey key, uint64_t size, std::vector<uint8_t> &buf)
{
    uint8_t header = (uint8_t)key << 5;
    if (size < 24)
    {
        buf.push_back(header + size);
    }
    else if (size < 0x100)
    {
        buf.push_back(header + (uint8_t)BitCount::Bit8);
        uint8_t s = size;
        ontoBuffer(s, buf);
    }
    else if (size < 0x10000)
    {
        buf.push_back(header + (uint8_t)BitCount::Bit16);
        uint16_t s = size;
        ontoBuffer(s, buf);
    }
    else if (size < 0x100000000)
    {
        buf.push_back(header + (uint8_t)BitCount::Bit32);
        uint32_t s = size;
        ontoBuffer(s, buf);
    }
    else
    {
        buf.push_back(header + (uint8_t)BitCount::Bit64);
        uint64_t s = size;
        ontoBuffer(s, buf);
    }
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
                writeHeader(MajorKey::POS_INT, number, result);
            else
                writeHeader(MajorKey::NEG_INT, std::abs(number), result);
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
            uint64_t len = str.size();
            writeHeader(MajorKey::UTF8, len, result);
            std::copy(str.begin(), str.end(), std::back_inserter(result));
            break;
        }
        case toml::value_t::array:
        {
            uint64_t len = node.as_array().size();
            writeHeader(MajorKey::ARRAY, len, result);
            for (auto n : node.as_array())
            {
                auto element = toCBOR(n);
                std::copy(element.begin(), element.end(), std::back_inserter(result));
            }
            break;
        }
        case toml::value_t::table:
        {
            uint64_t len = node.as_table().end() - node.as_table().begin();
            writeHeader(MajorKey::TABLE, len, result);
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
    auto [header, minor, size] = getHeader(bytes);
    switch (header)
    {
        case MajorKey::POS_INT: // Positive Int
        {
            result = size;
            break;
        }
        case MajorKey::NEG_INT: // Negative Int
        {
            result = -1 * size;
            break;
        }
        case MajorKey::STRING: // String
        case MajorKey::UTF8:   // String
        {
            std::string str;
            str.reserve(size);
            std::copy(bytes.begin(), bytes.begin() + size, std::back_inserter(str));
            bytes.advance(size);
            result = str;
            break;
        }
        case MajorKey::ARRAY: // Array
        {
            std::vector<SerialisedValue> buf;
            for (auto i = 0; i < size; ++i)
            {
                auto [elem, rest] = fromCBOR(bytes);
                bytes = rest;
                buf.push_back(elem);
            }
            result = buf;
            break;
        }
        case MajorKey::TABLE: // Map
        {
            SerialisedValue map;
            for (auto i = 0; i < size; ++i)
            {
                auto [key, rest] = fromCBOR(bytes);
                auto [value, remainder] = fromCBOR(rest);
                bytes = remainder;
                map[key.as_string()] = value;
            }
            result = map;
            break;
        }
        case MajorKey::TAG:
            Messenger::exception("Cannot parse CBOR Tag values");
            break;
        case MajorKey::MISC:
        {
            switch ((MiscMinor)minor)
            {
                case MiscMinor::FALSE:
                    result = false;
                    break;
                case MiscMinor::TRUE: // Boolean True
                    result = true;
                    break;
                case MiscMinor::FLOAT: // Float
                    result = fromBuffer<float>(bytes);
                    break;
                case MiscMinor::DOUBLE: // Float
                    result = fromBuffer<double>(bytes);
                    break;
                default:
                    Messenger::exception("Unknown type code {:x}, {:x}", (int)header, (int)minor);
            }
        }
    }
    return {result, bytes};
}
