#include <vector>
#include <string>
#include <fstream>

#include <henson/chai/stdlib.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>

std::unique_ptr<chaiscript::ChaiScript> chai_stdlib()
{
    std::unique_ptr<chaiscript::ChaiScript> chai { new chaiscript::ChaiScript(chaiscript::Std_Lib::library()) };

    using StringVector = std::vector<std::string>;
    chai->add(chaiscript::bootstrap::standard_library::vector_type<StringVector>("StringVector"));
    chai->add(chaiscript::fun([](std::string filename)
                             {
                                std::vector<std::string> lines;
                                std::ifstream in(filename);
                                std::string line;
                                while (std::getline(in, line))
                                    lines.push_back(line);

                                return lines;
                             }),               "readlines");

    return chai;
}
