// Baidu RPC - A framework to host and access services throughout Baidu. 
// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved

// Author: The baidu-rpc authors (pbrpc@baidu.com)
// Date: Sun Sep  7 17:24:45 CST 2014

#ifndef BRPC_SERVER_ID_H
#define BRPC_SERVER_ID_H

// To baidu-rpc developers: This is a header included by user, don't depend
// on internal structures, use opaque pointers instead.

#include "base/containers/hash_tables.h"   // hash
#include "base/containers/flat_map.h"
#include "brpc/socket_id.h"


namespace brpc {

// Representing a server inside LoadBalancer.
struct ServerId {
    ServerId() : id(0) {}
    explicit ServerId(SocketId id_in) : id(id_in) {}
    ServerId(SocketId id_in, const std::string& tag_in)
        : id(id_in), tag(tag_in) {}

    SocketId id;
    std::string tag;
};
inline bool operator==(const ServerId& id1, const ServerId& id2)
{ return id1.id == id2.id && id1.tag == id2.tag; }
inline bool operator!=(const ServerId& id1, const ServerId& id2)
{ return !(id1 == id2); }
inline bool operator<(const ServerId& id1, const ServerId& id2)
{ return id1.id != id2.id ? (id1.id < id2.id) : (id1.tag < id2.tag); }
inline std::ostream& operator<<(std::ostream& os, const ServerId& tsid) {
    os << tsid.id;
    if (!tsid.tag.empty()) {
        os << "(tag=" << tsid.tag << ')';
    }
    return os;
}

// Statefully map ServerId to SocketId.
class ServerId2SocketIdMapper {
public:
    ServerId2SocketIdMapper();
    ~ServerId2SocketIdMapper();
    // Remember duplicated count of server.id
    // Returns true if server.id does not exist before.
    bool AddServer(const ServerId& server);
    // Remove 1 duplication of server.id.
    // Returns true if server.id does not exist after.
    bool RemoveServer(const ServerId& server);
    // Remember duplicated counts of all SocketId in servers.
    // Returns list of SocketId that do not exist before.
    std::vector<SocketId>& AddServers(const std::vector<ServerId>& servers);
    // Remove 1 duplication of all SocketId in servers respectively.
    // Returns list of SocketId that do not exist after.
    std::vector<SocketId>& RemoveServers(const std::vector<ServerId>& servers);
private:
    base::FlatMap<SocketId, int> _nref_map;
    std::vector<SocketId> _tmp;
};

} // namespace brpc


namespace BASE_HASH_NAMESPACE {
#if defined(COMPILER_GCC)
template<>
struct hash<brpc::ServerId> {
    std::size_t operator()(const ::brpc::ServerId& tagged_id) const {
        return hash<std::string>()(tagged_id.tag) * 101 + tagged_id.id;
    }
};
#elif defined(COMPILER_MSVC)
inline size_t hash_value(const ::brpc::ServerId& tagged_id) {
    return hash_value(tagged_id.tag) * 101 + tagged_id.id;
}
#endif  // COMPILER
}

#endif  // BRPC_SERVER_ID_H