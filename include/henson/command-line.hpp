#ifndef HENSON_COMMAND_LINE_HPP
#define HENSON_COMMAND_LINE_HPP

namespace henson
{
struct CommandLine
{
    // need to disallow copy to avoid pitfalls with the argv pointers into arguments

                                CommandLine()                   =default;
                                CommandLine(const CommandLine&) =delete;
                                CommandLine(CommandLine&&)      =default;
                                CommandLine(const std::string& line)
    {
        int prev = -1;
        size_t pos = 0;
        while (pos != std::string::npos)
        {
            pos = line.find(' ', pos + 1);

            std::vector<char> arg(line.begin() + prev + 1, pos == std::string::npos ? line.end() : line.begin() + pos);
            for (char c : arg)
            {
                if (!std::isspace(c))
                {
                    arg.push_back('\0');
                    arguments.push_back(std::move(arg));
                    break;
                }
            }

            prev = pos;
        }

        for (auto& s : arguments)
            argv.push_back(&s[0]);
    }

    CommandLine&                operator=(const CommandLine&)   =delete;
    CommandLine&                operator=(CommandLine&&)        =default;

    std::string                 executable(std::string prefix = "") const
    {
        auto exec = std::string(arguments[0].begin(), arguments[0].end());
        if (exec[0] != '/' && exec[0] != '~')
            exec = prefix + exec;
        return exec;
    }

    std::vector<std::vector<char>>  arguments;
    std::vector<char*>              argv;
};

inline std::string prefix(std::string fn)
{
    if (fn[0] != '/' && fn[0] != '~')
        fn = "./" + fn;
    fn = fn.substr(0, fn.rfind('/') + 1);
    return fn;
}
}

#endif
