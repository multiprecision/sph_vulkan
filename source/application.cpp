// Copyright (c) 2017-2018, Samuel Ivan Gunadi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "application.hpp"

#include <cmath>
#include <cstring>
#include <string>
#include <algorithm>
#include <exception>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <thread>

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT obj_type, uint64_t obj, size_t location, int32_t code, const char* layer_prefix, const char* msg, void*)
{
	std::string tags;

	switch (flags)
	{
	case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		tags += "[ERROR]";
		break;
	case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		tags += "[WARN]";
		break;
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		tags += "[PERF]";
		break;
	case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		tags += "[INFO]";
		break;
	case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		tags += "[DEBUG]";
		break;
	default:
		tags += "[?]";
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
	default:
		tags += "[?]";
	}

	std::cout << tags << "[" << obj << "][" << location << "][" << code << "][" << layer_prefix << "] " << msg << std::endl;

	return VK_FALSE;
}

namespace sph
{

	application::application()
	{
		initialize_window();
		initialize_vulkan();
	}

	application::application(int64_t scene_id)
	{
		this->scene_id = scene_id;
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
		vkFreeCommandBuffers(logical_device_handle, compute_command_pool_handle, 1, &compute_command_buffer_handle);
		vkDestroyCommandPool(logical_device_handle, compute_command_pool_handle, NULL);
		vkDestroyDescriptorSetLayout(logical_device_handle, compute_descriptor_set_layout_handle, NULL);
		vkDestroyPipelineLayout(logical_device_handle, compute_pipeline_layout_handle, NULL);
		vkDestroyPipeline(logical_device_handle, compute_pipeline_handles[0], NULL);
		vkDestroyPipeline(logical_device_handle, compute_pipeline_handles[1], NULL);
		vkDestroyPipeline(logical_device_handle, compute_pipeline_handles[2], NULL);
		vkDestroySemaphore(logical_device_handle, render_finished_semaphore_handle, NULL);
		vkDestroySemaphore(logical_device_handle, image_available_semaphore_handle, NULL);
		for (const auto& handle : graphics_command_buffer_handles)
		{
			vkFreeCommandBuffers(logical_device_handle, graphics_command_pool_handle, 1, &handle);
		}
		vkDestroyCommandPool(logical_device_handle, graphics_command_pool_handle, NULL);
		vkDestroyPipeline(logical_device_handle, graphics_pipeline_handle, NULL);
		for (const auto& handle : shader_module_handles)
		{
			vkDestroyShaderModule(logical_device_handle, handle, NULL);
		}
		vkDestroyPipelineLayout(logical_device_handle, graphics_pipeline_layout_handle, NULL);
		for (const auto& handle : swapchain_frame_buffer_handles)
		{
			vkDestroyFramebuffer(logical_device_handle, handle, NULL);
		}

		vkDestroyBuffer(logical_device_handle, packed_particles_buffer_handle, NULL);
		vkFreeMemory(logical_device_handle, packed_particles_memory_handle, NULL);

		vkDestroyDescriptorPool(logical_device_handle, global_descriptor_pool_handle, NULL);

		vkDestroyPipelineCache(logical_device_handle, global_pipeline_cache_handle, NULL);

		vkDestroyRenderPass(logical_device_handle, render_pass_handle, NULL);
		for (const auto& handle : swapchain_image_view_handles)
		{
			vkDestroyImageView(logical_device_handle, handle, NULL);
		}
		vkDestroySwapchainKHR(logical_device_handle, swapchain_handle, NULL);
		vkDestroySurfaceKHR(instance_handle, surface_handle, NULL);
		vkDestroyDevice(logical_device_handle, NULL);
#ifdef _DEBUG
		reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance_handle, "vkDestroyDebugReportCallbackEXT"))(instance_handle, debug_report_callback_handle, NULL);
