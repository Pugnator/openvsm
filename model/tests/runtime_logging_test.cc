#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include "runtime_logging.hpp"

int main()
{
    const std::filesystem::path logPath{"log.txt"};
    std::error_code error;
    std::filesystem::remove(logPath, error);
    if (error)
    {
        std::cerr << "Could not remove the previous runtime log: " << error.message() << '\n';
        return 1;
    }

    DeviceSimulator::initializeRuntimeLogging();
    DeviceSimulator::logRuntimeDebug("Lua graphics callback diagnostic");

    std::ifstream logFile{logPath, std::ios::binary};
    if (!logFile.is_open())
    {
        std::cerr << "Runtime logging did not create log.txt\n";
        return 1;
    }

    const std::string contents{std::istreambuf_iterator<char>{logFile}, std::istreambuf_iterator<char>{}};
    if (contents.find("The model is being loaded") == std::string::npos)
    {
        std::cerr << "Runtime logging created log.txt but filtered the startup diagnostic\n";
        return 1;
    }
    if (contents.find("Lua graphics callback diagnostic") == std::string::npos)
    {
        std::cerr << "Runtime logging filtered the graphics callback diagnostic\n";
        return 1;
    }

    return 0;
}
