// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/neutronweights.h"
#include "classes/partialset.h"
#include "classes/partialsetaccumulator.h"
#include "genericitems/list.h"
#include "math/data1d.h"
#include "math/data2d.h"
#include "math/data3d.h"
#include "math/sampleddata1d.h"

/*
 * Data1D
 */

template <> GenericItemSearcher<const Data1D>::GenericItemSearcher()
{
    // Custom Classes
    registerSearcher<Array2D<Data1D>>([](const std::any &a, std::string_view dataName) {
        auto &array = std::any_cast<const Array2D<Data1D> &>(a);
        auto it = std::find_if(array.begin(), array.end(), [dataName](const auto &data) { return dataName == data.tag(); });
        return it == array.end() ? OptionalReferenceWrapper<const Data1D>() : OptionalReferenceWrapper<const Data1D>(*it);
    });
    registerSearcher<PartialSet>([](const std::any &a, std::string_view dataName) {
        auto &ps = std::any_cast<const PartialSet &>(a);
        return ps.searchData1D(dataName);
    });
}

/*
 * Data2D
 */

template <> GenericItemSearcher<const Data2D>::GenericItemSearcher() {}

/*
 * Data3D
 */

template <> GenericItemSearcher<const Data3D>::GenericItemSearcher() {}

/*
 * SampledData1D
 */

template <> GenericItemSearcher<const SampledData1D>::GenericItemSearcher()
{
    // Custom Classes
    registerSearcher<Array2D<SampledData1D>>([](const std::any &a, std::string_view dataName) {
        auto &array = std::any_cast<const Array2D<SampledData1D> &>(a);
        auto it = std::find_if(array.begin(), array.end(), [dataName](const auto &data) { return dataName == data.tag(); });
        return it == array.end() ? OptionalReferenceWrapper<const SampledData1D>()
                                 : OptionalReferenceWrapper<const SampledData1D>(*it);
    });
    registerSearcher<PartialSetAccumulator>([](const std::any &a, std::string_view dataName) {
        auto &psa = std::any_cast<const PartialSetAccumulator &>(a);
        return psa.searchSampledData1D(dataName);
    });
}
