// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "templates/algorithms.h"

// Base type for serialisable data
class SerialisableData
{
    public:
    SerialisableData(std::string_view key) : key_(key) {}
    virtual ~SerialisableData() = default;

    /*
     * Definition
     */
    protected:
    // Key for the data
    std::string key_;

    /*
     * Serialisation
     */
    public:
    // Return whether there is data to serialise
    virtual bool canSerialise() const { return false; }
    // Express as a serialised value
    virtual void serialise(std::string name, SerialisedValue &target) const { return; }
    // Read from a serialised value
    virtual void deserialise(const SerialisedValue &node) {};
    // Resolve named data
    virtual void resolve(const std::map<std::string, const Species *> &speciesInScope) {};
};

// Primary type for a SerialisableClass to a specific DataClass
template <typename DataClass> class SerialisableClass : public SerialisableData
{
    public:
    // Optional Vector of Serialisable
    SerialisableClass(std::string_view key, DataClass &targetData)
        requires(is_optional<DataClass> && is_instance_of_v<typename DataClass::value_type, std::vector> &&
                 std::is_base_of_v<Serialisable<>, typename DataClass::value_type::value_type>)
        : SerialisableData(key), data_(targetData),
          dataSerialiser_(
              [&]()
              {
                  return Serialisable<typename DataClass::value_type>::fromVector(data_.value(), [&](const auto &item)
                                                                                  { return item.into_toml(); });
              }),
          dataDeserialiser_(
              [&](const SerialisedValue &value)
              {
                  targetData.emplace();
                  Serialisable<typename DataClass::value_type>::toVector(value, [&](const auto &node)
                                                                         { data_->emplace_back().deserialise(node); });
              }),
          dataChecker_([&]() { return targetData.has_value() && !targetData.value().empty(); }),
          dataResolver_(
              [&](const std::map<std::string, const Species *> &reachableSpecies)
              {
                  if constexpr (std::is_base_of_v<ResolvableContext, typename DataClass::value_type::value_type>)
                      for (auto &item : data_.value())
                          item.resolve(reachableSpecies);
              })
    {
    }
    // Optional Serialisable
    SerialisableClass(std::string_view key, DataClass &targetData)
        requires(is_optional<DataClass> && std::is_base_of_v<Serialisable<>, typename DataClass::value_type>)
        : SerialisableData(key), data_(targetData), dataSerialiser_([&]() { return data_.value().into_toml(); }),
          dataDeserialiser_(
              [&](const SerialisedValue &value)
              {
                  targetData = typename DataClass::value_type();
                  data_->deserialise(value);
              }),
          dataChecker_([&]() { return targetData.has_value(); }),
          dataResolver_(
              [&](const std::map<std::string, const Species *> &reachableSpecies)
              {
                  if constexpr (std::is_base_of_v<ResolvableContext, typename DataClass::value_type>)
                      data_.value().resolve(reachableSpecies);
              })
    {
    }
    // Vector of Serialisable
    SerialisableClass(std::string_view key, DataClass &targetData)
        requires(is_instance_of_v<DataClass, std::vector> && std::is_base_of_v<Serialisable<>, typename DataClass::value_type>)
        : SerialisableData(key), data_(targetData),
          dataSerialiser_(
              [&]() { return Serialisable<DataClass>::fromVector(data_, [&](const auto &item) { return item.into_toml(); }); }),
          dataDeserialiser_(
              [&](const SerialisedValue &value)
              {
                  data_.clear();
                  Serialisable<DataClass>::toVector(value, [&](const auto &node) { data_.emplace_back().deserialise(node); });
              }),
          dataChecker_([&]() { return !targetData.empty(); }),
          dataResolver_(
              [&](const std::map<std::string, const Species *> &reachableSpecies)
              {
                  if constexpr (std::is_base_of_v<ResolvableContext, typename DataClass::value_type>)
                      for (auto &item : data_)
                          item.resolve(reachableSpecies);
              })
    {
    }
    // Trivial Non-Serialisable Value
    SerialisableClass(std::string_view key, DataClass &value)
        requires(std::is_trivial_v<DataClass>)
        : SerialisableData(key), data_(value), dataSerialiser_(
                                                   [&]()
                                                   {
                                                       SerialisedValue value;
                                                       value = data_;
                                                       return value;
                                                   }),
          dataDeserialiser_(
              [&](const SerialisedValue &value)
              {
                  if constexpr (std::is_floating_point_v<DataClass>)
                      data_ = value.as_floating();
                  else if constexpr (std::is_integral_v<DataClass>)
                      data_ = value.as_integer();
              })
    {
    }
    // Serialisable
    SerialisableClass(std::string_view key, DataClass &value)
        requires(std::is_base_of_v<Serialisable<>, DataClass>)
        : SerialisableData(key), data_(value), dataResolver_(
                                                   [&](const std::map<std::string, const Species *> &reachableSpecies)
                                                   {
                                                       if constexpr (std::is_base_of_v<ResolvableContext, DataClass>)
                                                           data_.resolve(reachableSpecies);
                                                   })
    {
    }
    ~SerialisableClass() override = default;

    /*
     * Data
     */
    protected:
    // Reference to target data
    DataClass &data_;
    // Serialiser for target data
    using DataSerialiser = std::function<SerialisedValue()>;
    DataSerialiser dataSerialiser_{[&]() { return data_.into_toml(); }};
    // Deserialiser for target data
    using DataDeserialiser = std::function<void(const SerialisedValue &value)>;
    DataDeserialiser dataDeserialiser_{[&](const SerialisedValue &value) { data_.deserialise(value); }};
    // Value checker for data, returning whether there is actually data to write
    using ValueChecker = std::function<bool()>;
    ValueChecker dataChecker_{[&]() { return true; }};
    // Resolver function for data
    using DataResolver = std::function<void(const std::map<std::string, const Species *> &)>;
    DataResolver dataResolver_{[&]() { return; }};

    /*
     * Serialisation
     */
    public:
    // Return whether there is data to serialise
    bool canSerialise() const override { return dataChecker_(); }
    // Express as a serialised value
    void serialise(std::string name, SerialisedValue &target) const override { target[name] = dataSerialiser_(); };
    // Read from a serialised value
    void deserialise(const SerialisedValue &node) override { dataDeserialiser_(node); }
    // Resolve named data
    void resolve(const std::map<std::string, const Species *> &speciesInScope) override { dataResolver_(speciesInScope); };
};
