---
title: Data Management
---

## Introduction

### Purpose
This document gives an overview of the handling (storage, retrieval, I/O) of generic data within Dissolve.

### Scope
This overview provides implementation details on data storage and management techniques used with the Dissolve code, presenting the salient design decisions, architectures, and classes relevant to its implementation. The document is suitable for developers wishing to understand, modify, fix, or otherwise extend the code.

### Definitions


## Overview

### Brief

The core functionality of Dissolve is contained within independent [modules]({{< ref "modules" >}}), which may run in any order. Each module typically creates useful data that are of interest to the user (e.g. calculations which yield some specific property of interest), to other modules (which take pre-existing data and use it as the basis for other calculations), or which is necessary to store in order to permit a successful restart of the simulation (i.e. historic data used when performing averaging). These data may be PODs, opaque classes, or templated objects.

### Rationale

Storage of these data locally within the modules itself and providing standard accessors is undesirable since this requires that modules requiring data stored in another module must know the address and relevant member accessor function to retrieve it, increasing the interface complexity as well as the fragility between (strictly independent) modules. Instead, data are stored in a type-agnostic container class, tagged with a descriptive name in order to permit retrieval. These containers are used as centralised repositories (located in the main `Dissolve` class or locally in configurations) for data created during the main simulation loop

This 'blackboard-style' approach to the storage of data has the following benefits:
1. 'Useful' data is localised in specific places, rather than being stored locally within module instances, and so can be easily transferred around the code as is necessary.
1. Modules may implement different algorithms in order to calculate the same (named) data, and which can then be accessed by other modules without requiring knowledge of the exact nature of the creating module.
1. Data relevant to the restarting / continuation of simulations is contained in a single area outside of any modules, greatly simplifying the creation and loading of restart files.
1. Since the sequence of modules to be executed is not known _a priori_, the lack of availability of a named piece of data is meaningful, and eliminates the need to check for specific module types having already been executed and their data prepared and available for others to use.

## Implementation

### Relevant Classes

- [GenericList](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/list.h)
- [GenericListHelper<T>](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/listhelper.h) (template)
- [GenericItem](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/item.h)
- [GenericItemContainer<T>](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/container.h) (template)
- [GenericItemBase](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/base.h)

### Core Container Class

The [`GenericItemContainer`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/container.h) is a template with single class parameter `T`, and derives from the non-template [`GenericItem`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/item.h) class as follows to form the basic storage container:

{{< mermaid >}}
classDiagram
    GenericItemContainer <-- GenericItem
    class GenericItemContainer {
        - T data_
        + itemClassName()* const char
        + read(LineParser &parser)* bool
        + write(LineParser &parser, const CoreData &coreData)* bool
        + broadcast(ProcessPool &procPool, const int root, const CoreData &coreData) bool
        + equality(ProcessPool &procPool) bool
    }
    class GenericItem {
        - CharString name_
        - CharString description_
        - int version_
        - int flags_
        # createItem(const char *className, const char *name, int flags = 0)* GenericItem
        + itemClassName()* const char
        + read(LineParser &parser)* bool
        + write(LineParser &parser, const CoreData &coreData)* bool
        + broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)* bool
        + equality(ProcessPool &procPool)* bool
    }
{{< /mermaid >}}

[`GenericItem`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/item.h) defines the basic interface required for any object to be committed to our data store, and provides a common base to all templated containers in order to allow congruent list storage and searching. Necessarily [`GenericItem`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/item.h) contains a handful of pure virtual functions providing read/write capability for the data, as well as some related to data transfer when running in parallel (the `broadcast()` and `equality()` member functions). Generic implementations of all four of these functions are provided by [`GenericItemContainer<T>`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/container.h) which simply assumes the presence of functions with identical signatures in `T data_`. For PODs this of course is not the case, and so template specialisations are written for each relevant POD (see, for instance, the specialisation for [`bool`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/bool.h)). Classes that are to be stored within a [`GenericItemContainer`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/container.h) must therefor provide suitable implementations of these functions - to help enforce this such classes should derive from [`GenericItemBase`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/base.h), which contains the virtual functions required, and provides default implementations with suitable error messaging if they are not overriden by the parent class.

### Storing the Container Class

The main storage class for a collection of generic data is the [`GenericList`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/list.h) class, which stores a searchable-by-name list of [`GenericItem`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/item.h) objects. Locating data of a specific name is simply a case of querying [`GenericList`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/list.h) as to its presence. However, creation and retrieval of the actual data (i.e. the templated data contained in [`GenericItemContainer<T>`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/container.h)) requires templated methods to match. This is the role of [`GenericListHelper`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/listhelper.h), which provides static functions to operate on a target [`GenericList`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/list.h) and create and/or return named data of a specific type.

For example, creation, retrieval, and usage of a user-defined class data object proceeds as follows:

```cpp
// A Custom Class
class MyClass : public GenericItemBase
{
	int i_;
	SomeOtherClass val_;
	/* ... */
};

// Create a list
GenericList dataList;

// Create a new MyClass data item in the list
MyClass &newData = GenericListHelper<MyClass>::add(dataList, "SomeNewData", "NamePrefix");

// Do something with the data
newData.init();
newData.setI(99);
/* ... */

// Retrieve the named data from the list
MyClass &newData = dataList.retrieve<MyClass>( "SomeNewData");

// Fails - named data does not exist, default value returned (MyClass())
MyClass &newData = dataList.retrieve<MyClass>( "DifferentNewData");

// Fails - can't convert class types, default value returned (SomeOtherClass())
SomeOtherClass &otherData = dataList.retrieve<SomeOtherClass>( "SomeNewData");
```

### Versioning

[`GenericItem`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/item.h) is able to store rudimentary version information (`GenericItem::version_`). In conjunction with [`GenericListHelper`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/listhelper.h) the modification of data within in a [`GenericItemContainer`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/container.h) can be loosely tracked. The `GenericListHelper<T>::retrieve()` method returns a non-const reference to the contained data - as such it makes the assumption that the data will be modified, and calls `GenericItem::bumpVersion()` before returning the reference. Contrarily, the `GenericListHelper<T>::value()` method returns a const reference to the contained data, and so leaves the current version intact. The latter method should be strongly preferred if it is known that the data is only required for examination rather than modification.

### Item Flags

As well as the descriptive name of the data, [`GenericItem`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/item.h) can also store relevant flags that affect its handling.  At present the only flag is `GenericList::InRestartFileFlag`, which states that the data should be written to the [restart file]({{< ref "restart" >}}) at the appropriate time.

### Type Information

To enable the creation of suitably-templated data programmatically, [`GenericItem`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/item.h) also maintains a list of all the valid names of storable classes (from their `itemClassName()` member functions, or the specialisations thereof). Various static members hold the list of available item classes and functions to create typed data by name. As-implemented, the available classes is itself stored as a list of [`GenericItem`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/item.h)s created in [`Dissolve::registerGenericItems()`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/main/dissolve.cpp#L124) and passed to `GenericItem::addItemClass()` where they are stored in a list. When a new item of a named type is required (e.g. when reading in the [restart file]({{< ref "restart" >}})) `GenericItem::newItem()` scans through the list of registered item types for a matching name and (assuming it is found) invokes the virtual `createItem()` function to return a new [`GenericItemContainer`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/genericitems/container.h) of the underlying type.
