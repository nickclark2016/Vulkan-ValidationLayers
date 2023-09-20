/*
 * Copyright (c) 2015-2023 The Khronos Group Inc.
 * Copyright (c) 2015-2023 Valve Corporation
 * Copyright (c) 2015-2023 LunarG, Inc.
 * Copyright (c) 2015-2023 Google, Inc.
 * Modifications Copyright (C) 2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */

#include "utils/cast_utils.h"
#include "generated/enum_flag_bits.h"
#include "../framework/layer_validation_tests.h"

TEST_F(NegativeSparseImage, BindingImageBufferCreate) {
    TEST_DESCRIPTION("Create buffer/image with sparse attributes but without the sparse_binding bit set");

    ASSERT_NO_FATAL_FAILURE(Init());

    VkBufferCreateInfo buf_info = vku::InitStructHelper();
    buf_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buf_info.size = 2048;
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = NULL;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_device->phy().features().sparseResidencyBuffer) {
        buf_info.flags = VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT;
        CreateBufferTest(*this, &buf_info, "VUID-VkBufferCreateInfo-flags-00918");
    } else {
        GTEST_SKIP() << "Test requires unsupported sparseResidencyBuffer feature";
    }

    if (m_device->phy().features().sparseResidencyAliased) {
        buf_info.flags = VK_BUFFER_CREATE_SPARSE_ALIASED_BIT;
        CreateBufferTest(*this, &buf_info, "VUID-VkBufferCreateInfo-flags-00918");
    } else {
        GTEST_SKIP() << "Test requires unsupported sparseResidencyAliased feature";
    }

    VkImageCreateInfo image_create_info = vku::InitStructHelper();
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_create_info.extent.width = 512;
    image_create_info.extent.height = 64;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices = NULL;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_device->phy().features().sparseResidencyImage2D) {
        image_create_info.flags = VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT;
        CreateImageTest(*this, &image_create_info, "VUID-VkImageCreateInfo-flags-00987");
    } else {
        GTEST_SKIP() << "Test requires unsupported sparseResidencyImage2D feature";
    }

    if (m_device->phy().features().sparseResidencyAliased) {
        image_create_info.flags = VK_IMAGE_CREATE_SPARSE_ALIASED_BIT;
        CreateImageTest(*this, &image_create_info, "VUID-VkImageCreateInfo-flags-00987");
    } else {
        GTEST_SKIP() << "Test requires unsupported sparseResidencyAliased feature";
    }
}

TEST_F(NegativeSparseImage, ResidencyImageCreateUnsupportedTypes) {
    TEST_DESCRIPTION("Create images with sparse residency with unsupported types");

    // Determine which device feature are available
    VkPhysicalDeviceFeatures device_features = {};
    ASSERT_NO_FATAL_FAILURE(InitFramework(m_errorMonitor));
    ASSERT_NO_FATAL_FAILURE(GetPhysicalDeviceFeatures(&device_features));

    // Mask out device features we don't want and initialize device state
    device_features.sparseResidencyImage2D = VK_FALSE;
    device_features.sparseResidencyImage3D = VK_FALSE;
    ASSERT_NO_FATAL_FAILURE(InitState(&device_features));

    if (!m_device->phy().features().sparseBinding) {
        GTEST_SKIP() << "Test requires unsupported sparseBinding feature";
    }

    VkImageCreateInfo image_create_info = vku::InitStructHelper();
    image_create_info.imageType = VK_IMAGE_TYPE_1D;
    image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_create_info.extent.width = 512;
    image_create_info.extent.height = 1;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices = NULL;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.flags = VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT | VK_IMAGE_CREATE_SPARSE_BINDING_BIT;

    // 1D image w/ sparse residency is an error
    CreateImageTest(*this, &image_create_info, "VUID-VkImageCreateInfo-imageType-00970");

    // 2D image w/ sparse residency when feature isn't available
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.height = 64;
    CreateImageTest(*this, &image_create_info, "VUID-VkImageCreateInfo-imageType-00971");

    // 3D image w/ sparse residency when feature isn't available
    image_create_info.imageType = VK_IMAGE_TYPE_3D;
    image_create_info.extent.depth = 8;
    CreateImageTest(*this, &image_create_info, "VUID-VkImageCreateInfo-imageType-00972");
}

