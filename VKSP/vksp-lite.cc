#include "vksp_utils.h"

#include <iostream>

static inline std::string OPT_NAMESPACE = "";

void Prompt()
{
    std::cout <<
        "[VKSP Lite] Version: 1.0.1\n"
        "Intro:   A toy for parsing SPIR-V shader files to a C++ Header\n"
        "Author:  Frozen (contact@ljyc.me)\n"
        "Example: ./vksp-lite \"output_file\" \"vertex_shader\" \"fragment_shader\"\n"
        << std::endl;
}

void Parse( const char* output_file,
	        const char* vertex_shader,
	        const char* fragment_shader);

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
            "(ARG(1) \"output_file\" ARG(2)\"vertex_shader\" ARG(3)\"fragment_shader\")\n";
        }
    }
    catch (const std::exception& err)
    {
        std::cerr << err.what() << '\n';
        std::exit(-1);
    }
    
    return 0;
}

void Parse(
	const char* output_file,
	const char* vertex_shader,
	const char* fragment_shader)
{
	// Parse Vertex Shader
	std::string vert = ReadBinary(vertex_shader);

	// Parse Fragment Shader
	std::string frag = ReadBinary(fragment_shader);

	// Write to File
	std::ofstream out(output_file);

	if (out.is_open())
	{
		out << std::format(
			"#pragma once\n"
			"// Date: {}\n"
			"// Tool: VKSP Lite\n"
			"// File: {}\n"
			"// Vertex   Shader: {}\n"
			"// Fragment Shader: {}\n"
			"\n"
			"// [API List]\n"
			"// 1. GetVertexShader()   -> const std::vector<char8_t>\n"
			"// 2. GetFragmentShader() -> const std::vector<char8_t>\n"
			"\n"
			"// [Header Mess]\n"
			"#include <vector>\n"
			"\n"
			"namespace {}\n"
			"{{\n"

			// API: GetVertexShader()
			"\tstatic inline auto GetVertexShader() -> const std::vector<char8_t>\n"
			"\t{{\n"
			"\t\treturn {{\n\t\t{}}};\n"
			"\t}}\n"
			// API: GetFragmentShader()
			"\tstatic inline auto GetFragmentShader() -> const std::vector<char8_t>\n"
			"\t{{\n"
			"\t\treturn {{\n\t\t{}}};\n"
			"\t}}\n"

			"}}\n",
			CurrentDate(),
			output_file,
			vertex_shader,
			fragment_shader,
			OPT_NAMESPACE,
			BinToHex(vert),
			BinToHex(frag)
		);
		out.close();
		std::cout << std::format("Generated {} successfully!\n", output_file);
	}
	else throw std::runtime_error(std::format("Failed to open file {}!\n", output_file));
}