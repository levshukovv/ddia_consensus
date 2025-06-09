#!/bin/bash

# etcd Management Script for Leader Election Development

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COMPOSE_FILE="$SCRIPT_DIR/docker-compose.single.yml"
CLUSTER_COMPOSE_FILE="$SCRIPT_DIR/docker-compose.yml"

show_help() {
    echo "Usage: $0 [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  start         Start single etcd node (development)"
    echo "  start-cluster Start 3-node etcd cluster"
    echo "  stop          Stop etcd services"
    echo "  restart       Restart etcd services"
    echo "  status        Show etcd health status"
    echo "  logs          Show etcd logs"
    echo "  shell         Connect to etcd container shell"
    echo "  test          Run basic etcd operations test"
    echo "  watch         Watch for changes in leader/ prefix"
    echo "  clean         Stop and remove all etcd data"
    echo "  help          Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 start                    # Start single node for development"
    echo "  $0 start-cluster           # Start 3-node cluster"
    echo "  $0 test                     # Test etcd operations"
    echo "  $0 watch                    # Watch leader election keys"
}

wait_for_etcd() {
    echo "Waiting for etcd to be ready..."
    for i in {1..30}; do
        if docker exec etcd-single etcdctl endpoint health >/dev/null 2>&1; then
            echo "etcd is ready!"
            return 0
        fi
        echo -n "."
        sleep 1
    done
    echo "etcd failed to start properly"
    return 1
}

case "${1:-help}" in
    start)
        echo "Starting single etcd node..."
        docker-compose -f "$COMPOSE_FILE" up -d
        wait_for_etcd
        echo "etcd is running on http://localhost:2379"
        ;;
    start-cluster)
        echo "Starting 3-node etcd cluster..."
        docker-compose -f "$CLUSTER_COMPOSE_FILE" up -d
        echo "etcd cluster is starting..."
        echo "Node 1: http://localhost:2379"
        echo "Node 2: http://localhost:2381"
        echo "Node 3: http://localhost:2383"
        ;;
    stop)
        echo "Stopping etcd services..."
        docker-compose -f "$COMPOSE_FILE" down 2>/dev/null || true
        docker-compose -f "$CLUSTER_COMPOSE_FILE" down 2>/dev/null || true
        ;;
    restart)
        $0 stop
        sleep 2
        $0 start
        ;;
    status)
        echo "Checking etcd status..."
        if docker ps | grep -q etcd; then
            echo "etcd containers are running:"
            docker ps | grep etcd
            echo ""
            echo "Health check:"
            docker exec etcd-single etcdctl endpoint health 2>/dev/null || echo "Single node not available"
        else
            echo "No etcd containers are running"
        fi
        ;;
    logs)
        echo "etcd logs:"
        docker-compose -f "$COMPOSE_FILE" logs -f etcd-single
        ;;
    shell)
        echo "Connecting to etcd container..."
        docker exec -it etcd-single /bin/sh
        ;;
    test)
        echo "Running etcd operations test..."
        echo "1. Putting test data..."
        docker exec etcd-single etcdctl put /test/key1 "value1"
        docker exec etcd-single etcdctl put /test/key2 "value2"
        
        echo "2. Getting test data..."
        docker exec etcd-single etcdctl get /test/key1
        docker exec etcd-single etcdctl get /test/key2
        
        echo "3. Listing all keys with /test prefix..."
        docker exec etcd-single etcdctl get /test/ --prefix
        
        echo "4. Testing leader election key..."
        docker exec etcd-single etcdctl put /leader/election "leader-$(date +%s)"
        docker exec etcd-single etcdctl get /leader/election
        
        echo "Test completed successfully!"
        ;;
    watch)
        echo "Watching for changes in /leader/ prefix..."
        echo "In another terminal, try: ./etcd.sh test"
        docker exec etcd-single etcdctl watch /leader/ --prefix
        ;;
    clean)
        echo "Stopping and cleaning all etcd data..."
        docker-compose -f "$COMPOSE_FILE" down -v 2>/dev/null || true
        docker-compose -f "$CLUSTER_COMPOSE_FILE" down -v 2>/dev/null || true
        echo "All etcd data cleaned"
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo "Unknown command: $1"
        echo "Use '$0 help' for usage information"
        exit 1
        ;;
esac
