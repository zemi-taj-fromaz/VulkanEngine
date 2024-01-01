#pragma once

#include "Platform/Vulkan/VulkanInit.h"
#include "Platform/Vulkan/Mesh.h"
#include "Platform/Vulkan/Camera.h"
#include "Platform/Vulkan/Descriptor.h"
//#include "Platform/Vulkan/RenderObject.h"


class AppVulkanImpl;
class RenderObject;

#include <string>
#include <vector>
#include <functional>

struct BufferWrapper
{
	size_t bufferSize;
	int bufferCount{ 1 };
	VkBuffer buffer;
	VkDeviceMemory deviceMemory;
	void* bufferMapped;
};

//struct DescriptorSetLayout
//{
//	Descriptor* descriptor;
//	VkDescriptorSetLayout layout;
//};

struct PipelineLayout
{
	PipelineLayout(){}
	PipelineLayout(std::vector<std::shared_ptr<Descriptor>> layouts) : descriptorSetLayout(layouts){}

	std::vector<std::shared_ptr<Descriptor>> descriptorSetLayout;
	VkPipelineLayout layout;


};


struct Pipeline
{
	Pipeline(std::shared_ptr<PipelineLayout> pipelineLayout, std::vector<std::string> ShaderNames) : pipelineLayout(pipelineLayout), ShaderNames(ShaderNames){}
	Pipeline(std::shared_ptr<PipelineLayout> pipelineLayout, std::vector<std::string> ShaderNames, unsigned int DepthTest, bool Skybox, VkCullModeFlags cullMode) 
		: pipelineLayout(pipelineLayout), ShaderNames(ShaderNames), DepthTest(DepthTest), Skybox(Skybox), cullMode(cullMode) {}

	std::shared_ptr<PipelineLayout> pipelineLayout;
	std::vector<std::string> ShaderNames;
	unsigned int DepthTest{ VK_TRUE };
	bool Skybox{ false };
	VkCullModeFlags cullMode{ VK_CULL_MODE_BACK_BIT };
	VkPolygonMode PolygonMode{ VK_POLYGON_MODE_FILL };
	VkPrimitiveTopology Topology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
	VkPipeline pipeline;
};

struct MeshWrapper
{
	MeshWrapper(std::shared_ptr<Pipeline> pipeline, Mesh mesh) : pipeline(pipeline), mesh(mesh) {}
	MeshWrapper(std::shared_ptr<Pipeline> pipeline, Mesh mesh, bool illuminated) : pipeline(pipeline), mesh(mesh), illuminated(illuminated) {}
	MeshWrapper(std::shared_ptr<Pipeline> pipeline, Mesh mesh,  std::shared_ptr<Texture> texture) : pipeline(pipeline), mesh(mesh), texture(texture) {}
	MeshWrapper(std::shared_ptr<Pipeline> pipeline, Mesh mesh, std::string animation) : pipeline(pipeline), mesh(mesh), animated(animation) {}

	std::shared_ptr<Pipeline> pipeline;
	Mesh mesh;
	std::shared_ptr<Texture> texture{ nullptr };
	std::optional<std::string> animated{ std::nullopt };
	bool illuminated{ false };
	bool isSkybox{ false };
	
	std::optional<glm::mat4> translation;
	std::optional<glm::mat4> rotation;
	std::optional<glm::mat4> scale;

	std::shared_ptr<RenderObject> object;
	std::shared_ptr<MeshWrapper> head;
};


struct ParticleCreateStruct
{
	int particleNumber;
	MeshWrapper meshWrapper;
};

struct Particles
{
	std::vector<MeshWrapper> particlesMesh;
};


class Layer
{
public:
	Layer(const std::string& name = "Layer");
	virtual ~Layer() {};

	virtual void on_update(float timeStep = 0) {}

	inline const std::string& get_name() const { return m_DebugName; }
	std::vector<std::shared_ptr<Descriptor>>& get_descriptors() { return m_Descriptors; }
	//std::vector<DescriptorSetLayout>& get_layouts() { return m_Layouts; }
	std::vector<std::shared_ptr<PipelineLayout>>& get_pipeline_layouts() { return m_PipelineLayouts; }
	std::vector<std::shared_ptr<Pipeline>>& get_pipelines() { return m_Pipelines; }
	std::vector<std::shared_ptr<Texture>>& get_textures() { return m_Textures; }
	std::vector<std::shared_ptr<MeshWrapper>>& get_mesh() { return m_Mesh; }
	std::vector<Particles>& get_particles() { return m_Particles; }
	std::shared_ptr<Pipeline>& get_compute_pipeline() { return m_ComputePipeline; }
	std::shared_ptr<Pipeline>& get_compute_graphics_pipeline() { return m_ComputeGraphicsPipeline; }

	void update_buffers(AppVulkanImpl* app, int imageIndex)
	{
		for (std::shared_ptr<Descriptor>& descriptor : m_Descriptors)
		{
			if (descriptor->descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
				descriptor->shaderFlags == VK_SHADER_STAGE_COMPUTE_BIT
				)
			{
				continue;
			}
			descriptor->bufferUpdateFunc(app, descriptor->bufferWrappers[imageIndex % descriptor->bufferWrappers.size()].bufferMapped);
		}
	}

	void update_compute_buffers(AppVulkanImpl* app, int imageIndex)
	{
		for (std::shared_ptr<Descriptor>& descriptor : m_Descriptors)
		{
			if (descriptor->descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
				descriptor->shaderFlags != VK_SHADER_STAGE_COMPUTE_BIT
				)
			{
				continue;
			}
			if (descriptor->tie) continue;
			descriptor->bufferUpdateFunc(app, descriptor->bufferWrappers[imageIndex % descriptor->bufferWrappers.size()].bufferMapped);
		}
	}


protected:

	void create_descriptors(std::vector<std::shared_ptr<Descriptor>> descriptors)
	{
		m_Descriptors = descriptors;
	}

	void create_layouts(std::vector<std::shared_ptr<PipelineLayout>> pipelineLayouts)
	{
		m_PipelineLayouts = pipelineLayouts;
	}

	void create_pipelines(std::vector<std::shared_ptr<Pipeline>> pipelines)
	{
		m_Pipelines = pipelines;
	}

	void create_textures(std::vector<std::shared_ptr<Texture>> textures)
	{
		this->m_Textures = textures;
	}

	void create_mesh(std::vector<std::shared_ptr<MeshWrapper>> mesh)
	{
		m_Mesh = mesh;

	}

	void create_particles(std::vector<ParticleCreateStruct> particles)
	{
		//m_Particles.resize(particles.size());
		//for (int i = 0; i < particles.size(); i++)
		//{
		//	particles[i].meshWrapper.pipeline = &m_Pipelines[particles[i].meshWrapper.PipelineIndex];
		//	m_Particles[i].particlesMesh.resize(particles[i].particleNumber, particles[i].meshWrapper);

		//}
	}


	std::shared_ptr<Pipeline> m_ComputePipeline;
	std::shared_ptr<Pipeline> m_ComputeGraphicsPipeline;

private:


	std::string m_DebugName;
	std::vector<std::shared_ptr<Descriptor>> m_Descriptors;
//	std::vector<DescriptorSetLayout> m_Layouts;
	
	std::vector<std::shared_ptr<PipelineLayout>> m_PipelineLayouts;
	std::vector<std::shared_ptr<Pipeline>> m_Pipelines;


	std::vector<std::shared_ptr<MeshWrapper>> m_Mesh;
	std::vector<Particles> m_Particles;
	std::vector<std::shared_ptr<Texture>> m_Textures;
};

