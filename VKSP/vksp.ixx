module;
#include <spirv_reflect.h>
#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <format>

export module vksp;
#define VKSP_API export

// [SECTION]: Parameters
static inline std::string OPT_NAMESPACE = "";

// [SECTION]: Error Code
enum ErrorCode
{
	ERR_FAILED_TO_OPEN_FILE =	-1,
	ERR_FAILED_TO_GET_TIME	=	-2,
};

// [SECTION]: Declaration
VKSP_API void Parse(
	const char* output_file,
	const char* vertex_shader,
	const char* fragment_shader);

std::string ReadBinary(const char* file);
std::string BinToHex(std::string binary);
std::string CurrentDate();

// [SECTION]: Implement
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
	
	if (!out.is_open())
	{
		//std::cerr << std::format("Failed to open file {}!\n", output_file);
		std::exit(ERR_FAILED_TO_OPEN_FILE);
	}

	out << std::format(
		"#pragma once\n"
		"// Date: {}\n"
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

std::string ReadBinary(const char* file)
{
	std::string buffer;
	std::ifstream fs(file, std::ios::ate | std::ios::binary);
	if (!fs.is_open())
	{
		std::cerr << std::format("Failed to open file {}!\n", file);
		std::exit(ERR_FAILED_TO_OPEN_FILE);
	}
	buffer.resize(fs.tellg());
	fs.seekg(0);
	fs.read(buffer.data(), buffer.size());

	return buffer;
}

std::string BinToHex(std::string binary)
{
	std::stringstream hex_stream;
	hex_stream << std::hex << std::uppercase << std::setfill('0');

	static const int column = 16;
	int i = 0;
	for (char8_t byte : binary)
	{
		hex_stream << "0x" << std::setw(2) << static_cast<uint32_t>(byte) << ',';
		if (++i % column == 0) hex_stream << "\n\t\t";
	}
	return hex_stream.str();
}

std::string CurrentDate()
{
	using namespace std::chrono_literals;
	auto UTC = std::chrono::locate_zone("UTC");
	return std::format("{:%Y-%m-%d %H:%M:%S}(UTC)",
		UTC->to_local(std::chrono::system_clock::now()));
}