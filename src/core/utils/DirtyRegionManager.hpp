#pragma once

#include <lvgl.h>
#include <vector>

/**
 * @brief Gestionnaire de zones "dirty" pour optimiser les redraws
 */
class DirtyRegionManager {
public:
    struct Region {
        int16_t x1, y1, x2, y2;
        bool operator==(const Region& other) const {
            return x1 == other.x1 && y1 == other.y1 && x2 == other.x2 && y2 == other.y2;
        }
    };
    
    /**
     * @brief Marque une région comme nécessitant un redraw
     */
    static void markDirty(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
        dirty_regions_.push_back({x1, y1, x2, y2});
        coalesceDirtyRegions();
    }
    
    /**
     * @brief Optimise les régions en les fusionnant
     */
    static void coalesceDirtyRegions() {
        if (dirty_regions_.size() < 2) return;
        
        // Fusionner les régions qui se chevauchent
        for (size_t i = 0; i < dirty_regions_.size(); ++i) {
            for (size_t j = i + 1; j < dirty_regions_.size(); ++j) {
                if (regionsOverlap(dirty_regions_[i], dirty_regions_[j])) {
                    dirty_regions_[i] = mergeRegions(dirty_regions_[i], dirty_regions_[j]);
                    dirty_regions_.erase(dirty_regions_.begin() + j);
                    --j;
                }
            }
        }
    }
    
    /**
     * @brief Applique les invalidations à LVGL
     */
    static void applyToLVGL(lv_obj_t* obj) {
        for (const auto& region : dirty_regions_) {
            lv_area_t area = {region.x1, region.y1, region.x2, region.y2};
            lv_obj_invalidate_area(obj, &area);
        }
        dirty_regions_.clear();
    }

private:
    static std::vector<Region> dirty_regions_;
    
    static bool regionsOverlap(const Region& a, const Region& b) {
        return !(a.x2 < b.x1 || b.x2 < a.x1 || a.y2 < b.y1 || b.y2 < a.y1);
    }
    
    static Region mergeRegions(const Region& a, const Region& b) {
        return {
            std::min(a.x1, b.x1), std::min(a.y1, b.y1),
            std::max(a.x2, b.x2), std::max(a.y2, b.y2)
        };
    }
};