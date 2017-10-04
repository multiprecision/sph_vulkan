/*
* Copyright (c) 2017, Samuel I. Gunadi
* All rights reserved.
*/

#include "application.hpp"

#include <cmath>
#include <string>
#include <algorithm>
#include <exception>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <thread>

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT obj_type, uint64_t obj, size_t location, int32_t code, const char* layer_prefix, const char* msg, void* user_data)
{
    std::string tags;

	switch (flags)
	{
	case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		tags += "[ERROR]";
		break;
	case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		tags += "[WARNING]";
		break;
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		tags += "[PERFORMANCE]";
		break;
	case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		tags += "[INFORMATION]";
		break;
	case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		tags += "[DEBUG]";
		break;
	default:
		tags += "[]";
	}
    
    switch (obj_type)
    {
        case VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT:
            tags += "[UNKNOWN]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT:
            tags += "[INSTANCE]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT:
            tags += "[PHYSICAL_DEVICE]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT:
            tags += "[DEVICE]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT:
            tags += "[QUEUE]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT:
            tags += "[SEMAPHORE]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT:
            tags += "[COMMAND_BUFFER]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT:
            tags += "[FENCE]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT:
            tags += "[DEVICE_MEMORY]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT:
            tags += "[BUFFER]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT:
            tags += "[IMAGE]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT:
            tags += "[EVENT]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT:
            tags += "[QUERY_POOL]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT:
            tags += "[BUFFER_VIEW]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT:
            tags += "[IMAGE_VIEW]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT:
            tags += "[SHADER_MODULE]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT:
            tags += "[PIPELINE_CACHE]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT:
            tags += "[PIPELINE_LAYOUT]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT:
            tags += "[RENDER_PASS]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT:
            tags += "[PIPELINE]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT:
            tags += "[DESCRIPTOR_SET_LAYOUT]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT:
            tags += "[SAMPLER]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT:
            tags += "[DESCRIPTOR_POOL]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT:
            tags += "[DESCRIPTOR_SET]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT:
            tags += "[FRAMEBUFFER]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT:
            tags += "[COMMAND_POOL]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT:
            tags += "[SURFACE_KHR]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT:
            tags += "[SWAPCHAIN_KHR]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT:
            tags += "[DEBUG_REPORT]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_KHR_EXT:
            tags += "[DISPLAY_KHR]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_MODE_KHR_EXT:
            tags += "[DISPLAY_MODE_KHR]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_OBJECT_TABLE_NVX_EXT:
            tags += "[OBJECT_TABLE_NVX]";
            break;
        case VK_DEBUG_REPORT_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX_EXT:
            tags += "[INDIRECT_COMMANDS_LAYOUT_NVX]";
            break;
        default:
            tags += "[]";
    }

    std::cout << tags << "[" << obj << "][" << location << "][" << code << "][" << layer_prefix << "] " << msg << std::endl;

    fflush(stdout);

    return VK_FALSE;
}

namespace sph
{

application::application()
{
    initialize_window();
    initialize_vulkan();
}

application::~application()
{

    destroy_vulkan();
    destroy_window();
}

void application::destroy_window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void application::destroy_vulkan()
{
	vkDeviceWaitIdle(logical_device_handle);
    // clean up
    vkDestroyFence(logical_device_handle, compute_fence_handle, nullptr);
    vkFreeCommandBuffers(logical_device_handle, compute_command_pool_handle, 1, &compute_command_buffer_handle);
    vkDestroyCommandPool(logical_device_handle, compute_command_pool_handle, nullptr);
    vkDestroyDescriptorSetLayout(logical_device_handle, compute_descriptor_set_layout_handle, nullptr);
    vkDestroyPipelineLayout(logical_device_handle, compute_pipeline_layout_handle, nullptr);
    vkDestroyPipeline(logical_device_handle, compute_pipeline_handles[0], nullptr);
    vkDestroyPipeline(logical_device_handle, compute_pipeline_handles[1], nullptr);
    vkDestroyPipeline(logical_device_handle, compute_pipeline_handles[2], nullptr);
    vkDestroySemaphore(logical_device_handle, render_finished_semaphore_handle, nullptr);
    vkDestroySemaphore(logical_device_handle, image_available_semaphore_handle, nullptr);
    for (auto del : graphics_command_buffer_handles)
    {
        vkFreeCommandBuffers(logical_device_handle, graphics_command_pool_handle, 1, &del);
    }
    vkDestroyCommandPool(logical_device_handle, graphics_command_pool_handle, nullptr);
    vkDestroyPipeline(logical_device_handle, graphics_pipeline_handle, nullptr);
    for (auto del : shader_module_handles)
    {
        vkDestroyShaderModule(logical_device_handle, del, nullptr);
    }
    vkDestroyPipelineLayout(logical_device_handle, graphics_pipeline_layout_handle, nullptr);
    for (auto del : swap_chain_frame_buffer_handles)
    {
        vkDestroyFramebuffer(logical_device_handle, del, nullptr);
    }

    vkDestroyBuffer(logical_device_handle, particle_buffer_handle, nullptr);
    vkFreeMemory(logical_device_handle, particle_buffer_device_memory_handle, nullptr);

    vkDestroyDescriptorPool(logical_device_handle, global_descriptor_pool_handle, nullptr);

    vkDestroyPipelineCache(logical_device_handle, global_pipeline_cache_handle, nullptr);

    vkDestroyRenderPass(logical_device_handle, render_pass_handle, nullptr);
    for (auto del : swap_chain_image_view_handles)
    {
        vkDestroyImageView(logical_device_handle, del, nullptr);
    }
    vkDestroySwapchainKHR(logical_device_handle, swap_chain_handle, nullptr);
    vkDestroySurfaceKHR(instance_handle, surface_handle, nullptr);
    vkDestroyDevice(logical_device_handle, nullptr);
#ifdef _DEBUG
    reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance_handle, "vkDestroyDebugReportCallbackEXT"))(instance_handle, debug_report_callback_handle, nullptr);
#endif _DEBUG
    vkDestroyInstance(instance_handle, nullptr);
}

void application::run()
{
    while (!glfwWindowShouldClose(window))
    {
        main_loop();
    }
}

