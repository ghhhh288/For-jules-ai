#pragma once

#include "../common.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace hopper::security {

// Trusted Execution Environment (TEE)
class TrustedExecutionEnvironment {
public:
    TrustedExecutionEnvironment();
    ~TrustedExecutionEnvironment() = default;
    
    void initialize();
    
    // TEE management
    uint32_t create_secure_enclave(const std::string& enclave_name, uint64_t memory_size);
    void destroy_secure_enclave(uint32_t enclave_id);
    bool is_enclave_active(uint32_t enclave_id) const;
    
    // Secure execution
    void execute_in_enclave(uint32_t enclave_id, const std::string& code_hash);
    void load_secure_data(uint32_t enclave_id, const std::vector<uint8_t>& encrypted_data);
    std::vector<uint8_t> extract_secure_result(uint32_t enclave_id);
    
    // Isolation guarantees
    void enable_memory_isolation(bool enable) { memory_isolation_enabled_ = enable; }
    void enable_execution_isolation(bool enable) { execution_isolation_enabled_ = enable; }
    
    // Attestation
    std::string generate_attestation_report(uint32_t enclave_id);
    bool verify_attestation_report(const std::string& report, const std::string& expected_hash);
    
    // Statistics and monitoring
    void print_tee_status() const;
    uint32_t get_active_enclave_count() const;
    
private:
    struct SecureEnclave {
        uint32_t id;
        std::string name;
        uint64_t memory_size;
        bool active;
        std::string code_hash;
        std::vector<uint8_t> secure_memory;
        uint64_t creation_timestamp;
    };
    
    std::map<uint32_t, std::unique_ptr<SecureEnclave>> enclaves_;
    uint32_t next_enclave_id_ = 1;
    bool memory_isolation_enabled_ = true;
    bool execution_isolation_enabled_ = true;
    
    // Security features
    std::string hardware_root_of_trust_;
    std::vector<uint8_t> platform_key_;
    
    void initialize_hardware_security();
    std::string calculate_enclave_hash(const SecureEnclave& enclave);
};

// Hardware-based Memory Encryption
class MemoryEncryptionEngine {
public:
    MemoryEncryptionEngine();
    ~MemoryEncryptionEngine() = default;
    
    void initialize();
    
    // Encryption management
    void enable_memory_encryption(bool enable) { encryption_enabled_ = enable; }
    void set_encryption_algorithm(const std::string& algorithm) { encryption_algorithm_ = algorithm; }
    
    // Key management
    void generate_encryption_keys();
    void rotate_encryption_keys();
    std::string get_key_id() const { return current_key_id_; }
    
    // Memory operations
    std::vector<uint8_t> encrypt_memory_block(const std::vector<uint8_t>& plaintext);
    std::vector<uint8_t> decrypt_memory_block(const std::vector<uint8_t>& ciphertext);
    
    // Performance impact
    double get_encryption_overhead() const { return encryption_overhead_percent_; }
    void benchmark_encryption_performance();
    
    // Security analysis
    void print_encryption_status() const;
    bool is_memory_secure() const { return encryption_enabled_ && keys_initialized_; }
    
private:
    bool encryption_enabled_ = true;
    std::string encryption_algorithm_ = "AES-256-GCM";
    std::string current_key_id_;
    bool keys_initialized_ = false;
    double encryption_overhead_percent_ = 2.0; // 2% performance overhead
    
    // Encryption keys (simulated)
    std::vector<uint8_t> master_key_;
    std::vector<uint8_t> data_encryption_key_;
    std::vector<uint8_t> initialization_vector_;
    
    // Statistics
    mutable uint64_t encryption_operations_ = 0;
    mutable uint64_t decryption_operations_ = 0;
    mutable double total_encryption_time_ = 0.0;
    
    void generate_random_key(std::vector<uint8_t>& key, size_t key_size);
    void update_encryption_statistics(double operation_time);
};

// Secure Boot and Firmware Verification
class SecureBootSystem {
public:
    SecureBootSystem();
    ~SecureBootSystem() = default;
    
    void initialize();
    
    // Boot process
    bool verify_firmware_signature(const std::string& firmware_hash);
    bool verify_driver_signature(const std::string& driver_hash);
    void establish_chain_of_trust();
    
    // Certificate management
    void load_root_certificate(const std::string& certificate);
    bool verify_certificate_chain(const std::vector<std::string>& certificate_chain);
    
    // Boot security
    void enable_secure_boot(bool enable) { secure_boot_enabled_ = enable; }
    void enable_measured_boot(bool enable) { measured_boot_enabled_ = enable; }
    
    // Integrity verification
    bool verify_system_integrity();
    std::string generate_platform_measurement();
    
    // Security status
    void print_boot_security_status() const;
    bool is_system_trusted() const;
    
private:
    bool secure_boot_enabled_ = true;
    bool measured_boot_enabled_ = true;
    bool chain_of_trust_established_ = false;
    
