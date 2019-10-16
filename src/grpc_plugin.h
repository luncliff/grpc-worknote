#pragma once

#include <plugin_core.h>
#include "service.grpc.pb.h"

struct plugin_request_t : plugins::Request1 {};
struct plugin_response_t : plugins::Response1 {};
