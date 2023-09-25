#include "handlers/database/databaseRequestHandlers.h"
#include "handlers/table/tableRequestHandlers.h"
#include "handlers/row/rowRequestHandlers.h"
#include "handlers/attribute/attributeRequestHandlers.h"

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/ServerApplication.h"

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

class RequestHandlerFactory: public HTTPRequestHandlerFactory
{
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override
    {
        const auto& uri = request.getURI();
        if (uri == "/database/create") {
            return new DatabaseCreateRequestHandler();
        } else if (uri == "/database/show") {
            return new DatabaseShowRequestHandler();
        } else if (uri == "/database/rename") {
            return new DatabaseRenameRequestHandler();
        } else if (uri == "/database/delete") {
            return new DatabaseDeleteRequestHandler();
        } else if (uri == "/table/create") {
            return new TableCreateRequestHandler();
        } else if (uri == "/table/show") {
            return new TableShowRequestHandler();
        } else if (uri == "/table/rename") {
            return new TableRenameRequestHandler();
        } else if (uri == "/table/delete") {
            return new TableDeleteRequestHandler();
        } else if (uri == "/row/add") {
            return new RowAddRequestHandler();
        } else if (uri == "/row/edit") {
            return new RowEditRequestHandler();
        } else if (uri == "/row/delete") {
            return new RowDeleteRequestHandler();
        } else if (uri == "/attribute/add") {
            return new AttributeAddRequestHandler();
        } else if (uri == "/attribute/edit") {
            return new AttributeEditRequestHandler();
        } else if (uri == "/attribute/delete") {
            return new AttributeDeleteRequestHandler();
        }
    }
};

class WebServerApp: public ServerApplication
{
    void initialize(Application& self) override
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    int main(const std::vector<std::string>&) override
    {
        UInt16 port = static_cast<UInt16>(config().getUInt("port", 8080));
        auto* params = new HTTPServerParams;
        params->setMaxQueued(100);
        params->setMaxThreads(16);
        HTTPServer srv(new RequestHandlerFactory, port, params);
        srv.start();
        logger().information("HTTP Server started on port %hu.", port);
        waitForTerminationRequest();
        logger().information("Stopping HTTP Server...");
        srv.stop();

        return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(WebServerApp)