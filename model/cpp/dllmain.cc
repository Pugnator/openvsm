#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include <vsm.hpp>
#include <log/log.hpp>
#include "model.hpp"

namespace
{
  bool vsmRegister(ILICENCESERVER *ils)
  {
    if (!ils->authorize(0, VSM_API_VERSION))
    {
      return false;
    }
    return true;
  }
}

extern "C"
{
  IDSIMMODEL __declspec(dllexport) * createdsimmodel(char *device, ILICENCESERVER *ils)
  {
    (void)device;
    vsmRegister(ils);
    return new DeviceSimulator::VirtualDevice;
  }

  void __declspec(dllexport) deletedsimmodel(IDSIMMODEL *model)
  {
    delete model;
  }
}

bool APIENTRY DllMain(HINSTANCE hInstDLL, uint32_t fdwReason, LPVOID lpvReserved)
{
  switch (fdwReason)
  {
  case DLL_PROCESS_ATTACH:
    Log::get().configure(TraceType::file);
    Log::get().set_level(TraceSeverity::debug);
    LOG_DEBUG("The model is being loaded\n");
    break;
  case DLL_PROCESS_DETACH:
    LOG_DEBUG("The model is being unloaded\n");
    break;
  case DLL_THREAD_ATTACH:
    LOG_DEBUG("A thread is being created\n");
    break;
  case DLL_THREAD_DETACH:
    LOG_DEBUG("A thread is being destroyed\n");
    break;
  }
  (void)hInstDLL;
  (void)fdwReason;
  (void)lpvReserved;
  return true;
}