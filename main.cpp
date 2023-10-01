#include "handlers/database/databaseRequestHandlers.h"
#include "handlers/table/tableRequestHandlers.h"
#include "handlers/row/rowRequestHandlers.h"
#include "handlers/attribute/attributeRequestHandlers.h"
#include "handlers/error/errorRequestHandler.h"

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/RegularExpression.h"

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

class RequestHandlerFactory: public HTTPRequestHandlerFactory
{
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override
    {
        RegularExpression showTablesRegex(R"(^\/database\/show\/[A-Za-z_]+$)"),
        showTableRegex(R"(^\/database\/[a-zA-Z_]+\/table\/show\/[a-zA-Z_]+$)");
        const auto& uri = request.getURI();
        if (uri == "/show") {
            return new DatabasesShowRequestHandler();
        } else if (uri == "/database/create") {
            return new DatabaseCreateRequestHandler();
        } else if (showTablesRegex.match(uri)) {
            return new DatabaseShowRequestHandler();
        } else if (uri == "/database/rename") {
            return new DatabaseRenameRequestHandler();
        } else if (uri == "/database/delete") {
            return new DatabaseDeleteRequestHandler();
        } else if (uri == "/database/table/create") {
            return new TableCreateRequestHandler();
        } else if (showTableRegex.match(uri)) {
            return new TableShowRequestHandler();
        } else if (uri == "/database/table/rename") {
            return new TableRenameRequestHandler();
        } else if (uri == "/database/table/delete") {
            return new TableDeleteRequestHandler();
        } else if (uri == "/database/table/row/add") {
            return new RowAddRequestHandler();
        } else if (uri == "/database/table/row/edit") {
            return new RowEditRequestHandler();
        } else if (uri == "/database/table/row/delete") {
            return new RowDeleteRequestHandler();
        } else if (uri == "/database/table/attribute/add") {
            return new AttributeAddRequestHandler();
        } else if (uri == "/database/table/attribute/edit") {
            return new AttributeEditRequestHandler();
        } else if (uri == "/database/table/attribute/delete") {
            return new AttributeDeleteRequestHandler();
        } else {
            return new NotFoundRequestHandler();
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