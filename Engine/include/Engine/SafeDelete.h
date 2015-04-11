#pragma once


namespace engine
{


// verify that types are complete for increased safety
template<class T> inline void checkedDelete(T* x)
{
    // intentionally complex - simplification causes regressions
    typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
    (void) sizeof(type_must_be_complete);

    delete x;
}


template<typename T>
void safeDelete(T*& a)
{
    if (a != NULL) {
        checkedDelete(a);
        a = NULL;
    }
}


}//namespace engine
