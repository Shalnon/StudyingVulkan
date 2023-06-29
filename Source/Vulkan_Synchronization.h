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
#pragma once
#ifndef VULKAN_SYCHRONIZATION_H
#define VULKAN_SYCHRONIZATION_H

#include "StudyingVulkan.h"

///@note: Currently unused
namespace VkSync
{
	static_assert(sizeof(uint64_t)                   == 8);
	static_assert(sizeof(uint8_t)                    == 1);
	static_assert(sizeof(uint64_t) / sizeof(uint8_t) == 8);

	struct SemaphoreList
	{
		static const uint32_t BITS_PER_QWORD = sizeof(uint64_t) * 8; // bits per qword

		VkSemaphore* pSemaphoreHandles;
		uint64_t*    pSemaphoreAvailabilityBitfields;
		uint32_t     numBitMaskQwordsAllocated;
		uint32_t	 numAllocatedHandles;
		uint32_t     numAvailableSemaphores;
		uint32_t	 maxNumAllocatedHandles;
		VkDevice     logicalDevice;

		SemaphoreList();
		SemaphoreList(VkDevice logicalDeviceIn, uint32_t maxSemaphores);

		VkSemaphore ObtainSemaphore();
		void        DepositUnusedSemaphore(VkSemaphore& semaphore);
		void        SetSemaphoreAvailabilityBit(uint32_t semaphoreIdx);
		void        ClearSemaphoreAvailabilityBit(uint32_t semaphoreIdx);

		int32_t    ScanForOccupiedSlot();
		int32_t    ScanForUnoccupiedSlot();


		inline uint32_t  NumQwordsNeededForBitmask(uint32_t numBitsNeeded)
		{
			uint32_t numQwords = numBitsNeeded / BITS_PER_QWORD;
			if ((numBitsNeeded % BITS_PER_QWORD) > 0) { numQwords++; }
			return numQwords;
		}
	};

	const uint32_t maxSemaphoreHandlesAllocated = 512;
	const uint32_t initialNumSemaphoreHandlesAllocated = 16;

	extern SemaphoreList SemaphoreDepot;

	void InitSemaphoreList(VkDevice logicalDeviceIn);
};



#endif // VULKAN_SYCHRONIZATION_H