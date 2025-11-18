#include "CacheManager.h"
#include "Session.h"
#include "platform/PlatformSDL.h"

#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;
namespace GameManager {

// Static member initialization
std::unordered_map<std::string, std::shared_ptr<Texture>> CacheManager::s_textureCache;
std::unordered_map<std::string, std::shared_ptr<Texture>> CacheManager::s_tempTextureCache;
std::unordered_map<std::string, TextureRecord> CacheManager::s_textureRecords;
std::mutex CacheManager::s_cacheLock;

void CacheManager::Clear(CacheType type)
{
    std::lock_guard<std::mutex> lock(s_cacheLock);

    std::cout << "CacheManager::Clear - Clearing cache type: " << static_cast<int>(type) << std::endl;

    // Always clear temporary textures
    s_tempTextureCache.clear();

    // Clear main cache based on type
    if (type != CacheType::None) {
        auto it = s_textureCache.begin();
        while (it != s_textureCache.end()) {
            const TextureRecord* record = GetTextureRecord(it->first);
            bool shouldRemove = false;

            if (type == CacheType::Live) {
                // Clear everything
                shouldRemove = true;
            }
            else if (type == CacheType::Scene) {
                // Clear Scene, Page, and Temp
                if (record && (record->CacheType == CacheType::Scene ||
                               record->CacheType == CacheType::Page ||
                               record->CacheType == CacheType::Temp)) {
                    shouldRemove = true;
                }
            }
            else if (type == CacheType::Page) {
                // Clear Page and Temp only
                if (record && (record->CacheType == CacheType::Page ||
                               record->CacheType == CacheType::Temp)) {
                    shouldRemove = true;
                }
            }
            else if (type == CacheType::Temp) {
                // Clear Temp only
                if (record && record->CacheType == CacheType::Temp) {
                    shouldRemove = true;
                }
            }

            if (shouldRemove) {
                it = s_textureCache.erase(it);
                std::cout << "  Cleared texture: " << (it != s_textureCache.end() ? (it)->first : "last") << std::endl;
            }
            else {
                ++it;
            }
        }
    }

    ValidateCache();
}

void CacheManager::Remove(const std::string& key)
{
    if (key.empty()) return;

    std::lock_guard<std::mutex> lock(s_cacheLock);
    
    auto it = s_textureCache.find(key);
    if (it != s_textureCache.end()) {
        std::cout << "CacheManager::Remove - Removing texture: " << key << std::endl;
        s_textureCache.erase(it);
    }
}

void CacheManager::RemoveTempTexture(const std::string& key)
{
    if (key.empty()) return;

    std::lock_guard<std::mutex> lock(s_cacheLock);
    
    auto it = s_tempTextureCache.find(key);
    if (it != s_tempTextureCache.end()) {
        std::cout << "CacheManager::RemoveTempTexture - Removing temp texture: " << key << std::endl;
        s_tempTextureCache.erase(it);
    }
}

std::shared_ptr<Texture> CacheManager::LoadTexture(const std::string& name, CacheType cacheType)
{
    if (name.empty()) return nullptr;

    {
        std::lock_guard<std::mutex> lock(s_cacheLock);
        
        // Check if already in cache
        auto it = s_textureCache.find(name);
        if (it != s_textureCache.end() && it->second != nullptr) {
            return it->second;
        }
    }

    // Load texture from file
    std::string filepath = ResolvePath(name);
    std::cout << "CacheManager::LoadTexture - Loading: " << name << " from " << filepath << std::endl;

    if (!fs::exists(filepath)) {
        std::cerr << "CacheManager::LoadTexture - File not found: " << filepath << std::endl;
        return nullptr;
    }

    // Get SDL renderer
    SDL_Renderer* renderer = Platform::GetSDLRenderer();
    if (!renderer) {
        std::cerr << "CacheManager::LoadTexture - Renderer not available" << std::endl;
        return nullptr;
    }

    // Load texture
    std::shared_ptr<Texture> texture = Texture::LoadFromFile(renderer, filepath);

    {
        std::lock_guard<std::mutex> lock(s_cacheLock);
        
        // Store in appropriate cache
        if (cacheType == CacheType::Temp) {
            s_tempTextureCache[name] = texture;
        }
        else {
            s_textureCache[name] = texture;
            
            // Update record if exists
            if (s_textureRecords.find(name) == s_textureRecords.end()) {
                s_textureRecords[name] = { name, cacheType, "" };
            }
            else {
                s_textureRecords[name].CacheType = cacheType;
            }
        }
    }

    return texture;
}

std::shared_ptr<Texture> CacheManager::LoadTempTexture(const std::string& name)
{
    return LoadTexture(name, CacheType::Temp);
}

size_t CacheManager::GetCachedTextureCount()
{
    std::lock_guard<std::mutex> lock(s_cacheLock);
    return s_textureCache.size();
}

size_t CacheManager::GetTempTextureCount()
{
    std::lock_guard<std::mutex> lock(s_cacheLock);
    return s_tempTextureCache.size();
}

size_t CacheManager::GetEstimatedMemoryUsage()
{
    std::lock_guard<std::mutex> lock(s_cacheLock);
    
    size_t totalBytes = 0;

    for (const auto& pair : s_textureCache) {
        if (pair.second) {
            // Rough estimate: width * height * 4 bytes per pixel (RGBA)
            totalBytes += static_cast<size_t>(pair.second->width()) * 
                         static_cast<size_t>(pair.second->height()) * 4;
        }
    }

    for (const auto& pair : s_tempTextureCache) {
        if (pair.second) {
            totalBytes += static_cast<size_t>(pair.second->width()) * 
                         static_cast<size_t>(pair.second->height()) * 4;
        }
    }

    return totalBytes;
}

void CacheManager::RegisterTextureRecord(const std::string& key, const TextureRecord& record)
{
    std::lock_guard<std::mutex> lock(s_cacheLock);
    s_textureRecords[key] = record;
    std::cout << "CacheManager::RegisterTextureRecord - Registered: " << key << std::endl;
}

const TextureRecord* CacheManager::GetTextureRecord(const std::string& key)
{
    auto it = s_textureRecords.find(key);
    if (it != s_textureRecords.end()) {
        return &it->second;
    }
    return nullptr;
}

void CacheManager::RegisterTextureAlias(const std::string& aliasKey, const std::string& sourceName)
{
    if (aliasKey.empty() || sourceName.empty()) return;

    // If alias already exists, nothing to do
    {
        std::lock_guard<std::mutex> lock(s_cacheLock);
        if (s_textureCache.find(aliasKey) != s_textureCache.end()) return;
        auto it = s_textureCache.find(sourceName);
        if (it != s_textureCache.end() && it->second) {
            s_textureCache[aliasKey] = it->second;
            return;
        }
    }

    // Not currently in cache; attempt to load the sourceName (may be a direct path)
    std::shared_ptr<Texture> tex = LoadTexture(sourceName, CacheType::Live);
    if (!tex) return;

    // Store under alias key
    {
        std::lock_guard<std::mutex> lock(s_cacheLock);
        s_textureCache[aliasKey] = tex;
        // Also ensure we have a record for the alias
        if (s_textureRecords.find(aliasKey) == s_textureRecords.end()) {
            s_textureRecords[aliasKey] = { aliasKey, CacheType::Live, "" };
        }
    }
}

bool CacheManager::IsCached(const std::string& key)
{
    std::lock_guard<std::mutex> lock(s_cacheLock);
    return s_textureCache.find(key) != s_textureCache.end() ||
           s_tempTextureCache.find(key) != s_tempTextureCache.end();
}

std::mutex& CacheManager::GetCacheLock()
{
    return s_cacheLock;
}

std::string CacheManager::ResolvePath(const std::string& name, const TextureRecord* record)
{
    // Try as-is first
    if (fs::exists(name)) {
        return name;
    }

    // Try in Content/Textures directory
    std::string pathWithContent = "Content/Textures/" + name;
    if (fs::exists(pathWithContent)) {
        return pathWithContent;
    }

    // Try corresponding ContentLite locations (some assets live in ContentLite)
    std::string pathWithContentLite = "ContentLite/Textures/" + name;
    if (fs::exists(pathWithContentLite)) {
        return pathWithContentLite;
    }

    std::string pathWithContentLiteResources = "ContentLite/Textures/Resources/" + name;
    if (fs::exists(pathWithContentLiteResources)) {
        return pathWithContentLiteResources;
    }

    // Try with extension if record provided
    if (record && !record->FileExtension.empty()) {
        std::string pathWithExt = "Content/Textures/" + name + record->FileExtension;
        if (fs::exists(pathWithExt)) {
            return pathWithExt;
        }
    }

    // Try common extensions
    const char* extensions[] = { ".png", ".jpg", ".jpeg", ".bmp" };
    for (const auto& ext : extensions) {
        std::string pathWithExt = "Content/Textures/" + name + ext;
        if (fs::exists(pathWithExt)) {
            return pathWithExt;
        }
    }

    // Return original name if all else fails (will fail to load)
    return name;
}

void CacheManager::ValidateCache()
{
    // Remove any null or invalid textures
    auto removeInvalid = [](auto& cache) {
        auto it = cache.begin();
        while (it != cache.end()) {
            if (!it->second) {
                std::cout << "CacheManager::ValidateCache - Removing invalid texture: " << it->first << std::endl;
                it = cache.erase(it);
            }
            else {
                ++it;
            }
        }
    };

    removeInvalid(s_textureCache);
    removeInvalid(s_tempTextureCache);

    size_t cachedCount = s_textureCache.size();
    size_t tempCount = s_tempTextureCache.size();
    size_t memoryMB = GetEstimatedMemoryUsage() / (1024 * 1024);

    std::cout << "CacheManager: Cache stats - Cached: " << cachedCount 
              << ", Temp: " << tempCount 
              << ", Memory: " << memoryMB << " MB" << std::endl;
}

} // namespace GameManager
