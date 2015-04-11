#pragma once

#include <string>

namespace engine
{


class Path
{
public:
    static std::wstring getExecutable();

    static std::wstring getExecutableDirPath();
};


}//namespace engine
