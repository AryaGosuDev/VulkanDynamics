#ifndef __VK_IMGUI_HPP__
#define __VK_IMGUI_HPP__

namespace VkApplication {

	VkSampler imgui_sampler;
	VkBuffer imgui_vertexBuffer;
	VkBuffer imgui_indexBuffer;
	VkDeviceMemory imgui_vertexBufferMemory;
	VkDeviceMemory imgui_indexBufferMemory;
	int32_t imgui_vertexCount = 0;
	int32_t imgui_indexCount = 0;
	VkDeviceMemory imgui_fontMemory = VK_NULL_HANDLE;
	VkImage imgui_fontImage = VK_NULL_HANDLE;
	VkImageView imgui_fontView = VK_NULL_HANDLE;
	VkPipelineCache imgui_pipelineCache;
	VkPipelineLayout imgui_pipelineLayout;
	VkPipeline imgui_pipeline;
	VkDescriptorPool imgui_descriptorPool;
	VkDescriptorSetLayout imgui_descriptorSetLayout;
	VkDescriptorSet imgui_descriptorSet;
	ImGuiStyle vulkanStyle;
	int selectedStyle = 0;

	struct UISettings {
		bool displayModels = true;
		bool displayLogos = true;
		bool displayBackground = true;
		bool animateLight = false;
		float lightSpeed = 0.25f;
		std::array<float, 50> frameTimes{};
		float frameTimeMin = 9999.0f, frameTimeMax = 0.0f;
		float lightTimer = 0.0f;
	} uiSettings;

	struct imgui_PushConstBlock {
		glm::vec2 scale;
		glm::vec2 translate;
	} imgui_pushConstBlock;

	void MainVulkApplication::createImguiContext() {

		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.DisplaySize = ImVec2(WIDTH, HEIGHT);
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value() };
		
