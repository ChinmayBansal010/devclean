#include "cleaner/CleanEngine.hpp"
#include "core/Config.hpp"
#include "scanner/ScannerEngine.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

#ifdef __linux__
#include <unistd.h>
#endif

namespace {

std::size_t bestEffortResidentSetBytes()
{
#ifdef __linux__
    std::ifstream statm("/proc/self/statm");
    long pages = 0;
    long resident = 0;
    if (statm >> pages >> resident)
    {
        const long pageSize = static_cast<long>(::sysconf(_SC_PAGESIZE));
        if (pageSize > 0)
            return static_cast<std::size_t>(resident) * static_cast<std::size_t>(pageSize);
    }
#endif
    return 0;
}

} // namespace

int main()
{
    AppConfig config = ConfigLoader::load();
    ScannerEngine scanner;

    const auto scanStart = std::chrono::steady_clock::now();
    const auto results = scanner.scan({}, config);
    const auto scanEnd = std::chrono::steady_clock::now();

    const std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "devclean-benchmark";
    std::filesystem::create_directories(tempDir / "nested");
    std::ofstream(tempDir / "nested" / "sample.txt") << "benchmark";

    CleanEngine cleaner;
    const auto deleteStart = std::chrono::steady_clock::now();
    const bool deleted = cleaner.removeDirectory(tempDir);
    const auto deleteEnd = std::chrono::steady_clock::now();

    const auto scanMs = std::chrono::duration_cast<std::chrono::milliseconds>(scanEnd - scanStart).count();
    const auto deleteMs = std::chrono::duration_cast<std::chrono::milliseconds>(deleteEnd - deleteStart).count();
    const auto memoryBytes = bestEffortResidentSetBytes();

    std::cout << "devclean benchmark\n";
    std::cout << "scan_time_ms=" << scanMs << '\n';
    std::cout << "scan_results=" << results.size() << '\n';
    std::cout << "delete_time_ms=" << deleteMs << '\n';
    std::cout << "delete_success=" << (deleted ? 1 : 0) << '\n';
    std::cout << "memory_bytes=" << memoryBytes << '\n';
    std::cout << "hardware_threads=" << std::thread::hardware_concurrency() << '\n';

    return deleted ? 0 : 1;
}