TEST_F(NegativeSparseImage, ResidencyImageCreateUnsupportedSamples) {
    TEST_DESCRIPTION("Create images with sparse residency with unsupported tiling or sample counts");

    // Determine which device feature are available
    VkPhysicalDeviceFeatures device_features = {};
    ASSERT_NO_FATAL_FAILURE(InitFramework(m_errorMonitor));
    ASSERT_NO_FATAL_FAILURE(GetPhysicalDeviceFeatures(&device_features));

    // These tests require that the device support sparse residency for 2D images
    if (VK_TRUE != device_features.sparseResidencyImage2D) {
        GTEST_SKIP() << "Test requires unsupported SparseResidencyImage2D feature";
    }

    // Mask out device features we don't want and initialize device state
    device_features.sparseResidency2Samples = VK_FALSE;
    device_features.sparseResidency4Samples = VK_FALSE;
    device_features.sparseResidency8Samples = VK_FALSE;
    device_features.sparseResidency16Samples = VK_FALSE;
    ASSERT_NO_FATAL_FAILURE(InitState(&device_features));

    VkImageCreateInfo image_create_info = vku::InitStructHelper();
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_create_info.extent.width = 64;
    image_create_info.extent.height = 64;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices = NULL;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.flags = VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT | VK_IMAGE_CREATE_SPARSE_BINDING_BIT;

    // 2D image w/ sparse residency and linear tiling is an error
    CreateImageTest(*this, &image_create_info, "VUID-VkImageCreateInfo-tiling-04121");
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;

    // Multi-sample image w/ sparse residency when feature isn't available (4 flavors)
    image_create_info.samples = VK_SAMPLE_COUNT_2_BIT;
    CreateImageTest(*this, &image_create_info, "VUID-VkImageCreateInfo-imageType-00973");

    image_create_info.samples = VK_SAMPLE_COUNT_4_BIT;
    CreateImageTest(*this, &image_create_info, "VUID-VkImageCreateInfo-imageType-00974");

    image_create_info.samples = VK_SAMPLE_COUNT_8_BIT;
    CreateImageTest(*this, &image_create_info, "VUID-VkImageCreateInfo-imageType-00975");

    image_create_info.samples = VK_SAMPLE_COUNT_16_BIT;
    CreateImageTest(*this, &image_create_info, "VUID-VkImageCreateInfo-imageType-00976");
}

TEST_F(NegativeSparseImage, ResidencyFlag) {
    TEST_DESCRIPTION("Try to use VkSparseImageMemoryBindInfo without sparse residency flag");

    ASSERT_NO_FATAL_FAILURE(Init());

    if (!m_device->phy().features().sparseResidencyImage2D) {
        GTEST_SKIP() << "Test requires unsupported SparseResidencyImage2D feature";
    }

    VkImageCreateInfo image_create_info = vku::InitStructHelper();
    image_create_info.flags = VK_IMAGE_CREATE_SPARSE_BINDING_BIT;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_create_info.extent.width = 512;
    image_create_info.extent.height = 64;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices = NULL;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkImageObj image(m_device);
    image.init_no_mem(*m_device, image_create_info);

    VkSparseImageMemoryBind image_memory_bind = {};
    image_memory_bind.subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    VkSparseImageMemoryBindInfo image_memory_bind_info = {};
    image_memory_bind_info.image = image.handle();
    image_memory_bind_info.bindCount = 1;
    image_memory_bind_info.pBinds = &image_memory_bind;

    VkBindSparseInfo bind_info = vku::InitStructHelper();
    bind_info.imageBindCount = 1;
    bind_info.pImageBinds = &image_memory_bind_info;

    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseImageMemoryBindInfo-image-02901");
    vk::QueueBindSparse(m_device->m_queue, 1, &bind_info, VK_NULL_HANDLE);
    m_errorMonitor->VerifyFound();
}

