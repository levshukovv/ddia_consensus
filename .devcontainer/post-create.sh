#!/bin/bash

# Post-create script for DDIA Consensus development environment (Python & C++)

echo "🚀 Setting up DDIA Consensus development environment..."

# Update package lists
sudo apt-get update

# Install additional C++ development tools
echo "🔧 Installing C++ development tools..."
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    gdb \
    valgrind \
    clang \
    clang-format \
    clang-tidy \
    lldb \
    pkg-config \
    libssl-dev \
    libcurl4-openssl-dev \
    libjson-c-dev \
    libzmq3-dev \
    libprotobuf-dev \
    protobuf-compiler \
    libgrpc++-dev \
    protobuf-compiler-grpc

# Install general development tools
echo "📦 Installing additional development tools..."
sudo apt-get install -y \
    curl \
    wget \
    jq \
    tree \
    htop \
    netcat \
    telnet \
    tcpdump \
    strace \
    vim \
    nano

# Install Python development tools
echo "🐍 Installing Python development tools..."
pip3 install --upgrade pip
pip3 install \
    black \
    pylint \
    pytest \
    mypy \
    jupyter \
    numpy \
    requests \
    asyncio \
    aiohttp \
    pyyaml \
    prometheus-client
    

echo "✅ Development environment setup complete!"
echo ""
echo "🔍 Available tools:"
echo "  - C++ compiler: $(g++ --version | head -1)"
echo "  - CMake: $(cmake --version | head -1)"
echo "  - Python: $(python3 --version)"
echo "  - etcd & etcdctl: $(etcd --version | head -1)"
echo "  - Zsh: $(zsh --version)"
echo ""
echo "🎯 Next steps:"
echo "  1. Configure git if needed"
echo "  2. Start developing your consensus algorithm!"
echo "  3. Use 'cmake -B build && cmake --build build' to build C++ code"
echo "  4. Use 'pip install -r requirements.txt' to install Python dependencies"
echo "  5. Use 'etcd' to start a local etcd server for testing"
echo ""
