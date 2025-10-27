#pragma once

#include "gpu_architecture.hpp"
#include "common.hpp"
#include "compute/streaming_multiprocessor.hpp"
#include "compute/tensor_core.hpp"
#include "compute/transformer_engine.hpp"
#include "memory/hbm3_memory.hpp"
#include "memory/memory_coherency.hpp"
#include "interconnect/nvlink_c2c.hpp"
#include "security/confidential_computing.hpp"
#include "performance/benchmark_suite.hpp"
#include <vector>
#include <memory>

namespace hopper {

// Main Hopper GPU Architecture Implementation
class HopperArchitecture : public GPUArchitecture {
public:
    HopperArchitecture(const std::string& variant = "H100_SXM5");
    ~HopperArchitecture() override = default;
    
    // GPUArchitecture interface implementation
    void initialize() override;
    void shutdown() override;
    PerformanceMetrics get_performance_metrics() const override;
    ArchitectureSpecs get_specifications() const override;
    std::string get_name() const override { return "NVIDIA Hopper"; }
    std::string get_generation() const override { return "Hopper (4th Gen Tensor Cores)"; }
    void print_architecture_info() const override;
    double simulate_workload(const std::string& workload_type, PrecisionFormat precision) override;
    
    // Hopper-specific methods
    void enable_transformer_engine(bool enable);
    void enable_sparsity_support(bool enable);
    void enable_confidential_computing(bool enable);
    void configure_memory_system(uint64_t hbm3_capacity_gb = 96);
    void setup_nvlink_c2c();
    void setup_multi_gpu_scaling(uint32_t gpu_count);
    
    // Component access
    std::vector<std::shared_ptr<compute::StreamingMultiprocessor>> get_streaming_multiprocessors() const;
    std::shared_ptr<memory::HBM3MemorySystem> get_memory_system() const;
    std::shared_ptr<interconnect::NVLinkC2C> get_nvlink_c2c() const;
    std::shared_ptr<security::ConfidentialComputingManager> get_security_manager() const;
    
    // Performance analysis
    void run_comprehensive_benchmarks();
    void analyze_workload_performance(const std::string& workload_type);
    void print_performance_summary() const;
    
    // System integration
    void configure_grace_hopper_superchip();
    void setup_hgx_platform(uint32_t gpu_count = 8);
    
private:
    std::string variant_;
    
    // Core components
    std::vector<std::shared_ptr<compute::StreamingMultiprocessor>> streaming_multiprocessors_;
    std::shared_ptr<compute::TransformerEngine> transformer_engine_;
    std::shared_ptr<compute::SparsityEngine> sparsity_engine_;
    
    // Memory system
    std::shared_ptr<memory::HBM3MemorySystem> hbm3_memory_;
    std::shared_ptr<memory::HopperCacheHierarchy> cache_hierarchy_;
    std::shared_ptr<memory::MemoryCoherencyController> coherency_controller_;
    std::shared_ptr<memory::UnifiedMemorySpace> unified_memory_;
    
    // Interconnect
    std::shared_ptr<interconnect::NVLinkC2C> nvlink_c2c_;
    std::shared_ptr<interconnect::NVLinkSwitchSystem> nvlink_switch_;
    
    // Security
    std::shared_ptr<security::ConfidentialComputingManager> security_manager_;
    
    // Performance
    std::shared_ptr<performance::HopperBenchmarkSuite> benchmark_suite_;
    
    // Configuration
    bool transformer_engine_enabled_ = true;
    bool sparsity_support_enabled_ = true;
    bool confidential_computing_enabled_ = true;
    bool grace_hopper_mode_ = false;
    
    // Architecture specifications by variant
    void configure_h100_sxm5();
    void configure_h100_pcie();
    void configure_grace_hopper();
    
    // Component initialization
    void initialize_compute_units();
    void initialize_memory_system();
    void initialize_interconnect();
    void initialize_security_features();
    void initialize_performance_monitoring();
    
    // Performance modeling
    double calculate_theoretical_performance(PrecisionFormat precision) const;
    double simulate_ai_workload(const std::string& workload_type, PrecisionFormat precision);
    double simulate_hpc_workload(const std::string& workload_type, PrecisionFormat precision);
    
    // System optimization
    void optimize_for_workload(const std::string& workload_type);
    void apply_performance_tuning();
};

// Grace Hopper Superchip Implementation
class GraceHopperSuperchip {
public:
    GraceHopperSuperchip();
    ~GraceHopperSuperchip() = default;
    
    void initialize();
    
    // Component access
    std::shared_ptr<HopperArchitecture> get_hopper_gpu() const { return hopper_gpu_; }
    
    // Grace CPU specifications
    struct GraceCPUSpecs {
        uint32_t core_count = 72;
        std::string isa = "Armv9.0-A";
        uint32_t simd_units_per_core = 4;
        uint32_t simd_width = 128; // bits
        uint64_t l3_cache_mb = 117;
        uint64_t lpddr5x_capacity_gb = 512;
        double lpddr5x_bandwidth_gb_s = 546.0;
    };
    
    GraceCPUSpecs get_cpu_specs() const { return cpu_specs_; }
    
    // Unified memory operations
    void* allocate_unified_memory(uint64_t size);
    void deallocate_unified_memory(void* ptr);
    void migrate_memory_to_gpu(void* ptr, uint64_t size);
    void migrate_memory_to_cpu(void* ptr, uint64_t size);
    
    // Performance characteristics
    uint64_t get_total_addressable_memory() const;
    double get_cpu_gpu_bandwidth() const;
    void print_superchip_info() const;
    
    // Workload execution
    void execute_heterogeneous_workload(const std::string& workload_type);
    void benchmark_cpu_gpu_collaboration();
    
private:
    std::shared_ptr<HopperArchitecture> hopper_gpu_;
    GraceCPUSpecs cpu_specs_;
    
    // Unified memory management
    std::shared_ptr<memory::UnifiedMemorySpace> unified_memory_space_;
    
    void initialize_grace_cpu();
    void setup_unified_memory_space();
    void configure_cache_coherency();
};

// Multi-GPU System (HGX Platform)
class HGXPlatform {
public:
    HGXPlatform(uint32_t gpu_count = 8);
    ~HGXPlatform() = default;
    
    void initialize();
    
    // GPU management
    void add_gpu(std::shared_ptr<HopperArchitecture> gpu);
    std::vector<std::shared_ptr<HopperArchitecture>> get_gpus() const { return gpus_; }
    uint32_t get_gpu_count() const { return gpus_.size(); }
    
    // Multi-GPU operations
    void execute_distributed_workload(const std::string& workload_type, 
                                    const std::vector<uint32_t>& participating_gpus);
    void perform_all_reduce(const std::vector<uint32_t>& gpus, uint64_t data_size);
    void perform_model_parallel_inference(const std::string& model_type, uint64_t model_size);
    
    // Performance scaling
    void benchmark_weak_scaling();
    void benchmark_strong_scaling();
    double get_scaling_efficiency(uint32_t gpu_count) const;
    
    // System management
    void print_platform_info() const;
    void monitor_power_consumption() const;
    void optimize_cooling_strategy();
    
private:
    std::vector<std::shared_ptr<HopperArchitecture>> gpus_;
    std::shared_ptr<interconnect::NVLinkSwitchSystem> nvlink_switch_system_;
    uint32_t target_gpu_count_;
    
    // Platform specifications
    double total_power_consumption_kw_ = 0.0;
    double cooling_capacity_kw_ = 10.0; // 10kW cooling capacity
    
    void setup_nvlink_topology();
    void configure_power_management();
    void initialize_cooling_system();
};

} // namespace hopper