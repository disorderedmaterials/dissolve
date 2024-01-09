// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/partialSet.h"
#include "items/item.h"
#include "math/data1D.h"
#include "templates/optionalRef.h"
#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>

// GenericItem Searcher
template <class D> class GenericItemSearcher
{
    private:
    GenericItemSearcher();

    public:
    GenericItemSearcher(const GenericItemSearcher &) = delete;
    GenericItemSearcher(GenericItemSearcher &&) = delete;
    GenericItemSearcher &operator=(const GenericItemSearcher &) = delete;
    GenericItemSearcher &operator=(GenericItemSearcher &&) = delete;

    /*
     * Searchers
     */
    private:
    // Searcher function type
    using SearcherFunction = std::function<OptionalReferenceWrapper<const D>(const std::any &a, std::string_view dataName)>;
    // Searchers for data types
    std::unordered_map<std::type_index, SearcherFunction> searchers_;

    private:
    // Register searcher for specific class
    template <class T> void registerSearcher(SearcherFunction func) { searchers_[typeid(T)] = std::move(func); };
    // Search for data of templated type
    OptionalReferenceWrapper<const D> searchObject(const std::any &a, std::string_view dataName) const
    {
        // Find a suitable searcher and call it
        auto it = searchers_.find(a.type());
        if (it == searchers_.end())
            return {};

        return (it->second)(a, dataName);
    }

    /*
     * Instance
     */
    private:
    // Return the searcher instance
    static const GenericItemSearcher &instance()
    {
        static GenericItemSearcher instance;

        return instance;
    }

    /*
     * Static Functions
     */
    public:
    // Search for data item of templated type in supplied object
    static OptionalReferenceWrapper<const D> search(const std::any &a, std::string_view dataName)
    {
        return instance().searchObject(a, dataName);
    }
};
