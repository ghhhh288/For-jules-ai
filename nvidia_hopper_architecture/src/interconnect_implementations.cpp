#include "hopper/interconnect/nvlink_c2c.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace hopper::interconnect {

// NVLinkC2C Implementation
NVLinkC2C::NVLinkC2C() {
    bandwidth_gb_s_ = C2C_BANDWIDTH_GB_S;
    latency_ns_ = C2C_LATENCY_NS;
}

void NVLinkC2C::initialize() {
    std::cout << "NVLink-C2C initialized with " << bandwidth_gb_s_ << " GB/s bandwidth\n";
}

void NVLinkC2C::establish_connection(uint32_t source, uint32_t destination) {
    connections_.emplace_back(source, destination);
    std::cout << "NVLink-C2C connection established between " << source << " and " << destination << "\n";
}

void NVLinkC2C::cpu_to_gpu_transfer(uint64_t size_bytes) {
    double transfer_time = static_cast<double>(size_bytes) / (bandwidth_gb_s_ * 1e9);
    update_transfer_statistics(size_bytes, transfer_time);
    cpu_to_gpu_bytes_ += size_bytes;
}

void NVLinkC2C::gpu_to_cpu_transfer(uint64_t size_bytes) {
    double transfer_time = static_cast<double>(size_bytes) / (bandwidth_gb_s_ * 1e9);
    update_transfer_statistics(size_bytes, transfer_time);
    gpu_to_cpu_bytes_ += size_bytes;
}

double NVLinkC2C::get_transfer_time(uint64_t size_bytes) const {
    return static_cast<double>(size_bytes) / (bandwidth_gb_s_ * 1e9) + latency_ns_ * 1e-9;
}

double NVLinkC2C::get_pcie_gen5_speedup() const {
    return bandwidth_gb_s_ / PCIE_GEN5_BANDWIDTH_GB_S;
}

void NVLinkC2C::print_c2c_advantages() const {
    std::cout << "NVLink-C2C Advantages:\n";
    std::cout << "  Bandwidth: " << bandwidth_gb_s_ << " GB/s (vs " << PCIE_GEN5_BANDWIDTH_GB_S 
              << " GB/s PCIe Gen5)\n";
    std::cout << "  Speedup over PCIe: " << std::fixed << std::setprecision(1) 
              << get_pcie_gen5_speedup() << "x\n";
    std::cout << "  Hardware Cache Coherency: " << (cache_coherency_enabled_ ? "Yes" : "No") << "\n";
    std::cout << "  Unified Memory Access: " << (unified_memory_enabled_ ? "Yes" : "No") << "\n";
    std::cout << "  Latency: " << latency_ns_ << " ns\n";
}

void NVLinkC2C::print_transfer_statistics() const {
    std::cout << "NVLink-C2C Transfer Statistics:\n";
    std::cout << "  CPU to GPU: " << Utils::format_number(cpu_to_gpu_bytes_) << " bytes\n";
    std::cout << "  GPU to CPU: " << Utils::format_number(gpu_to_cpu_bytes_) << " bytes\n";
    std::cout << "  Total Transfers: " << total_transfers_ << "\n";
    std::cout << "  Total Transfer Time: " << std::fixed << std::setprecision(3) 
              << total_transfer_time_ << " seconds\n";
    
    if (total_transfer_time_ > 0) {
        double avg_bandwidth = (cpu_to_gpu_bytes_ + gpu_to_cpu_bytes_) / (total_transfer_time_ * 1e9);
        std::cout << "  Average Bandwidth Utilization: " << std::fixed << std::setprecision(1) 
                  << avg_bandwidth << " GB/s (" << (avg_bandwidth / bandwidth_gb_s_) * 100 << "%)\n";
    }
}

void NVLinkC2C::reset_statistics() {
    cpu_to_gpu_bytes_ = 0;
    gpu_to_cpu_bytes_ = 0;
    total_transfers_ = 0;
    total_transfer_time_ = 0.0;
}

void NVLinkC2C::update_transfer_statistics(uint64_t bytes, double time_seconds) {
    total_transfers_++;
    total_transfer_time_ += time_seconds;
}

