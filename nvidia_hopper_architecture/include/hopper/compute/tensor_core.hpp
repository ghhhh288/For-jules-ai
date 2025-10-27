#pragma once

#include "../gpu_architecture.hpp"
#include "../common.hpp"
#include <map>

namespace hopper::compute {

// Fourth-generation Tensor Core implementation
class TensorCore : public ComputeUnit {
public:
    TensorCore(uint32_t core_id, uint32_t generation = 4);
    ~TensorCore() override = default;
    
    void initialize() override;
    double execute_operation(PrecisionFormat precision, uint64_t operation_count) override;
    PerformanceMetrics get_metrics() const override;
    std::string get_unit_type() const override { return "Tensor Core Gen4"; }
    
    // Tensor Core specific methods
    double execute_matrix_multiply(PrecisionFormat precision, 
                                 uint32_t m, uint32_t n, uint32_t k);
    void enable_sparsity_support(bool enable) { sparsity_enabled_ = enable; }
    void set_transformer_engine_mode(bool enable) { transformer_engine_mode_ = enable; }
    
    // Performance characteristics for different precisions
    double get_peak_tflops(PrecisionFormat precision) const;
    void print_capabilities() const;
    
private:
    uint32_t generation_;
    bool sparsity_enabled_ = false;
    bool transformer_engine_mode_ = false;
    
    // Performance lookup tables (TFLOPS per core)
    std::map<PrecisionFormat, double> peak_performance_ = {
        {PrecisionFormat::FP64, 0.42},      // 60 TFLOPS / 144 SMs
        {PrecisionFormat::FP32, 0.42},      // 60 TFLOPS / 144 SMs  
        {PrecisionFormat::TF32, 3.47},      // 500 TFLOPS / 144 SMs
        {PrecisionFormat::FP16, 6.94},      // 1000 TFLOPS / 144 SMs
        {PrecisionFormat::BF16, 6.94},      // 1000 TFLOPS / 144 SMs
        {PrecisionFormat::FP8_E4M3, 13.89}, // 2000 TFLOPS / 144 SMs
        {PrecisionFormat::FP8_E5M2, 13.89}, // 2000 TFLOPS / 144 SMs
        {PrecisionFormat::INT8, 13.89}      // 2000 TOPS / 144 SMs
    };
    
    double calculate_sparsity_speedup(PrecisionFormat precision) const;
    double calculate_transformer_engine_speedup(PrecisionFormat precision) const;
};

// DPX (Dynamic Programming Acceleration) Instructions
class DPXUnit : public ComputeUnit {
public:
    DPXUnit(uint32_t unit_id);
    ~DPXUnit() override = default;
    
    void initialize() override;
    double execute_operation(PrecisionFormat precision, uint64_t operation_count) override;
    PerformanceMetrics get_metrics() const override;
    std::string get_unit_type() const override { return "DPX Unit"; }
    
    // DPX specific algorithms
    double execute_smith_waterman(uint32_t sequence_length);
    double execute_floyd_warshall(uint32_t graph_size);
    double execute_needleman_wunsch(uint32_t sequence_length);
    
    void print_dpx_capabilities() const;
    
private:
    static constexpr double SMITH_WATERMAN_SPEEDUP = 7.0;  // 7x over A100
    static constexpr double FLOYD_WARSHALL_SPEEDUP = 7.0;  // 7x over A100
    static constexpr double BASE_OPERATIONS_PER_SECOND = 1e9; // 1 GOPS baseline
};

} // namespace hopper::compute