void application::initialize_window()
{
    if (!glfwInit())
    {
        throw std::runtime_error("glfw initialization failed");
    }
    if (!glfwVulkanSupported())
    {
        throw std::runtime_error("failed to find the Vulkan loader");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(1000, 1000, "", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("window creation failed");
    }
    glfwSetWindowPos(window, 5, 30);
}

void application::initialize_vulkan()
{
    create_instance();
#ifdef _DEBUG
    create_debug_callback();
#endif
    create_surface();
    select_physical_device();
    create_logical_device();
    get_device_queues();

    create_swap_chain();
    get_swap_chain_images();
    create_swap_chain_image_views();
    create_render_pass();
    create_swap_chain_frame_buffers();

    create_pipeline_cache();
    create_descriptor_pool();
    create_buffers();

    create_graphics_pipeline_layout();
    create_graphics_pipeline();
    create_graphics_command_pool();
    create_graphics_command_buffers();
    create_graphics_semaphores();

    create_compute_descriptor_set_layout();
    update_compute_descriptor_sets();
    create_compute_pipeline_layout();
    create_compute_pipelines();
    create_compute_command_pool();
    create_compute_command_buffer();
    create_compute_fence();

    initialize_buffers();
}


void application::create_instance()
{
    VkApplicationInfo my_app_info;
    my_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    my_app_info.pNext = nullptr;
    my_app_info.pApplicationName = "SPH Vulkan";
    my_app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    my_app_info.pEngineName = "My Awesome Wonderful Super Duper Great Vulkannn Game Engine";
    my_app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    my_app_info.apiVersion = VK_API_VERSION_1_0;
    
    uint32_t instance_layer_count;
    vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr);
    std::vector<VkLayerProperties> available_instance_layers(instance_layer_count);
    vkEnumerateInstanceLayerProperties(&instance_layer_count, available_instance_layers.data());
    std::cout << "available vulkan layers:" << std::endl;
    for (const auto& layer : available_instance_layers)
    {
        std::cout << "  name: " << layer.layerName << " desc: " << layer.description << " impl_ver: "
            << VK_VERSION_MAJOR(layer.implementationVersion) << "."
            << VK_VERSION_MINOR(layer.implementationVersion) << "."
            << VK_VERSION_PATCH(layer.implementationVersion)
            << " spec_ver: "
            << VK_VERSION_MAJOR(layer.specVersion) << "."
            << VK_VERSION_MINOR(layer.specVersion) << "."
            << VK_VERSION_PATCH(layer.specVersion)
            << std::endl;
    }

    uint32_t instance_extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr);
    std::vector<VkExtensionProperties> available_instance_extensions(instance_extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, available_instance_extensions.data());
    std::cout << "available vulkan extensions:" << std::endl;
    for (const auto& extension : available_instance_extensions)
    {
        std::cout << "  name: " << extension.extensionName << " spec ver: "
            << VK_VERSION_MAJOR(extension.specVersion) << "."
            << VK_VERSION_MINOR(extension.specVersion) << "."
            << VK_VERSION_PATCH(extension.specVersion) << std::endl;
    }

    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    std::vector<const char*> instance_extensions(glfw_extension_count);
    std::memcpy(instance_extensions.data(), glfw_extensions, sizeof(char*) * glfw_extension_count);

#ifdef _DEBUG
    instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

    VkInstanceCreateInfo my_instance_create_info;
    my_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    my_instance_create_info.pNext = nullptr;
    my_instance_create_info.flags = 0;
    my_instance_create_info.pApplicationInfo = &my_app_info;
    my_instance_create_info.enabledExtensionCount = (uint32_t) instance_extensions.size();
    my_instance_create_info.ppEnabledExtensionNames = instance_extensions.data();
#ifdef _DEBUG
    const char* validation_layer_names = "VK_LAYER_LUNARG_standard_validation";
    my_instance_create_info.enabledLayerCount = 1;
    my_instance_create_info.ppEnabledLayerNames = &validation_layer_names;
#else
    my_instance_create_info.enabledLayerCount = 0;
    my_instance_create_info.ppEnabledLayerNames = nullptr;
#endif

    if (vkCreateInstance(&my_instance_create_info, nullptr, &instance_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("vulkan instance creation failed");
    }
}


void application::create_debug_callback()
{
    VkDebugReportCallbackCreateInfoEXT my_debug_report_callback_create_info;
    my_debug_report_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    my_debug_report_callback_create_info.pNext = nullptr;
    my_debug_report_callback_create_info.pUserData = nullptr;
    my_debug_report_callback_create_info.flags = VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT;
    my_debug_report_callback_create_info.pfnCallback = vulkan_debug_callback;
    if (reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance_handle, "vkCreateDebugReportCallbackEXT"))(instance_handle, &my_debug_report_callback_create_info, nullptr, &debug_report_callback_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("debug callback setup failed");
    }
}

void application::create_surface()
{
    if (glfwCreateWindowSurface(instance_handle, window, NULL, &surface_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("surface creation failed");
    }
}

void application::select_physical_device()
{
    uint32_t physical_device_count = 0;
    vkEnumeratePhysicalDevices(instance_handle, &physical_device_count, nullptr);
    if (physical_device_count == 0)
    {
        throw std::runtime_error("unable to find any device with vulkan support");
    }
    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    vkEnumeratePhysicalDevices(instance_handle, &physical_device_count, physical_devices.data());

    // select first device and set it as the device used throughout the program
    physical_device_handle = physical_devices[0];

    // get this device properties and features
    vkGetPhysicalDeviceProperties(physical_device_handle, &physical_device_properties);
    vkGetPhysicalDeviceFeatures(physical_device_handle, &physical_device_features);
    // get this device extensions
    uint32_t device_extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device_handle, nullptr, &device_extension_count, nullptr);
    physical_device_extensions.resize(device_extension_count);
    vkEnumerateDeviceExtensionProperties(physical_device_handle, nullptr, &device_extension_count, physical_device_extensions.data());
    // get memory properties
    vkGetPhysicalDeviceMemoryProperties(physical_device_handle, &physical_device_memory_properties);
    // print info
    std::cout << "selected device" << std::endl
        << "  name: " << physical_device_properties.deviceName << std::endl
        << "  type: ";
    if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER)
    {
        std::cout << "VK_PHYSICAL_DEVICE_TYPE_OTHER";
    }
    else if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
    {
        std::cout << "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
    }
    else if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        std::cout << "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
    }
    else if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
    {
        std::cout << "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
    }
    else if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
    {
        std::cout << "VK_PHYSICAL_DEVICE_TYPE_CPU";
    }
    std::cout << " (" << physical_device_properties.deviceType << ")" << std::endl
        << "  driver version: "
        << VK_VERSION_MAJOR(physical_device_properties.driverVersion) << "."
        << VK_VERSION_MINOR(physical_device_properties.driverVersion) << "."
        << VK_VERSION_PATCH(physical_device_properties.driverVersion) << std::endl
        << "  vulkan api version: "
        << VK_VERSION_MAJOR(physical_device_properties.apiVersion) << "."
        << VK_VERSION_MINOR(physical_device_properties.apiVersion) << "."
        << VK_VERSION_PATCH(physical_device_properties.apiVersion) << std::endl;
    std::cout << "  available extensions:" << std::endl;
    for (auto& extension : physical_device_extensions)
    {
        std::cout << "    " << extension.extensionName << " spec_ver: "
            << VK_VERSION_MAJOR(extension.specVersion) << "."
            << VK_VERSION_MINOR(extension.specVersion) << "."
            << VK_VERSION_PATCH(extension.specVersion) << std::endl;
    }
}