// NVLinkSwitchSystem Implementation
NVLinkSwitchSystem::NVLinkSwitchSystem(uint32_t max_gpus) : max_gpus_(max_gpus) {
    bandwidth_gb_s_ = NVLINK_4_BANDWIDTH_PER_LINK * LINKS_PER_GPU;
    latency_ns_ = SWITCH_LATENCY_NS;
    
    // Initialize adjacency matrix
    adjacency_matrix_.resize(max_gpus, std::vector<uint32_t>(max_gpus, 0));
}

void NVLinkSwitchSystem::initialize() {
    std::cout << "NVLink Switch System initialized for up to " << max_gpus_ << " GPUs\n";
    build_adjacency_matrix();
    calculate_shortest_paths();
}

double NVLinkSwitchSystem::get_bandwidth() const {
    return bandwidth_gb_s_ * connected_gpus_.size();
}

void NVLinkSwitchSystem::establish_connection(uint32_t source, uint32_t destination) {
    if (source < max_gpus_ && destination < max_gpus_) {
        adjacency_matrix_[source][destination] = 1;
        adjacency_matrix_[destination][source] = 1;
        connections_.emplace_back(source, destination);
    }
}

void NVLinkSwitchSystem::add_gpu(uint32_t gpu_id) {
    if (std::find(connected_gpus_.begin(), connected_gpus_.end(), gpu_id) == connected_gpus_.end()) {
        connected_gpus_.push_back(gpu_id);
        std::cout << "GPU " << gpu_id << " added to NVLink Switch System\n";
    }
}

void NVLinkSwitchSystem::remove_gpu(uint32_t gpu_id) {
    auto it = std::find(connected_gpus_.begin(), connected_gpus_.end(), gpu_id);
    if (it != connected_gpus_.end()) {
        connected_gpus_.erase(it);
        std::cout << "GPU " << gpu_id << " removed from NVLink Switch System\n";
    }
}

void NVLinkSwitchSystem::gpu_to_gpu_transfer(uint32_t source_gpu, uint32_t dest_gpu, uint64_t size_bytes) {
    double transfer_time = calculate_transfer_time(source_gpu, dest_gpu, size_bytes);
    update_communication_statistics(size_bytes, transfer_time);
    total_gpu_transfers_++;
}

double NVLinkSwitchSystem::calculate_transfer_time(uint32_t source_gpu, uint32_t dest_gpu, uint64_t size_bytes) {
    auto path = get_shortest_path(source_gpu, dest_gpu);
    uint32_t hop_count = path.size() - 1;
    
    double transfer_time = static_cast<double>(size_bytes) / (NVLINK_4_BANDWIDTH_PER_LINK * 1e9);
    double total_latency = calculate_hop_latency(hop_count);
    
    return transfer_time + total_latency;
}

void NVLinkSwitchSystem::configure_topology(const std::string& topology_type) {
    topology_type_ = topology_type;
    std::cout << "NVLink topology configured as: " << topology_type << "\n";
    
    // Rebuild adjacency matrix based on topology
    build_adjacency_matrix();
    calculate_shortest_paths();
}

std::vector<uint32_t> NVLinkSwitchSystem::get_shortest_path(uint32_t source, uint32_t destination) {
    // Simplified shortest path (direct connection assumed)
    return {source, destination};
}

void NVLinkSwitchSystem::all_reduce_operation(const std::vector<uint32_t>& participating_gpus, uint64_t data_size) {
    std::cout << "All-reduce operation across " << participating_gpus.size() << " GPUs\n";
    
    // Simulate ring all-reduce
    double total_time = 0.0;
    for (size_t i = 0; i < participating_gpus.size(); ++i) {
        uint32_t source = participating_gpus[i];
        uint32_t dest = participating_gpus[(i + 1) % participating_gpus.size()];
        total_time += calculate_transfer_time(source, dest, data_size);
    }
    
    update_communication_statistics(data_size * participating_gpus.size(), total_time);
    total_collective_operations_++;
}

