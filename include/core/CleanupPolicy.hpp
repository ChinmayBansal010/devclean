#pragma once

#include "scanner/ScanResult.hpp"

#include <cstdint>

bool isCleanupSafe(const ScanResult& result);
int cleanupPriority(const ScanResult& result, bool safe);
bool cleanupCandidateMatches(const ScanResult& result, bool safeOnly);
uint64_t cleanupEffectiveBytes(const ScanResult& result);
