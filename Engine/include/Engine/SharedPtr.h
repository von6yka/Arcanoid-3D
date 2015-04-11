#pragma once

#include <assert.h>

#include "Engine/SafeDelete.h"


namespace engine
{


struct ReferenceCounter
{
    ReferenceCounter()
    {
        strong = 1;
    }

    int strong;
};


template<typename T>
class SharedPtr
{
public:
    SharedPtr() :
        _ptr(NULL),
        _referenceCounter(NULL)
    {
    }

    explicit SharedPtr(T* ptr) :
        _ptr(ptr),
        _referenceCounter(new ReferenceCounter)
    {
    }

    template<typename Y>
    SharedPtr(SharedPtr<Y>& other) :
        _ptr(other.get()),
        _referenceCounter(other.referenceCounter())
    {
        if (_referenceCounter != NULL) {
            ++_referenceCounter->strong;
        }
    }

    explicit SharedPtr(T* ptr, ReferenceCounter* referenceCounter) :
        _ptr(ptr),
        _referenceCounter(referenceCounter)
    {
        if (_referenceCounter != NULL) {
            ++_referenceCounter->strong;
        }
    }

    SharedPtr(const SharedPtr& other) :
        _ptr(other._ptr),
        _referenceCounter(other._referenceCounter)
    {
        if (_referenceCounter != NULL) {
            ++_referenceCounter->strong;
        }
    }

    ~SharedPtr()
    {
        reset();
    }

public:
    SharedPtr& operator = (const SharedPtr& other)
    {
        reset();

        _ptr = other._ptr;

        if (other._referenceCounter != NULL) {
            _referenceCounter = other._referenceCounter;
            ++_referenceCounter->strong;
        }

        return *this;
    }

    bool operator == (const SharedPtr& other) const
    {
        if (_ptr != other._ptr) {
            return false;
        }

        assert(_referenceCounter == other._referenceCounter);

        return true;
    }

    void reset(T* ptr = NULL)
    {
        if (_referenceCounter != NULL) {
            --_referenceCounter->strong;

            if (_referenceCounter->strong == 0) {
                safeDelete(_ptr);
                safeDelete(_referenceCounter);
            }
            else {
                _ptr = NULL;
                _referenceCounter = NULL;
            }
        }

        if (ptr != NULL) {
            _ptr = ptr;
            _referenceCounter = new ReferenceCounter;
        }
    }

    ReferenceCounter* referenceCounter()
    {
        return _referenceCounter;
    }

    T* operator -> ()
    {
        return _ptr;
    }

    const T* operator -> () const
    {
        return _ptr;
    }

    T& operator * ()
    {
        assert(_ptr != NULL);

        return *_ptr;
    }

    const T& operator * () const
    {
        assert(_ptr != NULL);

        return *_ptr;
    }

    T* get() const
    {
        return _ptr;
    }

    bool isNull() const
    {
        return _ptr == NULL;
    }

private:
    T* _ptr;
    ReferenceCounter* _referenceCounter;
};


}//namespace engine
