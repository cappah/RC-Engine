/*========================================================================================
|                                   RC-Engine (c) 2016                                   |
|                             Project: RC-Engine                                         |
|                             File: VulkanPipeline.cpp                                   |
|                             Author: Ruscris2                                           |
==========================================================================================*/

#include "VulkanPipeline.h"

VulkanPipeline::VulkanPipeline()
{
	descriptorLayout = VK_NULL_HANDLE;
	pipelineLayout = VK_NULL_HANDLE;
	pipelineCache = VK_NULL_HANDLE;
	pipeline = VK_NULL_HANDLE;
}

VulkanPipeline::~VulkanPipeline()
{
	descriptorLayout = VK_NULL_HANDLE;
	pipelineLayout = VK_NULL_HANDLE;
	pipeline = VK_NULL_HANDLE;
	pipelineCache = VK_NULL_HANDLE;
}

bool VulkanPipeline::Init(VulkanDevice * vulkanDevice, Shader * shader, VulkanRenderpass * vulkanRenderpass)
{
	VkResult result;

	// Vertex layout
	vertexBinding.binding = 0;
	vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexBinding.stride = sizeof(Vertex);

	VkVertexInputAttributeDescription vertexLayout[3];

	vertexLayout[0].binding = 0;
	vertexLayout[0].location = 0;
	vertexLayout[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexLayout[0].offset = 0;

	vertexLayout[1].binding = 0;
	vertexLayout[1].location = 1;
	vertexLayout[1].format = VK_FORMAT_R32G32_SFLOAT;
	vertexLayout[1].offset = sizeof(float) * 3;

	vertexLayout[2].binding = 0;
	vertexLayout[2].location = 2;
	vertexLayout[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexLayout[2].offset = sizeof(float) * 5;

	// Pipeline layout
	VkDescriptorSetLayoutBinding layoutBindings[3];

	// Vertex shader uniform buffer
	layoutBindings[0].binding = 0;
	layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings[0].descriptorCount = 1;
	layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	layoutBindings[0].pImmutableSamplers = VK_NULL_HANDLE;

	// Fragment shader sampler
	layoutBindings[1].binding = 1;
	layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layoutBindings[1].descriptorCount = 1;
	layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	layoutBindings[1].pImmutableSamplers = VK_NULL_HANDLE;

	// Fragment shader uniform buffer
	layoutBindings[2].binding = 2;
	layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings[2].descriptorCount = 1;
	layoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	layoutBindings[2].pImmutableSamplers = VK_NULL_HANDLE;

	VkDescriptorSetLayoutCreateInfo descriptorLayoutCI{};
	descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorLayoutCI.bindingCount = sizeof(layoutBindings) / sizeof(layoutBindings[0]);
	descriptorLayoutCI.pBindings = layoutBindings;

	result = vkCreateDescriptorSetLayout(vulkanDevice->GetDevice(), &descriptorLayoutCI, VK_NULL_HANDLE, &descriptorLayout);
	if (result != VK_SUCCESS)
		return false;

	VkPipelineLayoutCreateInfo pipelineLayoutCI{};
	pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCI.setLayoutCount = 1;
	pipelineLayoutCI.pSetLayouts = &descriptorLayout;

	result = vkCreatePipelineLayout(vulkanDevice->GetDevice(), &pipelineLayoutCI, VK_NULL_HANDLE, &pipelineLayout);
	if (result != VK_SUCCESS)
		return false;

	// Pipeline cache
	VkPipelineCacheCreateInfo pipelineCacheCI{};
	pipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipelineCacheCI.pNext = NULL;
	result = vkCreatePipelineCache(vulkanDevice->GetDevice(), &pipelineCacheCI, VK_NULL_HANDLE, &pipelineCache);
	if (result != VK_SUCCESS)
		return false;

	// Pipeline
	VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
	VkPipelineDynamicStateCreateInfo dynamicStateCI{};
	memset(dynamicStateEnables, 0, sizeof(dynamicStateEnables));
	dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCI.pNext = NULL;
	dynamicStateCI.pDynamicStates = dynamicStateEnables;
	dynamicStateCI.dynamicStateCount = 0;

	VkPipelineVertexInputStateCreateInfo vi{};
	memset(&vi, 0, sizeof(vi));
	vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vi.pNext = NULL;
	vi.flags = 0;
	vi.vertexBindingDescriptionCount = 1;
	vi.pVertexBindingDescriptions = &vertexBinding;
	vi.vertexAttributeDescriptionCount = sizeof(vertexLayout) / sizeof(vertexLayout[0]);
	vi.pVertexAttributeDescriptions = vertexLayout;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCI{};
	inputAssemblyCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCI.pNext = NULL;
	inputAssemblyCI.primitiveRestartEnable = VK_FALSE;
	inputAssemblyCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineRasterizationStateCreateInfo rs{};
	rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.pNext = 0;
	rs.polygonMode = VK_POLYGON_MODE_FILL;
	rs.cullMode = VK_CULL_MODE_BACK_BIT;
	rs.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rs.depthClampEnable = VK_TRUE;
	rs.rasterizerDiscardEnable = VK_FALSE;
	rs.depthBiasEnable = VK_FALSE;
	rs.depthBiasConstantFactor = 0;
	rs.depthBiasClamp = 0;
	rs.depthBiasSlopeFactor = 0;
	rs.lineWidth = 1.0f;

	VkPipelineColorBlendStateCreateInfo cb{};
	cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cb.flags = 0;
	cb.pNext = NULL;

	VkPipelineColorBlendAttachmentState att_state[1];
	att_state[0].colorWriteMask = 0xf;
	att_state[0].blendEnable = VK_FALSE;
	att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;
	att_state[0].colorBlendOp = VK_BLEND_OP_ADD;
	att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	cb.attachmentCount = 1;
	cb.pAttachments = att_state;
	cb.logicOpEnable = VK_FALSE;
	cb.logicOp = VK_LOGIC_OP_NO_OP;
	cb.blendConstants[0] = 1.0f;
	cb.blendConstants[1] = 1.0f;
	cb.blendConstants[2] = 1.0f;
	cb.blendConstants[3] = 1.0f;

	VkPipelineViewportStateCreateInfo vp{};
	vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vp.pNext = NULL;
	vp.viewportCount = 1;
	vp.scissorCount = 1;
	vp.pScissors = NULL;
	vp.pViewports = NULL;
	dynamicStateEnables[dynamicStateCI.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
	dynamicStateEnables[dynamicStateCI.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;

	VkPipelineDepthStencilStateCreateInfo ds{};
	ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	ds.pNext = NULL;
	ds.depthTestEnable = VK_TRUE;
	ds.depthWriteEnable = VK_TRUE;
	ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	ds.depthBoundsTestEnable = VK_FALSE;
	ds.stencilTestEnable = VK_FALSE;
	ds.back.failOp = VK_STENCIL_OP_KEEP;
	ds.back.passOp = VK_STENCIL_OP_KEEP;
	ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
	ds.back.compareMask = 0;
	ds.back.reference = 0;
	ds.back.depthFailOp = VK_STENCIL_OP_KEEP;
	ds.back.writeMask = 0;
	ds.minDepthBounds = 0;
	ds.maxDepthBounds = 0;
	ds.stencilTestEnable = VK_FALSE;
	ds.front = ds.back;

	VkPipelineMultisampleStateCreateInfo ms;
	ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ms.pNext = NULL;
	ms.flags = 0;
	ms.pSampleMask = NULL;
	ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	ms.sampleShadingEnable = VK_FALSE;
	ms.alphaToCoverageEnable = VK_FALSE;
	ms.alphaToOneEnable = VK_FALSE;
	ms.minSampleShading = 0.0;

	VkGraphicsPipelineCreateInfo pipelineCI{};
	pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCI.pNext = NULL;
	pipelineCI.layout = pipelineLayout;
	pipelineCI.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCI.basePipelineIndex = 0;
	pipelineCI.flags = 0;
	pipelineCI.pVertexInputState = &vi;
	pipelineCI.pInputAssemblyState = &inputAssemblyCI;
	pipelineCI.pRasterizationState = &rs;
	pipelineCI.pColorBlendState = &cb;
	pipelineCI.pTessellationState = VK_NULL_HANDLE;
	pipelineCI.pMultisampleState = &ms;
	pipelineCI.pDynamicState = &dynamicStateCI;
	pipelineCI.pViewportState = &vp;
	pipelineCI.pDepthStencilState = &ds;
	pipelineCI.pStages = shader->GetShaderStages();
	pipelineCI.stageCount = 2;
	pipelineCI.renderPass = vulkanRenderpass->GetRenderpass();
	pipelineCI.subpass = 0;

	result = vkCreateGraphicsPipelines(vulkanDevice->GetDevice(), pipelineCache, 1, &pipelineCI, VK_NULL_HANDLE, &pipeline);
	if (result != VK_SUCCESS)
		return false;
	
	return true;
}

void VulkanPipeline::Unload(VulkanDevice * vulkanDevice)
{
	vkDestroyPipelineLayout(vulkanDevice->GetDevice(), pipelineLayout, VK_NULL_HANDLE);
	vkDestroyDescriptorSetLayout(vulkanDevice->GetDevice(), descriptorLayout, VK_NULL_HANDLE);
	vkDestroyPipeline(vulkanDevice->GetDevice(), pipeline, VK_NULL_HANDLE);
	vkDestroyPipelineCache(vulkanDevice->GetDevice(), pipelineCache, VK_NULL_HANDLE);
}

void VulkanPipeline::SetActive(VulkanCommandBuffer * commandBuffer)
{
	vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

VkDescriptorSetLayout * VulkanPipeline::GetDescriptorLayout()
{
	return &descriptorLayout;
}

VkPipelineLayout VulkanPipeline::GetPipelineLayout()
{
	return pipelineLayout;
}
