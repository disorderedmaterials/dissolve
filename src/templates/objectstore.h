/*
    *** Object Store
    *** src/templates/objectstore.h
    Copyright T. Youngs 2013-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "base/sysfunc.h"
#include "templates/refdatalist.h"
#include <stdio.h>
#include <string.h>

// Object Info
// -- Object type_ should be set from a local enum, for instance, containing all object types relevant in the use case.
class ObjectInfo
{
    public:
    ObjectInfo()
    {
        type_ = 0;
        id_ = -1;
    }
    // Object types
    enum ObjectType
    {
        NoObject = 0,
        ConfigurationObject,
        Data1DObject,
        Data2DObject,
        Data3DObject,
        Histogram1DObject,
        Histogram2DObject,
        Histogram3DObject,
        SpeciesObject,
        nObjectTypes
    };

    private:
    // Object target type
    int type_;
    // Target object id
    int id_;
    // Target object tag
    std::string tag_;

    public:
    // Set object target type and id
    void set(int type, int id)
    {
        type_ = type;
        id_ = id;
    }
    // Return object target type
    int type() const { return type_; }
    // Return target object id
    int id() const { return id_; }
    // Set object tag
    void setTag(std::string_view tag) { tag_ = tag; }
    // Return object tag
    std::string_view tag() const { return tag_; }

    /*
     * Tag Control
     */
    private:
    // Whether automatic suffixing of tags is enabled
    static bool autoSuffixing_;
    // Suffix to append (if autoSuffixing_)
    static std::string autoSuffix_;

    public:
    // Enable automatic suffixing of tags
    static void enableAutoSuffixing(std::string_view suffix)
    {
        autoSuffixing_ = true;
        autoSuffix_ = suffix;
    }
    // Disable automatic suffixing of tags
    static void disableAutoSuffixing() { autoSuffixing_ = false; }
    // Return whether automatic suffixing of tags is enabled
    static bool autoSuffixing() { return autoSuffixing_; }
    // Return suffix to append (if autoSuffixing_)
    static std::string_view autoSuffix() { return autoSuffix_; }
};

