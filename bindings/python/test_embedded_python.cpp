#include <pybind11/embed.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <opts/opts.h>

namespace py = pybind11;

int main(int argc, char** argv)
{
    bool help;

    using namespace opts;
    Options ops;
    ops
        >> Option('h', "help",  help,        "show help")
    ;

    std::string fn;
    if (!ops.parse(argc,argv) || help || !(ops >> PosOption(fn)))
    {
        fmt::print("Usage: {} FILE.py\n", argv[0]);
        fmt::print("{}", ops);
        return 1;
    }

    fmt::print("Evaluating {}\n", fn);

    py::scoped_interpreter guard;

    py::object locals;
    py::eval_file(fn, py::globals(), locals);

    return 0;
}
