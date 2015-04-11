#pragma once

namespace engine
{


class NonCopyable
{
protected:
    NonCopyable() {}
    ~NonCopyable() {}

private:
    NonCopyable(const NonCopyable&);
    NonCopyable& operator = (const NonCopyable&);
};


}//namespace engine
