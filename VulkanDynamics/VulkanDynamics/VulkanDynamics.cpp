// VulkanDynamics.cpp
//
#include "VulkanDynamics.hpp"
#include "InputHandler.h"

using std::cout;
using std::endl;

typedef struct {
	float fieldOfView;
	float aspect;
	float nearPlane;
	float farPlane;
}perspectiveData;

perspectiveData pD;
 
// storage for matrices
float projMatrix[16];
glm::mat4 viewMatrix;
glm::mat4 eyeviewMatrix;
glm::vec4 ambientLight(0.1f, 0.1f, 0.1f, 1.0f);
glm::vec4 lightColor ( 0.8f, 0.8f, 0.8f, 1.0f );
glm::vec4 LightPosition ( 0.0f, -5.0f, 0.0f, 1.0f );
float Shininess = 1.1f;
float Strength = 60.0f;
glm::vec4 EyeDirection ( 0.1f,1.0f, -3.0f, 1.0f );
float ConstantAttenuation = 2.0f;
float LinearAttenuation = 0.0f;
float QuadraticAttenuation = 0.0f;
glm::mat4 normalModelViewMatrix;

std::stack<float *> first;
float tempMatrix[16];

glm::vec3 mainEyeLoc(4.0, 4.0, 4.0);
glm::vec3 centerLoc(0.0, 0.0, 0.0);
glm::vec3 up(0.0, 1.0, 0.0);
float fov = glm::radians<float>(90.0f);

//need to load in the value of the uniform buffers for the frag and vert shader + load attributes
void loadInitialVariables(MainVulkApplication & _app) {
	_app.ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	_app.ubo.view = glm::lookAt(mainEyeLoc, centerLoc, up);
	_app.ubo.proj = glm::perspective(fov, _app.swapChainExtent.width / (float)_app.swapChainExtent.height, 0.1f, 9.0f);
	_app.ubo.proj[1][1] *= -1.0f;
	glm::mat3 viewMatrix3x3(_app.ubo.view * _app.ubo.model);
	_app.ubo.normalMatrix = glm::inverseTranspose(viewMatrix3x3);
	_app.ubo.lightPos = LightPosition;

	_app.ufo.Ambient = ambientLight;
	_app.ufo.LightColor = lightColor;
	_app.ufo.Reflectivity = Shininess;
	_app.ufo.Strength = Strength;
	_app.ufo.EyeDirection = glm::vec4 (mainEyeLoc, 1.0f );
	_app.ufo.ConstantAttenuation = ConstantAttenuation;
	_app.ufo.LinearAttenuation = LinearAttenuation;
	_app.ufo.QuadraticAttenuation = QuadraticAttenuation;
	_app.ufo.viewMatrix = glm::inverseTranspose(viewMatrix3x3);;
	_app.ufo.eyeViewMatrix = glm::inverseTranspose(viewMatrix3x3);

	//initialize last one for ground
	for (int i = 0; i <= numberOfSpheres; ++i) {
		glm::mat4* modelMat = (glm::mat4*)(((uint64_t)_app.uboDataDynamic.model + (i * _app.dynamicAlignment)));

		*modelMat = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	}

}