void NVLinkSwitchSystem::all_gather_operation(const std::vector<uint32_t>& participating_gpus, uint64_t data_size) {
    std::cout << "All-gather operation across " << participating_gpus.size() << " GPUs\n";
    
    double total_time = 0.0;
    for (size_t i = 0; i < participating_gpus.size(); ++i) {
        for (size_t j = 0; j < participating_gpus.size(); ++j) {
            if (i != j) {
                total_time += calculate_transfer_time(participating_gpus[i], participating_gpus[j], data_size);
            }
        }
    }
    
    update_communication_statistics(data_size * participating_gpus.size() * (participating_gpus.size() - 1), total_time);
    total_collective_operations_++;
}

void NVLinkSwitchSystem::broadcast_operation(uint32_t source_gpu, const std::vector<uint32_t>& target_gpus, 
                                           uint64_t data_size) {
    std::cout << "Broadcast operation from GPU " << source_gpu << " to " << target_gpus.size() << " GPUs\n";
    
    double total_time = 0.0;
    for (uint32_t target : target_gpus) {
        total_time += calculate_transfer_time(source_gpu, target, data_size);
    }
    
    update_communication_statistics(data_size * target_gpus.size(), total_time);
    total_collective_operations_++;
}

void NVLinkSwitchSystem::print_topology_info() const {
    std::cout << "NVLink Switch System Topology:\n";
    std::cout << "  Topology Type: " << topology_type_ << "\n";
    std::cout << "  Connected GPUs: " << connected_gpus_.size() << "/" << max_gpus_ << "\n";
    std::cout << "  Links per GPU: " << LINKS_PER_GPU << "\n";
    std::cout << "  Bandwidth per Link: " << NVLINK_4_BANDWIDTH_PER_LINK << " GB/s\n";
    std::cout << "  Switch Latency: " << latency_ns_ << " ns\n";
}

void NVLinkSwitchSystem::print_bandwidth_matrix() const {
    std::cout << "GPU-to-GPU Bandwidth Matrix (GB/s):\n";
    std::cout << "     ";
    for (uint32_t gpu : connected_gpus_) {
        std::cout << std::setw(6) << gpu;
    }
    std::cout << "\n";
    
    for (uint32_t source : connected_gpus_) {
        std::cout << std::setw(3) << source << ": ";
        for (uint32_t dest : connected_gpus_) {
            if (source == dest) {
                std::cout << std::setw(6) << "-";
            } else {
                std::cout << std::setw(6) << static_cast<int>(NVLINK_4_BANDWIDTH_PER_LINK);
            }
        }
        std::cout << "\n";
    }
}

double NVLinkSwitchSystem::get_aggregate_bandwidth() const {
    return NVLINK_4_BANDWIDTH_PER_LINK * LINKS_PER_GPU * connected_gpus_.size() / 2; // Bidirectional
}

uint64_t NVLinkSwitchSystem::get_total_addressable_memory() const {
    // Assume 96 GB HBM3 + 512 GB LPDDR5X per GPU
    return connected_gpus_.size() * (96 + 512);
}

void NVLinkSwitchSystem::print_scalability_metrics() const {
    std::cout << "NVLink Switch System Scalability:\n";
    std::cout << "  Total Addressable Memory: " << get_total_addressable_memory() << " GB\n";
    std::cout << "  Aggregate Bandwidth: " << get_aggregate_bandwidth() << " GB/s\n";
    std::cout << "  GPU-to-GPU Transfers: " << total_gpu_transfers_ << "\n";
    std::cout << "  Collective Operations: " << total_collective_operations_ << "\n";
    std::cout << "  Total Communication Time: " << std::fixed << std::setprecision(3) 
              << total_communication_time_ << " seconds\n";
}

void NVLinkSwitchSystem::build_adjacency_matrix() {
    // Initialize all connections to 0
    for (auto& row : adjacency_matrix_) {
        std::fill(row.begin(), row.end(), 0);
    }
    
    // Build topology based on type
    if (topology_type_ == "Fat Tree") {
        // Simplified fat tree topology
        for (size_t i = 0; i < connected_gpus_.size(); ++i) {
            for (size_t j = i + 1; j < connected_gpus_.size(); ++j) {
                adjacency_matrix_[connected_gpus_[i]][connected_gpus_[j]] = 1;
                adjacency_matrix_[connected_gpus_[j]][connected_gpus_[i]] = 1;
            }
        }
    }
}

