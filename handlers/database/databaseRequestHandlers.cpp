#include "databaseRequestHandlers.h"

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

void DatabaseCreateRequestHandler::handleRequest(HTTPServerRequest& request,
                                                 HTTPServerResponse& response) {
        Application& app = Application::instance();
        app.logger().information("Request \"Create database\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName");
        if (databaseName == request.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
}

void DatabaseShowRequestHandler::handleRequest(HTTPServerRequest& request,
                                               HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Show database\" from %s",
                             request.clientAddress().toString());

    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    auto databaseName = request.find("databaseName");
    if (databaseName == request.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        response.setStatus(HTTPResponse::HTTP_OK);
    }
    response.send();
}

void DatabaseRenameRequestHandler::handleRequest(HTTPServerRequest& request,
                                                 HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Rename database\" from %s",
                             request.clientAddress().toString());

    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    auto databaseName = request.find("databaseName");
    if (databaseName == request.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        response.setStatus(HTTPResponse::HTTP_OK);
    }
    response.send();
}

void DatabaseDeleteRequestHandler::handleRequest(HTTPServerRequest& request,
                                                 HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Delete database\" from %s",
                             request.clientAddress().toString());

    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    auto databaseName = request.find("databaseName");
    if (databaseName == request.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        response.setStatus(HTTPResponse::HTTP_OK);
    }
    response.send();
}