void updateUniformBuffer(MainVulkApplication & _app) {

	for (int i = 0; i < numberOfSpheres; ++i) {
		glm::mat4* modelMat = (glm::mat4*)(((uint64_t)_app.uboDataDynamic.model + (i * _app.dynamicAlignment)));

		*modelMat = glm::rotate(glm::mat4(1.0f), (float)glm::radians(phi), glm::vec3(0.0f, 1.0f, 0.0f));
		*modelMat *= glm::rotate(glm::mat4(1.0f), (float)glm::radians(theta), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	if (changeLightPos[0] == 1) {
		_app.ubo.lightPos.x += lightPositionx;
		changeLightPos[0] = 0;
	}
	if (changeLightPos[1] == 1) {
		_app.ubo.lightPos.y += lightPositiony;
		changeLightPos[1] = 0;
	}
	if (changeLightPos[2] == 1) {
		_app.ubo.lightPos.z += lightPositionz;
		changeLightPos[2] = 0;
	}
}

void mainLoop(MainVulkApplication & _app) {

	glfwSetKeyCallback(_app.window, readInput_callback);
	glfwSetCursorPosCallback(_app.window, mouse_cursor_callback);
	glfwSetMouseButtonCallback(_app.window, mouse_button_callback);

	int WindowRes;

	while (!(WindowRes = glfwWindowShouldClose(_app.window))) {
		glfwPollEvents();
		updateUniformBuffer(_app);
		_app.drawFrame();
	}

	vkDeviceWaitIdle(_app.device);
 }

void solveDynamics() {

	glm::vec3 x1 (0.0, 0.0, 1.0);
	glm::vec3 xc (0.0, 0.0, 0.0);
	glm::vec3 xOrig = x1;

	glm::vec3 force(0.0, 0.2, 0.0);
	glm::vec3 velocity(0.0, 0.0, 0.0);

	glm::vec3 lagrange(0.0f, 0.0f, 0.0f);

	float timeStep = 1.0 / 30.0;

	int iterations = 0;

	while (true) {

		if (iterations > 0) force = glm::vec3(0.0f, 0.0f, 0.0f);
		iterations++;

		glm::vec3 constraint = abs(x1 - xc) - glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 dotConstraint = (x1 - xc) / (abs(x1 - xc));
		if (abs(x1 - xc).x == 0.0) dotConstraint.x = 0.0 ;
		if (abs(x1 - xc).y == 0.0) dotConstraint.y = 0.0;
		if (abs(x1 - xc).z == 0.0) dotConstraint.z = 0.0;
		glm::vec3 JW = dotConstraint * glm::mat3x3(1.0);

		float JWJt = JW.x * dotConstraint.x + JW.y * dotConstraint.y + JW.z * dotConstraint.z;

		glm::vec3 dotdotConstraint = (-(x1 * x1) + (2.0f * x1 * xc) - (xc * xc)) / (abs(x1 - xc) * abs(x1 - xc) * abs(x1 - xc));
		if ((abs(x1 - xc) * abs(x1 - xc) * abs(x1 - xc)).x == 0.0) dotdotConstraint.x = 0.0;
		if ((abs(x1 - xc) * abs(x1 - xc) * abs(x1 - xc)).y == 0.0) dotdotConstraint.y = 0.0;
		if ((abs(x1 - xc) * abs(x1 - xc) * abs(x1 - xc)).z == 0.0) dotdotConstraint.z = 0.0;

		//glm::vec3 dotdotConstraint = -( x1 / abs ( x1 - xc ) ) + ( xc / abs ( x1 - xc ) );
		//if ((abs(x1 - xc)).x == 0.0) dotdotConstraint.x = 0.0;
		//if ((abs(x1 - xc)).y == 0.0) dotdotConstraint.y = 0.0;
		//if ((abs(x1 - xc)).z == 0.0) dotdotConstraint.z = 0.0;
		
		float dotJdotQ = dotdotConstraint.x * velocity.x + dotdotConstraint.y * velocity.y + dotdotConstraint.z * velocity.z;
		dotJdotQ = dotJdotQ * -1.0;

		float JWQ = JW.x * force.x + JW.y * force.y + JW.z * force.z;
		JWQ = JWQ * -1.0f;

		lagrange = (dotJdotQ + JWQ - ( 1.0f * constraint) - ( 1.0f * dotConstraint)) / JWJt;

		glm::vec3 constraintForce(lagrange.x * dotConstraint.x, lagrange.y * dotConstraint.y, lagrange.z * dotConstraint.z);
		force += constraintForce;
		force = glm::normalize(force);

		x1.x = x1.x + (velocity.x * timeStep) + (((timeStep * timeStep) * force.x) / 2.0);
		x1.y = x1.y + (velocity.y * timeStep) + (((timeStep * timeStep) * force.y) / 2.0);
		x1.z = x1.z + (velocity.z * timeStep) + (((timeStep * timeStep) * force.z) / 2.0);

		velocity = (x1 - xOrig) / timeStep;
		xOrig = x1;
		velocity = glm::normalize(velocity);

		std::cout << x1.x << " : " << x1.y << " : " << x1.z << std::endl;
	}
}

int main() {

	//solveDynamics();
	MainVulkApplication app;

	try {
		//set_eyeView(5.0f, 5.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f);
		//loadModel(app);
		app.setup();
		loadInitialVariables(app);
		mainLoop(app);
		app.cleanupApp();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}