// Object Store
template <class T> class ObjectStore
{
    public:
    ObjectStore<T>(T *object)
    {
        // If the passed pointer is nullptr, do not add anything to the list (we were probably called from a copy
        // constructor)
        if (object == nullptr)
        {
            Messenger::error("ObjectStore was passed a NULL pointer...\n");
            object_ = nullptr;
            return;
        }

        // Store the parent object pointer, and add it to the master list
        object_ = object;
        objectInfo_.set(objectType_, objectCount_++);
        setObjectTag(fmt::format("{}", fmt::ptr(object_)));
        objects_.append(object_, objectInfo_.id());
    }
    ~ObjectStore<T>()
    {
        // Remove our pointer from the master list
        if (object_)
            objects_.remove(object_);
    }
    ObjectStore<T>(const ObjectStore<T> &source)
    {
        // ObjectStore<T> members are *not* copied, since a named object should be unique.
    }
    void operator=(const ObjectStore<T> &source)
    {
        // ObjectStore<T> members are *not* copied, since a named object should be unique.
    }
    // Object type identifier
    static int objectType_;
    // Object type name
    static std::string_view objectTypeName_;

    /*
     * Object Data
     */
    private:
    // Pointer to object that this ObjectStore was created with
    T *object_;
    // Object info
    ObjectInfo objectInfo_;

    public:
    // Return object type
    int objectType() { return objectInfo_.type(); }
    // Return object ID
    int objectId() { return objectInfo_.id(); }
    // Return object information
    ObjectInfo objectInfo() { return objectInfo_; }
    // Set tag for this object, prepending type name prefix
    void setObjectTag(std::string_view tag)
    {
        // Get actual tag of object - does the supplied tag already contain a type prefix?
        std::string objectTag;
        std::string_view typePrefix = DissolveSys::beforeChar(tag, '%');
        if (!typePrefix.empty())
        {
            objectTag = DissolveSys::afterChar(tag, '%');

            // Do a sanity check on the type prefix...
            if (!DissolveSys::sameString(typePrefix, objectTypeName_))
                Messenger::error("Setting an object tag ({}) with a string that contains the wrong type prefix "
                                 "('{}', while this class is '{}').\n",
                                 tag, typePrefix, objectTypeName_);
        }
        else
            objectTag = tag;

        // Append auto-suffix
        if (ObjectInfo::autoSuffixing())
            objectTag = fmt::format("{}@{}", objectTag, ObjectInfo::autoSuffix());

#ifdef CHECKS
        // Check for duplicate value already in list
        if (!objectTag.empty())
        {
            // Assemble object name
            std::string objectName = fmt::format("{}%{}", objectTypeName_, objectTag);

            T *object = findObject(objectName);
            if (object && (object != this))
            {
                Messenger::warn("ObjectStore<{}>::setObjectTag() - The object '{}%{}' already exists in the "
                                "ObjectStore. Behaviour will be undefined...\n",
                                objectTypeName_, objectTypeName_, objectTag);
            }
        }
#endif
        objectInfo_.setTag(fmt::format("{}%{}", objectTypeName_, objectTag));
    }
    // Return object tag
    std::string_view objectTag() const { return objectInfo_.tag(); }
    // Return whether object tag matches that supplied
    bool objectTagIs(std::string_view tag) const { return (DissolveSys::sameString(objectInfo_.tag(), tag, true)); }

    /*
     * Object List
     */
    private:
    // Master list of available objects
    static RefDataList<T, int> objects_;
    // Integer count for object IDs
    static int objectCount_;

    public:
    // Return number of available objects
    static int nObjects() { return objects_.nItems(); }
    // Return object with specified ID
    static T *object(int id)
    {
        for (RefDataItem<T, int> *ri = objects_.first(); ri != nullptr; ri = ri->next())
            if (ri->data() == id)
                return ri->item();
        return nullptr;
    }
    // Set id of specified object, returning if we were successful
    static bool setObjectId(T *target, int id)
    {
        // Find the RefDataItem object in the list
        RefDataItem<T, int> *targetRefItem = objects_.contains(target);
        if (targetRefItem == nullptr)
            return Messenger::error("Couldn't find specified object {} in object list.\n", fmt::ptr(target));

        // Can we find an object with the same id?
        RefDataItem<T, int> *rj = objects_.containsData(id);
        if ((rj != nullptr) && (rj != targetRefItem))
        {
            Messenger::error("Another object with id {} already exists in the ObjectStore, so refusing to "
                             "duplicate it.\n",
                             id);
            return false;
        }

        if (rj == targetRefItem)
        {
            Messenger::error("ObjectStore::setObjectId() - Target object already has id specified ({}).\n", id);
            return true;
        }

        target->objectInfo_.set(target->objectInfo_.type(), id);
        targetRefItem->data = id;

        return true;
    }
    // Find specified object by its tag
    static T *findObject(const std::string_view objectTag)
    {
        // Does the supplied tag contain a type prefix? If so, check it and then strip it
        std::string_view typePrefix = DissolveSys::beforeChar(objectTag, '%');
        if (typePrefix.empty())
        {
            // No type prefix, so add ours and do the search
            std::string tag = fmt::format("{}%{}", objectTypeName_, objectTag);
            for (RefDataItem<T, int> *ri = objects_.first(); ri != nullptr; ri = ri->next())
            {
                if (ri->item()->objectTagIs(tag))
                    return ri->item();
            }
        }
        else
        {
            // Check the type prefix
            if (!DissolveSys::sameString(typePrefix, objectTypeName_))
            {
                Messenger::error("Searched for object '{}' in a store containing objects of type '{}'.\n", typePrefix,
                                 objectTypeName_);
                return nullptr;
            }
            for (RefDataItem<T, int> *ri = objects_.first(); ri != nullptr; ri = ri->next())
            {
                if (ri->item()->objectTagIs(objectTag))
                    return ri->item();
            }
        }
        return nullptr;
    }
};
