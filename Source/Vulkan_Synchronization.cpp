/* Copyright 2023 Sean Halnon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**  http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissionsand
**  limitations under the License.
*/
#ifndef VULKAN_SYNCHRONIZATION_CPP
#define VULKAN_SYNCHRONIZATION_CPP

#include "Vulkan_Synchronization.h"
#include "Vulkan_Utils.h"

//const uint32_t VkSync::maxSemaphoreHandlesAllocated        = 512;
//const uint32_t VkSync::initialNumSemaphoreHandlesAllocated = 16;

#if !defined(SEMAPHORE_DEPOT_DEFINITION)
VkSync::SemaphoreList VkSync::SemaphoreDepot = {};
#define SEMAPHORE_DEPOT_DEFINITION
#endif;

VkSync::SemaphoreList::SemaphoreList() :
    pSemaphoreHandles       (0),
    numAllocatedHandles     (0),
    numAvailableSemaphores  (0),
    maxNumAllocatedHandles  (0),
    logicalDevice           (0),
    pSemaphoreAvailabilityBitfields(0),
    numBitMaskQwordsAllocated(0)
{
}


VkSync::SemaphoreList::SemaphoreList(VkDevice logicalDeviceIn, uint32_t maxSemaphores)
    : pSemaphoreHandles(0),
      numAllocatedHandles(0),
      numAvailableSemaphores(0),
      maxNumAllocatedHandles(0),
      logicalDevice(0),
      pSemaphoreAvailabilityBitfields(0),
      numBitMaskQwordsAllocated(0)
{
    uint32_t    initialListAllocSize = BITS_PER_QWORD;
    logicalDevice                    = logicalDeviceIn;
    numAllocatedHandles              = (initialListAllocSize > 0) ? initialListAllocSize : 1;
    maxNumAllocatedHandles           = (maxSemaphores > MIN_SEMAPHORE_LIST_SIZE) ? maxSemaphores : MIN_SEMAPHORE_LIST_SIZE;

    // Should be 1 bit availabler per handle
    uint32_t numQwords = NumQwordsNeededForBitmask(initialListAllocSize);
    assert((numQwords * sizeof(uint64_t) * 8) >= initialListAllocSize);

    pSemaphoreAvailabilityBitfields  = static_cast<uint64_t*>(calloc(numQwords, sizeof(uint64_t)));
    pSemaphoreHandles                = static_cast<VkSemaphore*>(calloc(numAllocatedHandles, sizeof(VkSemaphore)));
    numBitMaskQwordsAllocated        = numQwords;
}

void VkSync::SemaphoreList::SetSemaphoreAvailabilityBit(uint32_t semaphoreIdx)
{
    assert(semaphoreIdx < numAvailableSemaphores);

    static const uint32_t bpQword  = (sizeof(uint64_t) * 8); // bits per qword
    const uint32_t        qwordIdx = semaphoreIdx / bpQword;
    const uint32_t        bitIdx   = semaphoreIdx % bpQword;

    assert(qwordIdx < numAvailableSemaphores / bpQword);

    pSemaphoreAvailabilityBitfields[qwordIdx] |= uint64_t(1) << bitIdx;
}

void VkSync::SemaphoreList::ClearSemaphoreAvailabilityBit(uint32_t semaphoreIdx)
{
    assert(semaphoreIdx < numAvailableSemaphores);

    const uint32_t        qwordIdx = semaphoreIdx / BITS_PER_QWORD;
    const uint32_t        bitIdx   = semaphoreIdx % BITS_PER_QWORD;

    assert(qwordIdx < numAvailableSemaphores / BITS_PER_QWORD);

    pSemaphoreAvailabilityBitfields[qwordIdx] &= ~(uint64_t(1) << bitIdx);
}

int32_t VkSync::SemaphoreList::ScanForOccupiedSlot()
{
    if (numBitMaskQwordsAllocated > 0)
    {
        uint32_t qwordIdx = 0;
        for (uint64_t qword = pSemaphoreAvailabilityBitfields[qwordIdx]; qwordIdx < numBitMaskQwordsAllocated; qword = pSemaphoreAvailabilityBitfields[qwordIdx++])
        {
            for (uint32_t bitIdx = 0; bitIdx < BITS_PER_QWORD; bitIdx++)
            {
                const uint64_t currentBit = (qword & (uint64_t(1) << bitIdx));
                if (currentBit != 0)
                {
                    return (qwordIdx * BITS_PER_QWORD) + bitIdx;
                }
            }
        }
    }

    return -1;
}

