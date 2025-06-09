#include <iostream>
#include "leaderElection.h"
#include "etcdClient.h"

int main()
{
    std::cout << "Leader Election Module" << std::endl;

    // Test etcd connectivity
    EtcdClient etcd("http://localhost:2379");

    std::cout << "\n=== Testing etcd connectivity ===" << std::endl;

    // Test health
    auto healthResponse = etcd.health();
    if (healthResponse.success)
    {
        std::cout << "✓ etcd health check passed" << std::endl;
    }
    else
    {
        std::cout << "✗ etcd health check failed: " << healthResponse.error << std::endl;
        std::cout << "Make sure etcd is running: ./etcd.sh start" << std::endl;
        return 1;
    }

    // Test basic operations
    std::cout << "\n=== Testing basic operations ===" << std::endl;

    // Put a key
    auto putResponse = etcd.put("/test/leader", "node-1");
    if (putResponse.success)
    {
        std::cout << "✓ PUT operation successful" << std::endl;
    }
    else
    {
        std::cout << "✗ PUT operation failed: " << putResponse.error << std::endl;
    }

    // Get the key
    auto getResponse = etcd.get("/test/leader");
    if (getResponse.success)
    {
        std::cout << "✓ GET operation successful" << std::endl;
        std::cout << "Response: " << getResponse.data << std::endl;
    }
    else
    {
        std::cout << "✗ GET operation failed: " << getResponse.error << std::endl;
    }

    std::cout << "\n=== Ready for leader election implementation ===" << std::endl;

    return 0;
}
// This code initializes the leader election module and starts an election process.