void application::create_logical_device()
{
    graphics_presentation_compute_queue_family_index = UINT32_MAX;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_handle, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_handle, &queue_family_count, queue_families.data());
    std::cout << "available queue families:" << std::endl;
    // look for queue family indices
    for (uint32_t index = 0; index < queue_families.size(); index++)
    {
        std::cout << "  flags: ";
        if (queue_families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            std::cout << "VK_QUEUE_GRAPHICS_BIT ";
        }
        if (queue_families[index].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            std::cout << "VK_QUEUE_COMPUTE_BIT ";
        }
        if (queue_families[index].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            std::cout << "VK_QUEUE_TRANSFER_BIT ";
        }
        if (queue_families[index].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
        {
            std::cout << "VK_QUEUE_SPARSE_BINDING_BIT ";
        }
        std::cout << "(" << queue_families[index].queueFlags << ") count: " << queue_families[index].queueCount << std::endl;

        // try to search a queue family that contain graphics queue, compute queue, and presentation queue
        // note: queue family index must be unique in the device queue create info
        VkBool32 presentation_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device_handle, index, surface_handle, &presentation_support);
        if (queue_families[index].queueCount > 0 && queue_families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT && presentation_support && queue_families[index].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            graphics_presentation_compute_queue_family_index = index;
        }
    }
    if (graphics_presentation_compute_queue_family_index == UINT32_MAX)
    {
        throw std::runtime_error("unable to find a family queue with graphics, presentation, and compute queue");
    }
    const float queue_priority[3] = { 1.f, 1.f, 1.f };
    VkDeviceQueueCreateInfo queue_create_info[1];
    queue_create_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info[0].pNext = nullptr;
    queue_create_info[0].flags = 0;
    queue_create_info[0].queueFamilyIndex = graphics_presentation_compute_queue_family_index;
    queue_create_info[0].queueCount = 3; // 3 queue for graphics queue, compute queue, and presentation queue
    queue_create_info[0].pQueuePriorities = queue_priority;


    // enabled features are empty
    VkPhysicalDeviceFeatures enabled_device_features;
    std::memset(&enabled_device_features, 0, sizeof(VkPhysicalDeviceFeatures));

    const char* enabled_extensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    VkDeviceCreateInfo device_create_info;
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = nullptr;
    device_create_info.flags = 0;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = queue_create_info;
    device_create_info.enabledExtensionCount = 1;
    device_create_info.ppEnabledExtensionNames = &enabled_extensions;
    device_create_info.enabledLayerCount = 0;
    device_create_info.ppEnabledLayerNames = nullptr;
    device_create_info.pEnabledFeatures = &enabled_device_features;

    if (vkCreateDevice(physical_device_handle, &device_create_info, nullptr, &logical_device_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("logical device creation failed");
    }
}

void application::get_device_queues()
{
    vkGetDeviceQueue(logical_device_handle, graphics_presentation_compute_queue_family_index, 0, &graphics_queue_handle);
    vkGetDeviceQueue(logical_device_handle, graphics_presentation_compute_queue_family_index, 1, &compute_queue_handle);
    vkGetDeviceQueue(logical_device_handle, graphics_presentation_compute_queue_family_index, 2, &presentation_queue_handle);
 }

void application::create_swap_chain()
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_handle, surface_handle, &surface_capabilities);
    uint32_t surface_format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_handle, surface_handle, &surface_format_count, nullptr);
    if (surface_format_count > 0)
    {
        surface_formats.resize(surface_format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_handle, surface_handle, &surface_format_count, surface_formats.data());
    }
    uint32_t surface_presentation_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_handle, surface_handle, &surface_presentation_mode_count, nullptr);

    if (surface_presentation_mode_count > 0)
    {
        surface_presentation_modes.resize(surface_presentation_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_handle, surface_handle, &surface_presentation_mode_count, surface_presentation_modes.data());
    }
    VkSurfaceFormatKHR surface_format;
    surface_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;

    swap_chain_image_format = surface_format.format;

    VkExtent2D surface_extent;
    surface_extent.width = window_width;
    surface_extent.height = window_height;
    swap_chain_extent = surface_extent;
    
    // triple buffering
    uint32_t image_count = 3;

    VkSwapchainCreateInfoKHR swap_chain_create_info;
    swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swap_chain_create_info.pNext = nullptr;
    swap_chain_create_info.flags = 0;
    swap_chain_create_info.surface = surface_handle;
    swap_chain_create_info.minImageCount = image_count;
    swap_chain_create_info.imageFormat = surface_format.format;
    swap_chain_create_info.imageColorSpace = surface_format.colorSpace;
    swap_chain_create_info.imageExtent = surface_extent;
    swap_chain_create_info.imageArrayLayers = 1;
    swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swap_chain_create_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; //vsync off
    swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swap_chain_create_info.queueFamilyIndexCount = 0;
    swap_chain_create_info.pQueueFamilyIndices = nullptr;
    swap_chain_create_info.preTransform = surface_capabilities.currentTransform;
    swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swap_chain_create_info.clipped = VK_TRUE;
    swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(logical_device_handle, &swap_chain_create_info, nullptr, &swap_chain_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("swap chain creation failed");
    }
}

void application::get_swap_chain_images()
{
    uint32_t swap_chain_image_count;
    vkGetSwapchainImagesKHR(logical_device_handle, swap_chain_handle, &swap_chain_image_count, nullptr);
    swap_chain_image_handles.resize(swap_chain_image_count);
    vkGetSwapchainImagesKHR(logical_device_handle, swap_chain_handle, &swap_chain_image_count, swap_chain_image_handles.data());
}

void application::create_swap_chain_image_views()
{
    swap_chain_image_view_handles.resize(swap_chain_image_handles.size());
    for (uint32_t i = 0; i < swap_chain_image_view_handles.size(); i++)
    {
        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.pNext = nullptr;
        image_view_create_info.flags = 0;
        image_view_create_info.image = swap_chain_image_handles[i];
        
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = swap_chain_image_format;

        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logical_device_handle, &image_view_create_info, nullptr, &swap_chain_image_view_handles[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("image views creation failed");
        }
    }
}

