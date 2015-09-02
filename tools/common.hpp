#ifndef HENSON_TOOLS_COMMON_H
#define HENSON_TOOLS_COMMON_H

#include <stdexcept>
#include <string>
#include <format.h>

struct tags
{
    enum { data = 0, stop = 1, request_data = 2};
};

struct Variable
{
    std::string     name;
    std::string     type;
};

Variable parse_variable(const std::string& var)
{
    // parse the variable
    size_t colon = var.rfind(':');
    if (colon == std::string::npos)
        throw std::runtime_error(fmt::format("Couldn't find type of variable in {}\n", var));

    std::string type = var.substr(colon+1);
    std::string name = var.substr(0,colon);

    Variable v = { name, type };
    return v;
}

// From http://stackoverflow.com/questions/236129/split-a-string-in-c
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

static const float growth_multiplier = 1.5;

template<class T>
void write(std::vector<char>& buffer, size_t& position, const T& x, size_t count = sizeof(T))
{
    if (position + count > buffer.capacity())
        buffer.reserve((position + count) * growth_multiplier);         // if we have to grow, grow geometrically

    if (position + count > buffer.size())
        buffer.resize(position + count);

    const char* x_ptr = (const char*) &x;
    std::copy(x_ptr, x_ptr + count, &buffer[position]);
    position += count;
}

template<class T>
void read(std::vector<char>& buffer, size_t& position, T& x, size_t count = sizeof(T))
{
    char* x_ptr = (char*) &x;

    std::copy(&buffer[position], &buffer[position + count], x_ptr);
    position += count;
}

#endif