TEST_F(NegativeSparseImage, ImageUsageBits) {
    TEST_DESCRIPTION("Try to use VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT with sparse image");

    ASSERT_NO_FATAL_FAILURE(Init());

    VkPhysicalDeviceFeatures device_features = {};
    ASSERT_NO_FATAL_FAILURE(GetPhysicalDeviceFeatures(&device_features));

    if (!device_features.sparseBinding) {
        GTEST_SKIP() << "No sparseBinding feature";
    }

    auto image_create_info = vku::InitStruct<VkImageCreateInfo>();
    image_create_info.flags = VK_IMAGE_CREATE_SPARSE_BINDING_BIT;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_create_info.extent.width = 32;
    image_create_info.extent.height = 32;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices = NULL;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    CreateImageTest(*this, &image_create_info, "VUID-VkImageCreateInfo-None-01925");
}

TEST_F(NegativeSparseImage, MemoryBindOffset) {
    TEST_DESCRIPTION("Try to use VkSparseImageMemoryBind with offset not less than memory size");

    ASSERT_NO_FATAL_FAILURE(Init());

    VkBufferCreateInfo buffer_create_info = vku::InitStructHelper();
    buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffer_create_info.size = 1024;
    buffer_create_info.queueFamilyIndexCount = 0;
    buffer_create_info.pQueueFamilyIndices = NULL;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_device->phy().features().sparseResidencyBuffer) {
        buffer_create_info.flags = VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT | VK_BUFFER_CREATE_SPARSE_BINDING_BIT;
    } else {
        GTEST_SKIP() << "Test requires unsupported sparseResidencyBuffer feature";
    }

    VkImageCreateInfo image_create_info = vku::InitStructHelper(nullptr);
    image_create_info.flags = 0;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_create_info.extent.width = 64;
    image_create_info.extent.height = 64;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices = NULL;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_device->phy().features().sparseResidencyImage2D) {
        image_create_info.flags = VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT | VK_IMAGE_CREATE_SPARSE_BINDING_BIT;
    } else {
        GTEST_SKIP() << "Test requires unsupported sparseResidencyImage2D feature";
    }

    vkt::Buffer buffer;
    buffer.init_no_mem(*m_device, buffer_create_info);

    VkImageObj image(m_device);
    image.init_no_mem(*m_device, image_create_info);

    VkMemoryAllocateInfo mem_alloc = vku::InitStructHelper(nullptr);
    mem_alloc.allocationSize = 1024;

    vkt::DeviceMemory mem;
    mem.init(*m_device, mem_alloc);

    VkSparseMemoryBind buffer_memory_bind = {};
    buffer_memory_bind.size = mem_alloc.allocationSize;
    buffer_memory_bind.memory = mem.handle();
    buffer_memory_bind.memoryOffset = 2048;

    VkSparseImageMemoryBind image_memory_bind = {};
    image_memory_bind.subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_memory_bind.memoryOffset = 4096;
    image_memory_bind.memory = mem.handle();

    VkSparseBufferMemoryBindInfo buffer_memory_bind_info = {};
    buffer_memory_bind_info.buffer = buffer.handle();
    buffer_memory_bind_info.bindCount = 1;
    buffer_memory_bind_info.pBinds = &buffer_memory_bind;

    VkSparseImageOpaqueMemoryBindInfo image_opaque_memory_bind_info = {};
    image_opaque_memory_bind_info.image = image.handle();
    image_opaque_memory_bind_info.bindCount = 1;
    image_opaque_memory_bind_info.pBinds = &buffer_memory_bind;

    VkSparseImageMemoryBindInfo image_memory_bind_info = {};
    image_memory_bind_info.image = image.handle();
    image_memory_bind_info.bindCount = 1;
    image_memory_bind_info.pBinds = &image_memory_bind;

    VkBindSparseInfo bind_info = vku::InitStructHelper();
    bind_info.bufferBindCount = 1;
    bind_info.pBufferBinds = &buffer_memory_bind_info;
    bind_info.imageOpaqueBindCount = 1;
    bind_info.pImageOpaqueBinds = &image_opaque_memory_bind_info;
    bind_info.imageBindCount = 1;
    bind_info.pImageBinds = &image_memory_bind_info;

    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseMemoryBind-memoryOffset-01101");
    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseMemoryBind-memoryOffset-01101");
    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseMemoryBind-memoryOffset-01101");
    vk::QueueBindSparse(m_device->m_queue, 1, &bind_info, VK_NULL_HANDLE);
    m_errorMonitor->VerifyFound();
}