void NVLinkSwitchSystem::calculate_shortest_paths() {
    // Floyd-Warshall algorithm for shortest paths
    // Simplified implementation for demonstration
    for (uint32_t k = 0; k < max_gpus_; ++k) {
        for (uint32_t i = 0; i < max_gpus_; ++i) {
            for (uint32_t j = 0; j < max_gpus_; ++j) {
                if (adjacency_matrix_[i][k] && adjacency_matrix_[k][j]) {
                    adjacency_matrix_[i][j] = 1;
                }
            }
        }
    }
}

double NVLinkSwitchSystem::calculate_hop_latency(uint32_t hop_count) const {
    return hop_count * latency_ns_ * 1e-9; // Convert to seconds
}

void NVLinkSwitchSystem::update_communication_statistics(uint64_t bytes, double time_seconds) {
    total_communication_time_ += time_seconds;
}

// NVLinkPerformanceAnalyzer Implementation
NVLinkPerformanceAnalyzer::NVLinkPerformanceAnalyzer() {}

void NVLinkPerformanceAnalyzer::initialize() {
    std::cout << "NVLink Performance Analyzer initialized\n";
}

void NVLinkPerformanceAnalyzer::benchmark_c2c_performance(NVLinkC2C& c2c_link) {
    std::cout << "Benchmarking NVLink-C2C performance...\n";
    
    // Test different transfer sizes
    std::vector<uint64_t> transfer_sizes = {1024, 1024*1024, 100*1024*1024, 1024*1024*1024}; // 1KB to 1GB
    
    for (uint64_t size : transfer_sizes) {
        auto start = std::chrono::high_resolution_clock::now();
        c2c_link.cpu_to_gpu_transfer(size);
        auto end = std::chrono::high_resolution_clock::now();
        
        double time_seconds = std::chrono::duration<double>(end - start).count();
        double bandwidth = static_cast<double>(size) / (time_seconds * 1e9);
        
        BenchmarkResult result;
        result.test_name = "C2C_Transfer_" + Utils::format_number(size) + "B";
        result.bandwidth_gb_s = bandwidth;
        result.latency_ns = c2c_link.get_latency();
        result.efficiency_percent = (bandwidth / c2c_link.get_bandwidth()) * 100;
        
        benchmark_results_.push_back(result);
    }
    
    analyze_results();
}

void NVLinkPerformanceAnalyzer::benchmark_switch_performance(NVLinkSwitchSystem& switch_system) {
    std::cout << "Benchmarking NVLink Switch System performance...\n";
    
    // Add some GPUs for testing
    for (uint32_t i = 0; i < 8; ++i) {
        switch_system.add_gpu(i);
    }
    
    // Test collective operations
    std::vector<uint32_t> all_gpus = {0, 1, 2, 3, 4, 5, 6, 7};
    uint64_t data_size = 1024 * 1024 * 1024; // 1 GB
    
    auto start = std::chrono::high_resolution_clock::now();
    switch_system.all_reduce_operation(all_gpus, data_size);
    auto end = std::chrono::high_resolution_clock::now();
    
    double time_seconds = std::chrono::duration<double>(end - start).count();
    double effective_bandwidth = (data_size * all_gpus.size()) / (time_seconds * 1e9);
    
    BenchmarkResult result;
    result.test_name = "All_Reduce_8GPU";
    result.bandwidth_gb_s = effective_bandwidth;
    result.latency_ns = switch_system.get_latency();
    result.efficiency_percent = (effective_bandwidth / switch_system.get_aggregate_bandwidth()) * 100;
    
    benchmark_results_.push_back(result);
    analyze_results();
}

double NVLinkPerformanceAnalyzer::analyze_bandwidth_utilization(const std::vector<double>& transfer_sizes,
                                                               const std::vector<double>& transfer_times) {
    if (transfer_sizes.size() != transfer_times.size() || transfer_sizes.empty()) {
        return 0.0;
    }
    
    double total_utilization = 0.0;
    for (size_t i = 0; i < transfer_sizes.size(); ++i) {
        double bandwidth = transfer_sizes[i] / (transfer_times[i] * 1e9);
        total_utilization += bandwidth;
    }
    
    average_bandwidth_utilization_ = total_utilization / transfer_sizes.size();
    return average_bandwidth_utilization_;
}

