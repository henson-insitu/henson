#ifndef HENSON_DATA_HPP
#define HENSON_DATA_HPP

#include <string>
#include <map>
#include <queue>
#include <vector>
#include <memory>

#include "variant.hpp"
#include "serialization.hpp"

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

    std::shared_ptr<std::vector<char>>  storage {};     // necessary for de-serialization
};

template<>
struct Serialization< Array >
{
  static void         save(BinaryBuffer& bb, const Array& x)
  {
    henson::save(bb, x.type);
    henson::save(bb, x.count);
    for (size_t i = 0; i < x.count; ++i)
        henson::save(bb, static_cast<char*>(x.address) + i*x.stride, x.type);
  }

  static void         load(BinaryBuffer& bb, Array& x)
  {
    henson::load(bb, x.type);
    henson::load(bb, x.count);
    x.storage = std::make_shared<std::vector<char>>(x.count*x.type);
    henson::load(bb, x.storage->data(), x.storage->size());
    x.address = x.storage->data();
    x.stride = x.type;
  }
};

using Value = mpark::variant<int, size_t, void*, float, double, Array>;

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