TEST_F(NegativeSparseImage, QueueBindSparseMemoryType) {
    TEST_DESCRIPTION("Test QueueBindSparse with lazily allocated memory");

    ASSERT_NO_FATAL_FAILURE(Init());

    if (!m_device->phy().features().sparseResidencyBuffer) {
        GTEST_SKIP() << "Test requires unsupported sparseResidencyBuffer feature";
    } else if (!m_device->phy().features().sparseResidencyImage2D) {
        GTEST_SKIP() << "Test requires unsupported sparseResidencyImage2D feature";
    }

    VkPhysicalDeviceMemoryProperties memory_info;
    vk::GetPhysicalDeviceMemoryProperties(gpu(), &memory_info);
    uint32_t lazily_allocated_index = memory_info.memoryTypeCount;  // Set to an invalid value just in case
    for (uint32_t i = 0; i < memory_info.memoryTypeCount; ++i) {
        if ((memory_info.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) != 0) {
            lazily_allocated_index = i;
            break;
        }
    }
    if (lazily_allocated_index == memory_info.memoryTypeCount) {
        GTEST_SKIP() << "Did not find memory with VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT";
    }

    VkBufferCreateInfo buffer_create_info = vku::InitStructHelper();
    buffer_create_info.flags = VK_BUFFER_CREATE_SPARSE_BINDING_BIT;
    buffer_create_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    buffer_create_info.size = 1024;

    VkImageCreateInfo image_create_info = vku::InitStructHelper(nullptr);
    image_create_info.flags = VK_IMAGE_CREATE_SPARSE_BINDING_BIT;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_create_info.extent.width = 64;
    image_create_info.extent.height = 64;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    vkt::Buffer buffer;
    buffer.init_no_mem(*m_device, buffer_create_info);

    VkImageObj image(m_device);
    image.init_no_mem(*m_device, image_create_info);

    VkMemoryRequirements buffer_mem_reqs;
    vk::GetBufferMemoryRequirements(device(), buffer.handle(), &buffer_mem_reqs);
    auto buffer_mem_alloc = vku::InitStruct<VkMemoryAllocateInfo>();
    buffer_mem_alloc.allocationSize = buffer_mem_reqs.size;
    buffer_mem_alloc.memoryTypeIndex = lazily_allocated_index;

    VkMemoryRequirements image_mem_reqs;
    vk::GetImageMemoryRequirements(device(), image.handle(), &image_mem_reqs);
    auto image_mem_alloc = vku::InitStruct<VkMemoryAllocateInfo>();
    image_mem_alloc.allocationSize = image_mem_reqs.size;
    image_mem_alloc.memoryTypeIndex = lazily_allocated_index;

    vkt::DeviceMemory buffer_mem;
    buffer_mem.init(*m_device, buffer_mem_alloc);

    vkt::DeviceMemory image_mem;
    image_mem.init(*m_device, image_mem_alloc);

    VkSparseMemoryBind buffer_memory_bind = {};
    buffer_memory_bind.size = buffer_mem_reqs.size;
    buffer_memory_bind.memory = buffer_mem.handle();

    VkSparseMemoryBind image_memory_bind = {};
    image_memory_bind.size = image_mem_reqs.size;
    image_memory_bind.memory = image_mem.handle();

    VkSparseBufferMemoryBindInfo buffer_memory_bind_info = {};
    buffer_memory_bind_info.buffer = buffer.handle();
    buffer_memory_bind_info.bindCount = 1;
    buffer_memory_bind_info.pBinds = &buffer_memory_bind;

    VkSparseImageOpaqueMemoryBindInfo image_opaque_memory_bind_info = {};
    image_opaque_memory_bind_info.image = image.handle();
    image_opaque_memory_bind_info.bindCount = 1;
    image_opaque_memory_bind_info.pBinds = &image_memory_bind;

    VkBindSparseInfo bind_info = vku::InitStructHelper();
    bind_info.pBufferBinds = &buffer_memory_bind_info;
    bind_info.pImageOpaqueBinds = &image_opaque_memory_bind_info;

    // Validate only buffer
    {
        bind_info.bufferBindCount = 1;
        bind_info.imageOpaqueBindCount = 0;
        m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseMemoryBind-memory-01097");
        vk::QueueBindSparse(m_device->m_queue, 1, &bind_info, VK_NULL_HANDLE);
        m_errorMonitor->VerifyFound();
    }

    // Validate only image
    {
        bind_info.bufferBindCount = 0;
        bind_info.imageOpaqueBindCount = 1;
        m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseMemoryBind-memory-01097");
        vk::QueueBindSparse(m_device->m_queue, 1, &bind_info, VK_NULL_HANDLE);
        m_errorMonitor->VerifyFound();
    }

    // Validate both a buffer and image error occur
    {
        bind_info.bufferBindCount = 1;
        bind_info.imageOpaqueBindCount = 1;
        m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseMemoryBind-memory-01097");
        m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseMemoryBind-memory-01097");
        vk::QueueBindSparse(m_device->m_queue, 1, &bind_info, VK_NULL_HANDLE);
        m_errorMonitor->VerifyFound();
    }
}

