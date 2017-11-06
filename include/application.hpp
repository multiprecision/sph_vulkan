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
#include <atomic>

// constants
#define SPH_NUM_PARTICLES 20000
#define SPH_PARTICLE_RADIUS 0.005f

#define SPH_WORK_GROUP_SIZE 128
// work group count is the ceiling of particle count divided by work group size
#define SPH_NUM_WORK_GROUPS ((SPH_NUM_PARTICLES + SPH_WORK_GROUP_SIZE - 1) / SPH_WORK_GROUP_SIZE)

namespace sph
{

class application
{
public:
    application();
    explicit application(int64_t scene_id);
    application(const application&) = delete;
    ~application();
    void run();

private:
    void initialize_window();
    void initialize_vulkan();

    void destroy_window();
    void destroy_vulkan();

    void main_loop();
    void run_simulation();
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
    void create_semaphores();

    void create_compute_descriptor_set_layout();
    void update_compute_descriptor_sets();
    void create_compute_pipeline_layout();
    void create_compute_pipelines();
    void create_compute_command_pool();
    void create_compute_command_buffer();

    void set_initial_particle_data();

    GLFWwindow* window = nullptr;
    uint32_t window_height = 1000;
    uint32_t window_width = 1000;
    
    std::atomic_uint64_t frame_number = 1;
    double frame_time = 0;

    bool paused = false;
    uint64_t scene_id = 0;

    // vulkan resources
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

    VkBuffer packed_particles_buffer_handle = VK_NULL_HANDLE;
    VkDeviceMemory packed_particles_memory_handle = VK_NULL_HANDLE;

    // synchronization
    VkSemaphore image_available_semaphore_handle = VK_NULL_HANDLE;
    VkSemaphore render_finished_semaphore_handle = VK_NULL_HANDLE;

    // helper functions
    VkShaderModule create_shader_module_from_file(std::string path_to_file);
    // get index to the memory type
    uint32_t get_memory_type_index(uint32_t type, VkMemoryPropertyFlags memory_property_flags);

    // rendering routine
    VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    uint32_t image_index;
    VkSubmitInfo compute_submit_info
    {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        0,
        nullptr,
        0,
        1,
        &compute_command_buffer_handle,
        0,
        nullptr
    };
    VkSubmitInfo graphics_submit_info
    {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        1,
        &image_available_semaphore_handle,
        &wait_dst_stage_mask,
        1,
        VK_NULL_HANDLE,
        1,
        &render_finished_semaphore_handle
    };
    VkPresentInfoKHR present_info
    {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        nullptr,
        1,
        &render_finished_semaphore_handle,
        1,
        &swap_chain_handle,
        &image_index,
        nullptr
    };
    // ssbo sizes
    const uint64_t position_ssbo_size = sizeof(glm::vec2) * SPH_NUM_PARTICLES;
    const uint64_t velocity_ssbo_size = sizeof(glm::vec2) * SPH_NUM_PARTICLES;
    const uint64_t force_ssbo_size = sizeof(glm::vec2) * SPH_NUM_PARTICLES;
    const uint64_t density_ssbo_size = sizeof(float) * SPH_NUM_PARTICLES;
    const uint64_t pressure_ssbo_size = sizeof(float) * SPH_NUM_PARTICLES;

    const uint64_t packed_buffer_size = position_ssbo_size + velocity_ssbo_size + force_ssbo_size + density_ssbo_size + pressure_ssbo_size;
    // ssbo offsets
    const uint64_t position_ssbo_offset = 0;
    const uint64_t velocity_ssbo_offset = position_ssbo_size;
    const uint64_t force_ssbo_offset = velocity_ssbo_offset + velocity_ssbo_size;
    const uint64_t density_ssbo_offset = force_ssbo_offset + force_ssbo_size;
    const uint64_t pressure_ssbo_offset = density_ssbo_offset + density_ssbo_size;
};

} // namespace sph