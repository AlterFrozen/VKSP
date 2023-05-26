import std;
import vksp;

void Prompt() noexcept;

int main(int argc, char* argv[])
{
    try
    {
        switch (argc)
        {
        case 1: Prompt(); break;
        case 4: Parse(argv[1], argv[2], argv[3]); break;
        default: std::cerr <<
            "Parameter Error!\n"
            "(vksp \"output_file\" \"vertex_shader\" \"fragment_shader\")\n";
        }
    }
    catch (const std::exception& err)
    {
        std::cerr << err.what() << '\n';
        std::exit(-1);
    }
    
    return 0;
}

void Prompt() noexcept
{
    std::cout <<
        "[VKSP] Version: 1.0.1\n"
        "Intro:   A toy for parsing SPIR-V shader files to a C++ Header\n"
        "Author:  Frozen (contact@ljyc.me)\n"
        "Example: ./vksp \"output_file\" \"vertex_shader\" \"fragment_shader\"\n"
        << std::endl;
}