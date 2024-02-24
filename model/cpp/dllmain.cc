#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include <incbin.h>
#include <vsm.hpp>

INCBIN(logo, "incbin.h");

class ModelImpl : public IDSIMMODEL
{
public:
  INT isdigital(CHAR *pinname)
  {
    (void)pinname;
    return 1;
  }

  VOID setup(IINSTANCE *instance, IDSIMCKT *dsim)
  {
  }
  
  VOID runctrl(RUNMODES mode)
  {
  }

  VOID actuate(REALTIME time, ACTIVESTATE newstate)
  {
  }

  BOOL indicate(REALTIME time, ACTIVEDATA *newstate)
  {
    return true;
  }

  VOID simulate(ABSTIME time, DSIMMODES mode)
  {
  }

  VOID callback(ABSTIME time, EVENTID eventid)
  {
  }
};

bool vsmRegister(ILICENCESERVER *ils)
{
  if (!ils->authorize(0, VSM_API_VERSION))
  {
    return false;
  }
  return true;
}

extern "C"
{
  IDSIMMODEL __declspec(dllexport) * createdsimmodel(char *device, ILICENCESERVER *ils)
  {
    (void)device;
    vsmRegister(ils);
    return new ModelImpl;
  }

  void __declspec(dllexport) deletedsimmodel(IDSIMMODEL *model)
  {
  }

  void vsm_simulate(IDSIMMODEL *instance, uint32_t edx, ABSTIME atime, DSIMMODES mode)
  {
    (void)edx;
    (void)atime;
    (void)mode;
  }
}

bool APIENTRY DllMain(HINSTANCE hInstDLL, uint32_t fdwReason, LPVOID lpvReserved)
{
  (void)hInstDLL;
  (void)fdwReason;
  (void)lpvReserved;
  return true;
}