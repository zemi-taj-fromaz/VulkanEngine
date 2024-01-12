#include <optional>
#include <EntryPoint.h>
#include <iostream>

#include "BufferUpdateFunctions.h"
#include "ParticlesCreationFunctions.h"
#include "BufferUpdateObjects.h"

class ExampleLayer : public Layer
{
public:
	ExampleLayer() : Layer("Example")
	{

		//------------------------------ DESCRIPTORS ---------------------------------------------------

		auto camera = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(CameraBufferObject), Functions::cameraUpdateFunc);
		//auto scene = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT,   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(SceneData), Functions::sceneUpdateFunc);
		auto objects = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 3, sizeof(ObjectData) * 1000, Functions::objectsUpdateFunc);
		auto pointLights = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 3, sizeof(PointLight) * 20, Functions::pointLightsUpdateFunc);
		auto flashLights = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 3, sizeof(FlashLight) * 20, Functions::flashLightsUpdateFunc);
		auto globalLight = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(GlobalLight), Functions::globalLightUpdateFunc);
		auto resolution = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(Resolution), Functions::resolutionUpdateFunc);
		auto sampler = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		//auto albedo = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		//auto normals = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		//auto metallic = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		//auto roughness = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto deltaTime = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT), sizeof(float), Functions::deltaTimeUpdateFunc);
		auto totalTime = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float), Functions::totalTimeUpdateFunc);

		//auto ssboIn = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT), 3, sizeof(Particle) * 10e6,
		//	Functions::particlesUpdateFunc);
		//auto ssboOut = std::make_shared<Descriptor>(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT), 3, sizeof(Particle) * 10e6,
		//	Functions::particlesUpdateFunc);

	//	ssboIn->tie = ssboOut.get();

	//	ssboOut->particlesCreateFunction = Functions::mandelbulb;

		create_descriptors({ camera,objects, resolution, sampler, totalTime, pointLights, flashLights, globalLight //}, albedo, normals, metallic, roughness
	});// , deltaTime, ssboIn, ssboOut


		//------------------------------ PIPELINE LAYOUTS ---------------------------------------------------


		using TopoloG = std::vector<std::shared_ptr<Descriptor>>;
		TopoloG topology1({ camera, objects, sampler });
		TopoloG topology2({ camera, objects });
		TopoloG topology5({ camera, sampler });
		TopoloG topology6({ camera, objects, resolution, totalTime });
		TopoloG topology7({ camera, objects, pointLights, flashLights, globalLight });
		TopoloG topology8({ camera, objects, pointLights, flashLights, globalLight, sampler });

		TopoloG PBRtopology({ camera, objects, pointLights, flashLights, globalLight //}, albedo, normals, metallic, roughness
	});

		//	TopoloG topologyCompute({ deltaTime, ssboIn, ssboOut });

		TopoloG topologyComputeGraphics({ camera });

		TopoloG topologyMT({ objects, resolution });
		//Topoogy topologyEmpty({});

		auto pipelineLayout1 = std::make_shared<PipelineLayout>(topology1);
		auto pipelineLayout2 = std::make_shared<PipelineLayout>(topology2);
		auto pipelineLayout3 = std::make_shared<PipelineLayout>(topology2);
		auto pipelineLayout4 = std::make_shared<PipelineLayout>(topology1);
		auto pipelineLayout5 = std::make_shared<PipelineLayout>(topology5);
		auto pipelineLayout6 = std::make_shared<PipelineLayout>(topology6);
		auto pipelineLayout7 = std::make_shared<PipelineLayout>(topology7);
		auto pipelineLayout8 = std::make_shared<PipelineLayout>(topology8);

		auto pipelineLayoutPBR = std::make_shared<PipelineLayout>(PBRtopology);

		//auto pipelineLayoutCompute = std::make_shared<PipelineLayout>(topologyCompute);
		auto pipelineLayoutGraphics = std::make_shared<PipelineLayout>(topologyComputeGraphics);

		auto layoutMT = std::make_shared<PipelineLayout>(topologyMT);

		create_layouts({layoutMT, pipelineLayout1, pipelineLayout2 , pipelineLayout3 , pipelineLayout4 , pipelineLayout5, pipelineLayout6, pipelineLayout7, pipelineLayout8,  pipelineLayoutPBR, pipelineLayoutGraphics });// , pipelineLayoutCompute, pipelineLayoutGraphics

		//------------------------------- SHADERS ------------------------------------------------------------------

		std::vector<std::string> textureShaderNames({ "TextureShader.vert", "TextureShader.frag" });
		std::vector<std::string> plainShaderNames({ "PlainShader.vert", "PlainShader.frag" });
		std::vector<std::string> illuminateShaderNames({ "IlluminateShader.vert", "IlluminateShader.frag" });
		std::vector<std::string> skyboxShaderNames({ "SkyboxShader.vert", "SkyboxShader.frag" });
		std::vector<std::string> particleShaderNames({ "ParticleShader.vert", "ParticleShader.frag" });
		std::vector<std::string> cubemapShaderNames({ "CubemapShader.vert", "CubemapShader.frag" });
		std::vector<std::string> computeShaderName({ "ComputeShader.comp" });
		std::vector<std::string> computeShaderNames({ "ComputeShader.vert", "ComputeShader.frag" });
		std::vector<std::string> mandelbulbShaderNames({ "MandelbulbShader.vert", "MandelbulbShader.frag" });
		std::vector<std::string> aimShaderNames({ "AimShader.vert", "AimShader.frag" });
		//std::vector<std::string> PBRShaderNames({ "PBRShader.vert", "PBRShader.frag" });

		//------------------------------- PIPELINES ----------------------------------------------------------

		auto texturedPipeline = std::make_shared<Pipeline>(pipelineLayout1, textureShaderNames);
		auto plainPipeline = std::make_shared<Pipeline>(pipelineLayout2, plainShaderNames);
		auto illuminatePipeline = std::make_shared<Pipeline>(pipelineLayout8, illuminateShaderNames);
		auto skyboxPipeline = std::make_shared<Pipeline>(pipelineLayout5, skyboxShaderNames, VK_FALSE, true, VK_CULL_MODE_FRONT_BIT);
		auto particlesPipeline = std::make_shared<Pipeline>(pipelineLayout1, particleShaderNames, VK_TRUE, false, VK_CULL_MODE_NONE);
		auto cubemapPipeline = std::make_shared<Pipeline>(pipelineLayout8, cubemapShaderNames);
		auto mandelbulbPipeline = std::make_shared<Pipeline>(pipelineLayout6, mandelbulbShaderNames);
		auto aimPipeline = std::make_shared<Pipeline>(layoutMT, aimShaderNames, VK_TRUE, false, VK_CULL_MODE_NONE);
		aimPipeline->Topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

		auto linePipeline = std::make_shared<Pipeline>(pipelineLayout2, plainShaderNames, VK_TRUE, false, VK_CULL_MODE_NONE);
		linePipeline->Topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

	//	auto PBRPipeline = std::make_shared<Pipeline>(pipelineLayoutPBR, PBRShaderNames);

		//	this->m_ComputePipeline = std::make_shared<Pipeline>(pipelineLayoutCompute, computeShaderName);
		//	this->m_ComputeGraphicsPipeline = std::make_shared<Pipeline>(pipelineLayoutGraphics, computeShaderNames, VK_TRUE, false, VK_CULL_MODE_NONE);
		//	this->m_ComputeGraphicsPipeline->PolygonMode = VK_POLYGON_MODE_POINT;
		//	this->m_ComputeGraphicsPipeline->Topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		create_pipelines({ aimPipeline, texturedPipeline, plainPipeline, illuminatePipeline, skyboxPipeline, particlesPipeline, cubemapPipeline, mandelbulbPipeline, linePipeline// , PBRPipeline
	});//, m_ComputePipeline, m_ComputeGraphicsPipeline});

		//------------------------- TEXTURES ---------------------------------------------------------------

		std::shared_ptr<Texture> deerTex = std::make_shared<Texture>("deerAI.jpg");
		std::shared_ptr<Texture> skyboxTex = std::make_shared<Texture>("stormydays/");
		std::shared_ptr<Texture> jetTex = std::make_shared<Texture>("BODYMAINCOLORCG.png");
		std::shared_ptr<Texture> smokeTex = std::make_shared<Texture>("statue.jpg");
		std::shared_ptr<Texture> woodboxTex = std::make_shared<Texture>("wood/");


		std::shared_ptr<Texture> rustAlbedoTex = std::make_shared<Texture>("rustAlbedo.png");
		std::shared_ptr<Texture> rustNormalsTex = std::make_shared<Texture>("rustNormals.png");
		std::shared_ptr<Texture> rustMetallicTex = std::make_shared<Texture>("rustMetallic.png");
		std::shared_ptr<Texture> rustRoughnessTex = std::make_shared<Texture>("rustRoughness.png");

		create_textures({ skyboxTex, jetTex, smokeTex, deerTex, woodboxTex //}, rustAlbedoTex, rustNormalsTex, rustMetallicTex, rustRoughnessTex
	});

		//----------------------- MESH --------------------------------------------------------------------

		Mesh jetMesh("fighter_jet.obj");
		Mesh box("skybox.obj");
		Mesh particle("texture - Copy.obj");
		Mesh catMesh("cat.obj");
		Mesh square("texture - Copy.obj");
		Mesh sphere("sphere.OBJ");

		Mesh realSphere(MeshType::Sphere);
		Mesh cube(MeshType::Cube);
		Mesh quadMesh(MeshType::Quad);
		Mesh lineMesh(MeshType::Line);

		Mesh aim1(MeshType::Line);
		Mesh aim2(MeshType::Line);

		//-------------------- LIGHTS -----------------------------------------------------------------------

		auto pointLight = std::make_shared<LightProperties>(LightType::PointLight, glm::vec3(0.0f, 1.0f, 0.0f));
		auto flashLight = std::make_shared<LightProperties>(LightType::FlashLight, glm::vec3(1.0f, 0.0f, 0.0f));

		auto cameraFlash = std::make_shared<LightProperties>(LightType::FlashLight, glm::vec3(0.8f, 0.8f, 0.2f));
		cameraFlash->update_light = [this](float time, const glm::vec3& camera_position, Renderable* renderable)
		{
			auto Position = renderable->get_position();
			auto Direction = renderable->get_direction();
			Position = this->m_Camera.Position;
			Direction = this->m_Camera.direction;

		};
		cameraFlash->CLQ = glm::vec3(1.0, 0.07, 0.017);

		this->m_Camera = Camera();
		m_Camera.cameraLight = cameraFlash;
		m_Camera.Position = glm::vec3(0.0f, 0.0f, 50.0f);

		auto globalLighter = std::make_shared<LightProperties>(LightType::GlobalLight, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, -1.0f));

		//-------------------- MESH WRAPPERS ----------------------------------------------------------------


		auto jet = std::make_shared<MeshWrapper>(texturedPipeline, jetMesh);
		jet->textures.push_back(jetTex);
		jet->animated = "spiral.txt";
		jet->scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));

		auto skybox = std::make_shared<MeshWrapper>(skyboxPipeline, box);
		skybox->textures.push_back(skyboxTex);
		skybox->isSkybox = true;

		auto target = std::make_shared<MeshWrapper>(illuminatePipeline, box);
		target->scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
		target->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		target->illuminated = true;

		auto woodbox = std::make_shared<MeshWrapper>(cubemapPipeline, cube);
		woodbox->textures.push_back(woodboxTex);
		woodbox->illuminated = true;
	//	woodbox->scale = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));

		auto smoke = std::make_shared<MeshWrapper>(particlesPipeline, particle);
		smoke->textures.push_back(smokeTex);
		//smoke->scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 20.0f, 20.0f));
		smoke->head = jet;

		auto cat = std::make_shared<MeshWrapper>(illuminatePipeline, catMesh);
		//	cat->color = glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);
		cat->illuminated = true;
		cat->scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		//	cat->textures.push_back(skyboxTex

		//auto quad1 = std::make_shared<MeshWrapper>(texturedPipeline, square);
		//quad1->textures.push_back(deerTex);
		//quad1->translation = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f));

		//quad1->Billboard = true;



	//	quad2->Billboard = true;


		auto aimX = std::make_shared<MeshWrapper>(aimPipeline, aim1);
	//	aimX->translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
		aimX->scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 1.0f));
		aimX->rotation = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		auto aimY = std::make_shared<MeshWrapper>(aimPipeline, aim2);
		//aimY->translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		aimY->scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 1.0f));
		aimY->rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));



		std::vector<std::shared_ptr<MeshWrapper>> meshWrappers;
		//meshWrappers.push_back(sun);
	//	meshWrappers.push_back(moon);
		//meshWrappers.push_back(globe);

		//meshWrappers.push_back(jet);
		meshWrappers.push_back(skybox);
		//meshWrappers.push_back(quad1);
		meshWrappers.push_back(aimX);
		meshWrappers.push_back(aimY);
		//meshWrappers.push_back(woodbox);

		std::random_device rd;
		std::mt19937 gen(rd());
		// Generate a random angle in radians
		std::uniform_real_distribution<> angleDistribution(0.0, 360.0f);

		// Generate a random distance within the specified range
		std::uniform_real_distribution<> distanceDistribution(50.0f, 150.0f);



		for (int i = 0; i < 10; i++)
		{
			double angle = angleDistribution(gen);
			double distance = distanceDistribution(gen);
			
			double xOffset = distance * cos(angle);
			double zOffset = distance * sin(angle);

			glm::vec3 offset = m_Camera.Position + glm::vec3(xOffset, 0.0f, zOffset);

			auto deer = std::make_shared<MeshWrapper>(texturedPipeline, quadMesh);
			deer->textures.push_back(deerTex);
			deer->translation = glm::translate(glm::mat4(1.0f), offset);
			deer->Attacker = true;

			meshWrappers.push_back(deer);
			//meshWrappers.push_back(smoke);
		}
		create_mesh(meshWrappers);

		//m_Camera = Camera();
		//m_Camera.mesh = jet;

	}

	virtual bool poll_inputs(GLFWwindow* window, float deltaTime) override
	{
		float cameraSpeed = 20.0f;

		static float recall{ 1.0f };

		recall += deltaTime;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			std::cout << "Scoped in" << std::endl;

			auto app = reinterpret_cast<AppVulkanImpl*>(glfwGetWindowUserPointer(window));
			app->set_field_of_view(45.0f);
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			std::cout << "Gun fired" << std::endl;
			if (recall >= 1.0f)
			{
				auto app = reinterpret_cast<AppVulkanImpl*>(glfwGetWindowUserPointer(window));
				app->fire_gun();
				recall = 0.0f;
			}

			//app->set_field_of_view(45.0f);
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		{
			auto app = reinterpret_cast<AppVulkanImpl*>(glfwGetWindowUserPointer(window));
			app->set_field_of_view(-45.0f);
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			m_Camera.update_position(cameraSpeed * deltaTime * m_Camera.Front);
		}


		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			m_Camera.update_position(cameraSpeed * deltaTime * m_Camera.Right);
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			m_Camera.update_position(-cameraSpeed * deltaTime * m_Camera.Front);
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			m_Camera.update_position(-cameraSpeed * deltaTime * m_Camera.Right);
		}


		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			return false;
		}

		return true;
	}

	virtual void timed_action(GLFWwindow* window) override
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		// Generate a random angle in radians
		static std::uniform_real_distribution<> angleDistribution(0.0, 360.0f);

		// Generate a random distance within the specified range
		static std::uniform_real_distribution<> distanceDistribution(50.0f, 150.0f);

		static Mesh quadMesh(MeshType::Quad);

		for (int i = 0; i < 3; i++)
		{
			double angle = angleDistribution(gen);
			double distance = distanceDistribution(gen);

			double xOffset = distance * cos(angle);
			double zOffset = distance * sin(angle);

			glm::vec3 offset = m_Camera.Position + glm::vec3(xOffset, 0.0f, zOffset);

			auto deer = std::make_shared<MeshWrapper>(m_DeerPipeline, quadMesh);
			deer->textures.push_back(m_DeerTex);
			deer->translation = glm::translate(glm::mat4(1.0f), offset);
			deer->Attacker = true;

			auto app = reinterpret_cast<AppVulkanImpl*>(glfwGetWindowUserPointer(window));

			app->create_mesh(*deer);

			m_Mesh.push_back(deer);
			auto renderObj = std::shared_ptr<RenderObject>(new RenderObject(m_Mesh[m_Mesh.size()-1].get()));
			renderObj->get_mesh()->object = renderObj;
			auto& renderables = app->get_renderables();
			auto& attacker = app->get_attackers();
			renderables.push_back(renderObj);
			if (renderObj->get_mesh()->Attacker) attacker.push_back(renderObj);
			//meshWrappers.push_back(smoke);
		}
	}

private:

};


class MyApplication : public Application
{
public:
	MyApplication()
	{
		std::shared_ptr<Layer> layer = std::shared_ptr<Layer>(new ExampleLayer());
		push_layer(layer);
	}
};

std::unique_ptr<Application> CreateApplication()
{
	return std::unique_ptr<Application>(new MyApplication());
}

