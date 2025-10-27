#pragma once

#include "../gpu_architecture.hpp"
#include "../common.hpp"
#include <vector>
#include <memory>

namespace hopper::interconnect {

// NVLink Chip-to-Chip (C2C) Interconnect
class NVLinkC2C : public InterconnectSystem {
public:
    NVLinkC2C();
    ~NVLinkC2C() override = default;
    
    void initialize() override;
    double get_bandwidth() const override { return bandwidth_gb_s_; }
    double get_latency() const override { return latency_ns_; }
    void establish_connection(uint32_t source, uint32_t destination) override;
    std::string get_interconnect_type() const override { return "NVLink-C2C"; }
    
    // C2C specific methods
    void enable_cache_coherency(bool enable) { cache_coherency_enabled_ = enable; }
    void enable_unified_memory_access(bool enable) { unified_memory_enabled_ = enable; }
    
    // Memory access through C2C
    void cpu_to_gpu_transfer(uint64_t size_bytes);
    void gpu_to_cpu_transfer(uint64_t size_bytes);
    double get_transfer_time(uint64_t size_bytes) const;
    
    // Performance characteristics
    double get_bidirectional_bandwidth() const { return bandwidth_gb_s_; }
    double get_unidirectional_bandwidth() const { return bandwidth_gb_s_ / 2.0; }
    uint32_t get_lane_count() const { return lane_count_; }
    
    // Comparison with PCIe
    double get_pcie_gen5_speedup() const;
    void print_c2c_advantages() const;
    
    // Statistics and monitoring
    void print_transfer_statistics() const;
    void reset_statistics();
    
private:
    bool cache_coherency_enabled_ = true;
    bool unified_memory_enabled_ = true;
    uint32_t lane_count_ = 18; // 18 lanes for C2C
    
    // NVLink-C2C specifications
    static constexpr double C2C_BANDWIDTH_GB_S = 900.0; // 900 GB/s total (450 each direction)
    static constexpr double C2C_LATENCY_NS = 50.0; // Low latency
    static constexpr double PCIE_GEN5_BANDWIDTH_GB_S = 128.0; // x16 PCIe Gen5
    
    // Transfer statistics
    mutable uint64_t cpu_to_gpu_bytes_ = 0;
    mutable uint64_t gpu_to_cpu_bytes_ = 0;
    mutable uint64_t total_transfers_ = 0;
    mutable double total_transfer_time_ = 0.0;
    
    void update_transfer_statistics(uint64_t bytes, double time_seconds);
};

// NVLink Switch System for multi-GPU scaling
class NVLinkSwitchSystem : public InterconnectSystem {
public:
    NVLinkSwitchSystem(uint32_t max_gpus = 256);
    ~NVLinkSwitchSystem() override = default;
    
    void initialize() override;
    double get_bandwidth() const override;
    double get_latency() const override { return latency_ns_; }
    void establish_connection(uint32_t source, uint32_t destination) override;
    std::string get_interconnect_type() const override { return "NVLink Switch System"; }
    
    // Multi-GPU management
    void add_gpu(uint32_t gpu_id);
    void remove_gpu(uint32_t gpu_id);
    uint32_t get_connected_gpu_count() const { return connected_gpus_.size(); }
    
    // Memory access across GPUs
    void gpu_to_gpu_transfer(uint32_t source_gpu, uint32_t dest_gpu, uint64_t size_bytes);
    double calculate_transfer_time(uint32_t source_gpu, uint32_t dest_gpu, uint64_t size_bytes);
    
    // Topology management
    void configure_topology(const std::string& topology_type);
    std::vector<uint32_t> get_shortest_path(uint32_t source, uint32_t destination);
    
    // Collective operations
    void all_reduce_operation(const std::vector<uint32_t>& participating_gpus, uint64_t data_size);
    void all_gather_operation(const std::vector<uint32_t>& participating_gpus, uint64_t data_size);
    void broadcast_operation(uint32_t source_gpu, const std::vector<uint32_t>& target_gpus, 
                           uint64_t data_size);
    
    // Performance analysis
    void print_topology_info() const;
    void print_bandwidth_matrix() const;
    double get_aggregate_bandwidth() const;
    
    // Scalability metrics
    uint64_t get_total_addressable_memory() const;
    void print_scalability_metrics() const;
    
private:
    uint32_t max_gpus_;
    std::vector<uint32_t> connected_gpus_;
    std::string topology_type_ = "Fat Tree";
    
    // NVLink Switch specifications
    static constexpr double NVLINK_4_BANDWIDTH_PER_LINK = 100.0; // GB/s per link
    static constexpr double SWITCH_LATENCY_NS = 100.0;
    static constexpr uint32_t LINKS_PER_GPU = 18; // 18 NVLink connections per GPU
    
    // Topology and routing
    std::vector<std::vector<uint32_t>> adjacency_matrix_;
    std::map<std::pair<uint32_t, uint32_t>, std::vector<uint32_t>> routing_table_;
    
    // Statistics
    mutable uint64_t total_gpu_transfers_ = 0;
    mutable uint64_t total_collective_operations_ = 0;
    mutable double total_communication_time_ = 0.0;
    
    void build_adjacency_matrix();
    void calculate_shortest_paths();
    double calculate_hop_latency(uint32_t hop_count) const;
    void update_communication_statistics(uint64_t bytes, double time_seconds);
};

// NVLink Performance Analyzer
class NVLinkPerformanceAnalyzer {
public:
    NVLinkPerformanceAnalyzer();
    ~NVLinkPerformanceAnalyzer() = default;
    
    void initialize();
    
    // Performance benchmarking
    void benchmark_c2c_performance(NVLinkC2C& c2c_link);
    void benchmark_switch_performance(NVLinkSwitchSystem& switch_system);
    
    // Bandwidth utilization analysis
    double analyze_bandwidth_utilization(const std::vector<double>& transfer_sizes,
                                       const std::vector<double>& transfer_times);
    
    // Latency analysis
    void measure_latency_characteristics(InterconnectSystem& interconnect);
    double get_average_latency() const { return average_latency_ns_; }
    double get_peak_bandwidth() const { return peak_bandwidth_gb_s_; }
    
    // Comparison analysis
    void compare_with_alternatives() const;
    void print_performance_summary() const;
    
    // Optimization recommendations
    std::vector<std::string> get_optimization_recommendations() const;
    
private:
    double average_latency_ns_ = 0.0;
    double peak_bandwidth_gb_s_ = 0.0;
    double average_bandwidth_utilization_ = 0.0;
    
    // Benchmark results
    struct BenchmarkResult {
        std::string test_name;
        double bandwidth_gb_s;
        double latency_ns;
        double efficiency_percent;
    };
    
    std::vector<BenchmarkResult> benchmark_results_;
    
    void run_bandwidth_test(InterconnectSystem& interconnect, uint64_t transfer_size);
    void run_latency_test(InterconnectSystem& interconnect);
    void analyze_results();
};

} // namespace hopper::interconnect