#ifndef LEADER_ELECTION_H
#define LEADER_ELECTION_H

#include <string>
#include <memory>
#include "etcdClient.h"

class LeaderElection
{
private:
    std::string nodeId;
    std::string etcdEndpoint;

public:
    LeaderElection(const std::string &nodeId, const std::string &endpoint = "http://localhost:2379");
    ~LeaderElection();

    // Start the leader election process
    bool startElection(EtcdClient &etcd_client);

    // Check if this node is the current leader
    bool isLeader(EtcdClient &etcd_client) const;

    // Get the current leader ID
    std::string getCurrentLeader(EtcdClient &etcd_client) const;

    // Stop the election process
    void stopElection();
};

#endif // LEADER_ELECTION_H
