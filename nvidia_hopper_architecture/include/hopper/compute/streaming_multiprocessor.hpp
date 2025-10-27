#pragma once

#include "../gpu_architecture.hpp"
#include "../common.hpp"
#include "tensor_core.hpp"
#include <vector>
#include <memory>

namespace hopper::compute {

// Hopper Streaming Multiprocessor (SM)
class StreamingMultiprocessor : public ComputeUnit {
public:
    StreamingMultiprocessor(uint32_t sm_id);
    ~StreamingMultiprocessor() override = default;
    
    void initialize() override;
    double execute_operation(PrecisionFormat precision, uint64_t operation_count) override;
    PerformanceMetrics get_metrics() const override;
    std::string get_unit_type() const override { return "Hopper SM"; }
    
    // SM specific methods
    void add_tensor_core(std::unique_ptr<TensorCore> tensor_core);
    void add_dpx_unit(std::unique_ptr<DPXUnit> dpx_unit);
    
    // Shared memory and L1 cache (256 KB combined)
    void allocate_shared_memory(uint64_t size_kb);
    void deallocate_shared_memory(uint64_t size_kb);
    uint64_t get_available_shared_memory() const;
    
    // Thread block cluster support
    void enable_thread_block_clusters(bool enable) { thread_block_clusters_enabled_ = enable; }
    void set_cluster_size(uint32_t size) { cluster_size_ = size; }
    
    // Distributed shared memory
    void enable_distributed_shared_memory(bool enable) { distributed_shared_memory_ = enable; }
    
    // Asynchronous execution features
    void enable_tensor_memory_accelerator(bool enable) { tma_enabled_ = enable; }
    
    // Performance simulation
    double simulate_cuda_cores_workload(PrecisionFormat precision, uint64_t operations);
    double simulate_tensor_workload(PrecisionFormat precision, uint64_t operations);
    double simulate_dpx_workload(const std::string& algorithm, uint64_t problem_size);
    
    void print_sm_info() const;
    
private:
    static constexpr uint64_t SHARED_MEMORY_L1_SIZE_KB = 256;
    static constexpr uint32_t CUDA_CORES_PER_SM = 128;
    static constexpr uint32_t TENSOR_CORES_PER_SM = 4;
    static constexpr uint32_t DPX_UNITS_PER_SM = 1;
    
    std::vector<std::unique_ptr<TensorCore>> tensor_cores_;
    std::vector<std::unique_ptr<DPXUnit>> dpx_units_;
    
    uint64_t shared_memory_used_kb_ = 0;
    bool thread_block_clusters_enabled_ = false;
    uint32_t cluster_size_ = 1;
    bool distributed_shared_memory_ = false;
    bool tma_enabled_ = false;
    
    // CUDA cores performance (TFLOPS per SM)
    std::map<PrecisionFormat, double> cuda_core_performance_ = {
        {PrecisionFormat::FP64, 0.21},  // 30 TFLOPS / 144 SMs
        {PrecisionFormat::FP32, 0.42}   // 60 TFLOPS / 144 SMs
    };
};

// Thread Block Cluster
class ThreadBlockCluster {
public:
    ThreadBlockCluster(uint32_t cluster_id, uint32_t size);
    ~ThreadBlockCluster() = default;
    
    void add_sm(std::shared_ptr<StreamingMultiprocessor> sm);
    void synchronize_cluster();
    void enable_distributed_shared_memory();
    
    uint32_t get_cluster_size() const { return cluster_size_; }
    std::vector<std::shared_ptr<StreamingMultiprocessor>> get_sms() const { return sms_; }
    
private:
    uint32_t cluster_id_;
    uint32_t cluster_size_;
    std::vector<std::shared_ptr<StreamingMultiprocessor>> sms_;
};

} // namespace hopper::compute