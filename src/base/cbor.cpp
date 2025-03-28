// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "base/cbor.h"
#include "base/messenger.h"

// Convert a serialed Value to its CBOR representation
std::vector<char> toCBOR(const SerialisedValue &node)
{
    std::vector<char> result;

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

        default:
            Messenger::exception("Unknown TOMl type {}", (int)node.type());
    }
    return result;
}

// Parse a CBOR representation of a serialised value
SerialisedValue fromCBOR(std::vector<char> bytes)
{
    SerialisedValue result;
    if (bytes.empty())
        return result;
    switch ((int8_t)bytes.front())
    {
        case 0x1b:
        {
            int output;
            std::copy(bytes.begin() + 1, bytes.begin() + 1 + sizeof(output), reinterpret_cast<char *>(&output));
            result = output;
            break;
        }
        case 0x3b:
        {
            int output;
            std::copy(bytes.begin() + 1, bytes.begin() + 1 + sizeof(output), reinterpret_cast<char *>(&output));
            result = -1 * output;
            break;
        }
        default:
            Messenger::exception("Unknown type code {:x}", (int8_t)bytes.front());
    }
    return result;
}
