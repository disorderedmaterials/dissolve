// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include <any>
#include <cassert>
#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>

// Forward Declarations
class KeywordWidgetBase;
class QWidget;

// KeywordWidget Producer
class KeywordWidgetProducer
{
    private:
    KeywordWidgetProducer();

    public:
    KeywordWidgetProducer(const KeywordWidgetProducer &) = delete;
    KeywordWidgetProducer(KeywordWidgetProducer &&) = delete;
    KeywordWidgetProducer &operator=(const KeywordWidgetProducer &) = delete;
    KeywordWidgetProducer &operator=(KeywordWidgetProducer &&) = delete;

    /*
     * Producers
     */
    private:
    // Return type
    using WidgetKeywordProduct = std::pair<QWidget *, KeywordWidgetBase *>;
    // Producer function type
    using ProducerFunction = std::function<WidgetKeywordProduct(KeywordBase *keyword, const CoreData &coreData)>;
    // Producers for all data types
    std::unordered_map<std::type_index, ProducerFunction> producers_;

    private:
    // Return widget pair from supplied widget derived class
    template <class W> WidgetKeywordProduct formProduct(W *keywordWidget) { return {keywordWidget, keywordWidget}; }
    // Register producer for specific class
    template <class K> void registerProducer(ProducerFunction function) { producers_[typeid(K)] = std::move(function); }
    template <class K, class W> void registerProducer()
    {
        producers_[typeid(K *)] = [&](KeywordBase *keyword, const CoreData &coreData)
        {
            // Cast the base up to the full keyword
            auto *k = dynamic_cast<K *>(keyword);
            assert(k);
            auto *w = new W(nullptr, k, coreData);
            return WidgetKeywordProduct{w, w};
        };
    }
    template <class K> void registerNullProducer()
    {
        producers_[typeid(K *)] = [&](KeywordBase *keyword, const CoreData &coreData) {
            return WidgetKeywordProduct{nullptr, nullptr};
        };
    }
    // Produce object of specified type
    std::pair<QWidget *, KeywordWidgetBase *> produce(KeywordBase *keyword, const CoreData &coreData) const;

    /*
     * Instance
     */
    private:
    // Return the producer instance
    static const KeywordWidgetProducer &instance();

    /*
     * Static Functions
     */
    public:
    // Create new item via template
    static std::pair<QWidget *, KeywordWidgetBase *> create(KeywordBase *keyword, const CoreData &coreData)
    {
        return instance().produce(keyword, coreData);
    }
};
