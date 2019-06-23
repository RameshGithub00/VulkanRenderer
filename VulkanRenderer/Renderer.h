#pragma once

#include "singletons.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "basetypes.h"
#include <vulkan\vulkan.h>
struct LayerProperties 
{
	VkLayerProperties properties;
	GrowingArray<VkExtensionProperties> extensions;
};

class VulkanLayerAndExtensions
{
	friend class VulkanInstance;
public:
	GrowingArray<const char*> instanceExtensioNames;
	GrowingArray<const char*> instanceLayerNames;

	std::vector<LayerProperties> layerPropertiesList;
	//Layers and corresponding extension list
	VkResult getInstanceLayerProperties();

	//global extensions
	VkResult getExtensionProperties(LayerProperties &layerPops, VkPhysicalDevice* gpu = 0);

	//device based extensions
	VkResult getDeviceExtensionProperties(VkPhysicalDevice* gpu);

};


class VulkanInstance
{
	VkInstance m_VKInstance;
	VulkanLayerAndExtensions layerExtensions;
public:	
	VkInstance getVKInstance() { return m_VKInstance; }
	VkResult createInstance(GrowingArray<const char*> &layers, GrowingArray<const char*>& extensions, const char* applicationname);
	void destroyInstance();
};

class VulkanMemoryManager
{
public:

	VulkanMemoryManager(){}
	void Init();
private:
	VkPhysicalDeviceMemoryProperties m_MemoryProperties;
	
};

class VulkanDevice
{
public:
	VulkanDevice(VkPhysicalDevice* gpu);
	~VulkanDevice();

	VkDevice							m_Device;
	VkPhysicalDevice*					m_pGpu;
	VkPhysicalDeviceProperties			m_GpuProperties;
	VkPhysicalDeviceMemoryProperties	m_MemoryProperties;

	VkQueue	queue;
	GrowingArray<VkQueueFamilyProperties> m_QueueFamilProperties;

	u32	m_GraphicsQueueFamilyIndex;
	u32 m_QueueFamilyCount;
	VulkanLayerAndExtensions layerExtensions;

	VkResult CreateDevice(GrowingArray<const char*>& layers= EMPTY_GROWING_ARRAY(const char*), GrowingArray<const char*>&extensions= EMPTY_GROWING_ARRAY(const char*));
	void DestroyDevice();

	void MemoryTypeFromProperties();
	void InitializeDeviceQueue();
	void GetGraphicsQueueHandle();
	void GetPhysicalDeviceQueuesAndProperties();
};

class VulkanApplication
{
	VulkanInstance instance;
	GrowingArray<const char*> layerNames = { "VK_LAYER_LUNARG_api_dump" };
	GrowingArray<const char*> extensionNames = {};// { "VK_KHR_SURFACE_EXTENSION_NAME", "VK_KHR_WIN32_SURFACE_EXTENSION_NAME" };


public:
	VulkanApplication() {}
	~VulkanApplication() {}
	void CreateInstance() { instance.createInstance(layerNames, extensionNames, "HelloVulkan"); }
	void enumertatePhysicalDevice(GrowingArray<VkPhysicalDevice>& gpuList);
};

class Renderer : public Singleton<Renderer>
{
	VulkanDevice* m_LogicalDevice;
public:
	GrowingArray<VkPhysicalDevice> gpuList;
	const GrowingArray<VkPhysicalDevice>& getGpuList() { return gpuList; }
	void init();

};

class CommandBufferManager : public Singleton<CommandBufferManager>
{
public:
	static void allocCommandBuffer(const VkDevice* device, const VkCommandPool cmdPool, VkCommandBuffer* cmdBuff, const VkCommandBufferAllocateInfo* commandBufferAllocateInfo);
	static void beginCommandBuffer(VkCommandBuffer cmdbuff, VkCommandBufferBeginInfo* inCmdBuffInfo = NULL);
	static void endCommandBuffer(VkCommandBuffer cmdBuff);
	static void submitCommandBuffer(const VkQueue& queue, const VkCommandBuffer* cmdBuff, const VkSubmitInfo* submitInfo = NULL, const VkFence& fence= VK_NULL_HANDLE);
};

