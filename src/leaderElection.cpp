#include "leaderElection.h"

LeaderElection::LeaderElection(const std::string &nodeId, const std::string &endpoint)
    : nodeId(nodeId), etcdEndpoint(endpoint)
{
}

LeaderElection::~LeaderElection()
{
}

bool LeaderElection::startElection(EtcdClient &etcd_client)
{

    return true;
}

bool LeaderElection::isLeader(EtcdClient &etcd_client) const
{
    auto current_leader = getCurrentLeader(etcd_client);
    if (current_leader == nodeId)
    {
        return true; // This node is the leader
    }
    else if (current_leader.empty())
    {
        // No leader is currently elected
        return false;
    }
}

std::string LeaderElection::getCurrentLeader(EtcdClient &etcd_client) const
{
    // Implementation to get the current leader ID
    auto getResponse = etcd_client.get("/election/leader");
    if (getResponse.success)
    {
        // Assuming the response data is the leader ID
        return getResponse.data;
    }
    else
    {
        // Handle error or return empty string if no leader is found
        return "";
    }
}

void LeaderElection::stopElection()
{
    // Implementation to stop the election process
}
