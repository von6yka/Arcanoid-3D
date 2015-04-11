#pragma once

#include <list>
#include <string>

#include "Engine/NonCopyable.h"

namespace engine
{


class Object : NonCopyable
{
public:
#ifdef _DEBUG
    static int count;
#endif

    Object(Object* parent, const std::wstring& name = std::wstring());

    virtual ~Object();

public:
    void deleteChildren();

public:
    Object* parent() const
    {
        return _parent;
    }

    void setParent(Object* parent);

    std::wstring name() const
    {
        return _name;
    }

    void setName(const std::wstring& name)
    {
        _name = name;
    }

    std::list<Object*>& childList()
    {
        return _childList;
    }

private:
    Object* _parent;
    std::list<Object*> _childList;

    std::wstring _name;
};


}//namespace engine
