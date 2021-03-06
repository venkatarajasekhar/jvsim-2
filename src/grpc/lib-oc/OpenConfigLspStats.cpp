//
//  OpenConfigLspStats.cpp
//  agent-jv
//
//  Created by NITIN KUMAR on 1/12/16.
//  Copyright © 2016 Juniper Networks. All rights reserved.
//

#include "OpenConfigLspStats.hpp"
#include "oc.hpp"

std::string BASE_OC_PATH_LSP("/mpls/lsps/constrained-path/tunnels/tunnel");
std::string BASE_OC_PATH_LSP_ATTR("state/counters");

void
OpenConfigLspStats::iterate (JuniperNetworksSensors *handle, telemetry::OpenConfigData *datap)
{
    LspStats *message = handle->MutableExtension(jnpr_lsp_statistics_ext);
    telemetry::KeyValue *kv;
    

    for (int i = 0; i < message->lsp_stats_records_size(); i++) {
        const LspStatsRecord& record = message->lsp_stats_records(i);

        if (!record.packets()) {
            continue;
        }

        // Add Prefix
        oc_set_prefix(datap, BASE_OC_PATH_LSP, record.name(), BASE_OC_PATH_LSP_ATTR);

        kv = datap->add_kv();
        kv->set_key(record.counter_name() + "/packets");
        kv->set_int_value(record.packets());
        
        kv = datap->add_kv();
        kv->set_key(record.counter_name() + "/bytes");
        kv->set_int_value(record.bytes());
    }
}
