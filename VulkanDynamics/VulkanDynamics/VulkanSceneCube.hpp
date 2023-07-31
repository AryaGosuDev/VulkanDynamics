#ifndef __VK_SCENECUBE_HPP__
#define __VK_SCENECUBE_HPP__

namespace VkApplication {
    const int CUBE_W = 10;
    const int CUBE_H = 10;
    const int CUBE_L = 10;

    // Contains the instanced data
    struct InstanceBuffer {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        size_t size = 0;
        VkDescriptorBufferInfo descriptor;
    } instanceBufferCube;

    std::vector<VkCommandBuffer> commandBuffersCube;

    void MainVulkApplication::drawFrameCube() {
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
            imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        updateUniformBuffer(imageIndex);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffersCube[imageIndex], &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("failed to begin recording command buffer!");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        //std::array<VkClearValue, 1> clearValues{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffersCube[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffersCube[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkBuffer vertexBuffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };

        //draw cube spheres
        vkCmdBindDescriptorSets(commandBuffersCube[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[imageIndex], 0, NULL);
        vkCmdBindPipeline(commandBuffersCube[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.cube);
        //sphere mesh
        vkCmdBindVertexBuffers(commandBuffersCube[imageIndex], 0, 1, vertexBuffers, offsets);
        //instance data
        vkCmdBindVertexBuffers(commandBuffersCube[imageIndex], 1, 1, &instanceBufferCube.buffer, offsets);
        // Bind index buffer
        vkCmdBindIndexBuffer(commandBuffersCube[imageIndex], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        //draw ground
        vertexBuffers[0] = { vertexBuffer_ground };
        vkCmdBindDescriptorSets(commandBuffersCube[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[imageIndex], 0, NULL);
        vkCmdBindPipeline(commandBuffersCube[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.ground);
        vkCmdBindVertexBuffers(commandBuffersCube[imageIndex], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffersCube[imageIndex], indexBuffer_ground, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(commandBuffersCube[imageIndex], static_cast<uint32_t>(indices_ground.size()), 1, 0, 0, 0);

        //draw mirror
        /*
        vertexBuffers[0] = { vertexBuffer_mirror };
        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffers[imageIndex], indexBuffer_mirror, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[imageIndex], 0, NULL);
        vkCmdDrawIndexed(commandBuffers[imageIndex], static_cast<uint32_t>(indices_mirror.size()), 1, 0, 0, 0);
        */

        vkCmdEndRenderPass(commandBuffersCube[imageIndex]);

        /*
        // DRAW GUI
        render_gui();

        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = imgui_window.RenderPass;
        info.framebuffer = swapChainFramebuffers[currentFrame];
        info.renderArea.extent = swapChainExtent;
        info.clearValueCount = 1;

        std::array<VkClearValue, 2> clearValuesGui{};
        clearValuesGui[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
        clearValuesGui[1].depthStencil = { 1.0f, 0 };
        info.clearValueCount = static_cast<uint32_t>(clearValuesGui.size());
        info.pClearValues = clearValuesGui.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &info, VK_SUBPASS_CONTENTS_INLINE);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        // END DRAW GUI
        */
        if (vkEndCommandBuffer(commandBuffersCube[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        //VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame], imageAvailableSemaphores_Reflect };
        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffersCube[imageIndex];

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        check_vk_result(vkResetFences(device, 1, &inFlightFences[currentFrame]));

        VkResult returnThis = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);
        if (returnThis != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer! " + std::to_string(returnThis));
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        std::vector<VkResult> presentResults(swapChainImages.size());

        VkSwapchainKHR swapChains[] = { swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        //presentInfo.pResults = presentResults.data();

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        //std::cout << currentFrame << std::endl;
    }

    void MainVulkApplication::createCommandBuffersCube() {
        commandBuffersCube.resize(swapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate command buffers!");
    }

    void MainVulkApplication::prepareInstanceDataCube() {
        std::vector<InstanceData> instanceData;
        instanceData.resize(INSTANCE_COUNT_CUBE);
        int row_x = 0;
        glm::vec3 centerOfCube(0.0, 2.0, 0.0);
        double x_step = (double)CUBE_L / 4.0;
        double y_step = (double)CUBE_H / 4.0;
        double z_step = (double)CUBE_W / 4.0;
        double x_pos = centerOfCube.x - ((double)CUBE_L / 2.0);
       
        // Distribute spheres in cube evenly
        for (auto i = 0; i < 4; i++) {
            x_pos += x_step * (double)i;
            double y_pos = centerOfCube.x - ((double)CUBE_H / 2.0);
            for (auto j = 0; j < 4; ++j) {
                y_pos += y_step * (double)j;
                double z_pos = centerOfCube.x - ((double)CUBE_W / 2.0);
                for (auto k = 0; k < 4; ++k) {
                    z_pos += z_step * (double)k;
                    int indx = (i * 16) + (j * 4) + (k);
                    instanceData[indx].pos = glm::vec3(x_pos, y_step, z_pos);
                    instanceData[indx].rot = glm::vec3(1.0, 1.0, 1.0);
                    instanceData[indx].texIndex = 1;
                }
            }
        }

        instanceBufferCube.size = instanceData.size() * sizeof(InstanceData);

        // Staging
        // Instanced data is static, copy to device local memory
        // This results in better performance

        struct {
            VkDeviceMemory memory;
            VkBuffer buffer;
        } stagingBuffer;

        createBuffer(instanceBufferCube.size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer.buffer, stagingBuffer.memory);

        void* data;
        vkMapMemory(device, stagingBuffer.memory, 0, instanceBufferCube.size, 0, &data);
        memcpy(data, instanceData.data(), static_cast<size_t>(instanceBufferCube.size));
        vkUnmapMemory(device, stagingBuffer.memory);

        createBuffer(instanceBufferCube.size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, instanceBufferCube.buffer, instanceBufferCube.memory);

        copyBuffer(stagingBuffer.buffer, instanceBufferCube.buffer, instanceBufferCube.size);

        vkDestroyBuffer(device, stagingBuffer.buffer, nullptr);
        vkFreeMemory(device, stagingBuffer.memory, nullptr);
    }
}

#endif