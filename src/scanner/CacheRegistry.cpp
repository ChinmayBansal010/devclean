#include "scanner/CacheRegistry.hpp"

#include <algorithm>
#include <cstdlib>
#include <string>

namespace {

std::string normalize(const std::string& value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return result;
}

bool matchesFilter(const CacheDefinition& cache, const std::string& filter)
{
    const std::string normalizedFilter = normalize(filter);
    if (normalize(cache.name) == normalizedFilter)
        return true;

    for (const auto& alias : cache.aliases)
    {
        if (normalize(alias) == normalizedFilter)
            return true;
    }

    return false;
}

} // namespace

std::vector<CacheDefinition> CacheRegistry::getCaches()
{
    std::vector<CacheDefinition> caches;

    const char* home = std::getenv("HOME");
    const char* userProfile = std::getenv("USERPROFILE");
    const char* localAppData = std::getenv("LOCALAPPDATA");
    const char* appData = std::getenv("APPDATA");

    std::filesystem::path homePath;
    if (home != nullptr)
        homePath = home;

    std::filesystem::path userProfilePath;
    if (userProfile != nullptr)
        userProfilePath = userProfile;

    std::filesystem::path localAppDataPath;
    if (localAppData != nullptr)
        localAppDataPath = localAppData;

    std::filesystem::path appDataPath;
    if (appData != nullptr)
        appDataPath = appData;

#ifdef _WIN32
    caches.push_back({"pip", {"pip", "python"}, {}, localAppDataPath / "pip" / "Cache", "python", "Python package cache", {"PIP_CACHE_DIR"}, {"windows"}, 10});
    caches.push_back({"pipenv", {"pipenv", "python"}, {}, userProfilePath / ".cache" / "pipenv", "python", "Pipenv cache", {"PIPENV_CACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"poetry", {"poetry", "python"}, {}, userProfilePath / ".cache" / "pypoetry", "python", "Poetry cache", {"POETRY_CACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"uv", {"uv", "python"}, {}, userProfilePath / ".cache" / "uv", "python", "uv cache", {"UV_CACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"pixi", {"pixi", "python"}, {}, userProfilePath / ".pixi" / "cache", "python", "Pixi package cache", {"PIXI_CACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"mamba", {"mamba", "python"}, {}, userProfilePath / ".mamba" / "pkgs", "python", "Mamba package cache", {"CONDA_PKGS_DIRS"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"npm", {"npm", "javascript", "node"}, {}, appDataPath / "npm-cache", "javascript", "Node package cache", {"npm_config_cache"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"pnpm", {"pnpm", "javascript", "node"}, {}, localAppDataPath / "pnpm" / "store", "javascript", "pnpm store", {"PNPM_HOME"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"yarn", {"yarn", "javascript", "node"}, {}, localAppDataPath / "Yarn" / "Cache", "javascript", "Yarn cache", {"YARN_CACHE_FOLDER"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"bun", {"bun", "javascript", "node"}, {}, userProfilePath / ".bun" / "cache", "javascript", "Bun cache", {"BUN_INSTALL_CACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"nvm", {"nvm", "javascript", "node"}, {}, userProfilePath / ".nvm", "javascript", "nvm installation cache", {"NVM_DIR"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"cargo", {"cargo", "rust"}, {}, userProfilePath / ".cargo" / "registry", "rust", "Cargo registry cache", {"CARGO_HOME"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"rustup", {"rustup", "rust"}, {}, userProfilePath / ".rustup" / "toolchains", "rust", "Rust toolchain cache", {"RUSTUP_HOME"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"gradle", {"gradle", "java"}, {}, userProfilePath / ".gradle" / "caches", "java", "Gradle caches", {"GRADLE_USER_HOME"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"maven", {"maven", "java"}, {}, userProfilePath / ".m2" / "repository", "java", "Maven repository cache", {"M2_REPO"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"ccache", {"ccache", "build"}, {}, localAppDataPath / "ccache", "build", "Ccache build cache", {"CCACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"conan", {"conan", "cpp"}, {}, userProfilePath / ".conan" / "p", "cpp", "Conan package cache", {"CONAN_USER_HOME"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"vcpkg", {"vcpkg", "cpp"}, {}, userProfilePath / "vcpkg" / "downloads", "cpp", "vcpkg downloads cache", {"VCPKG_ROOT"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"docker", {"docker", "containers"}, {}, userProfilePath / ".docker" / "builder" / "docker-buildxin", "containers", "Docker builder cache", {"DOCKER_BUILDKIT"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"podman", {"podman", "containers"}, {}, userProfilePath / ".local" / "share" / "containers", "containers", "Podman container storage", {"PODMAN_USERNS"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"vscode", {"vscode", "editor"}, {}, userProfilePath / "AppData" / "Roaming" / "Code" / "CachedData", "editor", "VS Code cache", {"VSCODE_PORTABLE"}, {"windows", "linux", "macos"}, 6});
    caches.push_back({"jetbrains", {"jetbrains", "editor"}, {}, userProfilePath / ".idea" / "system", "editor", "JetBrains IDE cache", {"JETBRAINS_IDE"}, {"windows", "linux", "macos"}, 6});
    caches.push_back({"cmake", {"cmake", "build"}, {}, userProfilePath / ".cmake", "build", "CMake build cache", {"CMAKE_BUILD_PARALLEL_LEVEL"}, {"windows", "linux", "macos"}, 7});
    caches.push_back({"meson", {"meson", "build"}, {}, userProfilePath / ".cache" / "meson", "build", "Meson build cache", {"MESON_DIR"}, {"windows", "linux", "macos"}, 7});
    caches.push_back({"bazel", {"bazel", "build"}, {}, userProfilePath / "_bazel", "build", "Bazel cache", {"BAZELISK_HOME"}, {"windows", "linux", "macos"}, 7});
    caches.push_back({"apt", {"apt", "package managers"}, {}, userProfilePath / ".cache" / "apt", "package managers", "APT package cache", {"APT_CACHE_DIR"}, {"linux"}, 6});
    caches.push_back({"dnf", {"dnf", "package managers"}, {}, userProfilePath / ".cache" / "dnf", "package managers", "DNF cache", {"DNF_CACHE_DIR"}, {"linux"}, 6});
    caches.push_back({"pacman", {"pacman", "package managers"}, {}, userProfilePath / "AppData" / "Local" / "pacman" / "cache", "package managers", "Pacman cache", {"XDG_CACHE_HOME"}, {"windows", "linux", "macos"}, 6});
    caches.push_back({"winget", {"winget", "package managers"}, {}, userProfilePath / "AppData" / "Local" / "Packages" / "Microsoft.DesktopAppInstaller_8wekyb3d8bbwe" / "LocalState" / "Cache", "package managers", "Winget cache", {"LOCALAPPDATA"}, {"windows"}, 6});
    caches.push_back({"scoop", {"scoop", "package managers"}, {}, userProfilePath / "scoop" / "cache", "package managers", "Scoop cache", {"SCOOP"}, {"windows"}, 6});
    caches.push_back({"homebrew", {"homebrew", "package managers"}, {}, userProfilePath / "Library" / "Caches" / "Homebrew", "package managers", "Homebrew cache", {"HOMEBREW_CACHE"}, {"macos"}, 6});
#else
    caches.push_back({"pip", {"pip", "python"}, homePath / ".cache" / "pip", {}, "python", "Python package cache", {"PIP_CACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"pipenv", {"pipenv", "python"}, homePath / ".cache" / "pipenv", {}, "python", "Pipenv cache", {"PIPENV_CACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"poetry", {"poetry", "python"}, homePath / ".cache" / "pypoetry", {}, "python", "Poetry cache", {"POETRY_CACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"uv", {"uv", "python"}, homePath / ".cache" / "uv", {}, "python", "uv cache", {"UV_CACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"pixi", {"pixi", "python"}, homePath / ".cache" / "pixi", {}, "python", "Pixi package cache", {"PIXI_CACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"mamba", {"mamba", "python"}, homePath / ".cache" / "mamba", {}, "python", "Mamba package cache", {"CONDA_PKGS_DIRS"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"npm", {"npm", "javascript", "node"}, homePath / ".npm", {}, "javascript", "Node package cache", {"npm_config_cache"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"pnpm", {"pnpm", "javascript", "node"}, homePath / ".pnpm-store", {}, "javascript", "pnpm store", {"PNPM_HOME"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"yarn", {"yarn", "javascript", "node"}, homePath / ".cache" / "yarn", {}, "javascript", "Yarn cache", {"YARN_CACHE_FOLDER"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"bun", {"bun", "javascript", "node"}, homePath / ".bun" / "cache", {}, "javascript", "Bun cache", {"BUN_INSTALL_CACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"nvm", {"nvm", "javascript", "node"}, homePath / ".nvm", {}, "javascript", "nvm installation cache", {"NVM_DIR"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"cargo", {"cargo", "rust"}, homePath / ".cargo" / "registry", {}, "rust", "Cargo registry cache", {"CARGO_HOME"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"rustup", {"rustup", "rust"}, homePath / ".rustup", {}, "rust", "Rust toolchain cache", {"RUSTUP_HOME"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"gradle", {"gradle", "java"}, homePath / ".gradle" / "caches", {}, "java", "Gradle caches", {"GRADLE_USER_HOME"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"maven", {"maven", "java"}, homePath / ".m2" / "repository", {}, "java", "Maven repository cache", {"M2_REPO"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"ccache", {"ccache", "build"}, homePath / ".cache" / "ccache", {}, "build", "Ccache build cache", {"CCACHE_DIR"}, {"windows", "linux", "macos"}, 10});
    caches.push_back({"conan", {"conan", "cpp"}, homePath / ".conan" / "p", {}, "cpp", "Conan package cache", {"CONAN_USER_HOME"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"vcpkg", {"vcpkg", "cpp"}, homePath / ".cache" / "vcpkg", {}, "cpp", "vcpkg downloads cache", {"VCPKG_ROOT"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"docker", {"docker", "containers"}, homePath / ".cache" / "docker", {}, "containers", "Docker builder cache", {"DOCKER_BUILDKIT"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"podman", {"podman", "containers"}, homePath / ".local" / "share" / "containers", {}, "containers", "Podman container storage", {"PODMAN_USERNS"}, {"windows", "linux", "macos"}, 8});
    caches.push_back({"vscode", {"vscode", "editor"}, homePath / ".cache" / "Code", {}, "editor", "VS Code cache", {"VSCODE_PORTABLE"}, {"windows", "linux", "macos"}, 6});
    caches.push_back({"jetbrains", {"jetbrains", "editor"}, homePath / ".cache" / "JetBrains", {}, "editor", "JetBrains IDE cache", {"JETBRAINS_IDE"}, {"windows", "linux", "macos"}, 6});
    caches.push_back({"cmake", {"cmake", "build"}, homePath / ".cache" / "cmake", {}, "build", "CMake build cache", {"CMAKE_BUILD_PARALLEL_LEVEL"}, {"windows", "linux", "macos"}, 7});
    caches.push_back({"meson", {"meson", "build"}, homePath / ".cache" / "meson", {}, "build", "Meson build cache", {"MESON_DIR"}, {"windows", "linux", "macos"}, 7});
    caches.push_back({"bazel", {"bazel", "build"}, homePath / ".cache" / "bazel", {}, "build", "Bazel cache", {"BAZELISK_HOME"}, {"windows", "linux", "macos"}, 7});
    caches.push_back({"apt", {"apt", "package managers"}, homePath / ".cache" / "apt", {}, "package managers", "APT package cache", {"APT_CACHE_DIR"}, {"linux"}, 6});
    caches.push_back({"dnf", {"dnf", "package managers"}, homePath / ".cache" / "dnf", {}, "package managers", "DNF cache", {"DNF_CACHE_DIR"}, {"linux"}, 6});
    caches.push_back({"pacman", {"pacman", "package managers"}, homePath / ".cache" / "pacman", {}, "package managers", "Pacman cache", {"XDG_CACHE_HOME"}, {"linux"}, 6});
    caches.push_back({"homebrew", {"homebrew", "package managers"}, homePath / ".cache" / "Homebrew", {}, "package managers", "Homebrew cache", {"HOMEBREW_CACHE"}, {"macos"}, 6});
#endif

    return caches;
}

std::vector<CacheDefinition> CacheRegistry::getMatchingCaches(
    const std::vector<std::string>& filters
)
{
    const auto allCaches = getCaches();

    if (filters.empty())
        return allCaches;

    std::vector<CacheDefinition> matched;
    matched.reserve(allCaches.size());

    for (const auto& cache : allCaches)
    {
        if (!cache.enabled)
            continue;

        for (const auto& filter : filters)
        {
            if (matchesFilter(cache, filter))
            {
                matched.push_back(cache);
                break;
            }
        }
    }

    return matched;
}

std::vector<CacheDefinition> CacheRegistry::customCaches;

std::vector<CacheDefinition> CacheRegistry::getCachesWithPlugins()
{
    auto caches = getCaches();

    for (const auto& custom : customCaches) {
        caches.push_back(custom);
    }

    return caches;
}

void CacheRegistry::addCustomCache(const CacheDefinition& cache)
{
    customCaches.push_back(cache);
}

std::vector<CacheDefinition> CacheRegistry::getCustomCaches()
{
    return customCaches;
}