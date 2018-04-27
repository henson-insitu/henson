#ifndef HENSON_DATA_HPP
#define HENSON_DATA_HPP

#include <string>
#include <map>
#include <queue>

namespace henson
{

struct Array
{
            Array(void* address_ = 0, ssize_t type_ = 0, size_t count_ = 0, size_t stride_ = 0):
                address(address_), type(type_), count(count_), stride(stride_)      {}

    void*   address;
    ssize_t type;
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
        typedef     std::queue<Value>                                   ValueQueue;
        typedef     std::map<std::string, ValueQueue>                   QueuesMap;

    public:
        void        add(const std::string& name, Value value)
        {
            auto it = queues_.find(name);
            if (it != queues_.end())
                it->second.push(value);
            else
                values_[name] = value;
        }

        Value       get(const std::string& name)
        {
            auto it = queues_.find(name);
            if (it != queues_.end())
            {
                auto& q = it->second;
                Value v = q.front();
                q.pop();
                return v;
            }
            else return
                values_.find(name)->second;
        }

        void        create_queue(const std::string& name)               { queues_.emplace(name, ValueQueue()); }
        bool        queue_empty(const std::string& name) const          { return queues_.find(name)->second.empty(); }

        bool        exists(const std::string& name) const               { return values_.find(name) != values_.end() || queues_.find(name) != queues_.end(); }

        void        clear()                                             { values_.clear(); queues_.clear(); }

    private:
        DataMap     values_;
        QueuesMap   queues_;
};

bool        exists(const std::string& name);

void        save(const std::string& name, Value x);

NameMap*    get_namemap();

Value       load(const std::string& name);

}

#endif
