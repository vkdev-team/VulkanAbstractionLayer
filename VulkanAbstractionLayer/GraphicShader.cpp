// Copyright(c) 2021, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "GraphicShader.h"
#include "VectorMath.h"
#include "VulkanContext.h"

namespace VulkanAbstractionLayer
{
    void GraphicShader::Destroy()
    {
        auto& vulkan = GetCurrentVulkanContext();
        auto& device = vulkan.GetDevice();
        if ((bool)this->vertexShader) device.destroyShaderModule(this->vertexShader);
        if ((bool)this->fragmentShader) device.destroyShaderModule(this->fragmentShader);
        // descriptorSetLayout is not deleted as it belongs to VulkanContext::DescriptorCache

        this->vertexShader = vk::ShaderModule{ };
        this->fragmentShader = vk::ShaderModule{ };
        this->descriptorSetLayout = vk::DescriptorSetLayout{ };
    }

    GraphicShader::GraphicShader(const ShaderData& vertex, const ShaderData& fragment)
    {
        this->Init(vertex, fragment);
    }

    void GraphicShader::Init(const ShaderData& vertex, const ShaderData& fragment)
    {
        auto& vulkan = GetCurrentVulkanContext();

        vk::ShaderModuleCreateInfo vertexShaderInfo;
        vertexShaderInfo.setCode(vertex.Bytecode);
        this->vertexShader = vulkan.GetDevice().createShaderModule(vertexShaderInfo);

        vk::ShaderModuleCreateInfo fragmentShaderInfo;
        fragmentShaderInfo.setCode(fragment.Bytecode);
        this->fragmentShader = vulkan.GetDevice().createShaderModule(fragmentShaderInfo);

        this->vertexAttributes = vertex.InputAttributes;
        
        auto specification = std::array{
            DescriptorCache::UniformsPerStage{ vertex.UniformBlocks[0], vk::ShaderStageFlagBits::eVertex },
            DescriptorCache::UniformsPerStage{ fragment.UniformBlocks[0], vk::ShaderStageFlagBits::eFragment },
        };

        this->descriptorSetLayout = vulkan.GetDescriptorCache().GetDescriptorSetLayout(specification);
    }

    GraphicShader::GraphicShader(GraphicShader&& other) noexcept
    {
        this->vertexShader = other.vertexShader;
        this->fragmentShader = other.fragmentShader;
        this->descriptorSetLayout = other.descriptorSetLayout;
        this->vertexAttributes = std::move(other.vertexAttributes);

        other.vertexShader = vk::ShaderModule{ };
        other.fragmentShader = vk::ShaderModule{ };
        other.descriptorSetLayout = vk::DescriptorSetLayout{ };
    }

    GraphicShader& GraphicShader::operator=(GraphicShader&& other) noexcept
    {
        this->Destroy();

        this->vertexShader = other.vertexShader;
        this->fragmentShader = other.fragmentShader;
        this->descriptorSetLayout = other.descriptorSetLayout;
        this->vertexAttributes = std::move(other.vertexAttributes);

        other.vertexShader = vk::ShaderModule{ };
        other.fragmentShader = vk::ShaderModule{ };
        other.descriptorSetLayout = vk::DescriptorSetLayout{ };

        return *this;
    }

    GraphicShader::~GraphicShader()
    {
        this->Destroy();
    }
}