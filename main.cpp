#include "databaseRequestHandlers.h"

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/ServerApplication.h"

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

class TableCreateRequestHandler: public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        Application& app = Application::instance();
        app.logger().information("Request \"Create table\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
    }
};

class TableShowRequestHandler: public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        Application& app = Application::instance();
        app.logger().information("Request \"Show table\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
    }
};

class TableRenameRequestHandler: public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        Application& app = Application::instance();
        app.logger().information("Request \"Rename table\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
    }
};

class TableDeleteRequestHandler: public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        Application& app = Application::instance();
        app.logger().information("Request \"Delete table\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
    }
};

class RowAddRequestHandler: public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        Application& app = Application::instance();
        app.logger().information("Request \"Add row\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
    }
};

class RowEditRequestHandler: public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        Application& app = Application::instance();
        app.logger().information("Request \"Edit row\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
    }
};

class RowDeleteRequestHandler: public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        Application& app = Application::instance();
        app.logger().information("Request \"Delete row\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
    }
};

class AttributeAddRequestHandler: public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        Application& app = Application::instance();
        app.logger().information("Request \"Add attribute\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
    }
};

class AttributeEditRequestHandler: public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        Application& app = Application::instance();
        app.logger().information("Request \"Edit attribute\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
    }
};

class AttributeDeleteRequestHandler: public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        Application& app = Application::instance();
        app.logger().information("Request \"Delete attribute\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
    }
};

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
        } else if (uri == "table/create") {
            return new TableCreateRequestHandler();
        } else if (uri == "table/show") {
            return new TableShowRequestHandler();
        } else if (uri == "table/rename") {
            return new TableRenameRequestHandler();
        } else if (uri == "table/delete") {
            return new TableDeleteRequestHandler();
        } else if (uri == "row/add") {
            return new RowAddRequestHandler();
        } else if (uri == "row/edit") {
            return new RowEditRequestHandler();
        } else if (uri == "row/delete") {
            return new RowDeleteRequestHandler();
        } else if (uri == "attribute/add") {
            return new AttributeAddRequestHandler();
        } else if (uri == "attribute/edit") {
            return new AttributeEditRequestHandler();
        } else if (uri == "attribute/delete") {
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