//
//  AgentClient.hpp
//  agent-jv-client
//
//  Created by NITIN KUMAR on 12/29/15.
//  CoAuthors: ABBAS SAKARWALA
//
//  Copyright © 2015 Juniper Networks. All rights reserved.
//

#ifndef AgentClient_hpp
#define AgentClient_hpp

#include <iostream>
#include <string>
#include <grpc++/grpc++.h>
#include "agent.grpc.pb.h"
#include "AgentServerLog.hpp"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using telemetry::OpenConfigTelemetry;

using telemetry::CancelSubscriptionReply;
using telemetry::CancelSubscriptionRequest;
using telemetry::Collector;
using telemetry::DataEncodingReply;
using telemetry::DataEncodingRequest;
using telemetry::GetOperationalStateReply;
using telemetry::GetOperationalStateRequest;
using telemetry::GetSubscriptionsReply;
using telemetry::GetSubscriptionsRequest;
using telemetry::KeyValue;
using telemetry::OpenConfigData;
using telemetry::Path;
using telemetry::SubscriptionAdditionalConfig;
using telemetry::SubscriptionInput;
using telemetry::SubscriptionReply;
using telemetry::SubscriptionRequest;
using telemetry::SubscriptionResponse;
using telemetry::ReturnCode;
using telemetry::VerbosityLevel;
using telemetry::EncodingType;

// AGENT_SERVER_ADDRESS
extern std::string server_ip, server_port;
#define AGENT_SERVER_IP_PORT  (std::string((server_ip + ":" + server_port)))

// Name of Management Client
#define AGENTCLIENT_MGMT        "Management-Client"

// AgentClient Class
class AgentClient {
    std::string _name;
    std::string _logfile;
    bool _active;
    bool _debug_log;

    // ID used by the client
    uint32_t _id;

    // ID allocated by the server
    bool     _subscription_id_valid;
    uint32_t _subscription_id;

public:
    // Stub Variable
    std::unique_ptr<OpenConfigTelemetry::Stub> stub_;

    AgentClient (std::shared_ptr<Channel> channel,
                 const std::string& name,
                 uint32_t id,
                 const std::string& logfile_dir) :
                 stub_(OpenConfigTelemetry::NewStub(channel)),
                 _name(name), _active(true), _id(id)
    {
        std::string s(logfile_dir);
        _logfile = s + _name;
        _subscription_id_valid = false;
        _subscription_id       = 0;
        _debug_log = false;
    }   

    ~AgentClient();

    static AgentClient *create(std::shared_ptr<Channel> channel,
                               std::string& name,
                               uint32_t id,
                               const std::string& logfile_dir);
    static AgentClient *find(std::string name);
    static void         print(void);
    void                rehome(void);

    std::string getName (void)          { return _name;                  }
    bool        getActive (void)        { return _active;                }
    uint32_t    getId (void)            { return _id;                    }
    uint32_t    getServerId (void)      { return _subscription_id;       }
    bool        getServerIdValid (void) { return _subscription_id_valid; }
    bool        getDebug (void)         { return _debug_log;             }
    void        setDebug(bool val)      { _debug_log = val;              }

    void subscribeTelemetry(std::vector<std::string> path_list,
                            uint32_t sample_frequency,
                            bool     telegraph = false,
                            uint32_t limit_records = 0,
                            uint32_t limit_seconds = 0);
    void cancelSubscribeTelemetry(void);
    void cancelSubscribeTelemetryId(uint32_t subscription_id);
    void listSubscriptions(uint32_t subscription_id);
    void getOperational(uint32_t subscription_id,
                        telemetry::VerbosityLevel verbosity);

    void errorMsg (std::string err_str, Status code)
    {
        std::cout << err_str << std::endl;
    }
};

#endif /* AgentClient_hpp */
