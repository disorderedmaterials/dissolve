---
title: Keyword Options
---

## Introduction

### Purpose
This document describes the available keyword option classes for use in controlling modules.

### Scope
This overview provides implementation details on the module keywords used to store control variables for modules, and the variable data types represented. The document is suitable for developers wishing to add new modules (or new options to existing modules).

### Definitions


## Overview

### Brief

[Modules](../modules.md) provide the core functionality of Dissolve, implementing algorithms, calculations, and processes that allow the main simulation workflow to be constructed. Each of these modules is typically controlled by a set of keyword options that are set in the input file in order to control or fine-tune the behaviour of the module. In order to provide consistency of style, input and output, and limit checking for these options a general framework for creating and defining options working on specific PODs or classes has been defined.  Related classes within the GUI provide interface widgets for all keyword types, allowing seamless editing of keyword options within the GUI without manual coding of the necessary controls.

### Rationale

Providing a class-based system for defining keyword options permits transparent input / output of keyword data, and greatly reduces the amount of coding necessary when new keyword options for modules are required. In particular, these classes also have widget representations in the GUI, meaning that a single line of code within a module is enough to provide read, write, and UI interaction for a given keyword option.

## Implementation

### Relevant Classes

- [`ModuleKeywordBase`](https://github.com/projectdissolve/dissolve/tree/develop/src/module/keywordbase.h)
- [`ModuleKeywordData`](https://github.com/projectdissolve/dissolve/tree/develop/src/module/keyworddata.h) (template)
- [`ModuleKeywordList`](https://github.com/projectdissolve/dissolve/tree/develop/src/module/keywordlist.h)
- [`Module`](https://github.com/projectdissolve/dissolve/tree/develop/src/module/module.h)

### Class Dependency

_Note: Only the principal members are shown for brevity and to aid discussion._

`ModuleKeywordData` is a template class with a single template parameter `T`.
`KeywordDataType` is an enumeration owned by `ModuleKeywordBase`, and contains all possible keyword variable types.

{{< mermaid >}}
classDiagram
    ModuleKeywordData --> ModuleKeywordBase
    ModuleKeywordList <-- ModuleKeywordBase
    Module <-- ModuleKeywordList
    class ModuleKeywordBase {
        - Module moduleParent_
        - KeywordDataType type_
        - const char keyword_
        - const char arguments_
        - const char description_
        + isSet()* :  bool
        + minArguments()* int
        + maxArguments()* int
        + read(LineParser &parser, int stargArg, const CoreData &coreData, ProcessPool &procPool)* bool
        + write(LineParser &parser, const char *prefix)* bool
        + asBool()* bool
        + asInt()* int
        + asDouble()* double
        + asString()* const char*
        + asVec3Int()* Vec3~int~
        + asVec3Double()* Vec3~double~
        # data_ : T
        # set_ : bool
        + virtual isValid(T value) : bool
        - keywords_ : List~ModuleKeywordBase~
    }
    class Module {
        # ModuleKeywordList keywords_
    }
{{< /mermaid >}}

### Details

The template-guided `ModuleKeywordData` provides the actual storage object for the `data_` of type `T`, and derives from `ModuleKeywordBase` to provide higher-level set/get and conversion functions. `ModuleKeywordBase` contains five key pure virtual functions, and as such is an abstract class. Keyword option classes therefore derive from `ModuleKeywordData` and implement the necessary virtuals from `ModuleKeywordBase`, including any relevant conversion functions (e.g. `asInt()`, `asDouble()` etc.).

As a complete example consider the `BoolModuleKeyword` class, with code provided in-place in the header for illustration. We begin with the declaration of the class and its dependency, and the constructor / destructor:

```cpp
// Keyword with Bool Data
class BoolModuleKeyword : public ModuleKeywordData<bool>
{
	public:
	// Constructor
	BoolModuleKeyword(bool value) : ModuleKeywordData<bool>(ModuleKeywordBase::BoolData, value)
	{
	}
	// Destructor
	~BoolModuleKeyword()
	{
	}

	...
```

All module keyword option classes should take in their constructor any necessary parameters or initialisation values for the underlying `data_` - in this case, we provide the starting value of the bool.

We then move on to implement the virtual `ModuleKeywordBase::isSet()` function, and override the default implementation of `ModuleKeywordData::isValid(T value)`:

```cpp
	// Return whether the current data value has ever been set
	bool isSet()
	{
		return set_;
	}

	// Validate supplied value
	bool isValid(bool value)
	{
		return true;
	}
```

The former is a trivial implementation in the present example, but it is sometimes necessary to provide more rigorous checks for whether more complex data has been set. Keeping track of whether a keyword option has ever been set (i.e. changed) is useful when writing input files, since those keyword still containing their default values can be excluded from being written. Our validation function is also trivial in the present case, but for more complex `data_` more complex checking may be required.

Now we define the minimum and maximum number of arguments the keyword data can accept (one in the present case) and suitable read/write functions:

```cpp
	// Return minimum number of arguments accepted
	int minArguments()
	{
		return 1;
	}

	// Return maximum number of arguments accepted
	int maxArguments()
	{
		return 1;
	}

	// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
	bool read(LineParser &parser, int startArg, const CoreData &coreData, ProcessPool &procPool)
	{
		if (parser.hasArg(startArg))
		{
			setData(parser.argb(startArg));
	
			return true;
		}
		return false;
	}

	// Write keyword data to specified LineParser
	bool write(LineParser &parser, const char *prefix)
	{
		return parser.writeLineF("%s%s  %s\n", prefix, keyword(), DissolveSys::btoa(data_));
	}
```

The `read()` function takes a reference to a `LineParser` object which already contains the arguments from the input file, and the integer index of where to start looking for arguments to our keyword. The `ModuleKeywordData::setData(T value)` function is called to actually set the underlying data, as this performs validity checks on the supplied value as well as flagging that the `data_` has actually been `set_`, if appropriate.

Lastly, we override some relevant POD conversion functions:

```cpp
	// Return value (as bool)
	bool asBool()
	{
		return data_;
	}

	// Return value (as int)
	int asInt()
	{
		return data_ ? 1 : 0;
	}

	// Return value (as double)
	double asDouble()
	{
		return data_ ? 1.0 : 0.0;
	}

	// Return value (as string)
	const char *asString()
	{
		return DissolveSys::btoa(data_);
	}
```

Our `BoolModuleKeyword` is now complete.  Using it in a module is a simple case of creating an instance of the class within `Module::setUpKeywords()` as follows:

```cpp
// Set up keywords for Module
void DummyModule::setUpKeywords()
{
	ModuleKeywordGroup *group = addKeywordGroup("Flags");
	group->add(new BoolModuleKeyword(true), "Flag1", "Set the status of the first flag (default = true)");
	group->add(new BoolModuleKeyword(false), "Flag2", "Set the status of the second flag (default = true)");
	...
}
```

We first create a group for the keyword options to live in - this affects the display of the options in the GUI - and then call `ModuleKeywordGroup::add()` to own them. The `new`d `BoolModuleKeyword`s are owned by the underlying `ModuleKeywordList` of the module, and given the name and description provided (stored via the `ModuleKeywordBase::set()` function.

Retrieving the value of a keyword is then achieved in the module's main processing, for instance, by:

```cpp
// Run main processing
bool DummyModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
	bool flag1 = keywords_.asBool("Flag1");
	bool flag2 = keywords_.asBool("Flag2");
	...
}
```

For opaque classes stored as `data_` and for which no simple conversion to a POD is possible, the `KeywordListHelper<T>` template class must be used. For the `BroadeningFunction` class and its associated `BroadeningFunctionModuleKeyword` the code is as follows:

```cpp
// Set up keywords for Module
void DummyModule::setUpKeywords()
{
	ModuleKeywordGroup* group = addKeywordGroup("Flags");
	group->add(new BroadeningFunctionModuleKeyword(BroadeningFunction()), "Broadening", "Broadening function to apply to data")
	...
}

/* ... */

// Run main processing
bool DummyModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
	const BroadeningFunction &broadening = KeywordListHelper<BroadeningFunction>::retrieve(keywords_, "Broadening", BroadeningFunction());
	...
}
```

`KeywordListHelper` looks through the list of `ModuleKeywordBase` owned by the supplied `keywords_` object, searching for the named keyword ("Broadening"). If found, the matching `ModuleKeywordBase` is cast into its `ModuleKeywordData<T>` descendent, and the underlying data of type `T` is returned.

