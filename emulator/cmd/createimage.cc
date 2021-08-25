#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include "../compiler/compilationresult.hh"
#include "../compiler/compiler.hh"
#include "../emulator/imagefile.hh"

struct Options {
    std::string input;
    std::string output;
    size_t      image_size = 64;
};

void print_help(char* program)
{
    std::cout << "Usage: " << program << " [-s IMAGE_SIZE_IN_MB] -o OUTPUT INPUT\n";
}

Options get_options(int argc, char** argv)
{
    Options options;
    
    while (true) {
        int idx = 0;
        static option long_options[] = {
                { "size", required_argument, nullptr, 's' },
                { "help", no_argument, nullptr, 'h' },
                { "output", required_argument, nullptr, 'o' },
                { 0, 0, nullptr, 0 }
        };
        int c = getopt_long(argc, argv, "s:ho:", long_options, &idx);
        if (c == -1)
            break;
        
        switch (c) {
            case 's':
                options.image_size = strtol(optarg, nullptr, 10);
                break;
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
        
        if (optind > argc - 1) {
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
    if (options.input.empty() || options.output.empty()) {
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }
    
    try {
        CompilationResult result = Compiler().compile_from_project_file(options.input);
        ImageFile(options.image_size, result, false, options.output);
    } catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        exit(EXIT_FAILURE);
    }
}

