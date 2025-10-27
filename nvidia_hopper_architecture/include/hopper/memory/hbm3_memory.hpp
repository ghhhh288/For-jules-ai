#pragma once

#include "../gpu_architecture.hpp"
#include "../common.hpp"
#include <vector>
#include <memory>

namespace hopper::memory {

// HBM3 Memory Stack
class HBM3Stack {
public:
    HBM3Stack(uint32_t stack_id, uint64_t capacity_gb);
    ~HBM3Stack() = default;
    
    void initialize();
    
    // Memory operations
    void read_data(uint64_t address, uint64_t size);
    void write_data(uint64_t address, uint64_t size);
    
    // Performance characteristics
    double get_bandwidth_gb_s() const { return bandwidth_gb_s_; }
    double get_latency_ns() const { return latency_ns_; }
    uint64_t get_capacity_gb() const { return capacity_gb_; }
    
    // Power and thermal
    double get_power_consumption() const { return power_watts_; }
    double get_temperature() const { return temperature_celsius_; }
    
    void print_stack_info() const;
    
private:
    uint32_t stack_id_;
    uint64_t capacity_gb_;
    double bandwidth_gb_s_;
    double latency_ns_;
    double power_watts_;
    double temperature_celsius_;
    
    // HBM3 specifications
    static constexpr double HBM3_BANDWIDTH_PER_STACK = 375.0; // GB/s per stack
    static constexpr double HBM3_LATENCY = 100.0; // ns
    static constexpr double HBM3_POWER_PER_GB = 2.5; // Watts per GB
};

// HBM3 Memory System (multiple stacks)
class HBM3MemorySystem : public MemorySystem {
public:
    HBM3MemorySystem(uint32_t num_stacks = 8, uint64_t capacity_per_stack_gb = 12);
    ~HBM3MemorySystem() override = default;
    
    void initialize() override;
    uint64_t get_capacity() const override { return total_capacity_; }
    double get_bandwidth() const override { return bandwidth_gb_s_; }
    double get_latency() const override { return latency_ns_; }
    void allocate_memory(uint64_t size) override;
    void deallocate_memory(uint64_t size) override;
    std::string get_memory_type() const override { return "HBM3"; }
    
    // HBM3 specific methods
    void enable_compression(bool enable) { compression_enabled_ = enable; }
    void enable_prefetching(bool enable) { prefetching_enabled_ = enable; }
    void set_refresh_rate(double rate_hz) { refresh_rate_hz_ = rate_hz; }
    
    // Memory efficiency features
    double get_effective_bandwidth() const;
    double get_compression_ratio() const;
    uint64_t get_available_capacity() const;
    
    // Performance monitoring
    void update_memory_statistics(uint64_t read_bytes, uint64_t write_bytes);
    void print_memory_statistics() const;
    
private:
    std::vector<std::unique_ptr<HBM3Stack>> memory_stacks_;
    uint32_t num_stacks_;
    bool compression_enabled_ = true;
    bool prefetching_enabled_ = true;
    double refresh_rate_hz_ = 32.0; // 32ms refresh
    
    // Statistics
    mutable uint64_t total_reads_ = 0;
    mutable uint64_t total_writes_ = 0;
    mutable double compression_ratio_ = 1.2; // 20% compression
    
    void calculate_aggregate_performance();
};

// Cache hierarchy
class CacheLevel {
public:
    CacheLevel(const std::string& name, uint64_t size_kb, double bandwidth_gb_s, 
               double latency_ns, uint32_t associativity);
    ~CacheLevel() = default;
    
    void initialize();
    
    // Cache operations
    bool lookup(uint64_t address);
    void insert(uint64_t address, uint64_t size);
    void evict(uint64_t address);
    void invalidate();
    
    // Cache statistics
    double get_hit_rate() const;
    double get_miss_rate() const { return 1.0 - get_hit_rate(); }
    uint64_t get_hits() const { return cache_hits_; }
    uint64_t get_misses() const { return cache_misses_; }
    
    // Performance characteristics
    uint64_t get_size_kb() const { return size_kb_; }
    double get_bandwidth_gb_s() const { return bandwidth_gb_s_; }
    double get_latency_ns() const { return latency_ns_; }
    
    void print_cache_info() const;
    void reset_statistics();
    
private:
    std::string name_;
    uint64_t size_kb_;
    double bandwidth_gb_s_;
    double latency_ns_;
    uint32_t associativity_;
    
    // Cache statistics
    mutable uint64_t cache_hits_ = 0;
    mutable uint64_t cache_misses_ = 0;
    mutable uint64_t total_accesses_ = 0;
    
    // Simple cache simulation
    std::vector<uint64_t> cache_lines_;
    uint64_t next_eviction_index_ = 0;
};

// Hopper Cache Hierarchy
class HopperCacheHierarchy {
public:
    HopperCacheHierarchy();
    ~HopperCacheHierarchy() = default;
    
    void initialize();
    
    // Cache access simulation
    double access_memory(uint64_t address, uint64_t size, bool is_write = false);
    
    // Cache management
    void flush_all_caches();
    void prefetch_data(uint64_t address, uint64_t size);
    
    // Performance analysis
    void print_cache_hierarchy_info() const;
    void print_cache_statistics() const;
    double get_average_access_latency() const;
    
private:
    std::unique_ptr<CacheLevel> l1_cache_;      // 256 KB per SM (shared + L1)
    std::unique_ptr<CacheLevel> l2_cache_;      // 60 MB shared
    std::unique_ptr<HBM3MemorySystem> hbm3_;   // Main memory
    
    // Cache hierarchy specifications
    static constexpr uint64_t L1_SIZE_KB = 256;
    static constexpr double L1_BANDWIDTH_GB_S = 1000.0;
    static constexpr double L1_LATENCY_NS = 1.0;
    
    static constexpr uint64_t L2_SIZE_KB = 60 * 1024; // 60 MB
    static constexpr double L2_BANDWIDTH_GB_S = 500.0;
    static constexpr double L2_LATENCY_NS = 10.0;
    
    double simulate_cache_access(uint64_t address, uint64_t size);
};

} // namespace hopper::memory