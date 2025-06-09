#ifndef ETCD_CLIENT_H
#define ETCD_CLIENT_H

#include <string>
#include <map>
#include <memory>

struct EtcdResponse
{
    bool success;
    int httpCode;
    std::string data;
    std::string error;
};

class EtcdClient
{
private:
    std::string baseUrl;

    // Helper method to perform HTTP requests
    EtcdResponse performRequest(const std::string &method,
                                const std::string &endpoint,
                                const std::string &data = "");

public:
    EtcdClient(const std::string &endpoint = "http://localhost:2379");
    ~EtcdClient();

    // Basic key-value operations
    EtcdResponse put(const std::string &key, const std::string &value, int ttl = 0);
    EtcdResponse get(const std::string &key);
    EtcdResponse del(const std::string &key);

    // Watch operations (simplified)
    EtcdResponse watch(const std::string &key, bool prefix = false);

    // Lease operations for leader election
    EtcdResponse createLease(int ttl);
    EtcdResponse renewLease(const std::string &leaseId);
    EtcdResponse revokeLease(const std::string &leaseId);

    // Health check
    EtcdResponse health();

    // Utility methods
    static std::string urlEncode(const std::string &value);
    static std::string base64Encode(const std::string &value);
};

#endif // ETCD_CLIENT_H
