#pragma once

class response
{
    public:
        std::string responseToClient;
        size_t readOffset;
        size_t HeadSize;
        int  count;
        bool isDone;
        bool WriteHead;
        bool WriteBody;
        bool SetupOffset;
        size_t sizeOfchunk;
        bool   sizeIsdone;
        std::string stock;
        bool insideS;
        long long size_length;
        long long mbc;
        std::string client_cookie;
};