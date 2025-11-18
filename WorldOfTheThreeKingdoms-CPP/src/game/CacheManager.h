#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <mutex>
#include <vector>
#include "platform/Types.h"
#include "platform/Texture.h"

namespace GameManager {

/// <summary>
/// Cache type enumeration for resource eviction policies
/// </summary>
enum class CacheType {
    Live,   // Never cleared (persistent across entire game)
    Scene,  // Cleared when scene changes
    Page,   // Cleared when page/screen changes
    Temp,   // Cleared immediately when done
    None    // Unmanaged
};

/// <summary>
/// Texture cache entry metadata
/// </summary>
struct TextureRecord {
    std::string Key;
    CacheType CacheType = CacheType::Temp;
    std::string FileExtension;  // e.g., ".jpg", ".png"
};

/// <summary>
/// CacheManager handles all texture caching with different eviction policies.
/// Mirrors C# GameManager.CacheManager (753 lines).
/// Thread-safe resource management for textures and fonts.
/// </summary>
class CacheManager {
public:
    /// <summary>
    /// Clear cached resources based on type
    /// </summary>
    static void Clear(CacheType type);

    /// <summary>
    /// Remove a specific texture from cache
    /// </summary>
    static void Remove(const std::string& key);

    /// <summary>
    /// Remove a temporary texture from cache
    /// </summary>
    static void RemoveTempTexture(const std::string& key);

    /// <summary>
    /// Load or get a cached texture
    /// </summary>
    static std::shared_ptr<Texture> LoadTexture(const std::string& name, CacheType cacheType = CacheType::Live);

    /// <summary>
    /// Load a temporary texture (not cached permanently)
    /// </summary>
    static std::shared_ptr<Texture> LoadTempTexture(const std::string& name);

    /// <summary>
    /// Get texture statistics
    /// </summary>
    static size_t GetCachedTextureCount();
    static size_t GetTempTextureCount();
    static size_t GetEstimatedMemoryUsage();

    /// <summary>
    /// Register a texture record (metadata about texture)
    /// </summary>
    static void RegisterTextureRecord(const std::string& key, const TextureRecord& record);

    /// <summary>
    /// Register an alias key that points to an existing texture or file path.
    /// Useful to map logical resource keys (e.g. "Maps/overview") to actual files.
    /// </summary>
    static void RegisterTextureAlias(const std::string& aliasKey, const std::string& sourceName);

    /// <summary>
    /// Get texture record
    /// </summary>
    static const TextureRecord* GetTextureRecord(const std::string& key);

    /// <summary>
    /// Check if texture is in cache
    /// </summary>
    static bool IsCached(const std::string& key);

    /// <summary>
    /// Get global cache lock for thread safety
    /// </summary>
    static std::mutex& GetCacheLock();

private:
    // Main texture cache (permanent and scene-based)
    static std::unordered_map<std::string, std::shared_ptr<Texture>> s_textureCache;
    
    // Temporary texture cache
    static std::unordered_map<std::string, std::shared_ptr<Texture>> s_tempTextureCache;
    
    // Texture metadata records
    static std::unordered_map<std::string, TextureRecord> s_textureRecords;
    
    // Thread safety lock
    static std::mutex s_cacheLock;

    /// <summary>
    /// Internal helper to resolve texture path
    /// </summary>
    static std::string ResolvePath(const std::string& name, const TextureRecord* record = nullptr);

    /// <summary>
    /// Validate and cleanup disposed textures
    /// </summary>
    static void ValidateCache();
};

} // namespace GameManager
