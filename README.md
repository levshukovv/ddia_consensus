# DDIA Consensus - Leader Election with etcd

A C++ implementation of distributed leader election using etcd as the consensus backend. This project demonstrates key concepts from "Designing Data-Intensive Applications" (DDIA) by implementing a robust leader election mechanism suitable for distributed systems.

## 🚀 Features

- **Leader Election**: Fault-tolerant leader election using etcd leases
- **etcd Integration**: Full HTTP REST API client for etcd v3
- **Docker Support**: Containerized etcd setup (single node & cluster)
- **Simple Build**: Makefile-based build system (no CMake required)
- **Health Monitoring**: Built-in health checks and connectivity testing
- **Memory Safe**: Valgrind and GDB support for debugging

## 📋 Prerequisites

- **C++17** compatible compiler (g++)
- **libcurl** development libraries
- **Docker** and **Docker Compose**
- **Make** build system

## 🏗️ Project Structure

```
ddia_consensus/
├── README.md                   # This file
├── Makefile                    # Build configuration
├── LICENSE                     # Project license
├── main.cpp                    # Main application entry point
├── test_etcd.cpp              # etcd operations test program
├── include/                    # Header files
│   ├── etcdClient.h           # etcd HTTP client interface
│   └── leaderElection.h       # Leader election logic interface
├── src/                       # Source files
│   ├── etcdClient.cpp         # etcd HTTP client implementation
│   └── leaderElection.cpp     # Leader election logic implementation
├── docker-compose.yml         # 3-node etcd cluster configuration
├── docker-compose.single.yml  # Single-node etcd configuration
└── etcd.sh                    # etcd management script
```

## 🚀 Quick Start

### 1. Start etcd Server

```bash
# Start single etcd node (recommended for development)
./etcd.sh start

# Or start a 3-node cluster (for production-like testing)
./etcd.sh start-cluster
```

### 2. Build the Project

```bash
# Build the main application
make clean && make

# Or build individual components
make                    # Build main application
make test              # Build test programs
```

### 3. Run the Application

```bash
# Run main leader election application
./consensus

# Run etcd operations test
./test_etcd

# Or use make targets
make run               # Equivalent to ./consensus
```

## 🔧 Available Commands

### Build Commands

```bash
make                   # Build the project
make clean             # Remove build artifacts
make rebuild           # Clean and build
make run               # Build and run
make debug             # Build and run with GDB
make memcheck          # Run with Valgrind memory checking
```

### etcd Management

```bash
./etcd.sh start        # Start single etcd node
./etcd.sh start-cluster # Start 3-node etcd cluster
./etcd.sh stop         # Stop etcd services
./etcd.sh restart      # Restart etcd services
./etcd.sh status       # Show etcd health status
./etcd.sh logs         # Show etcd logs
./etcd.sh test         # Run basic etcd operations test
./etcd.sh clean        # Stop and remove all etcd data
./etcd.sh help         # Show all available commands
```

## 📡 etcd Client Usage

The project includes a comprehensive etcd client that communicates via HTTP REST API:

```cpp
#include "etcdClient.h"

// Create client
EtcdClient etcd("http://localhost:2379");

// Basic operations
auto response = etcd.put("/config/key", "value");
auto value = etcd.get("/config/key");
etcd.del("/config/key");

// Leader election operations
auto lease = etcd.createLease(30);  // 30-second TTL
etcd.renewLease(leaseId);           // Keep lease alive
etcd.revokeLease(leaseId);          // Cancel lease

// Health monitoring
auto health = etcd.health();
if (health.success) {
    std::cout << "etcd is healthy" << std::endl;
}
```

## 🏛️ Leader Election Algorithm

The leader election implementation follows these principles:

1. **Lease-based Leadership**: Uses etcd leases with TTL for automatic failover
2. **Atomic Operations**: Leverages etcd's strong consistency guarantees
3. **Health Monitoring**: Continuous health checks and lease renewal
4. **Graceful Handover**: Clean leadership transitions on planned shutdowns

### Election Process

1. **Request Lease**: Each node requests a lease from etcd
2. **Attempt Leadership**: Try to create a key with the lease
3. **Monitor Health**: Continuously renew lease if leader
4. **Watch for Changes**: Monitor leadership key for failover
5. **Automatic Recovery**: New election on leader failure

## 🔍 Troubleshooting

### Common Issues

**etcd Connection Failed**

```bash
# Check if etcd is running
./etcd.sh status

# View etcd logs
./etcd.sh logs

# Restart etcd
./etcd.sh restart
```

**Build Errors**

```bash
# Check if libcurl is installed
curl-config --version

# Install missing dependencies
sudo apt install libcurl4-openssl-dev
```

**Docker Issues**

```bash
# Check Docker status
docker ps

# Clean up containers
./etcd.sh clean
docker system prune
```

## 🧪 Testing

### Unit Tests

```bash
# Test etcd connectivity and operations
./test_etcd

# Test with etcdctl (if installed)
./etcd.sh test
```

### Memory Testing

```bash
# Run with Valgrind
make memcheck

# Debug with GDB
make debug
```

### Load Testing

```bash
# Start cluster for load testing
./etcd.sh start-cluster

# Run multiple instances
./consensus &
./consensus &
./consensus &
```

## 🔧 Configuration

### etcd Configuration

The project supports both single-node and cluster configurations:

- **Single Node**: `docker-compose.single.yml` - Perfect for development
- **3-Node Cluster**: `docker-compose.yml` - Production-like setup

### Client Configuration

Modify the etcd endpoint in your code:

```cpp
// Local development
EtcdClient etcd("http://localhost:2379");

// Remote etcd cluster
EtcdClient etcd("http://etcd-cluster:2379");
```

## 📚 References

- [etcd Documentation](https://etcd.io/docs/)
- [etcd API Reference](https://etcd.io/docs/v3.5/dev-guide/api_reference_v3/)
- [Designing Data-Intensive Applications](https://dataintensive.net/)
- [Raft Consensus Algorithm](https://raft.github.io/)

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

## 📄 License

This project is licensed under the terms specified in the LICENSE file.

## 🏗️ Development Notes

This project was developed in a VS Code dev container with:

- C++17 compiler toolchain
- Docker and Docker Compose
- libcurl for HTTP communication
- Valgrind and GDB for debugging
- All dependencies pre-installed

The implementation focuses on simplicity and educational value while maintaining production-ready code quality.
