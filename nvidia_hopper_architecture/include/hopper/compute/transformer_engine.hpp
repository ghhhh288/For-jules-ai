#pragma once

#include "../common.hpp"
#include "tensor_core.hpp"
#include <vector>
#include <memory>

namespace hopper::compute {

// Transformer Engine - Hardware-software co-design for transformer acceleration
class TransformerEngine {
public:
    TransformerEngine();
    ~TransformerEngine() = default;
    
    void initialize();
    
    // Dynamic precision management
    PrecisionFormat select_optimal_precision(const std::string& layer_type, 
                                            double accuracy_threshold = 0.99);
    
    // Scaling factor management for FP8
    void update_scaling_factors(const std::vector<double>& gradients);
    double get_scaling_factor(PrecisionFormat precision) const;
    
    // Transformer layer acceleration
    double execute_attention_layer(uint32_t batch_size, uint32_t sequence_length, 
                                 uint32_t hidden_size, PrecisionFormat precision);
    double execute_feedforward_layer(uint32_t batch_size, uint32_t sequence_length,
                                   uint32_t hidden_size, uint32_t intermediate_size,
                                   PrecisionFormat precision);
    double execute_layer_norm(uint32_t batch_size, uint32_t sequence_length,
                            uint32_t hidden_size, PrecisionFormat precision);
    
    // Performance optimization
    void enable_automatic_mixed_precision(bool enable) { amp_enabled_ = enable; }
    void enable_gradient_scaling(bool enable) { gradient_scaling_enabled_ = enable; }
    
    // Statistics and monitoring
    void print_precision_statistics() const;
    double get_average_speedup() const;
    
private:
    bool amp_enabled_ = true;
    bool gradient_scaling_enabled_ = true;
    
    // Scaling factors for different precisions
    std::map<PrecisionFormat, double> scaling_factors_ = {
        {PrecisionFormat::FP8_E4M3, 128.0},
        {PrecisionFormat::FP8_E5M2, 256.0},
        {PrecisionFormat::FP16, 1.0},
        {PrecisionFormat::BF16, 1.0}
    };
    
    // Precision selection history for statistics
    mutable std::map<PrecisionFormat, uint64_t> precision_usage_count_;
    
    // Performance characteristics
    double calculate_attention_complexity(uint32_t batch_size, uint32_t sequence_length,
                                        uint32_t hidden_size) const;
    double calculate_feedforward_complexity(uint32_t batch_size, uint32_t sequence_length,
                                          uint32_t hidden_size, uint32_t intermediate_size) const;
    
    // Precision selection logic
    bool is_precision_suitable_for_layer(PrecisionFormat precision, 
                                       const std::string& layer_type) const;
    double estimate_accuracy_loss(PrecisionFormat from, PrecisionFormat to) const;
};

// Sparsity Engine for structured sparsity support
class SparsityEngine {
public:
    SparsityEngine();
    ~SparsityEngine() = default;
    
    void initialize();
    
    // Sparsity pattern analysis
    double analyze_sparsity_pattern(const std::vector<std::vector<double>>& weights);
    bool is_structured_sparse(const std::vector<std::vector<double>>& weights, 
                            uint32_t block_size = 4) const;
    
    // Sparse computation acceleration
    double execute_sparse_gemm(uint32_t m, uint32_t n, uint32_t k, 
                             double sparsity_ratio, PrecisionFormat precision);
    
    // Sparsity optimization
    void enable_2_4_sparsity(bool enable) { structured_2_4_sparsity_ = enable; }
    void enable_block_sparsity(bool enable, uint32_t block_size = 4);
    
    double get_theoretical_speedup(double sparsity_ratio) const;
    void print_sparsity_info() const;
    
private:
    bool structured_2_4_sparsity_ = false;
    bool block_sparsity_enabled_ = false;
    uint32_t block_size_ = 4;
    
    // Sparsity statistics
    mutable double average_sparsity_ratio_ = 0.0;
    mutable uint64_t sparse_operations_count_ = 0;
    
    double calculate_sparse_efficiency(double sparsity_ratio) const;
};

} // namespace hopper::compute