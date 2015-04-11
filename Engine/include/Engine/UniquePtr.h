#pragma once

#include <assert.h>

#include "Engine/NonCopyable.h"
#include "Engine/SafeDelete.h"


namespace engine
{


template<typename T>
class UniquePtr : NonCopyable
{
public:
    explicit UniquePtr(T* ptr = NULL) :
        _ptr(ptr)
    {
    }

    ~UniquePtr()
    {
        reset();
    }

public:
    void reset(T* ptr = NULL)
    {
        if (_ptr != NULL) {
            safeDelete(_ptr);
        }

        _ptr = ptr;
    }

    T* operator -> ()
    {
        assert(_ptr != NULL);

        return _ptr;
    }

    const T* operator -> () const
    {
        assert(_ptr != NULL);

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
};


}//namespace engine