TEST_F(NegativeSparseImage, ImageMemoryBind) {
    TEST_DESCRIPTION("Try to bind sparse resident image with invalid VkSparseImageMemoryBind");

    ASSERT_NO_FATAL_FAILURE(Init());

    if (!m_device->phy().features().sparseBinding || !m_device->phy().features().sparseResidencyImage3D) {
        GTEST_SKIP() << "sparseBinding && sparseResidencyImage3D features are required.";
    }

    const std::optional<uint32_t> sparse_index = m_device->QueueFamilyMatching(VK_QUEUE_SPARSE_BINDING_BIT, 0u);
    if (!sparse_index) {
        GTEST_SKIP() << "Required queue families not present";
    }

    VkImageCreateInfo create_info = vkt::Image::create_info();
    create_info.flags = VK_IMAGE_CREATE_SPARSE_BINDING_BIT | VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT;
    create_info.imageType = VK_IMAGE_TYPE_3D;
    create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    create_info.format = VK_FORMAT_B8G8R8A8_UNORM;
    create_info.extent.width = 1024;
    create_info.extent.height = 1024;
    create_info.arrayLayers = 1;

    VkImageObj image{m_device};
    image.init_no_mem(*m_device, create_info);

    VkMemoryRequirements image_mem_reqs;
    vk::GetImageMemoryRequirements(m_device->handle(), image.handle(), &image_mem_reqs);
    const auto image_mem_alloc =
        vkt::DeviceMemory::get_resource_alloc_info(*m_device, image_mem_reqs, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkt::DeviceMemory image_mem;
    image_mem.init(*m_device, image_mem_alloc);

    uint32_t requirements_count = 0u;
    vk::GetImageSparseMemoryRequirements(m_device->handle(), image.handle(), &requirements_count, nullptr);

    if (requirements_count == 0u) {
        GTEST_SKIP() << "No sparse image requirements for image format VK_FORMAT_B8G8R8A8_UNORM";
    }

    std::vector<VkSparseImageMemoryRequirements> sparse_reqs(requirements_count);
    vk::GetImageSparseMemoryRequirements(m_device->handle(), image.handle(), &requirements_count, sparse_reqs.data());

    VkExtent3D granularity = sparse_reqs[0].formatProperties.imageGranularity;
    VkSparseImageMemoryBind image_bind{};
    image_bind.subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_bind.memory = image_mem.handle();
    image_bind.extent = granularity;

    VkSparseImageMemoryBindInfo image_bind_info{};
    image_bind_info.image = image.handle();
    image_bind_info.bindCount = 1u;
    image_bind_info.pBinds = &image_bind;

    VkBindSparseInfo bind_info = vku::InitStructHelper();
    bind_info.imageBindCount = 1u;
    bind_info.pImageBinds = &image_bind_info;

    VkQueue sparse_queue = m_device->graphics_queues()[sparse_index.value()]->handle();

    // Force offset.x to invalid value
    image_bind.offset.x = granularity.width - 1;
    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseImageMemoryBind-offset-01107");
    vk::QueueBindSparse(sparse_queue, 1, &bind_info, VK_NULL_HANDLE);
    m_errorMonitor->VerifyFound();
    image_bind.offset.x = 0u;

    // Force offset.y to invalid value
    image_bind.offset.y = granularity.height - 1;
    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseImageMemoryBind-offset-01109");
    vk::QueueBindSparse(sparse_queue, 1, &bind_info, VK_NULL_HANDLE);
    m_errorMonitor->VerifyFound();
    image_bind.offset.y = 0u;

    // Force offset.y to invalid value
    image_bind.offset.z = granularity.depth - 1;
    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseImageMemoryBind-offset-01111");
    vk::QueueBindSparse(sparse_queue, 1, &bind_info, VK_NULL_HANDLE);
    m_errorMonitor->VerifyFound();
    image_bind.offset.z = 0u;

    // Force extent.width to invalid value
    image_bind.extent.width = granularity.width - 1;
    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseImageMemoryBind-extent-01108");
    vk::QueueBindSparse(sparse_queue, 1, &bind_info, VK_NULL_HANDLE);
    m_errorMonitor->VerifyFound();
    image_bind.extent.width = 0u;

    // Force extent.height to invalid value
    image_bind.extent.height = granularity.height - 1;
    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseImageMemoryBind-extent-01110");
    vk::QueueBindSparse(sparse_queue, 1, &bind_info, VK_NULL_HANDLE);
    m_errorMonitor->VerifyFound();
    image_bind.extent.height = 0u;

    // Force extent.depth to invalid value
    image_bind.extent.depth = granularity.depth - 1;
    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseImageMemoryBind-extent-01112");
    vk::QueueBindSparse(sparse_queue, 1, &bind_info, VK_NULL_HANDLE);
    m_errorMonitor->VerifyFound();
    image_bind.extent.depth = 0u;

    // Force greater mip level
    image_bind.subresource.mipLevel = VK_REMAINING_MIP_LEVELS;
    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseImageMemoryBind-subresource-01106");
    vk::QueueBindSparse(sparse_queue, 1, &bind_info, VK_NULL_HANDLE);
    m_errorMonitor->VerifyFound();
    image_bind.subresource.mipLevel = 0;

    // Force greater array layer
    image_bind.subresource.arrayLayer = VK_REMAINING_ARRAY_LAYERS;
    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseImageMemoryBind-subresource-01106");
    vk::QueueBindSparse(sparse_queue, 1, &bind_info, VK_NULL_HANDLE);
    m_errorMonitor->VerifyFound();
    image_bind.subresource.arrayLayer = 0;

    // Force invalid aspect mask
    image_bind.subresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    m_errorMonitor->SetDesiredFailureMsg(kErrorBit, "VUID-VkSparseImageMemoryBind-subresource-01106");
    vk::QueueBindSparse(sparse_queue, 1, &bind_info, VK_NULL_HANDLE);
    m_errorMonitor->VerifyFound();
    image_bind.subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
}
