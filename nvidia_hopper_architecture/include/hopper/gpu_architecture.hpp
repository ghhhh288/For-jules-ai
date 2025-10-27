#pragma once

#include "common.hpp"
#include <vector>
#include <memory>

namespace hopper {

// Abstract base class for GPU architectures
class GPUArchitecture {
public:
    virtual ~GPUArchitecture() = default;
    
    // Core architecture methods
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual PerformanceMetrics get_performance_metrics() const = 0;
    virtual ArchitectureSpecs get_specifications() const = 0;
    
    // Information methods
    virtual std::string get_name() const = 0;
    virtual std::string get_generation() const = 0;
    virtual void print_architecture_info() const = 0;
    
    // Performance simulation
    virtual double simulate_workload(const std::string& workload_type, 
                                   PrecisionFormat precision) = 0;
    
protected:
    bool initialized_ = false;
    PerformanceMetrics metrics_;
    ArchitectureSpecs specs_;
};

// Base class for compute units
class ComputeUnit {
public:
    virtual ~ComputeUnit() = default;
    
    virtual void initialize() = 0;
    virtual double execute_operation(PrecisionFormat precision, 
                                   uint64_t operation_count) = 0;
    virtual PerformanceMetrics get_metrics() const = 0;
    virtual std::string get_unit_type() const = 0;
    
protected:
    uint32_t unit_id_;
    bool active_ = false;
    PerformanceMetrics unit_metrics_;
};

// Base class for memory systems
class MemorySystem {
public:
    virtual ~MemorySystem() = default;
    
    virtual void initialize() = 0;
    virtual uint64_t get_capacity() const = 0;
    virtual double get_bandwidth() const = 0;
    virtual double get_latency() const = 0;
    virtual void allocate_memory(uint64_t size) = 0;
    virtual void deallocate_memory(uint64_t size) = 0;
    virtual std::string get_memory_type() const = 0;
    
protected:
    uint64_t total_capacity_;
    uint64_t used_capacity_ = 0;
    double bandwidth_gb_s_;
    double latency_ns_;
};

// Base class for interconnect systems
class InterconnectSystem {
public:
    virtual ~InterconnectSystem() = default;
    
    virtual void initialize() = 0;
    virtual double get_bandwidth() const = 0;
    virtual double get_latency() const = 0;
    virtual void establish_connection(uint32_t source, uint32_t destination) = 0;
    virtual std::string get_interconnect_type() const = 0;
    
protected:
    double bandwidth_gb_s_;
    double latency_ns_;
    std::vector<std::pair<uint32_t, uint32_t>> connections_;
};

} // namespace hopper