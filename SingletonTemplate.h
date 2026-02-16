#pragma once

struct Singleton
{
    static Singleton& get_instance()
    {
        static Singleton instance;
        return instance;
    }

    // The copy constructor is deleted, to prevent client code from creating new
    // instances of this class by copying the instance returned by get_instance()
    Singleton(Singleton const&) = delete;

    // The move constructor is deleted, to prevent client code from moving from
    // the object returned by get_instance(), which could result in other clients
    // retrieving a reference to an object with unspecified state.
    Singleton(Singleton&&) = delete;

private:

    // Default-constructor is private, to prevent client code from creating new
    // instances of this class. The only instance shall be retrieved through the
    // get_instance() function.
    Singleton() {}

};