#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <format>

// [SECTION]: Declaration
std::string ReadBinary(const char* file);
std::string BinToHex(std::string binary);
std::string CurrentDate();

// [SECTION]: Implement
std::string ReadBinary(const char* file)
{
	std::string buffer;
	std::ifstream fs(file, std::ios::ate | std::ios::binary);
	if (!fs.is_open())
	{
		throw std::runtime_error(std::format("Failed to open file {}!\n", file));
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