int32_t VkSync::SemaphoreList::ScanForUnoccupiedSlot()
{
    assert(numBitMaskQwordsAllocated > 0);
    uint32_t qwordIdx = 0;
    if (numAvailableSemaphores < numAllocatedHandles)
    {
        for (uint64_t qword = pSemaphoreAvailabilityBitfields[qwordIdx]; qwordIdx < numBitMaskQwordsAllocated; qword = pSemaphoreAvailabilityBitfields[qwordIdx++])
        {
            for (uint32_t bitIdx = 0; bitIdx < BITS_PER_QWORD; bitIdx++)
            {
                const uint64_t currentBit = (~qword) & (uint64_t(1) << bitIdx);
                if (currentBit != 0)
                {
                    return (qwordIdx * BITS_PER_QWORD) + bitIdx;
                }
            }
        }
    }

    return -1;
}

VkSemaphore VkSync::SemaphoreList::ObtainSemaphore()
{
    assert(numAllocatedHandles > numAvailableSemaphores);

    VkSemaphore semaphoreOut = VK_NULL_HANDLE;


    if (numAvailableSemaphores == 0)
    {
        VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO , 0, 0 };

        // Dont need to increment or decrement anything since this semaphore is going to be returned directly to caller without first touching the semaphore list
        VkResult result = vkCreateSemaphore(/*..VkDevice.......................device.......*/ logicalDevice,
                                            /*..const.VkSemaphoreCreateInfo*...pCreateInfo..*/ &createInfo,
                                            /*..const.VkAllocationCallbacks*...pAllocator...*/ nullptr,
                                            /*..VkSemaphore*...................pSemaphore...*/ &semaphoreOut);
        assert(result == VK_SUCCESS);
    }
    //Shouldnt ever need to expand the handles allocation since ObtainSemaphore shouldnt be able to cause numAvailableSemaphores to become larger than numAllocatedHandles
    else if (numAvailableSemaphores > 0)
    {
        const int32_t idxOfAvailableSemaphore = ScanForOccupiedSlot();
        assert(idxOfAvailableSemaphore > -1);

        semaphoreOut = pSemaphoreHandles[idxOfAvailableSemaphore];
        ClearSemaphoreAvailabilityBit(idxOfAvailableSemaphore);
        numAvailableSemaphores--;

    }

    assert(semaphoreOut != VK_NULL_HANDLE);

    return semaphoreOut;
}

void VkSync::SemaphoreList::DepositUnusedSemaphore(VkSemaphore& semaphore)
{
    assert(semaphore != VK_NULL_HANDLE);
    uint32_t firstUnusedSemaphoreSlot = ScanForUnoccupiedSlot();

    if (firstUnusedSemaphoreSlot != -1) // There are enough elements allocated already
    {

        pSemaphoreHandles[firstUnusedSemaphoreSlot] = semaphore;
        numAvailableSemaphores++;
    }
    // Expand the allocation since there are no unused slots.
    else if ((numAvailableSemaphores == numAllocatedHandles) &&// Grow list
        (numAllocatedHandles < maxNumAllocatedHandles))
    {
        uint32_t     expandedListNumElements = numAllocatedHandles + SEMAPHORE_LIST_GROWTH_RATE;
        VkSemaphore* expandedListAllocation = static_cast<VkSemaphore*>(calloc(expandedListNumElements, sizeof(VkSemaphore)));
        assert(expandedListAllocation);
        memcpy(expandedListAllocation, pSemaphoreHandles, numAvailableSemaphores); //Copy existing data
        free(pSemaphoreHandles);
        pSemaphoreHandles = expandedListAllocation;
        numAllocatedHandles = expandedListNumElements;

        pSemaphoreHandles[numAvailableSemaphores++] = semaphore;

        uint32_t  numQwordsInBitmask = NumQwordsNeededForBitmask(expandedListNumElements);
        uint64_t* bitmaskList = static_cast<uint64_t*>(calloc(numQwordsInBitmask, sizeof(uint64_t)));

        numBitMaskQwordsAllocated = numQwordsInBitmask;

        assert(pSemaphoreAvailabilityBitfields != 0);
        assert(bitmaskList != 0);

        memcpy(bitmaskList, pSemaphoreAvailabilityBitfields, numBitMaskQwordsAllocated * sizeof(uint64_t));
        free(pSemaphoreAvailabilityBitfields);
        pSemaphoreAvailabilityBitfields = bitmaskList;
    }
    else
    {
        printf("shouldnt be here!!!\n");
        assert(false);
    }

    semaphore = VK_NULL_HANDLE;
}

void VkSync::InitSemaphoreList(VkDevice logicalDeviceIn)
{
    VkSync::SemaphoreDepot = VkSync::SemaphoreList(logicalDeviceIn, maxSemaphoreHandlesAllocated);

    printf("VkSync::SemaphoreDepot.numAllocatedHandles = % u\n", VkSync::SemaphoreDepot.numAllocatedHandles);
}

#endif // VULKAN_SYNCHRONIZATION_CPP