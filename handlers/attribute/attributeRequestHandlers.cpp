#include "attributeRequestHandlers.h"

void AttributeAddRequestHandler::handleRequest(HTTPServerRequest& request,
                                               HTTPServerResponse& response) {
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

void AttributeEditRequestHandler::handleRequest(HTTPServerRequest& request,
                                               HTTPServerResponse& response) {
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

void AttributeDeleteRequestHandler::handleRequest(HTTPServerRequest& request,
                                                  HTTPServerResponse& response) {
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