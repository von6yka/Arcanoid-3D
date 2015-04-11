#pragma once

#include <assert.h>

#include "Engine/UniquePtr.h"

namespace engine
{


class BindArgs0Abstract
{
public:
    virtual void operator()() = 0;
    virtual BindArgs0Abstract* copy() const = 0;
};


template<typename T>
class BindArgs0 : public BindArgs0Abstract
{
public:
    typedef void (T::*CallbackFunction)();

    BindArgs0(T* object, CallbackFunction member) :
        _object(object),
        _member(member)
    {
    }

public:
    BindArgs0Abstract* copy() const override
    {
        return new BindArgs0(_object, _member);
    }

    void operator()() override
    {
        (_object->*_member)();
    }

private:
    T* _object;
    CallbackFunction _member;
};


class CallbackArgs0
{
public:
    CallbackArgs0()
    {}

    CallbackArgs0(const BindArgs0Abstract& bind)
    {
        _bind.reset(bind.copy());
    }

    CallbackArgs0(const CallbackArgs0& callback)
    {
        if (!callback._bind.isNull()) {
            _bind.reset(callback._bind->copy());
        }
    }

public:
    CallbackArgs0& operator = (const CallbackArgs0& callback)
    {
        if (!callback._bind.isNull()) {
            _bind.reset(callback._bind->copy());
        }
        else {
            _bind.reset();
        }

        return *this;
    }

    void operator()()
    {
        assert(!_bind.isNull());
        (*_bind)();
    }

private:
    UniquePtr<BindArgs0Abstract> _bind;
};


}//namespace engine
