#pragma once

#include "../common.hpp"
#include "../compute/tensor_core.hpp"
#include "../compute/streaming_multiprocessor.hpp"
#include "../memory/hbm3_memory.hpp"
#include "../interconnect/nvlink_c2c.hpp"
#include <chrono>
#include <vector>
#include <memory>

namespace hopper::performance {

// Benchmark result structure
struct BenchmarkResult {
    std::string benchmark_name;
    std::string workload_type;
    PrecisionFormat precision;
    double performance_tflops;
    double bandwidth_gb_s;
    double latency_ms;
    double power_watts;
    double efficiency_tflops_per_watt;
    std::chrono::duration<double> execution_time;
    
    void calculate_efficiency() {
        if (power_watts > 0) {
            efficiency_tflops_per_watt = performance_tflops / power_watts;
        }
    }
};

// AI Workload Benchmarks
class AIWorkloadBenchmarks {
public:
    AIWorkloadBenchmarks();
    ~AIWorkloadBenchmarks() = default;
    
    void initialize();
    
    // Large Language Model benchmarks
    BenchmarkResult benchmark_llm_training(PrecisionFormat precision, uint64_t model_parameters);
    BenchmarkResult benchmark_llm_inference(PrecisionFormat precision, uint64_t model_parameters,
                                           uint32_t batch_size, uint32_t sequence_length);
    
    // Computer Vision benchmarks
    BenchmarkResult benchmark_image_classification(PrecisionFormat precision, 
                                                  uint32_t batch_size, uint32_t image_size);
    BenchmarkResult benchmark_object_detection(PrecisionFormat precision,
                                              uint32_t batch_size, uint32_t image_size);
    
    // Transformer-specific benchmarks
    BenchmarkResult benchmark_attention_mechanism(PrecisionFormat precision,
                                                 uint32_t batch_size, uint32_t sequence_length,
                                                 uint32_t hidden_size);
    BenchmarkResult benchmark_feedforward_network(PrecisionFormat precision,
                                                 uint32_t batch_size, uint32_t hidden_size,
                                                 uint32_t intermediate_size);
    
    // Multi-modal benchmarks
    BenchmarkResult benchmark_multimodal_transformer(PrecisionFormat precision,
                                                    uint32_t batch_size, uint32_t sequence_length);
    
    // Performance comparison
    void compare_with_previous_generation() const;
    void print_ai_benchmark_summary() const;
    
private:
    std::vector<BenchmarkResult> benchmark_results_;
    
    // AI workload characteristics
    double calculate_llm_complexity(uint64_t parameters, uint32_t batch_size, uint32_t sequence_length);
    double calculate_attention_complexity(uint32_t batch_size, uint32_t sequence_length, uint32_t hidden_size);
    double estimate_memory_bandwidth_requirement(const std::string& workload, PrecisionFormat precision);
    
    // Performance modeling
    double model_tensor_core_performance(PrecisionFormat precision, uint64_t operations);
    double model_memory_bandwidth_utilization(uint64_t data_size, double compute_time);
};

// HPC Workload Benchmarks
class HPCWorkloadBenchmarks {
public:
    HPCWorkloadBenchmarks();
    ~HPCWorkloadBenchmarks() = default;
    
    void initialize();
    
    // Scientific computing benchmarks
    BenchmarkResult benchmark_molecular_dynamics(uint32_t particle_count, uint32_t timesteps);
    BenchmarkResult benchmark_climate_modeling(uint32_t grid_size, uint32_t time_iterations);
    BenchmarkResult benchmark_quantum_simulation(uint32_t qubit_count, uint32_t gate_operations);
    
    // Linear algebra benchmarks
    BenchmarkResult benchmark_matrix_multiplication(PrecisionFormat precision,
                                                   uint32_t matrix_size);
    BenchmarkResult benchmark_eigenvalue_decomposition(PrecisionFormat precision,
                                                      uint32_t matrix_size);
    BenchmarkResult benchmark_sparse_matrix_operations(PrecisionFormat precision,
                                                      uint32_t matrix_size, double sparsity);
    
    // Genomics benchmarks (DPX instructions)
    BenchmarkResult benchmark_smith_waterman(uint32_t sequence_length);
    BenchmarkResult benchmark_sequence_alignment(uint32_t sequence_count, uint32_t sequence_length);
    BenchmarkResult benchmark_genome_assembly(uint32_t read_count, uint32_t read_length);
    
    // CFD and numerical simulation
    BenchmarkResult benchmark_fluid_dynamics(uint32_t grid_size, uint32_t iterations);
    BenchmarkResult benchmark_finite_element_analysis(uint32_t element_count, uint32_t dof);
    
