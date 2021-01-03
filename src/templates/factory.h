// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/list.h"
#include "templates/listitem.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Object Chunk
template <class T> class ObjectChunk : public ListItem<ObjectChunk<T>>
{
    /*
     * Chunk of objects, maintained by an ObjectFactory
     */
    public:
    ObjectChunk<T>(int size) : ListItem<ObjectChunk<T>>(), nObjects_(size)
    {
        objectArray_ = new T[nObjects_];
        objectUsed_ = new bool[nObjects_];
        objectSize_ = sizeof(T);

        markAllObjectsUnused();
    }
    ~ObjectChunk()
    {
        if (objectArray_)
            delete[] objectArray_;
        if (objectUsed_)
            delete[] objectUsed_;
    }

    /*
     * Chunk Data
     */
    private:
    // Number of objects in chunk
    const int nObjects_;
    // Size of individual object
    int objectSize_;
    // Object array
    T *objectArray_;
    // Object usage flags
    bool *objectUsed_;
    // Number of unused objects in chunk
    int nUnusedObjects_;
    // Index of next available object
    int nextAvailableObject_;

    private:
    // Determine array offset of object
    int objectOffset(T *object)
    {
        intptr_t offset = intptr_t(object) - intptr_t(&objectArray_[0]);
        if (offset < 0)
            return -1;
        int index = offset / objectSize_;
        return (index < nObjects_ ? index : -1);
    }

    public:
    // Return next available object
    T *nextAvailable()
    {
        if (nextAvailableObject_ == -1)
            return nullptr;
        T *object = &objectArray_[nextAvailableObject_];
        objectUsed_[nextAvailableObject_] = true;
        --nUnusedObjects_;

        // If there are no more unused objects, then we are done
        if (nUnusedObjects_ == 0)
        {
            nextAvailableObject_ = -1;
            return object;
        }

        // Search for next available object before we return the object
        int nextFree = nextAvailableObject_ + 1;
        // -- First part - search to end of current array
        while (nextFree < nObjects_)
        {
            if (!objectUsed_[nextFree])
            {
                nextAvailableObject_ = nextFree;
                return object;
            }
            ++nextFree;
        }
        // -- Second part - search beginning of array up to current position
        nextFree = 0;
        while (nextFree < nextAvailableObject_)
        {
            if (!objectUsed_[nextFree])
            {
                nextAvailableObject_ = nextFree;
                return object;
            }
            ++nextFree;
        }

        // Shouldn't get here!
        Messenger::error("ObjectChunk.\n");
        return object;
    }
    // Return specified object to pool
    bool returnObject(T *object)
    {
        // Get the item offset of the object
        int offset = objectOffset(object);
        if (offset == -1)
            return false;

        // Mark the object as unused, and increase the unused counter
        objectUsed_[offset] = false;

        ++nUnusedObjects_;
        if (nextAvailableObject_ == -1)
            nextAvailableObject_ = offset;
        return true;
    }
    // Mark all objects as unused
    void markAllObjectsUnused()
    {
        for (auto n = 0; n < nObjects_; ++n)
            objectUsed_[n] = false;
        nextAvailableObject_ = 0;
        nUnusedObjects_ = nObjects_;
    }
    // Return whether object is part of this chunk
    bool ownsObject(T *object)
    {
        // Calculate array offset of this object
        return (objectOffset(object) != -1);
    }
    // Return whether there are unused objects in the chunk
    bool hasUnusedObjects() { return (nUnusedObjects_ != 0); }
};

// Object Factory Class
template <class T> class ObjectFactory
{
    public:
    ObjectFactory<T>()
    {
        currentChunk_ = nullptr;
        chunkSize_ = 256;
    }

    /*
     * Object Store
     */
    private:
    // Chunk size
    int chunkSize_;
    // List of object chunks maintained by this factory
    List<ObjectChunk<T>> objectChunks_;
    // Current chunk from which objects are being taken
    ObjectChunk<T> *currentChunk_;

    public:
    // Set chunksize to use when creating new chunks
    void setChunkSize(int chunkSize) { chunkSize_ = chunkSize; }

    /*
     * Object Access
     */
    public:
    // Produce a new object
    T *produce()
    {
        if (currentChunk_ == nullptr)
        {
            currentChunk_ = new ObjectChunk<T>(chunkSize_);
            objectChunks_.own(currentChunk_);
            return currentChunk_->nextAvailable();
        }
        else if (currentChunk_->hasUnusedObjects())
            return currentChunk_->nextAvailable();
        else
        {
            // Must search current chunk list to see if any current chunks have available space. If not, we will
            // create a new one
            for (ObjectChunk<T> *chunk = objectChunks_.first(); chunk != nullptr; chunk = chunk->next())
            {
                if (chunk == currentChunk_)
                    continue;
                if (chunk->hasUnusedObjects())
                {
                    currentChunk_ = chunk;
                    return currentChunk_->nextAvailable();
                }
            }

            // No dice - make a new chunk
            currentChunk_ = new ObjectChunk<T>(chunkSize_);
            objectChunks_.own(currentChunk_);
            return currentChunk_->nextAvailable();
        }

        // If we get here, then something has gone horribly wrong...
        Messenger::error("Couldn't find an empty chunk to return an object from.\n");
        return nullptr;
    }
    // Return specified object to factory
    void returnObject(T *object)
    {
        // Must find chunk which owns this object
        for (ObjectChunk<T> *chunk = objectChunks_.first(); chunk != nullptr; chunk = chunk->next())
            if (chunk->returnObject(object))
                return;

        // Couldn't find it!
        Messenger::error("Tried to return an object to an ObjectFactory which didn't produce it.\n");
    }
    // Mark all objects as unused
    void markAllObjectsUnused()
    {
        for (ObjectChunk<T> *chunk = objectChunks_.first(); chunk != nullptr; chunk = chunk->next())
            chunk->markAllObjectsUnused();
    }
};
