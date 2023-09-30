#pragma once

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/JSON/Object.h"

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

class NotFoundRequestHandler: public HTTPRequestHandler {
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override;
};