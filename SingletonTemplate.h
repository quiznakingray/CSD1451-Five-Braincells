//---------------------------------------------------------
// author:   Kon Xin Yi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#pragma once

template<typename T>
class Singleton
{
public:
    static T& GetInstance()
    {
        static T instance;
        return instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton() = default;
    ~Singleton() = default;
};
