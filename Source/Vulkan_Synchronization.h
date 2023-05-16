#pragma once
#if !defined(VULKAN_SYCHRONIZATION_H)
#define VULKAN_SYCHRONIZATION_H

#include "StudyingVulkan.h"


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

//static VkSync::SemaphoreList VkSync::SemaphoreDepot = {};

#endif