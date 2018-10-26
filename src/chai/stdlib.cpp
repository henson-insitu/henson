#include <henson/chai/stdlib.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>

std::unique_ptr<chaiscript::ChaiScript> chai_stdlib()
{
    return std::unique_ptr<chaiscript::ChaiScript> { new chaiscript::ChaiScript(chaiscript::Std_Lib::library()) };
}
