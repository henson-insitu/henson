#ifndef HENSON_DATA_HPP
#define HENSON_DATA_HPP

#include <string>
#include <map>

namespace henson
{

struct Array
{
            Array(void* address_ = 0, size_t type_ = 0, size_t count_ = 0, size_t stride_ = 0):
                address(address_), type(type_), count(count_), stride(stride_)      {}

    void*   address;
    size_t  type;
    size_t  count;
    size_t  stride;
};

struct Value
{
            Value(): tag(_int), i(0)        {}
    enum { _int, _size_t, _ptr, _float, _double, _array } tag;
    union
    {
        int     i;
        size_t  s;
        void*   p;
        float   f;
        double  d;
        Array   a;
    };
};


class NameMap
{
    public:
        typedef     std::map<std::string, Value>                        DataMap;

    public:
        void        add(const std::string& name, Value value)           { values_[name] = value; }
        Value       get(const std::string& name) const                  { return values_.find(name)->second; }

        bool        exists(const std::string& name) const               { return values_.find(name) != values_.end(); }

        void        clear()                                             { values_.clear(); }

                    ~NameMap()                                          { clear(); }

    private:
        DataMap     values_;
};

bool        exists(const std::string& name);

void        save(const std::string& name, Value x);

NameMap*    get_namemap();

Value       load(const std::string& name);

}

#endif
