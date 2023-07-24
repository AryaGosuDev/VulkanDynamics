#ifndef __VK_REFLECT_HPP__
#define __VK_REFLECT_HPP__

namespace VkApplication {

    VkRenderPass renderPass_reflect;
    VkDescriptorSetLayout descriptorSetLayout_reflect;
    VkPipelineLayout pipelineLayout_reflect;
    VkPipeline graphicsPipeline_reflect;

    VkImage textureImage_reflect;
    VkDeviceMemory textureImageMemory_reflect;
    VkImageView textureImageView_reflect;
    VkSampler textureSampler_reflect;

    VkFramebuffer reflectFramebuffers;
    VkDescriptorSet descriptorSetReflect;

    VkBuffer uniformBuffer_Reflect;
    VkDeviceMemory uniformBuffersMemory_Reflect;

    VkBuffer uniformFragBuffer_Reflect;
    VkDeviceMemory uniformFragBuffersMemory_Reflect;

    VkCommandBuffer commandBuffer_Reflect;

    VkSemaphore imageAvailableSemaphores_Reflect;
    VkSemaphore renderFinishedSemaphores_Reflect;
    VkFence inFlightFences_reflect;

    void MainVulkApplication::createGraphicsPipelineReflect() {

        auto vertShaderCode = readFile("shaders/vertReflect.spv");
        auto fragShaderCode = readFile("shaders/fragReflect.spv");

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

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
 
        VkViewport reflectViewport;
        reflectViewport.x = 0.0f;                              // X offset of the viewport
        reflectViewport.y = 0.0f;                              // Y offset of the viewport
        reflectViewport.width = reflectingSurfaceWidth;        // Width of the reflecting surface
        reflectViewport.height = reflectingSurfaceHeight;      // Height of the reflecting surface
        reflectViewport.minDepth = 0.0f;                       // Minimum depth value
        reflectViewport.maxDepth = 1.0f;                       // Maximum depth value

        VkRect2D reflectScissor;
        reflectScissor.offset = { 0, 0 };                      // Offset of the scissor rectangle
        reflectScissor.extent = { (unsigned int )reflectingSurfaceWidth,(unsigned int) reflectingSurfaceHeight }; // Extent of the scissor rectangle

        // Fill in the reflectViewportStateInfo structure
        VkPipelineViewportStateCreateInfo reflectViewportStateInfo = {};
        reflectViewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        reflectViewportStateInfo.pNext = nullptr;
        reflectViewportStateInfo.flags = 0;
        reflectViewportStateInfo.viewportCount = 1;            // Number of viewports
        reflectViewportStateInfo.pViewports = &reflectViewport;
        reflectViewportStateInfo.scissorCount = 1;             // Number of scissors
        reflectViewportStateInfo.pScissors = &reflectScissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // Fill in the reflectDepthStencilInfo structure
        VkPipelineDepthStencilStateCreateInfo reflectDepthStencilInfo = {};
        reflectDepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        reflectDepthStencilInfo.pNext = nullptr;
        reflectDepthStencilInfo.flags = 0;
        reflectDepthStencilInfo.depthTestEnable = VK_TRUE;              // Enable depth testing
        reflectDepthStencilInfo.depthWriteEnable = VK_TRUE;             // Enable writing to the depth buffer
        reflectDepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;    // Depth comparison operation (adjust as needed)
        reflectDepthStencilInfo.depthBoundsTestEnable = VK_FALSE;       // Disable depth bounds testing
        reflectDepthStencilInfo.stencilTestEnable = VK_FALSE;           // Disable stencil testing
        reflectDepthStencilInfo.front = {};                             // Stencil operations for front-facing polygons
        reflectDepthStencilInfo.back = {};                              // Stencil operations for back-facing polygons
        reflectDepthStencilInfo.minDepthBounds = 0.0f;                  // Minimum depth bound
        reflectDepthStencilInfo.maxDepthBounds = 1.0f;
        
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout_reflect) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &reflectViewportStateInfo;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &reflectDepthStencilInfo;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = pipelineLayout_reflect;
        pipelineInfo.renderPass = renderPass_reflect;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline_reflect) != VK_SUCCESS)
            throw std::runtime_error("failed to create graphics pipeline!");

        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }

    void MainVulkApplication::createRenderPassReflect() {
        // Define the color attachment description
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;            // Format of the color attachment
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;              // Number of samples
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;         // Clear the attachment at the beginning
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;       // Store the attachment at the end
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // Not using stencil, so set to DONT_CARE
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;  // Not using stencil, so set to DONT_CARE
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;    // Layout before render pass
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // Layout after render pass

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        // Define the attachment reference
        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;                            // Index of the color attachment in the attachment descriptions
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // Define the subpass description
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;  // Bind point for the subpass
        subpass.colorAttachmentCount = 1;                             // Number of color attachments
        subpass.pColorAttachments = &colorAttachmentRef;              // Color attachment reference(s)
        subpass.pDepthStencilAttachment = &depthAttachmentRef;                    // No depth/stencil attachment

        // Define the subpass dependency
        VkSubpassDependency subpassDependency = {};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;           // Implicit subpass before the render pass
        subpassDependency.dstSubpass = 0;                              // Subpass index of this render pass
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        
        // Fill in the reflectRenderPassInfo structure
        VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo reflectRenderPassInfo = {};
        reflectRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        reflectRenderPassInfo.attachmentCount = 2;                     // Number of attachments
        reflectRenderPassInfo.pAttachments = attachments;         // Attachment description(s)
        reflectRenderPassInfo.subpassCount = 1;                        // Number of subpasses
        reflectRenderPassInfo.pSubpasses = &subpass;                   // Subpass description(s)
        reflectRenderPassInfo.dependencyCount = 1;                     // Number of subpass dependencies
        reflectRenderPassInfo.pDependencies = &subpassDependency;       // Subpass dependency(ies)

        if (vkCreateRenderPass(device, &reflectRenderPassInfo, nullptr, &renderPass_reflect) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void MainVulkApplication::createDescriptorSetReflect() {
        
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &descriptorSetLayout;

        if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSetReflect) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
        
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffer_Reflect;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorBufferInfo bufferFragInfo = {};
        bufferFragInfo.buffer = uniformFragBuffer_Reflect;
        bufferFragInfo.offset = 0;
        bufferFragInfo.range = sizeof(UniformFragmentObject);

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSetReflect;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSetReflect;
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &bufferFragInfo;

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    void MainVulkApplication::drawFrameReflect() {
        vkWaitForFences(device, 1, &inFlightFences_reflect, VK_TRUE, UINT64_MAX);

        //std::cout << "draw" << std::endl;

        //uint32_t imageIndex;
        //VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
            //imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        updateUniformBufferReflect();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer_Reflect, &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("failed to begin recording command buffer!");

        // Perform the initial image layout transition before starting the render pass
        VkImageMemoryBarrier barrierToColorAttachment = {};
        barrierToColorAttachment.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierToColorAttachment.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrierToColorAttachment.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrierToColorAttachment.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierToColorAttachment.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierToColorAttachment.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrierToColorAttachment.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrierToColorAttachment.image = textureImage_reflect;
        barrierToColorAttachment.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        vkCmdPipelineBarrier(
            commandBuffer_Reflect,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrierToColorAttachment
        );

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass_reflect;
        renderPassInfo.framebuffer = reflectFramebuffers;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { (unsigned int)reflectingSurfaceWidth,(unsigned int)reflectingSurfaceHeight };

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer_Reflect, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer_Reflect, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkBuffer vertexBuffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };

        //draw
        vkCmdBindVertexBuffers(commandBuffer_Reflect, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer_Reflect, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer_Reflect, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_reflect, 0, 1, &descriptorSetReflect, 0, NULL);
        vkCmdDrawIndexed(commandBuffer_Reflect, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        //draw
        vertexBuffers[0] = { vertexBuffer_ground };
        vkCmdBindVertexBuffers(commandBuffer_Reflect, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer_Reflect, indexBuffer_ground, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer_Reflect, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_reflect, 0, 1, &descriptorSetReflect, 0, NULL);
        vkCmdDrawIndexed(commandBuffer_Reflect, static_cast<uint32_t>(indices_ground.size()), 1, 0, 0, 0);
        /*
        //draw
        vertexBuffers[0] = { vertexBuffer_mirror };
        vkCmdBindVertexBuffers(commandBuffer_Reflect, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer_Reflect, indexBuffer_mirror, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer_Reflect, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_reflect, 0, 1, &descriptorSetReflect, 0, NULL);
        vkCmdDrawIndexed(commandBuffer_Reflect, static_cast<uint32_t>(indices_mirror.size()), 1, 0, 0, 0);
        */
        vkCmdEndRenderPass(commandBuffer_Reflect);

        // Perform the image layout transition back after ending the render pass
        VkImageMemoryBarrier barrierToShaderReadOnly = {};
        barrierToShaderReadOnly.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierToShaderReadOnly.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrierToShaderReadOnly.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrierToShaderReadOnly.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierToShaderReadOnly.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierToShaderReadOnly.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrierToShaderReadOnly.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrierToShaderReadOnly.image = textureImage_reflect;
        barrierToShaderReadOnly.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        vkCmdPipelineBarrier(
            commandBuffer_Reflect,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrierToShaderReadOnly
        );


        if (vkEndCommandBuffer(commandBuffer_Reflect) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = NULL;
        submitInfo.pWaitDstStageMask = NULL;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer_Reflect;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &imageAvailableSemaphores_Reflect;

        check_vk_result(vkResetFences(device, 1, &inFlightFences_reflect));

        VkResult returnThis = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences_reflect);
        if (returnThis != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer! " + std::to_string(returnThis));
        }
    }

    void MainVulkApplication::updateUniformBufferReflect() {
        
        void* data;
        vkMapMemory(device, uniformBuffersMemory_Reflect, 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, uniformBuffersMemory_Reflect);

        void* data2;
        vkMapMemory(device, uniformFragBuffersMemory_Reflect, 0, sizeof(ufo), 0, &data2);
        memcpy(data2, &ufo, sizeof(ufo));
        vkUnmapMemory(device, uniformFragBuffersMemory_Reflect);
    }

    void MainVulkApplication::createUniformBufferReflect() {

        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        VkDeviceSize bufferFragSize = sizeof(UniformFragmentObject);

        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            uniformBuffer_Reflect, uniformBuffersMemory_Reflect);
        createBuffer(bufferFragSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            uniformFragBuffer_Reflect, uniformFragBuffersMemory_Reflect);
    }

    void MainVulkApplication::createCommandBuffers_Reflect() {

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer_Reflect) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate command buffers!");
    }
    
    void MainVulkApplication::createReflectImage() {
        //VkFormat ReflectFormat = findReflectFormat();

        createImage(reflectingSurfaceWidth, reflectingSurfaceHeight, swapChainImageFormat, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            textureImage_reflect, textureImageMemory_reflect);
        textureImageView_reflect = createImageView(textureImage_reflect, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
        
        //transitionImageLayout(textureImage_reflect, ReflectFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }

    VkFormat MainVulkApplication::findReflectFormat() {
        return findSupportedFormat(
            { VK_FORMAT_R8G8B8A8_UNORM },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
    }

    void MainVulkApplication::createReflectFramebuffer() {

        VkImageView attachments[] = { textureImageView_reflect, depthImageView };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass_reflect;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = reflectingSurfaceWidth;
        framebufferInfo.height = reflectingSurfaceHeight;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &reflectFramebuffers) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    void MainVulkApplication::createReflectSyncObjects() {
        
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores_Reflect) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }

        check_vk_result(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences_reflect) );
    }

    void MainVulkApplication::transitionImageLayoutReflect() {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;  // or whatever the old layout happens to be
        barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = textureImage_reflect;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        // You might need to adjust these access flags and pipeline stages to fit your situation
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;  // depends on the old layout
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;  // depends on the old layout
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        endSingleTimeCommands(commandBuffer);
    }
}

#endif