void application::create_render_pass()
{
    VkAttachmentDescription attachment_description;
    attachment_description.flags = 0;
    attachment_description.format = swap_chain_image_format;
    attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_description;
    subpass_description.flags = 0;
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.inputAttachmentCount = 0;
    subpass_description.pResolveAttachments = nullptr;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &color_attachment_reference;
    subpass_description.pResolveAttachments = nullptr;
    subpass_description.pDepthStencilAttachment = nullptr;
    subpass_description.preserveAttachmentCount = 0;
    subpass_description.pResolveAttachments = nullptr;

    VkRenderPassCreateInfo render_pass_create_info;
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.pNext = nullptr;
    render_pass_create_info.flags = 0;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &attachment_description;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass_description;
    render_pass_create_info.dependencyCount = 0;
    render_pass_create_info.pDependencies = nullptr;

    if (vkCreateRenderPass(logical_device_handle, &render_pass_create_info, nullptr, &render_pass_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("render pass creation failed");
    }
}

void application::create_swap_chain_frame_buffers()
{
    swap_chain_frame_buffer_handles.resize(swap_chain_image_view_handles.size());

    for (size_t index = 0; index < swap_chain_image_view_handles.size(); index++)
    {
        VkFramebufferCreateInfo framebuffer_create_info;
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.pNext = nullptr;
        framebuffer_create_info.flags = 0;
        framebuffer_create_info.renderPass = render_pass_handle;
        framebuffer_create_info.attachmentCount = 1;
        framebuffer_create_info.pAttachments = &swap_chain_image_view_handles[index];
        framebuffer_create_info.width = swap_chain_extent.width;
        framebuffer_create_info.height = swap_chain_extent.height;
        framebuffer_create_info.layers = 1;

        if (vkCreateFramebuffer(logical_device_handle, &framebuffer_create_info, nullptr, &swap_chain_frame_buffer_handles[index]) != VK_SUCCESS)
        {
            throw std::runtime_error("frame buffer creation failed");
        }

    }
}

void application::create_descriptor_pool()
{
    VkDescriptorPoolSize descriptor_pool_size[1];
    descriptor_pool_size[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptor_pool_size[0].descriptorCount = 1;

    VkDescriptorPoolCreateInfo descriptor_pool_create_info;
    descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_create_info.pNext = nullptr;
    descriptor_pool_create_info.flags = 0;
    descriptor_pool_create_info.maxSets = 1;
    descriptor_pool_create_info.poolSizeCount = 1;
    descriptor_pool_create_info.pPoolSizes = descriptor_pool_size;
    
    if (vkCreateDescriptorPool(logical_device_handle, &descriptor_pool_create_info, nullptr, &global_descriptor_pool_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("descriptor pool creation failed");
    }
}

void application::create_pipeline_cache()
{
    VkPipelineCacheCreateInfo pipeline_cache_create_info = {};
    pipeline_cache_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipeline_cache_create_info.pNext = nullptr;
    pipeline_cache_create_info.flags = 0;
    pipeline_cache_create_info.initialDataSize = 0;
    pipeline_cache_create_info.pInitialData = nullptr;
    if (vkCreatePipelineCache(logical_device_handle, &pipeline_cache_create_info, nullptr, &global_pipeline_cache_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("pipeline cache creation failed");
    }
}

void application::create_buffers()
{
    VkBufferCreateInfo particle_buffer_create_info;
    particle_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    particle_buffer_create_info.pNext = nullptr;
    particle_buffer_create_info.flags = 0;
    particle_buffer_create_info.size = sizeof(particle_t) * particle_count;
    particle_buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    particle_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    particle_buffer_create_info.queueFamilyIndexCount = 0;
    particle_buffer_create_info.pQueueFamilyIndices = nullptr;
    vkCreateBuffer(logical_device_handle, &particle_buffer_create_info, nullptr, &particle_buffer_handle);

    VkMemoryRequirements particle_buffer_memory_requirements;
    vkGetBufferMemoryRequirements(logical_device_handle, particle_buffer_handle, &particle_buffer_memory_requirements);

    VkMemoryAllocateInfo particle_buffer_memory_allocation_info;
    particle_buffer_memory_allocation_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    particle_buffer_memory_allocation_info.pNext = nullptr;
    particle_buffer_memory_allocation_info.allocationSize = particle_buffer_memory_requirements.size;
    particle_buffer_memory_allocation_info.memoryTypeIndex = get_memory_type_index(particle_buffer_memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(logical_device_handle, &particle_buffer_memory_allocation_info, nullptr, &particle_buffer_device_memory_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("memory allocation failed");
    }
    // set descriptor buffer info
    particle_descriptor_buffer_info.buffer = particle_buffer_handle;
    particle_descriptor_buffer_info.offset = 0;
    particle_descriptor_buffer_info.range = particle_buffer_memory_allocation_info.allocationSize;
    // bind the memory to the buffer object
    vkBindBufferMemory(logical_device_handle, particle_buffer_handle, particle_buffer_device_memory_handle, 0);
}

void application::initialize_buffers()
{
    // staging buffer
    VkBuffer staging_buffer_handle = VK_NULL_HANDLE;
    VkDeviceMemory staging_buffer_memory_device_handle = VK_NULL_HANDLE;
    VkBufferCreateInfo staging_buffer_create_info;
    staging_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    staging_buffer_create_info.pNext = nullptr;
    staging_buffer_create_info.flags = 0;
    staging_buffer_create_info.size = sizeof(particle_t) * particle_count;
    staging_buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    staging_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    staging_buffer_create_info.queueFamilyIndexCount = 0;
    staging_buffer_create_info.pQueueFamilyIndices = nullptr;

    vkCreateBuffer(logical_device_handle, &staging_buffer_create_info, nullptr, &staging_buffer_handle);

    VkMemoryRequirements staging_buffer_memory_requirements;

    vkGetBufferMemoryRequirements(logical_device_handle, staging_buffer_handle, &staging_buffer_memory_requirements);

    VkMemoryAllocateInfo staging_buffer_memory_allocation_info;
    staging_buffer_memory_allocation_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    staging_buffer_memory_allocation_info.pNext = nullptr;
    staging_buffer_memory_allocation_info.allocationSize = staging_buffer_memory_requirements.size;
    staging_buffer_memory_allocation_info.memoryTypeIndex = get_memory_type_index(staging_buffer_memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(logical_device_handle, &staging_buffer_memory_allocation_info, nullptr, &staging_buffer_memory_device_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("memory allocation failed");
    }
    // bind the memory to the buffer object
    vkBindBufferMemory(logical_device_handle, staging_buffer_handle, staging_buffer_memory_device_handle, 0);

    void* map_ptr = nullptr;
    vkMapMemory(logical_device_handle, staging_buffer_memory_device_handle, 0, staging_buffer_memory_requirements.size, 0, &map_ptr);

	{
		particle_t* initial_particle_data = new particle_t[particle_count];
		// initialize to zero
		std::memset(initial_particle_data, 0, sizeof(particle_t) * particle_count);
		// initialize position
		// Vulkan coordinate system is different from OpenGL! y axis is pointing down
		size_t index = 0;
		for (size_t x = 0; x < 125; x++)
		{
			for (size_t y = 0; y < 160; y++)
			{
				initial_particle_data[index].position.x = -0.625f + particle_length * 2 * x;
				initial_particle_data[index].position.y = -1 + particle_length * 2 * y;
				index++;
			}
		}
		std::memcpy(map_ptr, initial_particle_data, static_cast<size_t>(staging_buffer_memory_requirements.size));
		delete[] initial_particle_data;
	}
    vkUnmapMemory(logical_device_handle, staging_buffer_memory_device_handle);

    // submit a command buffer to copy staging buffer to the particle buffer 
    VkCommandBuffer copy_command_buffer_handle;
    VkCommandBufferAllocateInfo copy_command_buffer_allocation_info;
    copy_command_buffer_allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    copy_command_buffer_allocation_info.pNext = nullptr;
    copy_command_buffer_allocation_info.commandPool = compute_command_pool_handle;
    copy_command_buffer_allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    copy_command_buffer_allocation_info.commandBufferCount = 1;
    if (vkAllocateCommandBuffers(logical_device_handle, &copy_command_buffer_allocation_info, &copy_command_buffer_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("copy command buffer creation failed");
    }

    VkCommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = nullptr;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    command_buffer_begin_info.pInheritanceInfo = nullptr;
    if (vkBeginCommandBuffer(copy_command_buffer_handle, &command_buffer_begin_info) != VK_SUCCESS)
    {
        throw std::runtime_error("copy command buffer begin failed");
    }
    VkBufferCopy buffer_copy_region;
    buffer_copy_region.srcOffset = 0;
    buffer_copy_region.dstOffset = 0;
    buffer_copy_region.size = staging_buffer_memory_requirements.size;
    vkCmdCopyBuffer(copy_command_buffer_handle, staging_buffer_handle, particle_buffer_handle, 1, &buffer_copy_region);

    if (vkEndCommandBuffer(copy_command_buffer_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("copy command buffer end failed");
    }

    VkSubmitInfo copy_submit_info;
    copy_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    copy_submit_info.pNext = nullptr;
    copy_submit_info.waitSemaphoreCount = 0;
    copy_submit_info.pWaitSemaphores = nullptr;
    copy_submit_info.pWaitDstStageMask = 0;
    copy_submit_info.commandBufferCount = 1;
    copy_submit_info.pCommandBuffers = &copy_command_buffer_handle;
    copy_submit_info.signalSemaphoreCount = 0;
    copy_submit_info.pSignalSemaphores = nullptr;

    if (vkQueueSubmit(compute_queue_handle, 1, &copy_submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        throw std::exception();
    }

    if(vkQueueWaitIdle(compute_queue_handle) != VK_SUCCESS)
    {
        throw std::exception();
    }

    vkFreeCommandBuffers(logical_device_handle, compute_command_pool_handle, 1, &copy_command_buffer_handle);

    vkDestroyBuffer(logical_device_handle, staging_buffer_handle, nullptr);

    vkFreeMemory(logical_device_handle, staging_buffer_memory_device_handle, nullptr);
}

void application::create_compute_descriptor_set_layout()
{
    // create descriptor layout
    VkDescriptorSetLayoutBinding descriptor_set_layout_binding;
    descriptor_set_layout_binding.binding = 0;
    descriptor_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptor_set_layout_binding.descriptorCount = 1;
    descriptor_set_layout_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    descriptor_set_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info;
    descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_create_info.pNext = nullptr;
    descriptor_set_layout_create_info.flags = 0;
    descriptor_set_layout_create_info.bindingCount = 1;
    descriptor_set_layout_create_info.pBindings = &descriptor_set_layout_binding;

    if (vkCreateDescriptorSetLayout(logical_device_handle, &descriptor_set_layout_create_info, nullptr, &compute_descriptor_set_layout_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("compute descriptor layout creation failed");
    }
}

void application::update_compute_descriptor_sets()
{
    // allocate descriptor sets
    VkDescriptorSetAllocateInfo descriptor_set_allocation_info;
    descriptor_set_allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_set_allocation_info.pNext = nullptr;
    descriptor_set_allocation_info.descriptorPool = global_descriptor_pool_handle;
    descriptor_set_allocation_info.descriptorSetCount = 1;
    descriptor_set_allocation_info.pSetLayouts = &compute_descriptor_set_layout_handle;

    if (vkAllocateDescriptorSets(logical_device_handle, &descriptor_set_allocation_info, &compute_descriptor_set_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("compute descriptor set allocation failed");
    }
    // write descriptor sets
    VkWriteDescriptorSet write_descriptor_sets[1];
    write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[0].pNext = nullptr;
    write_descriptor_sets[0].dstSet = compute_descriptor_set_handle;
    write_descriptor_sets[0].dstBinding = 0;
    write_descriptor_sets[0].dstArrayElement = 0;
    write_descriptor_sets[0].descriptorCount = 1;
    write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    write_descriptor_sets[0].pImageInfo = nullptr;
    write_descriptor_sets[0].pBufferInfo = &particle_descriptor_buffer_info;
    write_descriptor_sets[0].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(logical_device_handle, 1, write_descriptor_sets, 0, nullptr);
}
void application::create_compute_pipeline_layout()
{
    // create pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_create_info;
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = nullptr;
    pipeline_layout_create_info.flags = 0;
    pipeline_layout_create_info.setLayoutCount = 1;
    pipeline_layout_create_info.pSetLayouts = &compute_descriptor_set_layout_handle;
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(logical_device_handle, &pipeline_layout_create_info, nullptr, &compute_pipeline_layout_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("compute pipeline layout creation failed");
    }
}


void application::create_compute_pipelines()
{
    // create pipelines
    // first pass
    VkShaderModule compute_density_pressure_shader_module = create_shader_module_from_file("../shader/compute_density_pressure.comp.spv");

    VkPipelineShaderStageCreateInfo compute_shader_stage_create_info;
    compute_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    compute_shader_stage_create_info.pNext = nullptr;
    compute_shader_stage_create_info.flags = 0;
    compute_shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    compute_shader_stage_create_info.module = compute_density_pressure_shader_module;
    compute_shader_stage_create_info.pName = "main";
    compute_shader_stage_create_info.pSpecializationInfo = nullptr;

    VkComputePipelineCreateInfo compute_pipeline_create_info;
    compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_pipeline_create_info.pNext = nullptr;
    compute_pipeline_create_info.flags = 0;
    compute_pipeline_create_info.stage = compute_shader_stage_create_info;
    compute_pipeline_create_info.layout = compute_pipeline_layout_handle;
    compute_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    compute_pipeline_create_info.basePipelineIndex = -1;

    if (vkCreateComputePipelines(logical_device_handle, global_pipeline_cache_handle, 1, &compute_pipeline_create_info, nullptr, &compute_pipeline_handles[0]) != VK_SUCCESS)
    {
        throw std::runtime_error("first compute pipeline creation failed");
    }

    // second pass
    VkShaderModule compute_force_shader_module = create_shader_module_from_file("../shader/compute_force.comp.spv");
    compute_shader_stage_create_info.module = compute_force_shader_module;
    compute_pipeline_create_info.stage = compute_shader_stage_create_info;

    if (vkCreateComputePipelines(logical_device_handle, global_pipeline_cache_handle, 1, &compute_pipeline_create_info, nullptr, &compute_pipeline_handles[1]) != VK_SUCCESS)
    {
        throw std::runtime_error("second compute pipeline creation failed");
    }

    // third pass
    VkShaderModule integrate_shader_module = create_shader_module_from_file("../shader/integrate.comp.spv");
    compute_shader_stage_create_info.module = integrate_shader_module;
    compute_pipeline_create_info.stage = compute_shader_stage_create_info;

    if (vkCreateComputePipelines(logical_device_handle, global_pipeline_cache_handle, 1, &compute_pipeline_create_info, nullptr, &compute_pipeline_handles[2]) != VK_SUCCESS)
    {
        throw std::runtime_error("third compute pipeline creation failed");
    }
}


void application::create_compute_command_pool()
{
    // create compute command pool
    VkCommandPoolCreateInfo command_pool_create_info;
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.pNext = nullptr;
    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_create_info.queueFamilyIndex = graphics_presentation_compute_queue_family_index;
    if (vkCreateCommandPool(logical_device_handle, &command_pool_create_info, nullptr, &compute_command_pool_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("compute command pool creation failed");
    }
}

void application::create_compute_command_buffer()
{
    // allocate command buffer
	{
		VkCommandBufferAllocateInfo command_buffer_allocate_info;
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.pNext = nullptr;
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandPool = compute_command_pool_handle;
		command_buffer_allocate_info.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(logical_device_handle, &command_buffer_allocate_info, &compute_command_buffer_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("compute command buffer allocation failed");
		}
	}

    // build command buffer
	{
		VkCommandBufferBeginInfo command_buffer_begin_info;
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_begin_info.pNext = nullptr;
		command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		command_buffer_begin_info.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(compute_command_buffer_handle, &command_buffer_begin_info) != VK_SUCCESS)
		{
			throw std::runtime_error("compute command buffer begin failed");
		}

		// this memory barrier is to make sure the vertex shader in the graphics pipeline has finished reading the position buffer before the compute pipelines start writing to the buffer
		VkBufferMemoryBarrier buffer_memory_barrier;
		buffer_memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		buffer_memory_barrier.pNext = nullptr;
		buffer_memory_barrier.srcAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		buffer_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		buffer_memory_barrier.srcQueueFamilyIndex = graphics_presentation_compute_queue_family_index;
		buffer_memory_barrier.dstQueueFamilyIndex = graphics_presentation_compute_queue_family_index;
		buffer_memory_barrier.buffer = particle_buffer_handle;
		buffer_memory_barrier.size = particle_descriptor_buffer_info.range;
		buffer_memory_barrier.offset = 0;

		vkCmdPipelineBarrier(compute_command_buffer_handle, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 1, &buffer_memory_barrier, 0, nullptr);

		vkCmdBindPipeline(compute_command_buffer_handle, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_handles[0]);
		vkCmdBindDescriptorSets(compute_command_buffer_handle, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_layout_handle, 0, 1, &compute_descriptor_set_handle, 0, 0);

		// first pass
		vkCmdDispatch(compute_command_buffer_handle, group_count, 1, 1);

		// add memory barrier
		buffer_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		buffer_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		buffer_memory_barrier.buffer = particle_buffer_handle;
		buffer_memory_barrier.size = particle_descriptor_buffer_info.range;
		buffer_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		buffer_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		vkCmdPipelineBarrier(compute_command_buffer_handle, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 1, &buffer_memory_barrier, 0, nullptr);

		// second pass
		vkCmdBindPipeline(compute_command_buffer_handle, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_handles[1]);
		vkCmdDispatch(compute_command_buffer_handle, group_count, 1, 1);

		// add memory barrier again
		buffer_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		buffer_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		buffer_memory_barrier.buffer = particle_buffer_handle;
		buffer_memory_barrier.size = particle_descriptor_buffer_info.range;
		buffer_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		buffer_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		vkCmdPipelineBarrier(compute_command_buffer_handle, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 1, &buffer_memory_barrier, 0, nullptr);

		// third pass
		vkCmdBindPipeline(compute_command_buffer_handle, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_handles[2]);
		vkCmdDispatch(compute_command_buffer_handle, group_count, 1, 1);

		// add memory barrier again
		buffer_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		buffer_memory_barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		buffer_memory_barrier.buffer = particle_buffer_handle;
		buffer_memory_barrier.size = particle_descriptor_buffer_info.range;
		buffer_memory_barrier.srcQueueFamilyIndex = graphics_presentation_compute_queue_family_index;
		buffer_memory_barrier.dstQueueFamilyIndex = graphics_presentation_compute_queue_family_index;

		vkCmdPipelineBarrier(compute_command_buffer_handle, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, nullptr, 1, &buffer_memory_barrier, 0, nullptr);

		vkEndCommandBuffer(compute_command_buffer_handle);
	}
}

void application::create_compute_fence()
{
    // fence
    VkFenceCreateInfo fence_create_info;
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = nullptr;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (vkCreateFence(logical_device_handle, &fence_create_info, nullptr, &compute_fence_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("compute fence creation failed");
    }
}

void application::create_graphics_pipeline_layout()
{
    VkDescriptorSetLayoutBinding descriptor_set_layout_bindings[1];

    descriptor_set_layout_bindings[0].binding = 0;
    descriptor_set_layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = nullptr;
    pipeline_layout_create_info.flags = 0;
    pipeline_layout_create_info.setLayoutCount = 0;
    pipeline_layout_create_info.pSetLayouts = nullptr;
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = 0;

    if (vkCreatePipelineLayout(logical_device_handle, &pipeline_layout_create_info, nullptr, &graphics_pipeline_layout_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("pipeline layout creation failed");
    }
 }

 void application::create_graphics_pipeline()
 {
    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;
    // create shader stage infos
	{
		VkShaderModule vertex_shader_module = create_shader_module_from_file("../shader/particle.vert.spv");

		VkShaderModule fragment_shader_module = create_shader_module_from_file("../shader/particle.frag.spv");

		VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info;
		vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertex_shader_stage_create_info.pNext = nullptr;
		vertex_shader_stage_create_info.flags = 0;
		vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertex_shader_stage_create_info.module = vertex_shader_module;
		vertex_shader_stage_create_info.pName = "main";
		vertex_shader_stage_create_info.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info;
		fragment_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragment_shader_stage_create_info.pNext = nullptr;
		fragment_shader_stage_create_info.flags = 0;
		fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragment_shader_stage_create_info.module = fragment_shader_module;
		fragment_shader_stage_create_info.pName = "main";
		fragment_shader_stage_create_info.pSpecializationInfo = nullptr;

		shader_stage_create_infos.push_back(vertex_shader_stage_create_info);
		shader_stage_create_infos.push_back(fragment_shader_stage_create_info);
	}

    VkVertexInputBindingDescription vertex_input_binding_description;
    vertex_input_binding_description.binding = 0;
    vertex_input_binding_description.stride = sizeof(particle_t);
    vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertex_input_attribute_description;
    // layout(location = 0) in vec2 position;
    vertex_input_attribute_description.binding = 0;
    vertex_input_attribute_description.location = 0;
    vertex_input_attribute_description.format = VK_FORMAT_R32G32_SFLOAT;
    vertex_input_attribute_description.offset = 0;

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info;
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.pNext = nullptr;
    vertex_input_state_create_info.flags = 0;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
    vertex_input_state_create_info.pVertexBindingDescriptions = &vertex_input_binding_description;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = 1;
    vertex_input_state_create_info.pVertexAttributeDescriptions = &vertex_input_attribute_description;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info;
    input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state_create_info.pNext = nullptr;
    input_assembly_state_create_info.flags = 0;
    input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport;
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = (float)swap_chain_extent.width;
    viewport.height = (float)swap_chain_extent.height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = swap_chain_extent;

    VkPipelineViewportStateCreateInfo viewport_state_create_info;
    viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_create_info.pNext = nullptr;
    viewport_state_create_info.flags = 0;
    viewport_state_create_info.viewportCount = 1;
    viewport_state_create_info.pViewports = &viewport;
    viewport_state_create_info.scissorCount = 1;
    viewport_state_create_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info;
    rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_create_info.pNext = nullptr;
    rasterization_state_create_info.flags = 0;
    rasterization_state_create_info.depthClampEnable = VK_FALSE;
    rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state_create_info.lineWidth = 1.f;
    rasterization_state_create_info.cullMode = VK_CULL_MODE_NONE;
    rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state_create_info.depthBiasEnable = VK_FALSE;
    rasterization_state_create_info.depthBiasConstantFactor = 0.f;
    rasterization_state_create_info.depthBiasClamp = 0.f;
    rasterization_state_create_info.depthBiasSlopeFactor = 0.f;

    VkPipelineMultisampleStateCreateInfo multisample_state_create_info;
    multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state_create_info.pNext = nullptr;
    multisample_state_create_info.flags = 0;
    multisample_state_create_info.sampleShadingEnable = VK_FALSE;
    multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_state_create_info.minSampleShading = 0.0f;
    multisample_state_create_info.pSampleMask = nullptr;
    multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
    multisample_state_create_info.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment;
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info;
    color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state_create_info.pNext = nullptr;
    color_blend_state_create_info.flags = 0;
    color_blend_state_create_info.logicOpEnable = VK_FALSE;
    color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state_create_info.attachmentCount = 1;
    color_blend_state_create_info.pAttachments = &color_blend_attachment;
    color_blend_state_create_info.blendConstants[0] = 0.0f;
    color_blend_state_create_info.blendConstants[1] = 0.0f;
    color_blend_state_create_info.blendConstants[2] = 0.0f;
    color_blend_state_create_info.blendConstants[3] = 0.0f;

    VkDynamicState dynamic_states[2];
    dynamic_states[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamic_states[1] = VK_DYNAMIC_STATE_LINE_WIDTH;

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info;
    dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_create_info.pNext = nullptr;
    dynamic_state_create_info.flags = 0;
    dynamic_state_create_info.dynamicStateCount = 2;
    dynamic_state_create_info.pDynamicStates = dynamic_states;

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info;
    graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_create_info.pNext = nullptr;
    graphics_pipeline_create_info.flags = 0;
    graphics_pipeline_create_info.stageCount = (uint32_t) shader_stage_create_infos.size();
    graphics_pipeline_create_info.pStages = shader_stage_create_infos.data();
    graphics_pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
    graphics_pipeline_create_info.pTessellationState = nullptr;
    graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
    graphics_pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
    graphics_pipeline_create_info.pMultisampleState = &multisample_state_create_info;
    graphics_pipeline_create_info.pDepthStencilState = nullptr; 
    graphics_pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
    graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
    graphics_pipeline_create_info.layout = graphics_pipeline_layout_handle;
    graphics_pipeline_create_info.renderPass = render_pass_handle;
    graphics_pipeline_create_info.subpass = 0;
    graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_create_info.basePipelineIndex = -1;
    
    if (vkCreateGraphicsPipelines(logical_device_handle, global_pipeline_cache_handle, 1, &graphics_pipeline_create_info, nullptr, &graphics_pipeline_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("graphics pipeline creation failed");
    }
}

void application::create_graphics_command_pool()
{
    VkCommandPoolCreateInfo graphics_command_pool_create_info;
    graphics_command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    graphics_command_pool_create_info.pNext = nullptr;
    graphics_command_pool_create_info.flags = 0;
    graphics_command_pool_create_info.queueFamilyIndex = graphics_presentation_compute_queue_family_index;

    if (vkCreateCommandPool(logical_device_handle, &graphics_command_pool_create_info, nullptr, &graphics_command_pool_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("command pool creation failed");
    }
}

void application::create_graphics_command_buffers()
{
    graphics_command_buffer_handles.resize(swap_chain_frame_buffer_handles.size());

    VkCommandBufferAllocateInfo graphics_command_buffer_allocation_info;
    graphics_command_buffer_allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    graphics_command_buffer_allocation_info.pNext = nullptr;
    graphics_command_buffer_allocation_info.commandPool = graphics_command_pool_handle;
    graphics_command_buffer_allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    graphics_command_buffer_allocation_info.commandBufferCount = (uint32_t)graphics_command_buffer_handles.size();

    if (vkAllocateCommandBuffers(logical_device_handle, &graphics_command_buffer_allocation_info, graphics_command_buffer_handles.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("command buffers allocation failed");
    }

    for (size_t i = 0; i < graphics_command_buffer_handles.size(); i++)
    {
        VkCommandBufferBeginInfo command_buffer_begin_info;
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        command_buffer_begin_info.pNext = nullptr;
        command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        command_buffer_begin_info.pInheritanceInfo = nullptr;

        vkBeginCommandBuffer(graphics_command_buffer_handles[i], &command_buffer_begin_info);

        VkClearValue clear_value = { 0.0f, 0.0f, 0.0f, 1.0f };
        VkRenderPassBeginInfo render_pass_begin_info;
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.pNext = nullptr;
        render_pass_begin_info.renderPass = render_pass_handle;
        render_pass_begin_info.framebuffer = swap_chain_frame_buffer_handles[i];
        render_pass_begin_info.renderArea.offset = { 0, 0 };
        render_pass_begin_info.renderArea.extent = swap_chain_extent;
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_value;

        vkCmdBeginRenderPass(graphics_command_buffer_handles[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
        VkViewport viewport;
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = (float)swap_chain_extent.width;
        viewport.height = (float)swap_chain_extent.height;
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;

        VkRect2D scissor;
        scissor.offset = { 0, 0 };
        scissor.extent = swap_chain_extent;

        vkCmdSetViewport(graphics_command_buffer_handles[i], 0, 1, &viewport);
        vkCmdSetScissor(graphics_command_buffer_handles[i], 0, 1, &scissor);
        vkCmdBindPipeline(graphics_command_buffer_handles[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_handle);

        VkDeviceSize offsets = 0;
        vkCmdBindVertexBuffers(graphics_command_buffer_handles[i], 0, 1, &particle_buffer_handle, &offsets);
        vkCmdDraw(graphics_command_buffer_handles[i], particle_count, 1, 0, 0);

        vkCmdEndRenderPass(graphics_command_buffer_handles[i]);

        if (vkEndCommandBuffer(graphics_command_buffer_handles[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("command buffer creation failed");
        }
    }
}

void application::create_graphics_semaphores()
{
    VkSemaphoreCreateInfo semaphore_create_info;
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.pNext = nullptr;
    semaphore_create_info.flags = 0;

    if (vkCreateSemaphore(logical_device_handle, &semaphore_create_info, nullptr, &image_available_semaphore_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("semaphore creation failed");
    }
    if (vkCreateSemaphore(logical_device_handle, &semaphore_create_info, nullptr, &render_finished_semaphore_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("semaphore creation failed");
    }
}

VkShaderModule application::create_shader_module_from_file(std::string path_to_file)
{

    std::ifstream shader_file(path_to_file, std::ios::ate | std::ios::binary);
    if (!shader_file)
    {
        throw std::runtime_error("shader file load error");
    }
    size_t shader_file_size = (size_t)shader_file.tellg();
    std::vector<char> shader_code(shader_file_size);
    shader_file.seekg(0);
    shader_file.read(shader_code.data(), shader_file_size);
    shader_file.close();

    VkShaderModuleCreateInfo shader_module_create_info;
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.pNext = nullptr;
    shader_module_create_info.flags = 0;
    shader_module_create_info.codeSize = shader_code.size();
    shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());
    VkShaderModule shader_module;
    if (vkCreateShaderModule(logical_device_handle, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
    {
        throw std::runtime_error("shader module creation failed");
    }
    shader_module_handles.push_back(shader_module);

    return shader_module;
}


uint32_t application::get_memory_type_index(uint32_t type_bits, VkMemoryPropertyFlags memory_property_flags)
{
    for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; i++)
    {
        if ((type_bits & 1) == 1)
        {
            if ((physical_device_memory_properties.memoryTypes[i].propertyFlags & memory_property_flags) == memory_property_flags)
            {
                return i;
            }
        }
        type_bits >>= 1;
    }
    throw std::runtime_error("memory type not found");
}

void application::main_loop()
{
    // 1. handle user inputs first because it depends on nothing
    // 2. update objects because they depend on user inputs
    // 3. process physics because they depend on the new updated objects
    // 4. render scene because it depends on the latest physics state and object updates
    // 5. render UI because it depends on the scene already rendered

    frame_start = std::chrono::steady_clock::now();

    // CPU region
	{
        glfwPollEvents();
    }
    cpu_end = std::chrono::steady_clock::now();

    // GPU region
	{
		render();
	}

    // measure performance
	{
		frame_end = std::chrono::steady_clock::now();
		frame_time = std::chrono::duration_cast<std::chrono::duration<float>>(frame_end - frame_start).count();
		cpu_time = std::chrono::duration_cast<std::chrono::duration<float>>(cpu_end - frame_start).count();
		gpu_time = std::chrono::duration_cast<std::chrono::duration<float>>(frame_end - cpu_end).count();
		std::stringstream ss;
		ss.precision(3);
		ss.setf(std::ios_base::fixed, std::ios_base::floatfield);
		ss << "frame #" << frame_number << "|simulation_time(sec):" << time_step * frame_number << "|particle_count:" << particle_count << "|fps:" << 1.f / frame_time << "|frame_time(ms):" << frame_time * 1000 << "|cpu_time(ms)" << cpu_time * 1000 << "|gpu_time(ms)" << gpu_time * 1000 << "|vsync:off";
		glfwSetWindowTitle(window, ss.str().c_str());

		frame_number++;
	}
}

void application::render()
{

    // submit compute command buffer
	{
		VkSubmitInfo compute_submit_info;
		compute_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		compute_submit_info.pNext = nullptr;
		compute_submit_info.waitSemaphoreCount = 0;
		compute_submit_info.pWaitSemaphores = nullptr;
		compute_submit_info.pWaitDstStageMask = 0;
		compute_submit_info.commandBufferCount = 1;
		compute_submit_info.pCommandBuffers = &compute_command_buffer_handle;
		compute_submit_info.signalSemaphoreCount = 0;
		compute_submit_info.pSignalSemaphores = nullptr;
		if (vkQueueSubmit(compute_queue_handle, 1, &compute_submit_info, compute_fence_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("compute queue submission failed");
		}
	}

    vkWaitForFences(logical_device_handle, 1, &compute_fence_handle, VK_TRUE, UINT64_MAX);
    vkResetFences(logical_device_handle, 1, &compute_fence_handle);

    // sumbit graphics command buffer
	{
		uint32_t image_index;
		VkResult result = vkAcquireNextImageKHR(logical_device_handle, swap_chain_handle, UINT64_MAX, image_available_semaphore_handle, VK_NULL_HANDLE, &image_index);
		switch (result)
		{
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
		case VK_ERROR_OUT_OF_DATE_KHR:
			break;
		default:
			throw std::runtime_error("image acquisition failed");
		}

		VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submit_info;
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pNext = nullptr;
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &image_available_semaphore_handle;
		submit_info.pWaitDstStageMask = &wait_dst_stage_mask;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &graphics_command_buffer_handles[image_index];
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &render_finished_semaphore_handle;

		if (vkQueueSubmit(graphics_queue_handle, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			std::runtime_error("graphics queue submission failed");
		}

		// queue the image for presentation
		{
			VkPresentInfoKHR present_info;
			present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			present_info.pNext = nullptr;
			present_info.waitSemaphoreCount = 1;
			present_info.pWaitSemaphores = &render_finished_semaphore_handle;
			present_info.swapchainCount = 1;
			present_info.pSwapchains = &swap_chain_handle;
			present_info.pImageIndices = &image_index;
			present_info.pResults = nullptr;

			result = vkQueuePresentKHR(graphics_queue_handle, &present_info);

			switch (result)
			{
			case VK_SUCCESS:
			case VK_ERROR_OUT_OF_DATE_KHR:
			case VK_SUBOPTIMAL_KHR:
				break;
			default:
				throw std::runtime_error("image acquisition failed");
			}
		}
    }

}

} // namespace sph