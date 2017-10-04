/*
 * Copyright (c) 2017, Samuel I. Gunadi
 * All rights reserved.
 */

#pragma once

#include <vulkan/vulkan.h>

#include <glfw/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <chrono>
#include <cstdint>
#include <vector>

namespace sph
{
//constants
const float time_step = 0.0001f;
const float particle_length = 0.005f;
const size_t particle_count = 20000;
const size_t work_group_size = 128;
const uint32_t group_count = (size_t)std::ceil((float)particle_count / work_group_size);

class application
{
public:
    application();
    ~application();
    void run();

private:
    void initialize_window();
    void initialize_vulkan();

    void destroy_window();
    void destroy_vulkan();

    void main_loop();
    void render();

    void create_instance();
    void create_debug_callback();
    void create_surface();
    void select_physical_device();
    void create_logical_device();
    void get_device_queues();
    void create_swap_chain();
    void get_swap_chain_images();
    void create_swap_chain_image_views();
    void create_render_pass();
    void create_swap_chain_frame_buffers();

    void create_descriptor_pool();
    void create_pipeline_cache();
    void create_buffers();

    void create_graphics_pipeline_layout();
    void create_graphics_pipeline();
    void create_graphics_command_pool();
    void create_graphics_command_buffers();
    void create_graphics_semaphores();

    void create_compute_descriptor_set_layout();
    void update_compute_descriptor_sets();
    void create_compute_pipeline_layout();
    void create_compute_pipelines();
    void create_compute_command_pool();
    void create_compute_command_buffer();
    void create_compute_fence();

    void initialize_buffers();

    GLFWwindow* window;
    uint32_t window_height = 1000;
    uint32_t window_width = 1000;
    
    std::chrono::steady_clock::time_point frame_start;
    std::chrono::steady_clock::time_point cpu_end;
    std::chrono::steady_clock::time_point frame_end;

    uint64_t frame_number = 0;
    float frame_time = 0;
    float cpu_time = 0;
    float gpu_time = 0;

    // vulkan
    VkInstance instance_handle = VK_NULL_HANDLE;
    VkDebugReportCallbackEXT debug_report_callback_handle = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device_handle = VK_NULL_HANDLE;
    VkPhysicalDeviceFeatures physical_device_features;
    std::vector<VkExtensionProperties> physical_device_extensions;
    VkPhysicalDeviceProperties physical_device_properties;
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    VkDevice logical_device_handle = VK_NULL_HANDLE;
    VkSurfaceKHR surface_handle = VK_NULL_HANDLE;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    std::vector<VkSurfaceFormatKHR> surface_formats;
    std::vector<VkPresentModeKHR> surface_presentation_modes;
    VkSwapchainKHR swap_chain_handle;
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;
    std::vector<VkImage> swap_chain_image_handles;
    std::vector<VkImageView> swap_chain_image_view_handles;
    std::vector<VkFramebuffer> swap_chain_frame_buffer_handles;

    std::vector<VkShaderModule> shader_module_handles;

    VkRenderPass render_pass_handle = VK_NULL_HANDLE;

    uint32_t graphics_presentation_compute_queue_family_index = UINT32_MAX;

    VkQueue presentation_queue_handle = VK_NULL_HANDLE;
    VkQueue graphics_queue_handle = VK_NULL_HANDLE;
    VkQueue compute_queue_handle = VK_NULL_HANDLE;

    VkCommandPool graphics_command_pool_handle = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> graphics_command_buffer_handles;

    VkCommandPool compute_command_pool_handle = VK_NULL_HANDLE;
    VkCommandBuffer compute_command_buffer_handle = VK_NULL_HANDLE;

    VkDescriptorPool global_descriptor_pool_handle = VK_NULL_HANDLE;

    VkDescriptorSetLayout compute_descriptor_set_layout_handle = VK_NULL_HANDLE;
    VkDescriptorSet compute_descriptor_set_handle = VK_NULL_HANDLE;

    VkDescriptorSetLayout graphics_descriptor_set_layout_handle = VK_NULL_HANDLE;
    VkDescriptorSet graphics_descriptor_set_handle = VK_NULL_HANDLE;

    VkPipelineCache global_pipeline_cache_handle = VK_NULL_HANDLE;

    VkPipelineLayout graphics_pipeline_layout_handle = VK_NULL_HANDLE;
    VkPipeline graphics_pipeline_handle = VK_NULL_HANDLE;

    VkPipelineLayout compute_pipeline_layout_handle = VK_NULL_HANDLE;
    VkPipeline compute_pipeline_handles[3] = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE };

    struct particle_t
    {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec2 force;
        float density;
        float pressure;
    };

    VkBuffer particle_buffer_handle = VK_NULL_HANDLE;
    VkDeviceMemory particle_buffer_device_memory_handle = VK_NULL_HANDLE;
    VkDescriptorBufferInfo particle_descriptor_buffer_info = { VK_NULL_HANDLE, 0, 0 };

    // synchronization
    VkFence compute_fence_handle = VK_NULL_HANDLE;
    VkSemaphore image_available_semaphore_handle = VK_NULL_HANDLE;
    VkSemaphore render_finished_semaphore_handle = VK_NULL_HANDLE;

    // helper function

    VkShaderModule create_shader_module_from_file(std::string path_to_file);
    // get index to the memory type
    uint32_t get_memory_type_index(uint32_t type, VkMemoryPropertyFlags memory_property_flags);
};

} // namespace sph