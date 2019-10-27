#pragma once

#include "plugin_core.h"
#include "service.pb.h"

struct plugin_request_t : plugins_v1::Request1 {};
struct plugin_response_t : plugins_v1::Response1 {};
