// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/neutronweights.h"
#include "classes/partialset.h"
#include "classes/xrayweights.h"
#include "genericitems/list.h"
#include "genericitems/producers.h"
#include "math/data1d.h"
#include "math/data2d.h"
#include "math/data3d.h"
#include "math/histogram1d.h"
#include "math/histogram2d.h"
#include "math/histogram3d.h"
#include <ios>

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
        return ps.data1DWithTag(dataName);
    });
}

/*
 * Data2D
 */

template <> GenericItemSearcher<const Data2D>::GenericItemSearcher()
{
    // Custom Classes
}

/*
 * Data3D
 */

template <> GenericItemSearcher<const Data3D>::GenericItemSearcher()
{
    // Custom Classes
}
