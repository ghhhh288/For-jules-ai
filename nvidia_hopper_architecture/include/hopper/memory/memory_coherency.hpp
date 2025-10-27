#pragma once

#include "../common.hpp"
#include "hbm3_memory.hpp"
#include <unordered_map>
#include <mutex>

namespace hopper::memory {

// Memory coherency protocols for Grace Hopper Superchip
enum class CoherencyState {
    INVALID,
    SHARED,
    EXCLUSIVE,
    MODIFIED
};

// Cache line for coherency tracking
struct CoherencyCacheLine {
    uint64_t address;
    CoherencyState state;
    uint32_t owner_id;
    std::vector<uint32_t> sharers;
    uint64_t timestamp;
    
    CoherencyCacheLine(uint64_t addr) : address(addr), state(CoherencyState::INVALID), 
                                       owner_id(0), timestamp(0) {}
};

// Memory Coherency Controller
class MemoryCoherencyController {
public:
    MemoryCoherencyController();
    ~MemoryCoherencyController() = default;
    
    void initialize();
    
    // Coherency operations
    CoherencyState request_cache_line(uint64_t address, uint32_t requester_id, bool for_write);
    void invalidate_cache_line(uint64_t address, uint32_t requester_id);
    void writeback_cache_line(uint64_t address, uint32_t owner_id);
    void share_cache_line(uint64_t address, uint32_t requester_id);
    
    // Coherency protocol management
    void enable_hardware_coherency(bool enable) { hardware_coherency_enabled_ = enable; }
    void set_coherency_protocol(const std::string& protocol) { coherency_protocol_ = protocol; }
    
    // Statistics and monitoring
    void print_coherency_statistics() const;
    double get_coherency_overhead() const;
    uint64_t get_coherency_messages() const { return coherency_messages_; }
    
private:
    bool hardware_coherency_enabled_ = true;
    std::string coherency_protocol_ = "MESI";
    
    // Coherency tracking
    std::unordered_map<uint64_t, std::unique_ptr<CoherencyCacheLine>> coherency_table_;
    mutable std::mutex coherency_mutex_;
    
    // Statistics
    mutable uint64_t coherency_messages_ = 0;
    mutable uint64_t cache_invalidations_ = 0;
    mutable uint64_t writebacks_ = 0;
    mutable uint64_t sharing_requests_ = 0;
    
    // Helper methods
    void send_coherency_message(uint32_t target_id, const std::string& message_type);
    void update_coherency_state(uint64_t address, CoherencyState new_state, uint32_t owner_id);
    bool is_cache_line_shared(uint64_t address) const;
};

// Unified Memory Address Space (Grace Hopper)
class UnifiedMemorySpace {
public:
    UnifiedMemorySpace();
    ~UnifiedMemorySpace() = default;
    
    void initialize();
    
    // Memory space management
    void register_memory_region(const std::string& region_name, uint64_t base_address, 
                               uint64_t size, const std::string& memory_type);
    
    // Address translation
    uint64_t translate_virtual_to_physical(uint64_t virtual_address);
    std::string get_memory_type_for_address(uint64_t address);
    
    // Unified memory operations
    void* allocate_unified_memory(uint64_t size, const std::string& preferred_location = "GPU");
    void deallocate_unified_memory(void* ptr);
    void migrate_memory(void* ptr, const std::string& target_location);
    
    // Memory access optimization
    void enable_automatic_migration(bool enable) { auto_migration_enabled_ = enable; }
    void set_migration_threshold(double threshold) { migration_threshold_ = threshold; }
    
    // Statistics
    void print_memory_space_info() const;
    uint64_t get_total_addressable_memory() const;
    
private:
    struct MemoryRegion {
        std::string name;
        uint64_t base_address;
        uint64_t size;
        std::string memory_type;
        uint64_t usage_count = 0;
    };
    
    std::vector<MemoryRegion> memory_regions_;
    bool auto_migration_enabled_ = true;
    double migration_threshold_ = 0.8; // 80% access ratio
    
    // Grace Hopper memory configuration
    static constexpr uint64_t GPU_HBM3_BASE = 0x0;
    static constexpr uint64_t GPU_HBM3_SIZE = 96ULL * 1024 * 1024 * 1024; // 96 GB
    static constexpr uint64_t CPU_LPDDR5X_BASE = GPU_HBM3_BASE + GPU_HBM3_SIZE;
    static constexpr uint64_t CPU_LPDDR5X_SIZE = 512ULL * 1024 * 1024 * 1024; // 512 GB
    
    // Memory allocation tracking
    std::unordered_map<void*, uint64_t> allocation_sizes_;
    std::unordered_map<void*, std::string> allocation_locations_;
    
    MemoryRegion* find_memory_region(uint64_t address);
    void setup_default_memory_regions();
};

// Memory Bandwidth Optimization
class MemoryBandwidthOptimizer {
public:
    MemoryBandwidthOptimizer();
    ~MemoryBandwidthOptimizer() = default;
    
    void initialize();
    
    // Bandwidth optimization techniques
    void enable_memory_coalescing(bool enable) { memory_coalescing_enabled_ = enable; }
    void enable_burst_mode(bool enable) { burst_mode_enabled_ = enable; }
    void set_prefetch_distance(uint32_t distance) { prefetch_distance_ = distance; }
    
    // Memory access pattern analysis
    void analyze_access_pattern(const std::vector<uint64_t>& addresses);
    double calculate_bandwidth_efficiency(uint64_t bytes_transferred, double time_seconds);
    
    // Optimization recommendations
    std::vector<std::string> get_optimization_recommendations() const;
    void apply_automatic_optimizations();
    
    // Performance monitoring
    void print_bandwidth_statistics() const;
    double get_effective_bandwidth_utilization() const;
    
private:
    bool memory_coalescing_enabled_ = true;
    bool burst_mode_enabled_ = true;
    uint32_t prefetch_distance_ = 8;
    
    // Access pattern analysis
    struct AccessPattern {
        bool is_sequential = false;
        bool is_strided = false;
        uint64_t stride_size = 0;
        double locality_score = 0.0;
    };
    
    AccessPattern current_pattern_;
    
    // Statistics
    mutable uint64_t total_memory_accesses_ = 0;
    mutable uint64_t coalesced_accesses_ = 0;
    mutable double average_bandwidth_utilization_ = 0.0;
    
    void detect_access_pattern(const std::vector<uint64_t>& addresses);
    double calculate_locality_score(const std::vector<uint64_t>& addresses);
};

} // namespace hopper::memory