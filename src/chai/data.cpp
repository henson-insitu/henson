#include <fmt/format.h>

#include <henson/chai/data.hpp>
namespace h = henson;

#include <chaiscript/chaiscript_stdlib.hpp>

// pretty-print an Array
template<class T>
std::string convert(T* x, size_t count, size_t stride)
{
    std::string result = "[";
    for (size_t i = 0; i < count; ++i)
    {
        result += std::to_string(*x);
        if (i != count - 1)
            result += ", ";

        x = (T*) ((char*) x + stride);      // this is horribly ugly, but technically correct
    }
    result += "]";
    return result;
}

void chai_data(chaiscript::ChaiScript& chai, henson::NameMap& namemap)
{
    using BV = chaiscript::Boxed_Value;

    // Array
    chai.add(chaiscript::user_type<h::Array>(), "Array");
    chai.add(chaiscript::bootstrap::basic_constructors<h::Array>("Array"));
    chai.add(chaiscript::fun([](const h::Array& a)  { return a.count; }), "size");
    chai.add(chaiscript::fun(&h::Array::copy), "copy");
    chai.add(chaiscript::fun([](const h::Array& a, int i) -> BV
    {
        struct extract
        {
            BV  operator()(void* x) const       { throw std::runtime_error("Cannot access elements in a generic (void*) array"); }
            BV  operator()(int* x) const        { return BV(*(x + i)); }
            BV  operator()(long* x) const       { return BV(*(x + i)); }
            BV  operator()(float* x) const      { return BV(*(x + i)); }
            BV  operator()(double* x) const     { return BV(*(x + i)); }
            int i;
        };
        return visit(extract { i }, a.address);
    }), "[]");
    chai.add(chaiscript::fun([](const h::Array& a)
    {
        struct extract
        {
            std::string operator()(void* x) const   { return fmt::format("Array<void*> with {} elements", count); }
            std::string operator()(int* x) const    { return "Array<int*>"    + convert(x, count, stride); }
            std::string operator()(long* x) const   { return "Array<long*>"   + convert(x, count, stride); }
            std::string operator()(float* x) const  { return "Array<float*>"  + convert(x, count, stride); }
            std::string operator()(double* x) const { return "Array<double*>" + convert(x, count, stride); }

            size_t count, stride;
        };
        return visit(extract{a.count,a.stride}, a.address);
    }), "to_string");

    // NameMap
    // TODO: why not just create a new namemap?
    chai.add(chaiscript::fun([&namemap] () { return &namemap; }), "NameMap");
    // Probably should figure out what to do if something isn't in the map
    // NB: not exposed to chai: arrays
    chai.add(chaiscript::fun([](henson::NameMap* namemap, std::string name)
    {
        henson::Value val = namemap->get(name);
        struct extract
        {
            BV operator()(int x) const      { return BV(x); }
            BV operator()(size_t x) const   { return BV(x); }
            BV operator()(float x) const    { return BV(x); }
            BV operator()(double x) const   { return BV(x); }
            BV operator()(void* x) const    { return BV((intptr_t) x); }
            BV operator()(h::Array x) const { return BV(x); }
        };
        return visit(extract{}, val);
    }), "get");
    chai.add(chaiscript::fun([](h::NameMap* namemap, std::string name, int x)       { h::Value v = x; namemap->add(name, v); }), "add");
    chai.add(chaiscript::fun([](h::NameMap* namemap, std::string name, size_t x)    { h::Value v = x; namemap->add(name, v); }), "add");
    chai.add(chaiscript::fun([](h::NameMap* namemap, std::string name, float x)     { h::Value v = x; namemap->add(name, v); }), "add");
    chai.add(chaiscript::fun([](h::NameMap* namemap, std::string name, double x)    { h::Value v = x; namemap->add(name, v); }), "add");
    chai.add(chaiscript::fun([](h::NameMap* namemap, std::string name, h::Array x)  { h::Value v = x; namemap->add(name, v); }), "add");
    chai.add(chaiscript::fun(&h::NameMap::create_queue),                    "create_queue");
    chai.add(chaiscript::fun(&h::NameMap::queue_empty),                     "queue_empty");
    chai.add(chaiscript::fun(&h::NameMap::exists),                          "exists");
}
