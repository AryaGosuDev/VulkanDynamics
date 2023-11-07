// VulkanDynamics.cpp
//
#include "VulkanTemplate.hpp"
#include "InputHandler.h"

using std::cout;
using std::endl;

VkApplication::MainVulkApplication* VkApplication::MainVulkApplication::pinstance_ = NULL;

VkApplication::MainVulkApplication* VkApplication::MainVulkApplication::GetInstance() {
	if (pinstance_ == nullptr)
		pinstance_ = new MainVulkApplication();
	return pinstance_;
}

namespace VkApplication {

	struct perspectiveData {
		float fieldOfView;
		float aspect;
		float nearPlane;
		float farPlane;
	};

	perspectiveData pD;

	// storage for matrices
	glm::mat4 viewMatrix;
	glm::mat4 eyeviewMatrix;
	glm::vec4 ambientLight(0.1f, 0.1f, 0.1f, 1.0f);
	glm::vec4 lightColor(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 LightPosition(0.0f, 5.0f, 0.0f, 1.0f);
	float Shininess = 1.1f;
	float Strength = 60.0f;
	glm::vec4 EyeDirection(0.1f, 1.0f, -3.0f, 1.0f);
	float ConstantAttenuation = 2.0f;
	float LinearAttenuation = 0.0f;
	float QuadraticAttenuation = 0.0f;
	glm::mat4 normalModelViewMatrix;

	glm::vec3 mainEyeLoc(6.0, 0.0, 0.0);
	glm::vec3 centerLoc(0.0, 0.0, 0.0);
	glm::vec3 up(0.0, 1.0, 0.0);
	float fov = glm::radians<float>(90.0f);
	glm::mat4 copyView;
	glm::vec3 spindleAxis = glm::cross(mainEyeLoc, up);

	//need to load in the value of the uniform buffers for the frag and vert shader + load attributes
	void loadInitialVariables(VkApplication::MainVulkApplication* _app) {
		_app->ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		_app->ubo.view = glm::lookAt(mainEyeLoc, centerLoc, up);
		copyView = _app->ubo.view;
		_app->ubo.proj = glm::perspective(fov, _app->swapChainExtent.width / (float)_app->swapChainExtent.height, 0.1f, 20.0f);
		_app->ubo.proj[1][1] *= -1.0f;
		glm::mat3 viewMatrix3x3(_app->ubo.view * _app->ubo.model);
		_app->ubo.normalMatrix = glm::inverseTranspose(viewMatrix3x3);
		_app->ubo.lightPos = LightPosition;

		_app->ufo.Ambient = ambientLight;
		_app->ufo.LightColor = lightColor;
		_app->ufo.Reflectivity = Shininess;
		_app->ufo.Strength = Strength;
		_app->ufo.EyeDirection = glm::vec4(mainEyeLoc, 1.0f);
		_app->ufo.ConstantAttenuation = ConstantAttenuation;
		_app->ufo.LinearAttenuation = LinearAttenuation;
		_app->ufo.QuadraticAttenuation = QuadraticAttenuation;
		_app->ufo.viewMatrix = glm::inverseTranspose(viewMatrix3x3);;
		_app->ufo.eyeViewMatrix = glm::inverseTranspose(viewMatrix3x3);

		_app->ubo_reflect.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		_app->ubo_reflect.view = glm::lookAt(mainEyeLoc, centerLoc, up);
		copyView = _app->ubo.view;
		_app->ubo_reflect.proj = glm::perspective(fov, _app->swapChainExtent.width / (float)_app->swapChainExtent.height, 0.1f, 20.0f);
		_app->ubo_reflect.proj[1][1] *= -1.0f;
		viewMatrix3x3 = (_app->ubo.view * _app->ubo.model);
		_app->ubo_reflect.normalMatrix = glm::inverseTranspose(viewMatrix3x3);
		_app->ubo_reflect.lightPos = LightPosition;

		_app->ufo_reflect.Ambient = ambientLight;
		_app->ufo_reflect.LightColor = lightColor;
		_app->ufo_reflect.Reflectivity = Shininess;
		_app->ufo_reflect.Strength = Strength;
		_app->ufo_reflect.EyeDirection = glm::vec4(mainEyeLoc, 1.0f);
		_app->ufo_reflect.ConstantAttenuation = ConstantAttenuation;
		_app->ufo_reflect.LinearAttenuation = LinearAttenuation;
		_app->ufo_reflect.QuadraticAttenuation = QuadraticAttenuation;
		_app->ufo_reflect.viewMatrix = glm::inverseTranspose(viewMatrix3x3);;
		_app->ufo_reflect.eyeViewMatrix = glm::inverseTranspose(viewMatrix3x3);
	}
	
	void updateUniformBuffer(VkApplication::MainVulkApplication* _app) {
		if (motionFlying == true) {

			mainEyeLoc.x = 6.0f * sin(theta) * cos(phi);
			mainEyeLoc.y = 6.0f * cos(theta);
			mainEyeLoc.z = 6.0f * sin(theta) * sin(phi);
			_app->ubo.view = glm::lookAt(mainEyeLoc, centerLoc, up);
		}
		
		if (changeLightPos[0] == 1) {
			_app->ubo.lightPos.x += lightPositionx;
			changeLightPos[0] = 0;
		}
		if (changeLightPos[1] == 1) {
			_app->ubo.lightPos.y += lightPositiony;
			changeLightPos[1] = 0;
		}
		if (changeLightPos[2] == 1) {
			_app->ubo.lightPos.z += lightPositionz;
			changeLightPos[2] = 0;
		}
	}

	void updateUniformBuffer_reflect(VkApplication::MainVulkApplication* _app) {
		
		float planeDistance = 5.0f;  // the plane's distance from the origin along the normal
		glm::vec3 v1(5.0, -1.0, -5.0); glm::vec3 v2(5.0, -1.0, 5.0); glm::vec3 v3(5.0, 2.0, -5.0);

		glm::vec3 planeNormal( glm::cross ( v2 - v1, v3 - v1  )); // the plane's normal vector
		planeNormal = glm::normalize(planeNormal);

		// compute the reflection of cameraPos and lookAt across the plane
		glm::vec3 reflectedCameraPos = mainEyeLoc - 2.0f * (glm::dot(mainEyeLoc, planeNormal) + planeDistance) * planeNormal;
		glm::vec3 reflectedLookAt = centerLoc - 2.0f * (glm::dot(centerLoc, planeNormal) + planeDistance) * planeNormal;

		// compute the reflected view matrix
		_app->ubo_reflect.view = glm::lookAt(reflectedCameraPos, reflectedLookAt, up);
		_app->ubo_reflect.lightPos = _app->ubo.lightPos;
	}

	void mainLoop(VkApplication::MainVulkApplication* _app) {

		glfwSetKeyCallback(_app->window, readInput_callback);
		glfwSetCursorPosCallback(_app->window, mouse_cursor_callback);
		glfwSetMouseButtonCallback(_app->window, mouse_button_callback);

		int WindowRes;

		while (!(WindowRes = glfwWindowShouldClose(_app->window))) {
			glfwPollEvents();
			updateUniformBuffer(_app);
			updateUniformBuffer_reflect(_app);
			_app->drawFrameReflect();
			_app->drawFrameCube();
			//if (motionMode == 1) _app->drawObjectPick(pointx, pointy);
		}
		vkDeviceWaitIdle(_app->device);
	}
}

int main() {

	VkApplication::MainVulkApplication* vkApp_ = VkApplication::MainVulkApplication::GetInstance();

	try {
		vkApp_->setup();
		loadInitialVariables(vkApp_);
		mainLoop(vkApp_);
		vkApp_->cleanupApp();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}