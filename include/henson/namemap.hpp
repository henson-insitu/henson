#ifndef HENSON_NAMEMAP_HPP
#define HENSON_NAMEMAP_HPP

#include <map>
#include <string>
#include <vector>

#include <henson/diy/serialization.hpp>

namespace henson
{

typedef     std::map< std::string, ::diy::MemoryBuffer >      NameMap;

}

#endif
