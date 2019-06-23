#include "Renderer.h"
	
#include <iostream>

using namespace std;

VkResult VulkanLayerAndExtensions::getExtensionProperties(LayerProperties & layerProps, VkPhysicalDevice * gpu)
{
	u32 extensioncount;
	VkResult result;
	
	char *layerName = layerProps.properties.layerName;

	do {

		if (gpu)
		{
			result = vkEnumerateDeviceExtensionProperties(*gpu, layerName, &extensioncount, NULL);
		}
		else
		{
			result = vkEnumerateInstanceExtensionProperties(layerName, &extensioncount, NULL);
		}

		if (result == VK_NOT_READY || extensioncount == 0)
			continue;

		layerProps.extensions.resize(extensioncount);

		if (gpu)
		{
			result = vkEnumerateDeviceExtensionProperties(*gpu, layerName, &extensioncount, layerProps.extensions.data());
		}
		else
		{
			result = vkEnumerateInstanceExtensionProperties( layerName, &extensioncount, layerProps.extensions.data());
		}

	} while (result == VK_INCOMPLETE);

	return result;

}

VkResult VulkanLayerAndExtensions::getDeviceExtensionProperties(VkPhysicalDevice * gpu)
{
	VkResult result;

	cout << "\n Device Extensions\n";

	return VkResult();
}


VkResult VulkanLayerAndExtensions::getInstanceLayerProperties()
{
	uint32_t instanceLayerCount;
	GrowingArray<VkLayerProperties> layerProperties;
	VkResult result;
	do {
		result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
		if (result == VK_NOT_READY)
			return result;

		if (instanceLayerCount == 0)
			return VK_INCOMPLETE;//failed

		layerProperties.resize(instanceLayerCount);

		result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties.data());

	} while (result == VK_INCOMPLETE);

	//Query all the extensions for each layer and store it

	cout << "\n Instanced layers" << endl;
	for (auto globallayerproperties : layerProperties)
	{
		cout << "\n " << globallayerproperties.description <<
			"\n\t|\n\t|---[Layer Name]---> " <<
			globallayerproperties.layerName << "\n";


		LayerProperties layerProps;
		layerProps.properties = globallayerproperties;

		result = getExtensionProperties(layerProps);

		if (result == VK_NOT_READY)
			continue;

		layerPropertiesList.push_back(layerProps);

		for (auto extension : layerProps.extensions)
		{
			cout << "\t\t|\n\t\t|---[Layer Extension]---> " << extension.extensionName << "\n";
		}
	}
	return result;
}

VkResult VulkanInstance::createInstance(GrowingArray<const char*>& layers, GrowingArray<const char*>& extensions, const char * applicationname)
{
	layerExtensions.instanceExtensioNames = extensions;
	layerExtensions.instanceLayerNames = layers;

	VkApplicationInfo appinfo = {};
	appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinfo.pNext = NULL;
	appinfo.pApplicationName = applicationname;
	appinfo.applicationVersion = 1;
	appinfo.pEngineName = applicationname;
	appinfo.engineVersion = 1;
	appinfo.apiVersion = VK_API_VERSION_1_0;

	layerExtensions.getInstanceLayerProperties();

	VkInstanceCreateInfo createinfo = {};
	createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createinfo.pNext = NULL;
	createinfo.flags = 0;
	createinfo.pApplicationInfo = &appinfo;
	createinfo.enabledExtensionCount = extensions.size();
	createinfo.ppEnabledExtensionNames = extensions.data();
	createinfo.enabledLayerCount = layers.size();
	createinfo.ppEnabledLayerNames = layers.data();

	VkResult result = vkCreateInstance(&createinfo, NULL, &m_VKInstance);

	assert(result == VK_SUCCESS, result);

	return result;
}

void VulkanApplication::enumertatePhysicalDevice(GrowingArray<VkPhysicalDevice>& gpuList)
{
	u32 gpuDeviceCount;
	vkEnumeratePhysicalDevices(instance.getVKInstance(), &gpuDeviceCount, NULL);
	gpuList.resize(gpuDeviceCount);
	vkEnumeratePhysicalDevices(instance.getVKInstance(), &gpuDeviceCount, gpuList.data());
}

void VulkanInstance::destroyInstance()
{
	vkDestroyInstance(m_VKInstance,0);
}

//vulkan device
VulkanDevice::VulkanDevice(VkPhysicalDevice* gpu)
{
	m_pGpu = gpu;
}

VulkanDevice::~VulkanDevice()
{
}

