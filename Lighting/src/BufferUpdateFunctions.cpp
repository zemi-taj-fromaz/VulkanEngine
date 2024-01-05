#include "BufferUpdateFunctions.h"

namespace Functions
{
	std::function<void(AppVulkanImpl* app, void* bufferMapped)> cameraUpdateFunc = [](AppVulkanImpl* app, void* bufferMapped)
	{
		CameraBufferObject cbo{};
		Camera camera = app->get_camera();
		auto swapChainExtent = app->get_swapchain_extent();
		cbo.view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
		cbo.proj = glm::perspective(glm::radians(camera.Fov), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 500.0f);
		cbo.proj[1][1] *= -1;
		cbo.pos = glm::vec4(camera.Position, 1.0f);
		memcpy(bufferMapped, &cbo, sizeof(cbo));
	};

	std::function<void(AppVulkanImpl* app, void* bufferMapped)> sceneUpdateFunc = [](AppVulkanImpl* app, void* bufferMapped)
	{
		static int frameNumber = 0;
		frameNumber++;
		float framed = (frameNumber / 120.f);

		auto deltaTime = app->get_delta_time();

		auto sunPosition = glm::vec4(0.f, 0.f, 0.f, 1.f); // app->get_light_position();
		auto sunColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		SceneData sceneData{};
		sceneData.ambientColor = { sin(framed),0,cos(framed),1 };
		sceneData.ambientColor = { 0.2, 0.2, 0.2, 1.0 };
		sceneData.sunlightColor = sunColor;
		
		sceneData.sunPosition = sunPosition;// *glm::vec4(sunPosition, 1.0f);

		//sceneData.sunlightColor = { 1.0, 1.0, 0.2, 1.0 };

		//static const float rotationSpeed = static_cast<float>(glm::two_pi<float>()) / 10.0f; // Radians per second for a full rotation in 10 seconds

		//float angle = rotationSpeed * deltaTime;
		//glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

		//sceneData.sunPosition = rotationMatrix * sceneData.sunPosition;

		memcpy(bufferMapped, &sceneData, sizeof(SceneData));
	};

	std::function<void(AppVulkanImpl* app, void* bufferMapped)> objectsUpdateFunc = [](AppVulkanImpl* app, void* bufferMapped)
	{
		ObjectData* objectArray = (ObjectData*)bufferMapped;
		auto& renderables = app->get_renderables();
		const Camera& camera = app->get_camera();
		auto deltaTime = app->get_delta_time();
		auto time = app->get_total_time();

		for (size_t i = 0; i < renderables.size(); i++)
		{
			if (renderables[i]->is_animated())
			{
				renderables[i]->compute_animation(time);
			}
			if (renderables[i]->is_billboard())
			{
				renderables[i]->update_billboard(camera.Position);
			}

			//if (renderables[i]->is_light_source())
			//{
			//	renderables[i]->update_light_source(deltaTime);
			//	error

			//}
			renderables[i]->update(deltaTime, camera.Position);

			objectArray[i].Model = renderables[i]->get_model_matrix();
			objectArray[i].Color = renderables[i]->get_color();
		}
	};

	std::function<void(AppVulkanImpl* app, void* bufferMapped)> particlesUpdateFunc = [](AppVulkanImpl* app, void* bufferMapped)
	{

		auto deltaTime = app->get_delta_time();
		auto time = app->get_total_time();
		auto particlesSize = app->get_particles_size();

		Particle* particles = (Particle*)bufferMapped;

		for (size_t i = 0; i < particlesSize; i++)
		{
			particles[i].position += glm::vec3(particles[i].velocity, 0.0f) * deltaTime;
			particles[i].color = particles[i].color;

			if (particles[i].color.x > 0.8f)
			{
				continue;
			}
		}

	};

	std::function<void(AppVulkanImpl* app, void* bufferMapped)> deltaTimeUpdateFunc = [](AppVulkanImpl* app, void* bufferMapped)
	{
		auto deltaTime = app->get_delta_time();
		ParameterUBO ubo{};
		ubo.deltaTime = deltaTime;
		memcpy(bufferMapped, &ubo, sizeof(ParameterUBO));
	};

	std::function<void(AppVulkanImpl* app, void* bufferMapped)> totalTimeUpdateFunc = [](AppVulkanImpl* app, void* bufferMapped)
	{
		auto totalTime = app->get_total_time();
		ParameterUBO ubo{};
		ubo.deltaTime = totalTime;
		memcpy(bufferMapped, &ubo, sizeof(ParameterUBO));
	};

	std::function<void(AppVulkanImpl* app, void* bufferMapped)> resolutionUpdateFunc = [](AppVulkanImpl* app, void* bufferMapped)
	{
		Resolution ubo{};
		ubo.resolution = app->get_resolution();

		memcpy(bufferMapped, &ubo, sizeof(Resolution));
	};

	std::function<void(AppVulkanImpl* app, void* bufferMapped)> pointLightsUpdateFunc = [](AppVulkanImpl* app, void* bufferMapped)
	{

		PointLight* pointLightsArray = (PointLight*)bufferMapped;
		auto& pointLights = app->get_point_lights();


		for(int i = 0; i < pointLights.size(); i++)
		{
			//pointLights[i]->update(deltaTime, camera.Position);
			auto& lightProperties = pointLights[i]->get_mesh()->lightProperties;

			pointLightsArray[i].position =		glm::vec4(pointLights[i]->get_position(), 1.0f);
			pointLightsArray[i].ambientColor =	glm::vec4(lightProperties->ambientLight, 1.0f);
			pointLightsArray[i].diffColor =		glm::vec4(lightProperties->diffuseLight, 1.0f);
			pointLightsArray[i].specColor =		glm::vec4(lightProperties->specularLight, 1.0f);
			pointLightsArray[i].clq =			glm::vec4(lightProperties->CLQ, 1.0f);

			pointLightsArray[i].size = pointLights.size();

		}
	};


	std::function<void(AppVulkanImpl* app, void* bufferMapped)> flashLightsUpdateFunc = [](AppVulkanImpl* app, void* bufferMapped)
	{
		
		FlashLight* flashLightsArray = (FlashLight*)bufferMapped;
		auto& flashLights = app->get_flash_lights();

		for (int i = 0; i < flashLights.size(); i++)
		{
			//pointLights[i]->update(deltaTime, camera.Position);
			auto& lightProperties = flashLights[i]->get_mesh()->lightProperties;

			flashLightsArray[i].position = glm::vec4(flashLights[i]->get_position(), 1.0f);
			flashLightsArray[i].ambientColor = glm::vec4(lightProperties->ambientLight, 1.0f);
			flashLightsArray[i].diffColor = glm::vec4(lightProperties->diffuseLight, 1.0f);
			flashLightsArray[i].specColor = glm::vec4(lightProperties->specularLight, 1.0f);
			flashLightsArray[i].clq = glm::vec4(lightProperties->CLQ, 1.0f);

			flashLightsArray[i].direction = glm::vec4(flashLights[i]->get_direction(), 1.0f);
			flashLightsArray[i].innerCutoff = lightProperties->innerCutoff;
			flashLightsArray[i].outerCutoff = lightProperties->outerCutoff;

			flashLightsArray[i].size = flashLights.size();

		}
	};


}