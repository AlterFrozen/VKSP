#include <iostream>
import vksp;

void Prompt() noexcept;

int main(int argc, char* argv[])
{
    Prompt();

    try
    {
        Parse("shader.h",
            R"a(axis.vert.spv)a",
            R"a(axis.frag.spv)a");
    }
    catch (const std::exception& err)
    {
        std::cerr << err.what() << '\n';
        std::exit(EXIT_FAILURE);
    }
    
    return EXIT_SUCCESS;
}

void Prompt() noexcept
{
    std::cout <<
        ""
        ""
        "" 
        << std::endl;
}