VkResult VulkanDevice::CreateDevice(GrowingArray<const char*>& layers, GrowingArray<const char*>&extensions)
{
	VkResult result;
	float queuePriorities[1] = { 0.0f };


	vkGetPhysicalDeviceQueueFamilyProperties(*m_pGpu, &m_QueueFamilyCount, NULL);
	m_QueueFamilProperties.resize(m_QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*m_pGpu, &m_QueueFamilyCount, m_QueueFamilProperties.data());

	for (int i = 0; i < m_QueueFamilyCount; i++)
	{
		if (m_QueueFamilProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			printf("\nfamily %d is supports VK_QUEUE_GRAPHICS_BIT",i);
		if (m_QueueFamilProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			printf("\nfamily %d is supports VK_QUEUE_COMPUTE_BIT", i);
		if (m_QueueFamilProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			printf("\nfamily %d is supports VK_QUEUE_TRANSFER_BIT", i);
		if (m_QueueFamilProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
			printf("\nfamily %d is supports VK_QUEUE_SPARSE_BINDING_BIT", i);
	}

	//Create the object information
	VkDeviceQueueCreateInfo queueinfo = {};
	queueinfo.queueFamilyIndex = 1;
	queueinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueinfo.pNext = NULL;
	queueinfo.queueCount = 1;
	queueinfo.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo devicecreateinfo = {};
	devicecreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devicecreateinfo.pNext = NULL;
	devicecreateinfo.queueCreateInfoCount = 1;
	devicecreateinfo.pQueueCreateInfos = &queueinfo;

	devicecreateinfo.enabledExtensionCount = extensions.size();
	devicecreateinfo.ppEnabledExtensionNames = extensions.data();
	//device layers are deprecated
	devicecreateinfo.enabledLayerCount = layers.size();
	devicecreateinfo.ppEnabledLayerNames = layers.data();

	result = vkCreateDevice(*m_pGpu, &devicecreateinfo, NULL, &m_Device);

	assert(result == VK_SUCCESS,L"Error Creting Device");

	return result;
}



void Renderer::init()
{
//instance
	VulkanApplication app;
//physical device
	app.CreateInstance();
	app.enumertatePhysicalDevice(gpuList);

	VulkanMemoryManager memManager;
	memManager.Init();
	m_LogicalDevice = new VulkanDevice(&gpuList[0]);
	m_LogicalDevice->CreateDevice();
}

//VulkanMemoryManager
void VulkanMemoryManager::Init()
{
	vkGetPhysicalDeviceMemoryProperties(Renderer::getPtr()->getGpuList()[0], &m_MemoryProperties);

}

void CommandBufferManager::allocCommandBuffer(const VkDevice * device, const VkCommandPool cmdPool, VkCommandBuffer * cmdBuff, const VkCommandBufferAllocateInfo * commandBufferAllocateInfo)
{
	VkResult result;
	if (commandBufferAllocateInfo) 
	{
		result = vkAllocateCommandBuffers(*device, commandBufferAllocateInfo, cmdBuff);
		assert(result == VK_SUCCESS, "vkAllocateCommandBuffers Error");
		return;
	}
	VkCommandBufferAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = 0;
	info.commandPool = cmdPool;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	info.commandBufferCount = u32(sizeof(cmdBuff) / sizeof(VkCommandBuffer));
	result = vkAllocateCommandBuffers(*device, &info, cmdBuff);
}

void CommandBufferManager::beginCommandBuffer(VkCommandBuffer cmdbuff, VkCommandBufferBeginInfo * inCmdBuffInfo)
{
	VkResult result;
	if (inCmdBuffInfo) 
	{
		result = vkBeginCommandBuffer(cmdbuff, inCmdBuffInfo);
		assert(result == VK_SUCCESS, "vkBeginCommandBuffer Error");
		return;
	}
	VkCommandBufferBeginInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = nullptr;
	result = vkBeginCommandBuffer(cmdbuff, &info);
	assert(result == VK_SUCCESS, "vkBeginCommandBuffer Error");
}

void CommandBufferManager::endCommandBuffer(VkCommandBuffer cmdBuff)
{
	VkResult result;
	result = vkEndCommandBuffer(cmdBuff);
	assert(result == VK_SUCCESS, "vkEndCommandBuffer Error");
}

void CommandBufferManager::submitCommandBuffer(const VkQueue & queue, const VkCommandBuffer* cmdBuff, const VkSubmitInfo * inSubmitInfo, const VkFence & fence)
{
	VkResult result;
	if (inSubmitInfo) {
		vkQueueSubmit(queue, 1, inSubmitInfo, fence);
		result = vkQueueWaitIdle(queue);
		return;
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = 0;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = NULL;
	submitInfo.commandBufferCount = (uint32_t)sizeof(*cmdBuff) / sizeof(VkCommandBuffer);
	submitInfo.pCommandBuffers = cmdBuff;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;

	result = vkQueueSubmit(queue, 1, &submitInfo, fence);
	assert(result==VK_SUCCESS,"submitCommandBuffer error");

	result = vkQueueWaitIdle(queue);
	assert(result == VK_SUCCESS, "submitCommandBuffer error");
}
