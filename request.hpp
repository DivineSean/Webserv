#pragma once
#include "location.hpp"
class request
{
    public:
        std::string Head;
        std::string Body;
        bool        isHead;
        bool        isBody;
        int         valRead;
        bool        is_already_checked;
        size_t      ourTracker;
};