    // Trusted components
    std::vector<std::string> trusted_firmware_hashes_;
    std::vector<std::string> trusted_driver_hashes_;
    std::string root_certificate_;
    
    // Boot measurements
    std::vector<std::string> boot_measurements_;
    std::string platform_configuration_register_;
    
    void add_boot_measurement(const std::string& component, const std::string& hash);
    bool verify_hash_signature(const std::string& hash, const std::string& signature);
};

// Hardware Security Module (HSM) Interface
class HardwareSecurityModule {
public:
    HardwareSecurityModule();
    ~HardwareSecurityModule() = default;
    
    void initialize();
    
    // Key generation and management
    std::string generate_secure_key(const std::string& key_type, uint32_t key_size);
    void store_secure_key(const std::string& key_id, const std::vector<uint8_t>& key);
    std::vector<uint8_t> retrieve_secure_key(const std::string& key_id);
    void delete_secure_key(const std::string& key_id);
    
    // Cryptographic operations
    std::vector<uint8_t> sign_data(const std::vector<uint8_t>& data, const std::string& key_id);
    bool verify_signature(const std::vector<uint8_t>& data, const std::vector<uint8_t>& signature,
                         const std::string& key_id);
    
    // Random number generation
    std::vector<uint8_t> generate_true_random(uint32_t byte_count);
    uint64_t generate_random_uint64();
    
    // HSM status and capabilities
    void print_hsm_capabilities() const;
    bool is_hsm_available() const { return hsm_initialized_; }
    std::string get_hsm_version() const { return hsm_version_; }
    
private:
    bool hsm_initialized_ = false;
    std::string hsm_version_ = "Hopper HSM v1.0";
    
    // Secure key storage (simulated)
    std::map<std::string, std::vector<uint8_t>> secure_key_store_;
    
    // HSM capabilities
    std::vector<std::string> supported_algorithms_ = {
        "RSA-2048", "RSA-4096", "ECDSA-P256", "ECDSA-P384", "AES-256", "ChaCha20"
    };
    
    // Statistics
    mutable uint64_t key_operations_ = 0;
    mutable uint64_t crypto_operations_ = 0;
    mutable uint64_t random_bytes_generated_ = 0;
    
    bool validate_key_parameters(const std::string& key_type, uint32_t key_size);
    void update_hsm_statistics();
};

// Confidential Computing Manager
class ConfidentialComputingManager {
public:
    ConfidentialComputingManager();
    ~ConfidentialComputingManager() = default;
    
    void initialize();
    
    // Component integration
    void set_tee(std::shared_ptr<TrustedExecutionEnvironment> tee) { tee_ = tee; }
    void set_encryption_engine(std::shared_ptr<MemoryEncryptionEngine> engine) { encryption_engine_ = engine; }
    void set_secure_boot(std::shared_ptr<SecureBootSystem> boot_system) { secure_boot_ = boot_system; }
    void set_hsm(std::shared_ptr<HardwareSecurityModule> hsm) { hsm_ = hsm; }
    
    // Confidential computing operations
    uint32_t create_confidential_workload(const std::string& workload_name, 
                                         const std::vector<uint8_t>& encrypted_code);
    void execute_confidential_workload(uint32_t workload_id, 
                                     const std::vector<uint8_t>& encrypted_input);
    std::vector<uint8_t> get_confidential_result(uint32_t workload_id);
    
    // Security policy management
    void set_security_policy(const std::string& policy_name, const std::string& policy_rules);
    bool enforce_security_policy(const std::string& operation, const std::string& context);
    
    // Compliance and auditing
    void enable_audit_logging(bool enable) { audit_logging_enabled_ = enable; }
    std::vector<std::string> get_audit_log() const { return audit_log_; }
    void generate_compliance_report() const;
    
    // Overall security status
    void print_security_overview() const;
    double get_security_score() const;
    
private:
    std::shared_ptr<TrustedExecutionEnvironment> tee_;
    std::shared_ptr<MemoryEncryptionEngine> encryption_engine_;
    std::shared_ptr<SecureBootSystem> secure_boot_;
    std::shared_ptr<HardwareSecurityModule> hsm_;
    
    bool audit_logging_enabled_ = true;
    mutable std::vector<std::string> audit_log_;
    std::map<std::string, std::string> security_policies_;
    
    // Workload management
    struct ConfidentialWorkload {
        uint32_t id;
        std::string name;
        uint32_t enclave_id;
        bool active;
        std::vector<uint8_t> result_data;
    };
    
    std::map<uint32_t, std::unique_ptr<ConfidentialWorkload>> workloads_;
    uint32_t next_workload_id_ = 1;
    
    void log_security_event(const std::string& event) const;
    double calculate_security_score() const;
};

} // namespace hopper::security