    void print_hpc_benchmark_summary() const;
    
private:
    std::vector<BenchmarkResult> hpc_results_;
    
    // HPC performance modeling
    double calculate_flops_requirement(const std::string& workload, uint32_t problem_size);
    double estimate_memory_access_pattern_efficiency(const std::string& workload);
    double model_dpx_acceleration(const std::string& algorithm, uint32_t problem_size);
};

// Memory System Benchmarks
class MemoryBenchmarks {
public:
    MemoryBenchmarks();
    ~MemoryBenchmarks() = default;
    
    void initialize();
    
    // Memory bandwidth benchmarks
    BenchmarkResult benchmark_sequential_read(uint64_t data_size);
    BenchmarkResult benchmark_sequential_write(uint64_t data_size);
    BenchmarkResult benchmark_random_access(uint64_t access_count, uint64_t data_range);
    
    // Cache performance benchmarks
    BenchmarkResult benchmark_l1_cache_performance();
    BenchmarkResult benchmark_l2_cache_performance();
    BenchmarkResult benchmark_cache_hierarchy();
    
    // Memory coherency benchmarks
    BenchmarkResult benchmark_coherency_overhead();
    BenchmarkResult benchmark_unified_memory_access();
    
    // HBM3 specific benchmarks
    BenchmarkResult benchmark_hbm3_bandwidth();
    BenchmarkResult benchmark_hbm3_latency();
    BenchmarkResult benchmark_memory_compression();
    
    void print_memory_benchmark_summary() const;
    
private:
    std::vector<BenchmarkResult> memory_results_;
    
    double simulate_memory_access(uint64_t size, const std::string& pattern);
    double calculate_cache_hit_ratio(const std::string& access_pattern);
};

// Interconnect Benchmarks
class InterconnectBenchmarks {
public:
    InterconnectBenchmarks();
    ~InterconnectBenchmarks() = default;
    
    void initialize();
    
    // NVLink-C2C benchmarks
    BenchmarkResult benchmark_c2c_bandwidth();
    BenchmarkResult benchmark_c2c_latency();
    BenchmarkResult benchmark_cpu_gpu_coherency();
    
    // Multi-GPU communication benchmarks
    BenchmarkResult benchmark_gpu_to_gpu_transfer(uint32_t gpu_count);
    BenchmarkResult benchmark_all_reduce_operation(uint32_t gpu_count, uint64_t data_size);
    BenchmarkResult benchmark_collective_operations(uint32_t gpu_count);
    
    // Scalability benchmarks
    BenchmarkResult benchmark_weak_scaling(uint32_t max_gpus);
    BenchmarkResult benchmark_strong_scaling(uint32_t max_gpus);
    
    void print_interconnect_benchmark_summary() const;
    
private:
    std::vector<BenchmarkResult> interconnect_results_;
    
    double simulate_collective_communication(uint32_t gpu_count, uint64_t data_size, 
                                           const std::string& operation);
    double calculate_scaling_efficiency(uint32_t gpu_count, double baseline_performance);
};

// Comprehensive Benchmark Suite
class HopperBenchmarkSuite {
public:
    HopperBenchmarkSuite();
    ~HopperBenchmarkSuite() = default;
    
    void initialize();
    
    // Run all benchmark categories
    void run_all_benchmarks();
    void run_ai_benchmarks();
    void run_hpc_benchmarks();
    void run_memory_benchmarks();
    void run_interconnect_benchmarks();
    
    // Custom benchmark execution
    void run_custom_benchmark(const std::string& benchmark_name, 
                            const std::map<std::string, std::string>& parameters);
    
    // Results analysis
    void generate_performance_report() const;
    void compare_with_competitors() const;
    void analyze_performance_trends() const;
    
    // Export results
    void export_results_csv(const std::string& filename) const;
    void export_results_json(const std::string& filename) const;
    
    // Performance insights
    std::vector<std::string> get_performance_insights() const;
    std::vector<std::string> get_optimization_recommendations() const;
    
private:
    std::unique_ptr<AIWorkloadBenchmarks> ai_benchmarks_;
    std::unique_ptr<HPCWorkloadBenchmarks> hpc_benchmarks_;
    std::unique_ptr<MemoryBenchmarks> memory_benchmarks_;
    std::unique_ptr<InterconnectBenchmarks> interconnect_benchmarks_;
    
    std::vector<BenchmarkResult> all_results_;
    
    // Analysis methods
    void analyze_precision_impact() const;
    void analyze_scaling_characteristics() const;
    void identify_performance_bottlenecks() const;
    
    // Reporting
    void print_executive_summary() const;
    void print_detailed_results() const;
    void print_comparison_table() const;
};

} // namespace hopper::performance