#endif _DEBUG
		vkDestroyInstance(instance_handle, NULL);
	}

	void application::run()
	{
		// to measure performance
		std::thread
		(
			[this]()
			{
				std::this_thread::sleep_for(std::chrono::seconds(20));
				std::cout << "[INFO] frame count after 20 seconds after setup (do not pause or move the window): " << frame_number << std::endl;
			}
		).detach();

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
		window = glfwCreateWindow(1000, 1000, "", NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			throw std::runtime_error("window creation failed");
		}
		glfwMakeContextCurrent(window);
		glfwSwapInterval(0);
		// pass Application pointer to the callback using GLFW user pointer
		glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));
		// set key callback
		auto key_callback = [](GLFWwindow* window, int key, int, int action, int)
		{
			auto app_ptr = reinterpret_cast<sph::application*>(glfwGetWindowUserPointer(window));
			if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			{
				app_ptr->paused = !app_ptr->paused;
			}
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
		};

		glfwSetKeyCallback(window, key_callback);
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

		create_swapchain();
		get_swapchain_images();
		create_swapchain_image_views();
		create_render_pass();
		create_swapchain_frame_buffers();

		create_pipeline_cache();
		create_descriptor_pool();
		create_buffers();

		create_graphics_pipeline_layout();
		create_graphics_pipeline();
		create_graphics_command_pool();
		create_graphics_command_buffers();
		create_semaphores();

		create_compute_descriptor_set_layout();
		update_compute_descriptor_sets();
		create_compute_pipeline_layout();
		create_compute_pipelines();
		create_compute_command_pool();
		create_compute_command_buffer();

		set_initial_particle_data();
	}


	void application::create_instance()
	{
		VkApplicationInfo vk_app_info
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			NULL,
			"SPH Simulation Vulkan",
			VK_MAKE_VERSION(1, 0, 0),
			"Wonderful SPH Simulation Engine",
			VK_MAKE_VERSION(1, 0, 0),
			VK_API_VERSION_1_3
		};
		uint32_t instance_layer_count;
		vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
		std::vector<VkLayerProperties> available_instance_layers(instance_layer_count);
		vkEnumerateInstanceLayerProperties(&instance_layer_count, available_instance_layers.data());
		std::cout << "[INFO] available vulkan layers:" << std::endl;
		for (const auto& layer : available_instance_layers)
		{
			std::cout << "[INFO]     name: " << layer.layerName << " desc: " << layer.description << " impl_ver: "
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
		vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
		std::vector<VkExtensionProperties> available_instance_extensions(instance_extension_count);
		vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, available_instance_extensions.data());
		std::cout << "[INFO] available vulkan extensions:" << std::endl;
		for (const auto& extension : available_instance_extensions)
		{
			std::cout << "[INFO]     name: " << extension.extensionName << " spec_ver: "
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
		const char* validation_layer_names = "VK_LAYER_KHRONOS_validation";
#endif

		VkInstanceCreateInfo instance_create_info
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			NULL,
			0,
			&vk_app_info,
	#ifdef _DEBUG
			1,
			&validation_layer_names,
	#else
			0,
			NULL,
	#endif
			static_cast<uint32_t>(instance_extensions.size()),
			instance_extensions.data()
		};
		if (vkCreateInstance(&instance_create_info, NULL, &instance_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("vulkan instance creation failed");
		}
	}


	void application::create_debug_callback()
	{
		VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info
		{
			VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
			NULL,
			VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT,
			vulkan_debug_callback,
			NULL
		};
		if (reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance_handle, "vkCreateDebugReportCallbackEXT"))(instance_handle, &debug_report_callback_create_info, NULL, &debug_report_callback_handle) != VK_SUCCESS)
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
		vkEnumeratePhysicalDevices(instance_handle, &physical_device_count, NULL);
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
		vkEnumerateDeviceExtensionProperties(physical_device_handle, NULL, &device_extension_count, NULL);
		physical_device_extensions.resize(device_extension_count);
		vkEnumerateDeviceExtensionProperties(physical_device_handle, NULL, &device_extension_count, physical_device_extensions.data());
		// get memory properties
		vkGetPhysicalDeviceMemoryProperties(physical_device_handle, &physical_device_memory_properties);
		// print info
		std::cout << "[INFO] selected device name: " << physical_device_properties.deviceName << std::endl
			<< "[INFO] selected device type: ";
		switch (physical_device_properties.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			std::cout << "VK_PHYSICAL_DEVICE_TYPE_OTHER";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			std::cout << "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			std::cout << "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			std::cout << "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			std::cout << "VK_PHYSICAL_DEVICE_TYPE_CPU";
			break;
		default:
			;
		}
		std::cout << " (" << physical_device_properties.deviceType << ")" << std::endl
			<< "[INFO] selected device driver version: "
			<< VK_VERSION_MAJOR(physical_device_properties.driverVersion) << "."
			<< VK_VERSION_MINOR(physical_device_properties.driverVersion) << "."
			<< VK_VERSION_PATCH(physical_device_properties.driverVersion) << std::endl
			<< "[INFO] selected device vulkan api version: "
			<< VK_VERSION_MAJOR(physical_device_properties.apiVersion) << "."
			<< VK_VERSION_MINOR(physical_device_properties.apiVersion) << "."
			<< VK_VERSION_PATCH(physical_device_properties.apiVersion) << std::endl;
		std::cout << "[INFO] selected device available extensions:" << std::endl;
		for (const auto& extension : physical_device_extensions)
		{
			std::cout << "[INFO]     name: " << extension.extensionName << " spec_ver: "
				<< VK_VERSION_MAJOR(extension.specVersion) << "."
				<< VK_VERSION_MINOR(extension.specVersion) << "."
				<< VK_VERSION_PATCH(extension.specVersion) << std::endl;
		}
	}

	void application::create_logical_device()
	{
		graphics_presentation_compute_queue_family_index = UINT32_MAX;

		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device_handle, &queue_family_count, NULL);
		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device_handle, &queue_family_count, queue_families.data());
		std::cout << "[INFO] available queue families:" << std::endl;
		// look for queue family indices
		for (uint32_t index = 0; index < queue_families.size(); index++)
		{
			std::cout << "[INFO]     flags: ";
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
		const float queue_priorities[3]{ 1, 1, 1 };
		VkDeviceQueueCreateInfo queue_create_info
		{
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			NULL,
			0,
			graphics_presentation_compute_queue_family_index,
			3, // 3 queues: 1 graphics queue, 1 compute queue, and 1 presentation queue
			queue_priorities
		};

		const char* enabled_extensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
		VkDeviceCreateInfo device_create_info
		{
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			NULL,
			0,
			1,
			&queue_create_info,
			0,
			NULL,
			1,
			&enabled_extensions,
			NULL
		};
		if (vkCreateDevice(physical_device_handle, &device_create_info, NULL, &logical_device_handle) != VK_SUCCESS)
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

	void application::create_swapchain()
	{
		VkPresentModeKHR swapchain_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		VkExtent2D extent = { window_width, window_height };

		// G.5.1. Query the surface capabilities and select the swapchain's extent (width, height).
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		{
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_handle, surface_handle, &surfaceCapabilities);

			if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
				extent = surfaceCapabilities.currentExtent;
			}
		}

		// G.5.2. Select a surface format.
		{
			uint32_t format_count;
			vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_handle, surface_handle, &format_count, NULL);

			std::vector<VkSurfaceFormatKHR> surface_formats;
			surface_formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_handle, surface_handle, &format_count, surface_formats.data());

			for (VkSurfaceFormatKHR entry : surface_formats) {
				if ((entry.format == VK_FORMAT_B8G8R8A8_SRGB) && (entry.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
					surface_format = entry;
					break;
				}
			}
		}

		// For better performance, use "min + 1";
		uint32_t image_count = surfaceCapabilities.minImageCount + 1;
		VkSwapchainCreateInfoKHR create_info;
		{
			create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			create_info.pNext = NULL;
			create_info.flags = 0;
			create_info.surface = surface_handle;
			create_info.minImageCount = image_count;
			create_info.imageFormat = surface_format.format;
			create_info.imageColorSpace = surface_format.colorSpace;
			create_info.imageExtent = extent;
			create_info.imageArrayLayers = 1;
			create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			// If the graphics and presentation queue is different this should not be exclusive.
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = NULL;
			create_info.preTransform = surfaceCapabilities.currentTransform;
			create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			create_info.presentMode = swapchain_present_mode;
			create_info.clipped = VK_TRUE;
			create_info.oldSwapchain = VK_NULL_HANDLE;
		}

		if (vkCreateSwapchainKHR(logical_device_handle, &create_info, NULL, &swapchain_handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}
	}

	void application::get_swapchain_images()
	{
		uint32_t swap_chain_image_count;
		vkGetSwapchainImagesKHR(logical_device_handle, swapchain_handle, &swap_chain_image_count, NULL);
		swapchain_image_handles.resize(swap_chain_image_count);
		vkGetSwapchainImagesKHR(logical_device_handle, swapchain_handle, &swap_chain_image_count, swapchain_image_handles.data());
	}

	void application::create_swapchain_image_views()
	{
		swapchain_image_view_handles.resize(swapchain_image_handles.size());
		for (uint32_t i = 0; i < swapchain_image_view_handles.size(); i++)
		{
			VkImageViewCreateInfo image_view_create_info
			{
				VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				NULL,
				0,
				swapchain_image_handles[i],
				VK_IMAGE_VIEW_TYPE_2D,
				surface_format.format,
				{
					VK_COMPONENT_SWIZZLE_IDENTITY, // r
					VK_COMPONENT_SWIZZLE_IDENTITY, // g
					VK_COMPONENT_SWIZZLE_IDENTITY, // b
					VK_COMPONENT_SWIZZLE_IDENTITY // a
				},
				{
					VK_IMAGE_ASPECT_COLOR_BIT, // aspectMask
					0, // baseMipLevel
					1, // levelCount
					0, // baseArrayLayer
					1, // layerCount
				}
			};
			if (vkCreateImageView(logical_device_handle, &image_view_create_info, NULL, &swapchain_image_view_handles[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("image views creation failed");
			}
		}
	}

	void application::create_render_pass()
	{
		VkAttachmentDescription attachment_description
		{
			0,
			surface_format.format,
			VK_SAMPLE_COUNT_1_BIT,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		VkAttachmentReference color_attachment_reference
		{
			0,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpass_description
		{
			0,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			0,
			NULL,
			1,
			&color_attachment_reference,
			NULL,
			NULL,
			0,
			NULL
		};

		VkRenderPassCreateInfo render_pass_create_info
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			NULL,
			0,
			1,
			&attachment_description,
			1,
			&subpass_description,
			0,
			NULL
		};
		if (vkCreateRenderPass(logical_device_handle, &render_pass_create_info, NULL, &render_pass_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("render pass creation failed");
		}
	}

	void application::create_swapchain_frame_buffers()
	{
		swapchain_frame_buffer_handles.resize(swapchain_image_view_handles.size());

		for (size_t index = 0; index < swapchain_image_view_handles.size(); index++)
		{
			VkFramebufferCreateInfo framebuffer_create_info
			{
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				NULL,
				0,
				render_pass_handle,
				1,
				&swapchain_image_view_handles[index],
				window_width,
				window_height,
				1
			}; if (vkCreateFramebuffer(logical_device_handle, &framebuffer_create_info, NULL, &swapchain_frame_buffer_handles[index]) != VK_SUCCESS)
			{
				throw std::runtime_error("frame buffer creation failed");
			}

		}
	}

	void application::create_descriptor_pool()
	{
		VkDescriptorPoolSize descriptor_pool_size
		{
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			5
		};

		VkDescriptorPoolCreateInfo descriptor_pool_create_info
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			NULL,
			0,
			1,
			1,
			&descriptor_pool_size
		};
		if (vkCreateDescriptorPool(logical_device_handle, &descriptor_pool_create_info, NULL, &global_descriptor_pool_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("descriptor pool creation failed");
		}
	}

	void application::create_pipeline_cache()
	{
		VkPipelineCacheCreateInfo pipeline_cache_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
			NULL,
			0,
			0,
			NULL
		};
		if (vkCreatePipelineCache(logical_device_handle, &pipeline_cache_create_info, NULL, &global_pipeline_cache_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("pipeline cache creation failed");
		}
	}

	void application::create_buffers()
	{
		VkBufferCreateInfo packed_particles_buffer_create_info
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			NULL,
			0,
			packed_buffer_size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			NULL
		};
		vkCreateBuffer(logical_device_handle, &packed_particles_buffer_create_info, NULL, &packed_particles_buffer_handle);
		VkMemoryRequirements position_buffer_memory_requirements;
		vkGetBufferMemoryRequirements(logical_device_handle, packed_particles_buffer_handle, &position_buffer_memory_requirements);
		VkMemoryAllocateInfo particle_buffer_memory_allocation_info
		{
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			NULL,
			position_buffer_memory_requirements.size,
			get_memory_type_index(position_buffer_memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
		};
		if (vkAllocateMemory(logical_device_handle, &particle_buffer_memory_allocation_info, NULL, &packed_particles_memory_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("memory allocation failed");
		}
		// bind the memory to the buffer object
		vkBindBufferMemory(logical_device_handle, packed_particles_buffer_handle, packed_particles_memory_handle, 0);
	}

	void application::set_initial_particle_data()
	{
		// staging buffer
		VkBuffer staging_buffer_handle = VK_NULL_HANDLE;
		VkDeviceMemory staging_buffer_memory_device_handle = VK_NULL_HANDLE;

		VkBufferCreateInfo staging_buffer_create_info
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			NULL,
			0,
			packed_buffer_size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			NULL
		};
		vkCreateBuffer(logical_device_handle, &staging_buffer_create_info, NULL, &staging_buffer_handle);

		VkMemoryRequirements staging_buffer_memory_requirements;
		vkGetBufferMemoryRequirements(logical_device_handle, staging_buffer_handle, &staging_buffer_memory_requirements);

		VkMemoryAllocateInfo staging_buffer_memory_allocation_info
		{
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			NULL,
			staging_buffer_memory_requirements.size,
			get_memory_type_index(staging_buffer_memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
		};
		if (vkAllocateMemory(logical_device_handle, &staging_buffer_memory_allocation_info, NULL, &staging_buffer_memory_device_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("memory allocation failed");
		}
		// bind the memory to the buffer object
		vkBindBufferMemory(logical_device_handle, staging_buffer_handle, staging_buffer_memory_device_handle, 0);

		void* mapped_memory = NULL;
		vkMapMemory(logical_device_handle, staging_buffer_memory_device_handle, 0, staging_buffer_memory_requirements.size, 0, &mapped_memory);

		// set the initial particles data
		std::vector<glm::vec2> initial_particle_position(SPH_NUM_PARTICLES);

		// test case 1: dropping a cube of water
		if (scene_id == 0)
		{
			for (auto i = 0, x = 0, y = 0; i < SPH_NUM_PARTICLES; i++)
			{
				initial_particle_position[i].x = -0.625f + SPH_PARTICLE_RADIUS * 2 * x;
				initial_particle_position[i].y = -1 + SPH_PARTICLE_RADIUS * 2 * y;
				x++;
				if (x >= 125)
				{
					x = 0;
					y++;
				}
			}
		}
		// test case 2: dam break
		else
		{
			for (auto i = 0, x = 0, y = 0; i < SPH_NUM_PARTICLES; i++)
			{
				initial_particle_position[i].x = -1 + SPH_PARTICLE_RADIUS * 2 * x;
				initial_particle_position[i].y = 1 - SPH_PARTICLE_RADIUS * 2 * y;
				x++;
				if (x >= 100)
				{
					x = 0;
					y++;
				}
			}
		}
		// zero all 
		std::memset(mapped_memory, 0, packed_buffer_size);
		std::memcpy(mapped_memory, initial_particle_position.data(), position_ssbo_size);
		vkUnmapMemory(logical_device_handle, staging_buffer_memory_device_handle);

		// submit a command buffer to copy staging buffer to the particle buffer 
		VkCommandBuffer copy_command_buffer_handle;
		VkCommandBufferAllocateInfo copy_command_buffer_allocation_info
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			NULL,
			compute_command_pool_handle,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1
		};
		if (vkAllocateCommandBuffers(logical_device_handle, &copy_command_buffer_allocation_info, &copy_command_buffer_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("command buffer creation failed");
		}

		VkCommandBufferBeginInfo command_buffer_begin_info
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			NULL,
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			NULL
		};
		if (vkBeginCommandBuffer(copy_command_buffer_handle, &command_buffer_begin_info) != VK_SUCCESS)
		{
			throw std::runtime_error("command buffer begin failed");
		}
		VkBufferCopy buffer_copy_region
		{
			0,
			0,
			staging_buffer_memory_requirements.size
		};
		vkCmdCopyBuffer(copy_command_buffer_handle, staging_buffer_handle, packed_particles_buffer_handle, 1, &buffer_copy_region);

		if (vkEndCommandBuffer(copy_command_buffer_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("command buffer end failed");
		}

		VkSubmitInfo copy_submit_info
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			NULL,
			0,
			NULL,
			0,
			1,
			&copy_command_buffer_handle,
			0,
			NULL
		};
		if (vkQueueSubmit(compute_queue_handle, 1, &copy_submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			throw std::runtime_error("command buffer submission failed");
		}
		if (vkQueueWaitIdle(compute_queue_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("vkQueueWaitIdle failed");
		}

		vkFreeCommandBuffers(logical_device_handle, compute_command_pool_handle, 1, &copy_command_buffer_handle);
		vkFreeMemory(logical_device_handle, staging_buffer_memory_device_handle, NULL);
		vkDestroyBuffer(logical_device_handle, staging_buffer_handle, NULL);
	}

	void application::create_compute_descriptor_set_layout()
	{
		// create descriptor layout
		const VkDescriptorSetLayoutBinding descriptor_set_layout_bindings[]
		{
			{
				0,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_COMPUTE_BIT,
				NULL
			},
			{
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_COMPUTE_BIT,
				NULL
			},
			{
				2,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_COMPUTE_BIT,
				NULL
			},
			{
				3,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_COMPUTE_BIT,
				NULL
			},
			{
				4,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_COMPUTE_BIT,
				NULL
			},
		};

		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			NULL,
			0,
			5,
			descriptor_set_layout_bindings
		};
		if (vkCreateDescriptorSetLayout(logical_device_handle, &descriptor_set_layout_create_info, NULL, &compute_descriptor_set_layout_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("compute descriptor layout creation failed");
		}
	}

	void application::update_compute_descriptor_sets()
	{
		// allocate descriptor sets
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			NULL,
			global_descriptor_pool_handle,
			1,
			&compute_descriptor_set_layout_handle
		};
		if (vkAllocateDescriptorSets(logical_device_handle, &descriptor_set_allocate_info, &compute_descriptor_set_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("compute descriptor set allocation failed");
		}
		VkDescriptorBufferInfo descriptor_buffer_infos[]
		{
			{
				packed_particles_buffer_handle,
				position_ssbo_offset,
				position_ssbo_size
			},
			{
				packed_particles_buffer_handle,
				velocity_ssbo_offset,
				velocity_ssbo_size
			},
			{
				packed_particles_buffer_handle,
				force_ssbo_offset,
				force_ssbo_size
			},
			{
				packed_particles_buffer_handle,
				density_ssbo_offset,
				density_ssbo_size
			},
			{
				packed_particles_buffer_handle,
				pressure_ssbo_offset,
				pressure_ssbo_size
			}
		};
		// write descriptor sets
		VkWriteDescriptorSet write_descriptor_sets[]
		{
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				NULL,
				compute_descriptor_set_handle,
				0,
				0,
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				VK_NULL_HANDLE,
				&descriptor_buffer_infos[0],
				VK_NULL_HANDLE
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				NULL,
				compute_descriptor_set_handle,
				1,
				0,
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				VK_NULL_HANDLE,
				&descriptor_buffer_infos[1],
				VK_NULL_HANDLE
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				NULL,
				compute_descriptor_set_handle,
				2,
				0,
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				VK_NULL_HANDLE,
				&descriptor_buffer_infos[2],
				VK_NULL_HANDLE
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				NULL,
				compute_descriptor_set_handle,
				3,
				0,
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				VK_NULL_HANDLE,
				&descriptor_buffer_infos[3],
				VK_NULL_HANDLE
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				NULL,
				compute_descriptor_set_handle,
				4,
				0,
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				VK_NULL_HANDLE,
				&descriptor_buffer_infos[4],
				VK_NULL_HANDLE
			}
		};
		vkUpdateDescriptorSets(logical_device_handle, 5, write_descriptor_sets, 0, NULL);
	}

	void application::create_compute_pipeline_layout()
	{
		// create pipeline layout
		VkPipelineLayoutCreateInfo pipeline_layout_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			NULL,
			0,
			1,
			&compute_descriptor_set_layout_handle,
			0,
			NULL
		};
		if (vkCreatePipelineLayout(logical_device_handle, &pipeline_layout_create_info, NULL, &compute_pipeline_layout_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("compute pipeline layout creation failed");
		}
	}


	void application::create_compute_pipelines()
	{
		// create pipelines
		// first
		VkShaderModule compute_density_pressure_shader_module = create_shader_module_from_file("compute_density_pressure.comp.spv");

		VkPipelineShaderStageCreateInfo compute_shader_stage_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			NULL,
			0,
			VK_SHADER_STAGE_COMPUTE_BIT,
			compute_density_pressure_shader_module,
			"main",
			NULL
		};

		VkComputePipelineCreateInfo compute_pipeline_create_info
		{
			VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			NULL,
			0,
			compute_shader_stage_create_info,
			compute_pipeline_layout_handle,
			VK_NULL_HANDLE,
			0
		};

		if (vkCreateComputePipelines(logical_device_handle, global_pipeline_cache_handle, 1, &compute_pipeline_create_info, NULL, &compute_pipeline_handles[0]) != VK_SUCCESS)
		{
			throw std::runtime_error("first compute pipeline creation failed");
		}

		// second
		VkShaderModule compute_force_shader_module = create_shader_module_from_file("compute_force.comp.spv");
		compute_shader_stage_create_info.module = compute_force_shader_module;
		compute_pipeline_create_info.stage = compute_shader_stage_create_info;

		if (vkCreateComputePipelines(logical_device_handle, global_pipeline_cache_handle, 1, &compute_pipeline_create_info, NULL, &compute_pipeline_handles[1]) != VK_SUCCESS)
		{
			throw std::runtime_error("second compute pipeline creation failed");
		}

		// third
		VkShaderModule integrate_shader_module = create_shader_module_from_file("integrate.comp.spv");
		compute_shader_stage_create_info.module = integrate_shader_module;
		compute_pipeline_create_info.stage = compute_shader_stage_create_info;

		if (vkCreateComputePipelines(logical_device_handle, global_pipeline_cache_handle, 1, &compute_pipeline_create_info, NULL, &compute_pipeline_handles[2]) != VK_SUCCESS)
		{
			throw std::runtime_error("third compute pipeline creation failed");
		}
	}


	void application::create_compute_command_pool()
	{
		// create compute command pool
		VkCommandPoolCreateInfo command_pool_create_info
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			NULL,
			VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			graphics_presentation_compute_queue_family_index
		};
		if (vkCreateCommandPool(logical_device_handle, &command_pool_create_info, NULL, &compute_command_pool_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("command pool creation failed");
		}
	}

	void application::create_compute_command_buffer()
	{
		// allocate command buffer
		VkCommandBufferAllocateInfo command_buffer_allocate_info
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			NULL,
			compute_command_pool_handle,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1
		};
		if (vkAllocateCommandBuffers(logical_device_handle, &command_buffer_allocate_info, &compute_command_buffer_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("buffer allocation failed");
		}

		// build command buffer
		VkCommandBufferBeginInfo command_buffer_begin_info
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			NULL,
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			NULL
		};
		if (vkBeginCommandBuffer(compute_command_buffer_handle, &command_buffer_begin_info) != VK_SUCCESS)
		{
			throw std::runtime_error("command buffer begin failed");
		}

		vkCmdBindDescriptorSets(compute_command_buffer_handle, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_layout_handle, 0, 1, &compute_descriptor_set_handle, 0, NULL);

		// First dispatch
		vkCmdBindPipeline(compute_command_buffer_handle, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_handles[0]);
		vkCmdDispatch(compute_command_buffer_handle, SPH_NUM_WORK_GROUPS, 1, 1);

		// Barrier: compute to compute dependencies
		// First dispatch writes to a storage buffer, second dispatch reads from that storage buffer
		vkCmdPipelineBarrier(compute_command_buffer_handle, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, NULL, 0, NULL, 0, NULL);

		// Second dispatch
		vkCmdBindPipeline(compute_command_buffer_handle, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_handles[1]);
		vkCmdDispatch(compute_command_buffer_handle, SPH_NUM_WORK_GROUPS, 1, 1);

		// Barrier: compute to compute dependencies
		// Second dispatch writes to a storage buffer, third dispatch reads from that storage buffer
		vkCmdPipelineBarrier(compute_command_buffer_handle, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, NULL, 0, NULL, 0, NULL);

		// Third dispatch
		// Third dispatch writes to the storage buffer. Later, vkCmdDraw reads that buffer as a vertex buffer with vkCmdBindVertexBuffers.
		vkCmdBindPipeline(compute_command_buffer_handle, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_handles[2]);
		vkCmdDispatch(compute_command_buffer_handle, SPH_NUM_WORK_GROUPS, 1, 1);

		vkCmdPipelineBarrier(compute_command_buffer_handle, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, NULL, 0, NULL, 0, NULL);

		vkEndCommandBuffer(compute_command_buffer_handle);
	}

	void application::create_graphics_pipeline_layout()
	{
		VkPipelineLayoutCreateInfo pipeline_layout_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			NULL,
			0,
			0,
			NULL,
			0,
			NULL
		};
		if (vkCreatePipelineLayout(logical_device_handle, &pipeline_layout_create_info, NULL, &graphics_pipeline_layout_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("pipeline layout creation failed");
		}
	}

	void application::create_graphics_pipeline()
	{
		std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;

		// create shader stage infos
		VkShaderModule vertex_shader_module = create_shader_module_from_file("particle.vert.spv");

		VkShaderModule fragment_shader_module = create_shader_module_from_file("particle.frag.spv");

		VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			NULL,
			0,
			VK_SHADER_STAGE_VERTEX_BIT,
			vertex_shader_module,
			"main",
			NULL
		};

		VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			NULL,
			0,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			fragment_shader_module,
			"main",
			NULL
		};

		shader_stage_create_infos.push_back(vertex_shader_stage_create_info);
		shader_stage_create_infos.push_back(fragment_shader_stage_create_info);

		VkVertexInputBindingDescription vertex_input_binding_description
		{
			0,
			sizeof(glm::vec2),
			VK_VERTEX_INPUT_RATE_VERTEX
		};

		// layout(location = 0) in vec2 position;
		VkVertexInputAttributeDescription vertex_input_attribute_description
		{
			0,
			0,
			VK_FORMAT_R32G32_SFLOAT,
			0
		};

		VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			NULL,
			0,
			1,
			&vertex_input_binding_description,
			1,
			&vertex_input_attribute_description
		};

		VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			NULL,
			0,
			VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
			VK_FALSE
		};

		VkViewport viewport
		{
			0,
			0,
			static_cast<float>(window_width),
			static_cast<float>(window_height),
			0,
			1
		};

		VkRect2D scissor
		{
			{ 0, 0 },
			{ window_width, window_height }
		};

		VkPipelineViewportStateCreateInfo viewport_state_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			NULL,
			0,
			1,
			&viewport,
			1,
			&scissor
		};

		VkPipelineRasterizationStateCreateInfo rasterization_state_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			NULL,
			0,
			VK_FALSE,
			VK_FALSE,
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE,
			VK_FRONT_FACE_COUNTER_CLOCKWISE,
			VK_FALSE,
			0,
			0,
			0,
			1
		};

		VkPipelineMultisampleStateCreateInfo multisample_state_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			NULL,
			0,
			VK_SAMPLE_COUNT_1_BIT,
			VK_FALSE,
			0,
			NULL,
			VK_FALSE,
			VK_FALSE
		};

		VkPipelineColorBlendAttachmentState color_blend_attachment
		{
			VK_FALSE,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		VkPipelineColorBlendStateCreateInfo color_blend_state_create_info
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			NULL,
			0,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			1,
			&color_blend_attachment,
			{0, 0, 0, 0}
		};

		VkGraphicsPipelineCreateInfo graphics_pipeline_create_info
		{
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			NULL,
			0,
			static_cast<uint32_t>(shader_stage_create_infos.size()),
			shader_stage_create_infos.data(),
			&vertex_input_state_create_info,
			&input_assembly_state_create_info,
			NULL,
			&viewport_state_create_info,
			&rasterization_state_create_info,
			&multisample_state_create_info,
			NULL,
			&color_blend_state_create_info,
			NULL,
			graphics_pipeline_layout_handle,
			render_pass_handle,
			0,
			VK_NULL_HANDLE,
			-1
		};
		if (vkCreateGraphicsPipelines(logical_device_handle, global_pipeline_cache_handle, 1, &graphics_pipeline_create_info, NULL, &graphics_pipeline_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("graphics pipeline creation failed");
		}
	}

	void application::create_graphics_command_pool()
	{
		VkCommandPoolCreateInfo graphics_command_pool_create_info
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			NULL,
			0,
			graphics_presentation_compute_queue_family_index
		};
		if (vkCreateCommandPool(logical_device_handle, &graphics_command_pool_create_info, NULL, &graphics_command_pool_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("command pool creation failed");
		}
	}

	void application::create_graphics_command_buffers()
	{
		graphics_command_buffer_handles.resize(swapchain_frame_buffer_handles.size());

		VkCommandBufferAllocateInfo graphics_command_buffer_allocation_info
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			NULL,
			graphics_command_pool_handle,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			static_cast<uint32_t>(graphics_command_buffer_handles.size())
		};
		if (vkAllocateCommandBuffers(logical_device_handle, &graphics_command_buffer_allocation_info, graphics_command_buffer_handles.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("command buffers allocation failed");
		}

		for (size_t i = 0; i < graphics_command_buffer_handles.size(); i++)
		{
			VkCommandBufferBeginInfo command_buffer_begin_info
			{
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				NULL,
				VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
				NULL
			};
			vkBeginCommandBuffer(graphics_command_buffer_handles[i], &command_buffer_begin_info);

			VkClearValue clear_value{ 0.92f, 0.92f, 0.92f, 1.0f };
			VkRenderPassBeginInfo render_pass_begin_info
			{
				VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
				NULL,
				render_pass_handle,
				swapchain_frame_buffer_handles[i],
				{
					{ 0, 0 },
					{ window_width, window_height }
				},
				1,
				&clear_value
			};
			vkCmdBeginRenderPass(graphics_command_buffer_handles[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport viewport
			{
				0,
				0,
				static_cast<float>(window_width),
				static_cast<float>(window_height),
				0,
				1
			};

			VkRect2D scissor
			{
				{ 0, 0 },
				{ window_width, window_height }
			};

			vkCmdSetViewport(graphics_command_buffer_handles[i], 0, 1, &viewport);
			vkCmdSetScissor(graphics_command_buffer_handles[i], 0, 1, &scissor);
			vkCmdBindPipeline(graphics_command_buffer_handles[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_handle);

			VkDeviceSize offsets = 0;
			vkCmdBindVertexBuffers(graphics_command_buffer_handles[i], 0, 1, &packed_particles_buffer_handle, &offsets);
			vkCmdDraw(graphics_command_buffer_handles[i], SPH_NUM_PARTICLES, 1, 0, 0);

			vkCmdEndRenderPass(graphics_command_buffer_handles[i]);

			if (vkEndCommandBuffer(graphics_command_buffer_handles[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("command buffer creation failed");
			}
		}
	}

	void application::create_semaphores()
	{
		VkSemaphoreCreateInfo semaphore_create_info
		{
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			NULL,
			0
		};
		if (vkCreateSemaphore(logical_device_handle, &semaphore_create_info, NULL, &image_available_semaphore_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("semaphore creation failed");
		}
		if (vkCreateSemaphore(logical_device_handle, &semaphore_create_info, NULL, &render_finished_semaphore_handle) != VK_SUCCESS)
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
		shader_module_create_info.pNext = NULL;
		shader_module_create_info.flags = 0;
		shader_module_create_info.codeSize = shader_code.size();
		shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());
		VkShaderModule shader_module;
		if (vkCreateShaderModule(logical_device_handle, &shader_module_create_info, NULL, &shader_module) != VK_SUCCESS)
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
		static std::chrono::high_resolution_clock::time_point frame_start;
		static std::chrono::high_resolution_clock::time_point frame_end;
		static int64_t total_frame_time_ns;

		frame_start = std::chrono::high_resolution_clock::now();

		// process user inputs
		glfwPollEvents();

		// step through the simulation if not paused
		if (!paused)
		{
			run_simulation();
			frame_number++;
		}

		render();

		frame_end = std::chrono::high_resolution_clock::now();

		// measure performance
		total_frame_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(frame_end - frame_start).count();
		std::stringstream title;
		title.precision(3);
		title.setf(std::ios_base::fixed, std::ios_base::floatfield);
		title << "SPH (Vulkan) | "
			<< SPH_NUM_PARTICLES << " particles | "
			"frame #" << frame_number << " | "
			"render latency: " << 1e-6 * total_frame_time_ns << " ms | "
			"FPS: " << 1.0 / (1e-9 * total_frame_time_ns);
		glfwSetWindowTitle(window, title.str().c_str());
	}

	void application::run_simulation()
	{
		if (vkQueueSubmit(compute_queue_handle, 1, &compute_submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			throw std::runtime_error("compute queue submission failed");
		}
	}

	void application::render()
	{
		// submit graphics command buffer
		vkAcquireNextImageKHR(logical_device_handle, swapchain_handle, UINT64_MAX, image_available_semaphore_handle, VK_NULL_HANDLE, &image_index);
		graphics_submit_info.pCommandBuffers = graphics_command_buffer_handles.data() + image_index;
		if (vkQueueSubmit(graphics_queue_handle, 1, &graphics_submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			throw std::runtime_error("graphics queue submission failed");
		}
		// queue the image for presentation
		vkQueuePresentKHR(presentation_queue_handle, &present_info);

		vkQueueWaitIdle(presentation_queue_handle);
	}

} // namespace sph
