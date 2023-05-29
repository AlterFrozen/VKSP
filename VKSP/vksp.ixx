module;
#include "vksp_type.h"
export module vksp;
#define VKSP_API export

// [SECTION]: Parameters
static inline std::string OPT_NAMESPACE = "";

// [SECTION]: Declaration
VKSP_API void Parse(
	const char* output_file,
	const char* vertex_shader,
	const char* fragment_shader);

std::string ReadBinary(const char* file);
std::string BinToHex(std::string binary);
std::string AnalyseLayout(std::string_view vertex_shader, std::string_view fragment_shader);
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

#define SPVR_CHECK(result) if (result != SPV_REFLECT_RESULT_SUCCESS){ \
							std::cerr << "Failed to reflect shader!" \
							"\nFILE: "<< __FILE__ << "\nLINE: "<< __LINE__ << "\n";\
							std::exit(ERR_FAILED_TO_REFLECT);}

std::string AnalyseLayout(std::string_view vertex_shader, std::string_view fragment_shader)
{
	
	std::vector<DescriptorBinding> descriptor_set_layout_bindings;
	std::vector<VkPushConstantRange> push_constants;
	uint32_t count = 0;

	static auto ayalyse = [&](std::string_view shader)->void
	{
		SpvReflectShaderModule spvContext;
		SPVR_CHECK(spvReflectCreateShaderModule(shader.size(), shader.data(), &spvContext));

		// Descriptor Sets
		{
			SPVR_CHECK(spvReflectEnumerateDescriptorSets(&spvContext, &count, NULL));
			std::vector<SpvReflectDescriptorSet*> descriptorSets(count);
			SPVR_CHECK(spvReflectEnumerateDescriptorSets(&spvContext, &count, descriptorSets.data()));

			for (const auto& descriptorSet : descriptorSets)
			{
				DescriptorBinding descriptorBinding{ .set = descriptorSet->set, .stages = VK_SHADER_STAGE_VERTEX_BIT };

				auto& bindings = descriptorSet->bindings;
				for (uint32_t i = 0; i < descriptorSet->binding_count; ++i)
				{
					descriptorBinding.binding = bindings[i]->binding;
					descriptorBinding.count = bindings[i]->count;
					descriptorBinding.type = static_cast<VkDescriptorType>(bindings[i]->descriptor_type);
					descriptor_set_layout_bindings.emplace_back(descriptorBinding);
				}
			}
		}
		// Push Constant Range
		{
			SPVR_CHECK(spvReflectEnumeratePushConstants(&spvContext, &count, NULL));
			std::vector<SpvReflectBlockVariable*> pPushConstants(count);
			SPVR_CHECK(spvReflectEnumeratePushConstants(&spvContext, &count, pPushConstants.data()));

			for (const auto& pushConstant : pPushConstants)
			{
				push_constants.emplace_back(
					VkPushConstantRange
					{
						.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
						.offset = pushConstant->offset,
						.size = pushConstant->size
					});
			}
		}

		// Analyse
		{
			if (!descriptor_set_layout_bindings.empty())
			{
				if (descriptor_set_layout_bindings.size() > 1)
					std::sort(descriptor_set_layout_bindings.begin(), descriptor_set_layout_bindings.end(),
						[](const DescriptorBinding& prev, const DescriptorBinding& next)->bool
						{
							if (next.set != prev.set) return next.set < prev.set; // Descending Set Index
							else return next.binding < prev.binding; // Descending Binding Index
						});

				size_t max_set = descriptor_set_layout_bindings.front().set + 1;
				descriptor_set_layouts->resize(max_set);
				std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSets(max_set);

				for (auto& currentBinding : descriptor_set_layout_bindings)
				{
					auto& currentSet = descriptorSets[currentBinding.set];
					if (currentSet.empty())
					{
						currentSet.reserve(currentBinding.binding + 1); // DESC Sorted
						currentSet.emplace_back(currentBinding);
					}
					else
					{
						auto& previousBinding = currentSet.back();
						if (currentBinding.binding == previousBinding.binding)
						{
							previousBinding.stageFlags |= currentBinding.stages; // Same binding but different stages.
						}
						else currentSet.emplace_back(currentBinding); // Differernt bindings
					}
				}
				// Create Descriptor Set Layouts
				for (size_t current_set = 0; current_set < max_set; ++current_set)
				{
					auto& descriptor_set_layout = m_descriptor_set_layouts[current_set];
					VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo
					{
						.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
						.bindingCount = static_cast<uint32_t>(descriptorSets[current_set].size()),
						.pBindings = descriptorSets[current_set].data()
					};
					if (vkCreateDescriptorSetLayout(
						m_context->m_device,
						&descriptorSetLayoutCreateInfo,
						m_context->m_memory_allocation_callback,
						&descriptor_set_layout) != VK_SUCCESS)
						throw std::runtime_error("Failed to create the Vulkan Desceriptor Set Layout automatically!");
				}
			} // End create Descriptor Set Layouts

			if (push_constants && !push_constants->empty())
			{
				if (push_constants->size() > 1)
					std::sort(push_constants->begin(), push_constants->end(),
						[](const VkPushConstantRange& next, const VkPushConstantRange& prev)->bool
						{
							if (next.offset == prev.offset) return next.size < prev.size; // Descending Range Size
							else return next.offset < prev.offset; // Descending Range Offset
						});

				std::vector<VkPushConstantRange> pushConstants{ push_constants->front() };
				for (size_t i = 1; i < push_constants->size(); ++i)
				{
					auto& prevPushConstant = (*push_constants)[i - 1];
					auto& curPushConstant = (*push_constants)[i];
					if (curPushConstant.offset == prevPushConstant.offset &&
						curPushConstant.size == prevPushConstant.size)
					{
						prevPushConstant.stageFlags |= curPushConstant.stageFlags; // Same PushConstant but different stages.
					}
					else pushConstants.emplace_back(curPushConstant); // Differernt PushConstants
				}
				push_constants->swap(pushConstants);
			}
		}
		spvReflectDestroyShaderModule(&spvContext);
	};


	return std::format("");
}