#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include "chaiscript.hpp"
#include "chaiscript_stdlib.hpp"


std::map<std::string, int> convert_to_int_map(const std::map<std::string, chaiscript::Boxed_Value>& groups)
{
    std::map<std::string, int> new_map;
    
    std::transform(groups.begin(), groups.end(), std::inserter(new_map, new_map.begin()), [](const std::pair<std::string, chaiscript::Boxed_Value>& old_pair)
                                                                               { int temp = chaiscript::Boxed_Number(old_pair.second).get_as<int>();
                                                                                 std::pair<std::string, int> new_pair(old_pair.first, temp);
                                                                                 return new_pair; });
    if(!groups.empty() && new_map.empty())
       throw std::runtime_error("Error: new_map was not transformed correctly\n");

    return new_map;
}


int pass_down(std::map<std::string, chaiscript::Boxed_Value> mp)
{
    std::map<std::string, int> new_map = convert_to_int_map(mp);

    for(auto thing : new_map)
        std::cout << "[ " << thing.first << ", " << thing.second << " ]\n";

    if(new_map.size() == mp.size())
        return new_map.size();
    else
        throw std::runtime_error("Error: messed up!\n");

}


int main (int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage: ./prog chaiscript\n";
        return 1;
    }

    std::string script_name(argv[1]);

    chaiscript::ChaiScript chai(chaiscript::Std_Lib::library());

    chai.add(chaiscript::fun(&pass_down), "pass_down");

    chai.eval_file(script_name);

    chaiscript::Boxed_Value returned = chai.eval("world(\"stuff\")");
    std::cout << returned.get_type_info().name() << std::endl;
    
    if(returned.get_type_info().is_undef())
    {
        std::cout << "Got it the value was void!" << std::endl;
    }
    else
    {
        std::cout << "Returned value: " << chai.boxed_cast<int>(returned) << std::endl;
    }




    return 0;
}