void NVLinkPerformanceAnalyzer::measure_latency_characteristics(InterconnectSystem& interconnect) {
    std::cout << "Measuring latency characteristics...\n";
    
    // Simulate small message latency test
    average_latency_ns_ = interconnect.get_latency();
    peak_bandwidth_gb_s_ = interconnect.get_bandwidth();
    
    std::cout << "Average Latency: " << average_latency_ns_ << " ns\n";
    std::cout << "Peak Bandwidth: " << peak_bandwidth_gb_s_ << " GB/s\n";
}

void NVLinkPerformanceAnalyzer::compare_with_alternatives() const {
    std::cout << "NVLink Performance Comparison:\n";
    std::cout << "  NVLink-C2C: 900 GB/s\n";
    std::cout << "  PCIe Gen5 x16: 128 GB/s\n";
    std::cout << "  InfiniBand HDR: 200 Gb/s (25 GB/s)\n";
    std::cout << "  Ethernet 400GbE: 400 Gb/s (50 GB/s)\n";
    std::cout << "\n";
    std::cout << "NVLink-C2C provides:\n";
    std::cout << "  7.0x higher bandwidth than PCIe Gen5\n";
    std::cout << "  36x higher bandwidth than InfiniBand HDR\n";
    std::cout << "  18x higher bandwidth than 400GbE\n";
}

void NVLinkPerformanceAnalyzer::print_performance_summary() const {
    Utils::print_section("NVLink Performance Summary");
    
    std::cout << "Benchmark Results:\n";
    for (const auto& result : benchmark_results_) {
        std::cout << "  " << result.test_name << ":\n";
        std::cout << "    Bandwidth: " << std::fixed << std::setprecision(1) 
                  << result.bandwidth_gb_s << " GB/s\n";
        std::cout << "    Latency: " << result.latency_ns << " ns\n";
        std::cout << "    Efficiency: " << std::fixed << std::setprecision(1) 
                  << result.efficiency_percent << "%\n";
    }
    
    std::cout << "\nOverall Performance:\n";
    std::cout << "  Average Latency: " << average_latency_ns_ << " ns\n";
    std::cout << "  Peak Bandwidth: " << peak_bandwidth_gb_s_ << " GB/s\n";
    std::cout << "  Average Utilization: " << std::fixed << std::setprecision(1) 
              << average_bandwidth_utilization_ << " GB/s\n";
}

std::vector<std::string> NVLinkPerformanceAnalyzer::get_optimization_recommendations() const {
    std::vector<std::string> recommendations;
    
    if (average_bandwidth_utilization_ / peak_bandwidth_gb_s_ < 0.8) {
        recommendations.push_back("Consider optimizing data transfer patterns to improve bandwidth utilization");
    }
    
    if (average_latency_ns_ > 100) {
        recommendations.push_back("High latency detected - consider reducing message sizes or improving topology");
    }
    
    recommendations.push_back("Enable memory coalescing for better bandwidth efficiency");
    recommendations.push_back("Use asynchronous transfers to hide latency");
    recommendations.push_back("Optimize collective communication patterns");
    
    return recommendations;
}

void NVLinkPerformanceAnalyzer::analyze_results() {
    if (!benchmark_results_.empty()) {
        double total_bandwidth = 0.0;
        double total_latency = 0.0;
        
        for (const auto& result : benchmark_results_) {
            total_bandwidth += result.bandwidth_gb_s;
            total_latency += result.latency_ns;
        }
        
        average_bandwidth_utilization_ = total_bandwidth / benchmark_results_.size();
        average_latency_ns_ = total_latency / benchmark_results_.size();
        peak_bandwidth_gb_s_ = std::max_element(benchmark_results_.begin(), benchmark_results_.end(),
            [](const BenchmarkResult& a, const BenchmarkResult& b) {
                return a.bandwidth_gb_s < b.bandwidth_gb_s;
            })->bandwidth_gb_s;
    }
}

} // namespace hopper::interconnect