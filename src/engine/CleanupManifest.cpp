#include "engine/CleanupManifest.hpp"

#include <chrono>
#include <iomanip>
#include <random>
#include <sstream>

std::string createCleanupOperationId()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    std::mt19937_64 generator(static_cast<uint64_t>(now));
    std::ostringstream stream;
    stream << std::hex << static_cast<uint64_t>(now) << '-' << generator();
    return stream.str();
}

std::string serializeCleanupManifest(const CleanupManifest& manifest)
{
    std::ostringstream output;
    output << "{\n";
    output << "  \"operation_id\": \"" << manifest.operationId << "\",\n";
    output << "  \"created_at\": \"" << manifest.createdAt << "\",\n";
    output << "  \"entries\": [\n";
    for (std::size_t i = 0; i < manifest.entries.size(); ++i)
    {
        const auto& entry = manifest.entries[i];
        output << "    {\"cache\": \"" << entry.cache
               << "\", \"path\": \"" << entry.path
               << "\", \"bytes\": " << entry.bytes
               << ", \"completed\": " << (entry.completed ? "true" : "false") << "}";
        if (i + 1 < manifest.entries.size())
            output << ',';
        output << '\n';
    }
    output << "  ]\n}";
    return output.str();
}
