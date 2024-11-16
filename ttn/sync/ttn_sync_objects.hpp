#pragma once

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace sync {

    class Ttn_Sync_Objects {
      
      private:
        VkDevice vkDevice;

      public:
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence;

        Ttn_Sync_Objects(VkDevice);
        ~Ttn_Sync_Objects();

    };
  }
}