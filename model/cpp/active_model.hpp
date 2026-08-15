#pragma once

#include "model.hpp"

namespace DeviceSimulator
{
class LuaActiveModel final : public VirtualDevice, public IACTIVEMODEL
{
  public:
    void initialize(ICOMPONENT *component) override;
    ISPICEMODEL *getspicemodel(CHAR *primitive) override;
    IDSIMMODEL *getdsimmodel(CHAR *primitive) override;
    void plot(ACTIVESTATE state) override;
    void animate(INT element, ACTIVEDATA *newstate) override;
    BOOL actuate(WORD key, INT x, INT y, DWORD flags) override;

    void setup(IINSTANCE *instance, IDSIMCKT *dsim) override;

  private:
    void ensureGraphicsInitialized();

    ICOMPONENT *component_ = nullptr;
    bool graphicsInitialized_ = false;
};
} // namespace DeviceSimulator
