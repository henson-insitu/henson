#ifndef HENSON_DATA_HPP
#define HENSON_DATA_HPP

#include <string>
#include <map>

namespace henson
{


struct DataType
{
    virtual ~DataType()         {}      // will need to delete derived classes using pointer to DataType
};

struct Array: public DataType
{
            Array(void* address_ = 0, size_t type_ = 0, size_t count_ = 0, size_t stride_ = 0):
                address(address_), type(type_), count(count_), stride(stride_)      {}

    void*   address;
    size_t  type;
    size_t  count;
    size_t  stride;
};

template<class T>
struct Value: public DataType
{
            Value(T v): value(v)            {}
    T       value;
};


class NameMap
{
    public:
        typedef     std::map<std::string, DataType*>                    DataMap;

    public:
        void        add(const std::string& name, DataType* value)
        {
            DataMap::iterator it = values_.find(name);
            if (it == values_.end())
                values_[name] = value;
            else
            {
                delete it->second;
                it->second = value;
            }
        }
        DataType*   get(const std::string& name) const                  { return values_.find(name)->second; }

        template<class T>
        T*          get(const std::string& name) const                  { return static_cast<T*>(values_.find(name)->second); }

        bool        exists(const std::string& name) const               { return values_.find(name) != values_.end(); }

        void        clear()                                             { delete_all(); values_.clear(); }

                    ~NameMap()                                          { clear(); }

    private:
        void        delete_all()                                        { for (DataMap::iterator it = values_.begin(); it != values_.end(); ++it) delete it->second; }

    private:
        DataMap     values_;
};

bool        exists(const std::string& name);

void        save(const std::string& name, DataType* x);

NameMap*    get_namemap();

template<class T>
T*          load(const std::string& name)
{
    if (!get_namemap()) return 0;
    return get_namemap()->get<T>(name);
}

}

#endif
