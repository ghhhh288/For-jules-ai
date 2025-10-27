#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <map>
#include <cstdint>

namespace hopper {

// Common types and constants
using namespace std::chrono;

// Precision formats supported by Hopper
enum class PrecisionFormat {
    FP64,
    FP32,
    TF32,
    FP16,
    BF16,
    FP8_E4M3,
    FP8_E5M2,
    INT8,
    INT4
};

// Performance metrics
struct PerformanceMetrics {
    double tflops = 0.0;
    double bandwidth_gb_s = 0.0;
    double power_watts = 0.0;
    double efficiency_tflops_per_watt = 0.0;
    uint64_t operations_per_second = 0;
    
    void calculate_efficiency() {
        if (power_watts > 0) {
            efficiency_tflops_per_watt = tflops / power_watts;
        }
    }
};

// Architecture specifications
struct ArchitectureSpecs {
    std::string name;
    std::string process_node;
    uint64_t transistor_count;
    uint32_t sm_count;
    uint32_t tensor_core_count;
    uint64_t memory_capacity_gb;
    double memory_bandwidth_gb_s;
    uint32_t nvlink_lanes;
};

// Utility functions
class Utils {
public:
    static std::string format_number(uint64_t number) {
        if (number >= 1e12) return std::to_string(number / 1e12) + "T";
        if (number >= 1e9) return std::to_string(number / 1e9) + "B";
        if (number >= 1e6) return std::to_string(number / 1e6) + "M";
        if (number >= 1e3) return std::to_string(number / 1e3) + "K";
        return std::to_string(number);
    }
    
    static std::string precision_to_string(PrecisionFormat format) {
        switch (format) {
            case PrecisionFormat::FP64: return "FP64";
            case PrecisionFormat::FP32: return "FP32";
            case PrecisionFormat::TF32: return "TF32";
            case PrecisionFormat::FP16: return "FP16";
            case PrecisionFormat::BF16: return "BF16";
            case PrecisionFormat::FP8_E4M3: return "FP8_E4M3";
            case PrecisionFormat::FP8_E5M2: return "FP8_E5M2";
            case PrecisionFormat::INT8: return "INT8";
            case PrecisionFormat::INT4: return "INT4";
            default: return "Unknown";
        }
    }
    
    static void print_header(const std::string& title) {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "  " << title << "\n";
        std::cout << std::string(60, '=') << "\n";
    }
    
    static void print_section(const std::string& section) {
        std::cout << "\n" << std::string(40, '-') << "\n";
        std::cout << section << "\n";
        std::cout << std::string(40, '-') << "\n";
    }
};

} // namespace hopper