#pragma once

// Std
#include <atomic>
#include <chrono>
#include <cstdint>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

// Libraries
#include <XLink/XLinkPublicDefines.h>

namespace dai {

struct DeviceInfo {
    DeviceInfo() = default;
    DeviceInfo(const char*);
    DeviceInfo(std::string);
    deviceDesc_t desc = {};
    XLinkDeviceState_t state = X_LINK_ANY_STATE;
    std::string getMxId() const;
};

class XLinkConnection {
    static std::atomic<bool> xlinkGlobalInitialized;
    static XLinkGlobalHandler_t xlinkGlobalHandler;
    static void initXLinkGlobal();
    static std::mutex xlinkStreamOperationMutex;

   public:
    // static API
    static std::vector<DeviceInfo> getAllConnectedDevices(XLinkDeviceState_t state = X_LINK_ANY_STATE);
    static std::tuple<bool, DeviceInfo> getFirstDevice(XLinkDeviceState_t state = X_LINK_ANY_STATE);
    static std::tuple<bool, DeviceInfo> getDeviceByMxId(std::string, XLinkDeviceState_t state = X_LINK_ANY_STATE);

    XLinkConnection(const DeviceInfo& deviceDesc, std::vector<std::uint8_t> mvcmdBinary, XLinkDeviceState_t expectedState = X_LINK_BOOTED);
    XLinkConnection(const DeviceInfo& deviceDesc, std::string pathToMvcmd, XLinkDeviceState_t expectedState = X_LINK_BOOTED);
    explicit XLinkConnection(const DeviceInfo& deviceDesc, XLinkDeviceState_t expectedState = X_LINK_BOOTED);

    ~XLinkConnection();

    void setRebootOnDestruction(bool reboot);
    bool getRebootOnDestruction() const;

    int getLinkId() const;

   private:
    friend class XLinkStream;
    // static
    static bool bootAvailableDevice(const deviceDesc_t& deviceToBoot, const std::string& pathToMvcmd);
    static bool bootAvailableDevice(const deviceDesc_t& deviceToBoot, std::vector<std::uint8_t>& mvcmd);
    static std::string convertErrorCodeToString(XLinkError_t errorCode);

    void initDevice(const DeviceInfo& deviceToInit, XLinkDeviceState_t expectedState = X_LINK_BOOTED);

    bool bootDevice = true;
    bool bootWithPath = true;
    std::string pathToMvcmd;
    std::vector<std::uint8_t> mvcmd;

    bool rebootOnDestruction{true};

    int deviceLinkId = -1;

    constexpr static std::chrono::milliseconds WAIT_FOR_BOOTUP_TIMEOUT{5000};
    constexpr static std::chrono::milliseconds WAIT_FOR_CONNECT_TIMEOUT{5000};
};

}  // namespace dai
