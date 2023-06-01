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

typedef struct {
	float fieldOfView;
	float aspect;
	float nearPlane;
	float farPlane;
}perspectiveData;

perspectiveData pD;

int main() {

	VkApplication::MainVulkApplication*  vkApp_ = VkApplication::MainVulkApplication::GetInstance();
	vkApp_->setup("Vulkan Template App");
	vkApp_->run();

	return 0;
}