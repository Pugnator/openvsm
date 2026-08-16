#include <string>
#include <vector>
#include "model.hpp"
#include <vsm.hpp>

extern "C" IDSIMMODEL *createdsimmodel(char *device, ILICENCESERVER *licenceServer);

namespace
{
class LicenceServer final : public ILICENCESERVER
{
  public:
    explicit LicenceServer(bool authorized) : authorized_(authorized)
    {
    }

    BOOL authorize(DWORD productId, DWORD apiVersion) override
    {
        ++authorizationCalls;
        requestedProduct = productId;
        requestedApi = apiVersion;
        return authorized_;
    }

    BOOL authorizeex(DWORD productId, DWORD apiVersion, DWORD buildDate) override
    {
        (void)productId;
        (void)apiVersion;
        (void)buildDate;
        return false;
    }

    DWORD getid() override
    {
        return 0;
    }

    CHAR *getname() override
    {
        return emptyText;
    }

    CHAR *getcompany() override
    {
        return emptyText;
    }

    CHAR *getexpirydate() override
    {
        return emptyText;
    }

    int authorizationCalls = 0;
    DWORD requestedProduct = 0;
    DWORD requestedApi = 0;

  private:
    bool authorized_;
    CHAR emptyText[1] = {'\0'};
};
} // namespace

int main()
{
    if (createdsimmodel(nullptr, nullptr) != nullptr)
    {
        return 1;
    }

    LicenceServer denied(false);
    if (createdsimmodel(nullptr, &denied) != nullptr || denied.authorizationCalls != 1 ||
        denied.requestedProduct != 0 || denied.requestedApi != VSM_API_VERSION)
    {
        return 2;
    }

    LicenceServer allowed(true);
    IDSIMMODEL *model = createdsimmodel(nullptr, &allowed);
    if (!model || allowed.authorizationCalls != 1 || allowed.requestedProduct != 0 ||
        allowed.requestedApi != VSM_API_VERSION)
    {
        return 3;
    }

    delete static_cast<DeviceSimulator::VirtualDevice *>(model);
    return 0;
}
