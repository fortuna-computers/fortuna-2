#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include "../compiler/compilationresult.hh"
#include "../compiler/compiler.hh"
#include "../emulator/imagefile.hh"

struct Options {
    std::string input;
    std::string output;
};

void print_help(char* program)
{
    std::cout << "Usage: " << program << " -o OUTPUT INPUT\n";
}

Options get_options(int argc, char** argv)
{
    Options options;
    
    while (1) {
        int idx = 0;
        static option long_options[] = {
                { "help", no_argument, nullptr, 'h' },
                { "output", required_argument, nullptr, 'o' },
                { 0, 0, nullptr, 0 }
        };
        int c = getopt_long(argc, argv, "ho:", long_options, &idx);
        if (c == -1)
            break;
        
        switch (c) {
            case 'h':
                print_help(argv[0]);
                exit(EXIT_SUCCESS);
            case 'o':
                options.output = optarg;
                break;
            case '?':
                print_help(argv[0]);
                exit(EXIT_FAILURE);
        }
        
        if (optind == argc - 1) {
            options.input = argv[optind];
        } else {
            print_help(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    
    return options;
}

int main(int argc, char* argv[])
{
    Options options = get_options(argc, argv);
    try {
        CompilationResult result = Compiler().compile_from_project_file(options.input);
        ImageFile(result, false, options.output);
    } catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        exit(EXIT_FAILURE);
    }
}

