// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/cbor.h"
#include "base/messenger.h"
#include <bit>

namespace CBOR
{

// overloaded structure taken from c++ reference examples for std::visitor
// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// Push a value onto a buffer
template <typename T> void ontoBuffer(T value, std::vector<uint8_t> &buf)
{
    auto value_representation = std::bit_cast<std::array<uint8_t, sizeof(T)>>(value);
    if constexpr (std::endian::native == std::endian::big)
        std::copy(value_representation.begin(), value_representation.end(), std::back_inserter(buf));
    else
        std::copy(value_representation.rbegin(), value_representation.rend(), std::back_inserter(buf));
}

using ByteSource = std::variant<std::ranges::subrange<std::vector<uint8_t>::iterator>, std::ifstream>;

// Move forward in the byte stream
void bs_advance(ByteSource &bs, size_t step)
{
    std::visit(overloaded{[step](std::ranges::subrange<std::vector<uint8_t>::iterator> &source) { source.advance(step); },
                          [step](std::ifstream &source) { source.ignore(step); }},
               bs);
};

// Access the head of the byte structure
uint8_t bs_peek(ByteSource &bs)
{
    return std::visit(overloaded{[](std::ranges::subrange<std::vector<uint8_t>::iterator> &source) -> uint8_t
                                 { return *source.begin(); }, [](std::ifstream &source) -> uint8_t { return source.peek(); }},
                      bs);
};

// Copy from the stream onto a buffer
// The output type has to be a char* to allow ifstream::get to work
void bs_copy(ByteSource &bs, size_t size, char *output)
{
    std::visit(overloaded{[size, output](std::ranges::subrange<std::vector<uint8_t>::iterator> &source)
                          {
                              std::copy(source.begin(), source.begin() + size, output);
                              source.advance(size);
                          },
                          [size, output](std::ifstream &source) { source.read(output, size); }},
               bs);
}

// Copy from the stream onto a buffer with opposite endianness
// The output type has to be a char* to allow ifstream::get to work
void bs_reverse_copy(ByteSource &bs, size_t size, char *output)
{
    std::visit(overloaded{[size, output](std::ranges::subrange<std::vector<uint8_t>::iterator> &source)
                          {
                              std::reverse_copy(source.begin(), source.begin() + size, output);
                              source.advance(size);
                          },
                          [size, output](std::ifstream &source)
                          {
                              source.read(output, size);
                              std::reverse(output, output + size);
                          }},
               bs);
}

// Pull a value from a buffer
template <typename T> T fromBuffer(ByteSource &buf)
{
    T output;
    if constexpr (std::endian::native == std::endian::big)
        bs_copy(buf, sizeof(output), reinterpret_cast<char *>(&output));
    else
        bs_reverse_copy(buf, sizeof(output), reinterpret_cast<char *>(&output));
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
Header getHeader(ByteSource &buffer)
{
    uint64_t size = 0;

    uint8_t head = bs_peek(buffer);
    uint8_t key = (head & 0xE0) >> 5;
    BitCount minor = (BitCount)(head & 0x1F);
    bs_advance(buffer, 1);

    if (key != 7)
    {
        if (minor < BitCount::Bit8)
            size = (uint8_t)minor;
        else if (minor == BitCount::Bit8)
            size = fromBuffer<uint8_t>(buffer);
        else if (minor == BitCount::Bit16)
            size = fromBuffer<uint16_t>(buffer);
        else if (minor == BitCount::Bit32)
            size = fromBuffer<uint32_t>(buffer);
        else if (minor == BitCount::Bit64)
            size = fromBuffer<uint64_t>(buffer);
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

// Convert a serialised value to its CBOR representation
std::vector<uint8_t> to(const SerialisedValue &node)
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
                auto element = to(n);
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
                auto key = to(k);
                auto value = to(v);
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
std::tuple<SerialisedValue, ByteSource> fromInner(ByteSource &bytes)
{
    SerialisedValue result;
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
            str.resize(size);
            bs_copy(bytes, size, str.data());
            result = str;
            break;
        }
        case MajorKey::ARRAY: // Array
        {
            std::vector<SerialisedValue> buf;
            for (auto i = 0; i < size; ++i)
            {
                auto [elem, rest] = fromInner(bytes);
                bytes = std::move(rest);
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
                auto [key, rest] = fromInner(bytes);
                auto [value, remainder] = fromInner(rest);
                bytes = std::move(remainder);
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
    return {result, std::move(bytes)};
}

// Parse a CBOR representation of a serialised value
SerialisedValue from(std::ranges::subrange<std::vector<uint8_t>::iterator> bytes)
{
    ByteSource source{bytes};
    return std::get<0>(fromInner(source));
}

// Parse a CBOR representation of a serialised value
SerialisedValue from(std::ifstream &&infile)
{
    ByteSource source{std::move(infile)};
    return std::get<0>(fromInner(source));
}

void skipElement(ByteSource &source)
{
    auto [header, minor, size] = getHeader(source);
    switch (header)
    {
        case MajorKey::STRING: // String
        case MajorKey::UTF8:   // String
            bs_advance(source, size);
            break;
        case MajorKey::ARRAY: // Array
        {
            for (auto i = 0; i < size; ++i)
                skipElement(source);
            break;
        }
        case MajorKey::TABLE: // Map
        {
            for (auto i = 0; i < size; ++i)
            {
                skipElement(source); // Skip key
                skipElement(source); // Skip value
            }
            break;
        }
        case MajorKey::MISC:
        {
            switch ((MiscMinor)minor)
            {
                case MiscMinor::FLOAT: // Float
                    bs_advance(source, sizeof(float));
                    break;
                case MiscMinor::DOUBLE: // Float
                    bs_advance(source, sizeof(double));
                    break;
                default:
                    break;
            }
        }
        default:
            break;
    }
};

void skipTo(ByteSource &source, Path path)
{

    while (!path.empty())
    {
        auto loc = path.back();
        path.pop_back();

        std::visit(
            overloaded{[&source](const int step)
                       {
                           auto header = getHeader(source);
                           if (header.major != MajorKey::ARRAY)
                               Messenger::exception("Cannot descend into Major Key {}, especially with instruction step {}.",
                                                    (int)header.major, step);
                           if (step >= header.size)
                               Messenger::exception("Array index {} exceeds size {}", step, header.size);
                           for (auto i = 0; i < step; ++i)
                               skipElement(source);
                       },
                       [&source](const std::string &step)
                       {
                           auto header = getHeader(source);
                           if (header.major != MajorKey::TABLE)
                               Messenger::exception("Cannot descend into Major Key {}, especially with instruction step {}.",
                                                    (int)header.major, step);
                           for (auto i = 0; i < header.size; ++i)
                           {
                               auto [name, rest] = fromInner(source);
                               source = std::move(rest);
                               if (name.as_string() == step)
                                   break;
                               else
                                   skipElement(source);
                           }
                       }},
            loc);
    }
}

// Pull a piece of a TOML value from a large CBOR file
SerialisedValue extract(std::ifstream &&infile, Path path)
{
    ByteSource source{std::move(infile)};
    std::reverse(path.begin(), path.end());
    skipTo(source, path);
    return std::get<0>(fromInner(source));
}

} // namespace CBOR
