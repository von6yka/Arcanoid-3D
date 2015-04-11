#include "Engine/pch.h"

#include "Engine/Object.h"

namespace engine
{
#ifdef _DEBUG
int Object::count = 0;
#endif

Object::Object(Object* parent, const std::wstring& name) :
    _parent(parent),
    _name(name)
{
#ifdef _DEBUG
    ++count;
#endif

    if (_parent != NULL) {
        _parent->_childList.push_back(this);
    }
}


Object::~Object()
{
    if (_parent != NULL) {
        _parent->_childList.remove(this);
    }

    deleteChildren();

#ifdef _DEBUG
    --count;
#endif
}


void Object::setParent(Object* parent)
{
    if (_parent != NULL) {
        _parent->_childList.remove(this);
    }

    _parent = parent;

    if (_parent != NULL) {
        _parent->_childList.push_back(this);
    }
}


void Object::deleteChildren()
{
    while (!_childList.empty()) {
        Object* object = _childList.back();
        object->_parent = NULL;
        delete object;

        _childList.pop_back();
    }
}


}//namespace engine
