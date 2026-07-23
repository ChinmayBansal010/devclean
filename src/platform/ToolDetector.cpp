#include "platform/ToolDetector.hpp"

#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

ToolDetector& ToolDetector::getInstance()
{
    static ToolDetector instance;
    return instance;
}

namespace {

std::string normalize(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

} // namespace

bool ToolDetector::isInstalled(const std::string& toolName)
{
    if (!initialized) {
        detectedTools = detectInstalledTools();
        initialized = true;
    }
    return detectedTools.find(normalize(toolName)) != detectedTools.end();
}

bool ToolDetector::isRunning(const std::string& processName)
{
#ifdef _WIN32
    const std::string cmd = "tasklist /FI \"IMAGENAME eq " + processName + ".exe\" 2>NUL | find /I \"" + processName + ".exe\" >NUL";
#else
    const std::string cmd = "ps -eo comm= 2>/dev/null | grep -E \"(^| )" + processName + "($| )\" >/dev/null 2>&1";
#endif
    return std::system(cmd.c_str()) == 0;
}

bool ToolDetector::isDockerDaemonRunning()
{
    return isRunning("dockerd") || (isInstalled("docker") && std::system("docker info >/dev/null 2>&1") == 0) || isRunning("podman");
}

bool ToolDetector::isIdeRunning()
{
    return isRunning("code") || isRunning("idea") || isRunning("pycharm") || isRunning("clion") || isRunning("goland") || isRunning("studio64");
}

bool ToolDetector::hasActivePackageManager()
{
    return isRunning("apt") || isRunning("dnf") || isRunning("pacman") || isRunning("winget") || isRunning("scoop") || isRunning("brew");
}

std::vector<std::string> ToolDetector::getWarningsForCache(const std::string& cacheName)
{
    std::vector<std::string> warnings;
    const std::string lower = normalize(cacheName);

    if ((lower.find("docker") != std::string::npos || lower.find("podman") != std::string::npos) && isDockerDaemonRunning())
        warnings.emplace_back("Docker daemon is running; deleting related cache directories may disrupt active containers.");

    if ((lower.find("vscode") != std::string::npos || lower.find("jetbrains") != std::string::npos || lower.find("idea") != std::string::npos || lower.find("pycharm") != std::string::npos || lower.find("clion") != std::string::npos || lower.find("goland") != std::string::npos) && isIdeRunning())
        warnings.emplace_back("An IDE is running; deleting editor caches may affect current sessions.");

    if ((lower.find("apt") != std::string::npos || lower.find("dnf") != std::string::npos || lower.find("pacman") != std::string::npos || lower.find("winget") != std::string::npos || lower.find("scoop") != std::string::npos || lower.find("homebrew") != std::string::npos || lower.find("brew") != std::string::npos) && hasActivePackageManager())
        warnings.emplace_back("A package manager appears to be active; deleting its cache may affect ongoing operations.");

    return warnings;
}

std::unordered_set<std::string> ToolDetector::detectInstalledTools()
{
    std::unordered_set<std::string> installed;

    std::unordered_map<std::string, std::string> toolCommands = {
        {"bun", "bun --version"},
        {"cargo", "cargo --version"},
        {"bazel", "bazel --version"},
        {"ccache", "ccache --version"},
        {"cmake", "cmake --version"},
        {"conan", "conan --version"},
        {"npm", "npm --version"},
        {"pnpm", "pnpm --version"},
        {"pip", "pip --version"},
        {"pipenv", "pipenv --version"},
        {"pixi", "pixi --version"},
        {"poetry", "poetry --version"},
        {"uv", "uv --version"},
        {"yarn", "yarn --version"},
        {"gradle", "gradle --version"},
        {"maven", "mvn --version"},
        {"mamba", "mamba --version"},
        {"meson", "meson --version"},
        {"nvm", "nvm --version"},
        {"podman", "podman --version"},
        {"rustup", "rustup --version"},
        {"vcpkg", "vcpkg version"},
        {"python", "python --version"},
        {"python3", "python3 --version"},
        {"pip3", "pip3 --version"},
        {"go", "go version"},
        {"rustc", "rustc --version"},
        {"java", "java -version"},
        {"docker", "docker --version"},
        {"dotnet", "dotnet --version"},
        {"swift", "swift --version"},
        {"ruby", "ruby --version"},
        {"php", "php --version"},
    };

    for (const auto& [tool, command] : toolCommands) {
#ifdef _WIN32
        std::string cmd = command + " >nul 2>&1";
#else
        std::string cmd = command + " >/dev/null 2>&1";
#endif
        if (std::system(cmd.c_str()) == 0) {
            installed.insert(tool);
        }
    }

    return installed;
}

bool ToolDetector::checkToolInstalled(const std::string& toolName)
{
    return isInstalled(toolName);
}