		// Create font texture
		unsigned char* fontData;
		int texWidth, texHeight;
		io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
		VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);

		/*
		//SRS - Get Vulkan device driver information if available, use later for display
		if (device->extensionSupported(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME))
		{
			VkPhysicalDeviceProperties2 deviceProperties2 = {};
			deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			deviceProperties2.pNext = &driverProperties;
			driverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
			vkGetPhysicalDeviceProperties2(device->physicalDevice, &deviceProperties2);
		}
		*/

		// Create target image for copy
		VkImageCreateInfo imageInfo = {};
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageInfo.extent.width = texWidth;
		imageInfo.extent.height = texHeight;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		check_vk_result(vkCreateImage(device, &imageInfo, nullptr, &imgui_fontImage));
		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(device, imgui_fontImage, &memReqs);
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
		VkMemoryAllocateInfo memAllocInfo = {};
		memAllocInfo.allocationSize = memReqs.size; 
		int32_t memoryType =
			Tools::findProperties(&memoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		memAllocInfo.memoryTypeIndex = memoryType;
		check_vk_result(vkAllocateMemory(device, &memAllocInfo, nullptr, &imgui_fontMemory));
		check_vk_result(vkBindImageMemory(device, imgui_fontImage, imgui_fontMemory, 0));

		// Image view
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.image = imgui_fontImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.layerCount = 1;
		check_vk_result(vkCreateImageView(device, &viewInfo, nullptr, &imgui_fontView));

		// Staging buffers for font data upload
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		createBuffer(uploadSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);
		/*
		check_vk_result(device->createBuffer(
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer,
			uploadSize));
		*/

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, uploadSize, 0, &data);
		memcpy(data, fontData, static_cast<size_t>(uploadSize));
		vkUnmapMemory(device, stagingBufferMemory);
		/*
		stagingBuffer.map();
		memcpy(stagingBuffer.mapped, fontData, uploadSize);
		stagingBuffer.unmap();
		*/
		// Copy buffer data to font image
		//VkCommandBuffer copyCmd = createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
		VkCommandBufferAllocateInfo cmdBufAllocateInfo = Initializers::commandBufferAllocateInfo(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
		VkCommandBuffer cpycmdBuffer;
		check_vk_result(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &cpycmdBuffer));
		VkCommandBufferBeginInfo cmdBufInfo = Initializers::commandBufferBeginInfo();
		check_vk_result(vkBeginCommandBuffer(cpycmdBuffer, &cmdBufInfo));

		// Prepare for transfer
		Tools::setImageLayout(
			cpycmdBuffer,
			imgui_fontImage,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_PIPELINE_STAGE_HOST_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT);

		// Copy
		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageExtent.width = texWidth;
		bufferCopyRegion.imageExtent.height = texHeight;
		bufferCopyRegion.imageExtent.depth = 1;

		vkCmdCopyBufferToImage(cpycmdBuffer, stagingBuffer, imgui_fontImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

		// Prepare for shader read
		Tools::setImageLayout(
			cpycmdBuffer,
			imgui_fontImage,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

		Tools::flushCommandBuffer(cpycmdBuffer, graphicsQueue, device, commandPool, true);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		
		// Font texture Sampler
		VkSamplerCreateInfo imgui_samplerInfo = Initializers::samplerCreateInfo();
		imgui_samplerInfo.magFilter = VK_FILTER_LINEAR;
		imgui_samplerInfo.minFilter = VK_FILTER_LINEAR;
		imgui_samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		imgui_samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		imgui_samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		imgui_samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		imgui_samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		check_vk_result(vkCreateSampler(device, &imgui_samplerInfo, nullptr, &imgui_sampler));

		// Descriptor pool
		std::vector<VkDescriptorPoolSize> imgui_poolSizes = {
			Initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
		};
		VkDescriptorPoolCreateInfo imgui_descriptorPoolInfo = Initializers::descriptorPoolCreateInfo(imgui_poolSizes, 2);
		check_vk_result(vkCreateDescriptorPool(device, &imgui_descriptorPoolInfo, nullptr, &imgui_descriptorPool));

		// Descriptor set layout
		std::vector<VkDescriptorSetLayoutBinding> imgui_setLayoutBindings = {
			Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0),
		};
		VkDescriptorSetLayoutCreateInfo imgui_descriptorLayout = Initializers::descriptorSetLayoutCreateInfo(imgui_setLayoutBindings);
		check_vk_result(vkCreateDescriptorSetLayout(device, &imgui_descriptorLayout, nullptr, &imgui_descriptorSetLayout));

		// Descriptor set
		VkDescriptorSetAllocateInfo imgui_allocInfo = Initializers::descriptorSetAllocateInfo(imgui_descriptorPool, &imgui_descriptorSetLayout, 1);
		check_vk_result(vkAllocateDescriptorSets(device, &imgui_allocInfo, &imgui_descriptorSet));
		VkDescriptorImageInfo imgui_fontDescriptor = Initializers::descriptorImageInfo(
			imgui_sampler,
			imgui_fontView,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		);
		std::vector<VkWriteDescriptorSet> imgui_writeDescriptorSets = {
			Initializers::writeDescriptorSet(imgui_descriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, &imgui_fontDescriptor)
		};
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(imgui_writeDescriptorSets.size()), imgui_writeDescriptorSets.data(), 0, nullptr);

		// Pipeline cache
		VkPipelineCacheCreateInfo imgui_pipelineCacheCreateInfo = {};
		imgui_pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		check_vk_result(vkCreatePipelineCache(device, &imgui_pipelineCacheCreateInfo, nullptr, &imgui_pipelineCache));

		// Pipeline layout
		// Push constants for UI rendering parameters
		VkPushConstantRange imgui_pushConstantRange = Initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(imgui_PushConstBlock), 0);
		VkPipelineLayoutCreateInfo imgui_pipelineLayoutCreateInfo = Initializers::pipelineLayoutCreateInfo(&imgui_descriptorSetLayout, 1);
		imgui_pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		imgui_pipelineLayoutCreateInfo.pPushConstantRanges = &imgui_pushConstantRange;
		check_vk_result(vkCreatePipelineLayout(device, &imgui_pipelineLayoutCreateInfo, nullptr, &imgui_pipelineLayout));

		// Setup graphics pipeline for UI rendering
		VkPipelineInputAssemblyStateCreateInfo imgui_inputAssemblyState =
			Initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

		VkPipelineRasterizationStateCreateInfo imgui_rasterizationState =
			Initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);

		// Enable blending
		VkPipelineColorBlendAttachmentState imgui_blendAttachmentState{};
		imgui_blendAttachmentState.blendEnable = VK_TRUE;
		imgui_blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		imgui_blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		imgui_blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		imgui_blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		imgui_blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		imgui_blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		imgui_blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo imgui_colorBlendState =
			Initializers::pipelineColorBlendStateCreateInfo(1, &imgui_blendAttachmentState);

		VkPipelineDepthStencilStateCreateInfo imgui_depthStencilState =
			Initializers::pipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);

		VkPipelineViewportStateCreateInfo imgui_viewportState =
			Initializers::pipelineViewportStateCreateInfo(1, 1, 0);

		VkPipelineMultisampleStateCreateInfo imgui_multisampleState =
			Initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

		std::vector<VkDynamicState> imgui_dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo imgui_dynamicState =
			Initializers::pipelineDynamicStateCreateInfo(imgui_dynamicStateEnables);

		std::array<VkPipelineShaderStageCreateInfo, 2> imgui_shaderStages{};

		VkGraphicsPipelineCreateInfo imgui_pipelineCreateInfo = Initializers::pipelineCreateInfo(imgui_pipelineLayout, renderPass);

		imgui_pipelineCreateInfo.pInputAssemblyState = &imgui_inputAssemblyState;
		imgui_pipelineCreateInfo.pRasterizationState = &imgui_rasterizationState;
		imgui_pipelineCreateInfo.pColorBlendState = &imgui_colorBlendState;
		imgui_pipelineCreateInfo.pMultisampleState = &imgui_multisampleState;
		imgui_pipelineCreateInfo.pViewportState = &imgui_viewportState;
		imgui_pipelineCreateInfo.pDepthStencilState = &imgui_depthStencilState;
		imgui_pipelineCreateInfo.pDynamicState = &imgui_dynamicState;
		imgui_pipelineCreateInfo.stageCount = static_cast<uint32_t>(imgui_shaderStages.size());
		imgui_pipelineCreateInfo.pStages = imgui_shaderStages.data();

		// Vertex bindings an attributes based on ImGui vertex definition
		std::vector<VkVertexInputBindingDescription> imgui_vertexInputBindings = {
			Initializers::vertexInputBindingDescription(0, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX),
		};
		std::vector<VkVertexInputAttributeDescription> imgui_vertexInputAttributes = {
			Initializers::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, pos)),	// Location 0: Position
			Initializers::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, uv)),	// Location 1: UV
			Initializers::vertexInputAttributeDescription(0, 2, VK_FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, col)),	// Location 0: Color
		};
		VkPipelineVertexInputStateCreateInfo imgui_vertexInputState = Initializers::pipelineVertexInputStateCreateInfo();
		imgui_vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(imgui_vertexInputBindings.size());
		imgui_vertexInputState.pVertexBindingDescriptions = imgui_vertexInputBindings.data();
		imgui_vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(imgui_vertexInputAttributes.size());
		imgui_vertexInputState.pVertexAttributeDescriptions = imgui_vertexInputAttributes.data();

		imgui_pipelineCreateInfo.pVertexInputState = &imgui_vertexInputState;

		auto vertShaderCode = readFile("shaders/uivert.spv");
		auto fragShaderCode = readFile("shaders/uifrag.spv");

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		imgui_shaderStages[0] = vertShaderStageInfo;
		imgui_shaderStages[1] = fragShaderStageInfo;

		check_vk_result(vkCreateGraphicsPipelines(device, imgui_pipelineCache, 1, &imgui_pipelineCreateInfo, nullptr, &imgui_pipeline));
	}
	
	void MainVulkApplication::newFrame( bool updateFrameGraph) {
		ImGui::NewFrame();

		// Init imGui windows and elements

		// Debug window
		//ImGui::SetWindowPos(ImVec2(20 , 20 ), ImGuiSetCond_FirstUseEver);
		//ImGui::SetWindowSize(ImVec2(300 , 300 ), ImGuiSetCond_Always);
		//ImGui::TextUnformatted(example->title.c_str());
		//ImGui::TextUnformatted(device->properties.deviceName);

		//SRS - Display Vulkan API version and device driver information if available (otherwise blank)
		//ImGui::Text("Vulkan API %i.%i.%i", VK_API_VERSION_MAJOR(device->properties.apiVersion), VK_API_VERSION_MINOR(device->properties.apiVersion), VK_API_VERSION_PATCH(device->properties.apiVersion));
		//ImGui::Text("%s %s", driverProperties.driverName, driverProperties.driverInfo);

		// Update frame time display
		if (updateFrameGraph) {
			std::rotate(uiSettings.frameTimes.begin(), uiSettings.frameTimes.begin() + 1, uiSettings.frameTimes.end());
			//float frameTime = 1000.0f / (example->frameTimer * 1000.0f);
			float frameTime = 1000.0f / (1 * 1000.0f);
			uiSettings.frameTimes.back() = frameTime;
			if (frameTime < uiSettings.frameTimeMin) {
				uiSettings.frameTimeMin = frameTime;
			}
			if (frameTime > uiSettings.frameTimeMax) {
				uiSettings.frameTimeMax = frameTime;
			}
		}

		//ImGui::PlotLines("Frame Times", &uiSettings.frameTimes[0], 50, 0, "", uiSettings.frameTimeMin, uiSettings.frameTimeMax, ImVec2(0, 80));

		ImGui::Text("Camera");
		//ImGui::InputFloat3("position", camera.position.x, 2);
		//ImGui::InputFloat3("rotation", camera.rotation.x, 2);

		// Example settings window
		//ImGui::SetNextWindowPos(ImVec2(20 * example->UIOverlay.scale, 360 * example->UIOverlay.scale), ImGuiSetCond_FirstUseEver);
		//ImGui::SetNextWindowSize(ImVec2(300 * example->UIOverlay.scale, 200 * example->UIOverlay.scale), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Example settings");
		ImGui::Checkbox("Render models", &uiSettings.displayModels);
		ImGui::Checkbox("Display logos", &uiSettings.displayLogos);
		ImGui::Checkbox("Display background", &uiSettings.displayBackground);
		ImGui::Checkbox("Animate light", &uiSettings.animateLight);
		ImGui::SliderFloat("Light speed", &uiSettings.lightSpeed, 0.1f, 1.0f);
		//ImGui::ShowStyleSelector("UI style");

		ImGui::End();

		//SRS - ShowDemoWindow() sets its own initial position and size, cannot override here
		//ImGui::ShowDemoWindow();

		// Render to generate draw buffers
		ImGui::Render();
	}

	void MainVulkApplication::gui_updateBuffers()
	{
		ImDrawData* imDrawData = ImGui::GetDrawData();

		// Note: Alignment is done inside buffer creation
		VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
		VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

		if ((vertexBufferSize == 0) || (indexBufferSize == 0)) {
			return;
		}

		void* data = NULL;

		// Update buffers only if vertex or index count has been changed compared to current buffer size
		// Vertex buffer
		if ((imgui_vertexBuffer == VK_NULL_HANDLE) || (imgui_vertexCount != imDrawData->TotalVtxCount)) {
			if ( imgui_vertexBufferMemory != VK_NULL_HANDLE ) vkUnmapMemory(device, imgui_vertexBufferMemory);
			if (imgui_vertexBuffer != VK_NULL_HANDLE )  vkDestroyBuffer(device, imgui_vertexBuffer, nullptr);
			vkFreeMemory(device, imgui_vertexBufferMemory, nullptr);
			createBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				imgui_vertexBuffer, imgui_vertexBufferMemory);
			imgui_vertexCount = imDrawData->TotalVtxCount;
			vkMapMemory(device, imgui_vertexBufferMemory, 0, vertexBufferSize, 0, &data);
		}

		void* data1 = NULL;

		// Index buffer
		if ((imgui_indexBuffer == VK_NULL_HANDLE) || (imgui_indexCount < imDrawData->TotalIdxCount)) {
			if (imgui_indexBufferMemory != VK_NULL_HANDLE) vkUnmapMemory(device, imgui_indexBufferMemory);
			if (imgui_indexBuffer != VK_NULL_HANDLE) vkDestroyBuffer(device, imgui_indexBuffer, nullptr);
			vkFreeMemory(device, imgui_indexBufferMemory, nullptr);
			createBuffer(indexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				imgui_indexBuffer, imgui_indexBufferMemory);
			imgui_indexCount = imDrawData->TotalVtxCount;
			vkMapMemory(device, imgui_indexBufferMemory, 0, indexBufferSize, 0, &data1);
		}

		if (data1 == NULL || data == NULL) return;

		// Upload data
		ImDrawVert* vtxDst = (ImDrawVert*)data;
		ImDrawIdx* idxDst = (ImDrawIdx*)data1;

		for (int n = 0; n < imDrawData->CmdListsCount; n++) {
			const ImDrawList* cmd_list = imDrawData->CmdLists[n];
			memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			vtxDst += cmd_list->VtxBuffer.Size;
			idxDst += cmd_list->IdxBuffer.Size;
		}

		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = imgui_vertexBufferMemory;
		mappedRange.offset = 0;
		mappedRange.size = vertexBufferSize;
		check_vk_result(vkFlushMappedMemoryRanges(device, 1, &mappedRange));
		mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = imgui_indexBufferMemory;
		mappedRange.offset = 0;
		mappedRange.size = indexBufferSize;
		check_vk_result(vkFlushMappedMemoryRanges(device, 1, &mappedRange));
		// Flush to make writes visible to GPU
		//vertexBuffer.flush();
		//indexBuffer.flush();
	}

	// Draw current imGui frame into a command buffer
	void MainVulkApplication::drawFrame(VkCommandBuffer commandBuffer)
	{
		ImGuiIO& io = ImGui::GetIO();

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, imgui_pipelineLayout, 0, 1, &imgui_descriptorSet, 0, nullptr);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, imgui_pipeline);

		VkViewport viewport = Initializers::viewport(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, 0.0f, 1.0f);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		// UI scale and translate via push constants
		imgui_pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
		imgui_pushConstBlock.translate = glm::vec2(-1.0f);
		vkCmdPushConstants(commandBuffer, imgui_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(imgui_PushConstBlock), &imgui_pushConstBlock);

		// Render commands
		ImDrawData* imDrawData = ImGui::GetDrawData();
		int32_t vertexOffset = 0;
		int32_t indexOffset = 0;

		if (imDrawData->CmdListsCount > 0) {

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &imgui_vertexBuffer, offsets);
			vkCmdBindIndexBuffer(commandBuffer,imgui_indexBuffer, 0, VK_INDEX_TYPE_UINT16);

			for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
			{
				const ImDrawList* cmd_list = imDrawData->CmdLists[i];
				for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
				{
					const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
					VkRect2D scissorRect;
					scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
					scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
					scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
					scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
					vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
					vkCmdDrawIndexed(commandBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
					indexOffset += pcmd->ElemCount;
				}
				vertexOffset += cmd_list->VtxBuffer.Size;
			}
		}
	}

	// TODO : Destroy imgui and pool
}
#endif