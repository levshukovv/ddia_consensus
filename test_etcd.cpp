#include <iostream>
#include <string>
#include "etcdClient.h"

// Simple base64 decoder for demonstration
std::string base64Decode(const std::string &encoded)
{
    // Simple implementation - for production, use a proper library
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string decoded;
    int val = 0, valb = -8;

    for (unsigned char c : encoded)
    {
        if (c == '=')
            break;
        int idx = chars.find(c);
        if (idx == std::string::npos)
            continue;
        val = (val << 6) + idx;
        valb += 6;
        if (valb >= 0)
        {
            decoded.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return decoded;
}

int main()
{
    EtcdClient etcd("http://localhost:2379");

    std::cout << "=== Advanced etcd Operations Demo ===" << std::endl;

    // 1. Health check
    std::cout << "\n1. Health Check:" << std::endl;
    auto health = etcd.health();
    std::cout << "Status: " << (health.success ? "✓ Healthy" : "✗ Unhealthy") << std::endl;

    // 2. Put multiple keys
    std::cout << "\n2. Storing multiple keys:" << std::endl;
    etcd.put("/election/node1", "alive");
    etcd.put("/election/node2", "alive");
    etcd.put("/config/timeout", "30");
    std::cout << "✓ Stored election and config keys" << std::endl;

    // 3. Get and decode values
    std::cout << "\n3. Retrieving values:" << std::endl;
    auto response = etcd.get("/election/node1");
    if (response.success)
    {
        std::cout << "Raw response: " << response.data << std::endl;
        // In a real implementation, you'd parse the JSON and decode the base64 values
        std::cout << "Note: Values are base64 encoded in the JSON response" << std::endl;
    }

    // 4. Create a lease for leader election
    std::cout << "\n4. Creating lease for leader election:" << std::endl;
    auto lease = etcd.createLease(30);
    if (lease.success)
    {
        std::cout << "✓ Created 30-second lease" << std::endl;
        std::cout << "Lease response: " << lease.data << std::endl;
    }
    else
    {
        std::cout << "✗ Failed to create lease: " << lease.error << std::endl;
    }

    // 5. Test connectivity with different operations
    std::cout << "\n5. Testing various operations:" << std::endl;

    // Delete a key
    auto delResp = etcd.del("/config/timeout");
    std::cout << "Delete operation: " << (delResp.success ? "✓ Success" : "✗ Failed") << std::endl;

    // Try to get deleted key
    auto getDeleted = etcd.get("/config/timeout");
    std::cout << "Get deleted key: " << (getDeleted.success ? "Found (unexpected)" : "Not found (expected)") << std::endl;

    std::cout << "\n=== etcd Operations Complete ===" << std::endl;

    return 0;
}
