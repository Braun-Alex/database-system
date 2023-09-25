#include "rowRequestHandlers.h"

void RowAddRequestHandler::handleRequest(HTTPServerRequest& request,
                                         HTTPServerResponse& response) {
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

void RowEditRequestHandler::handleRequest(HTTPServerRequest& request,
                                          HTTPServerResponse& response) {
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

void RowDeleteRequestHandler::handleRequest(HTTPServerRequest& request,
                                            HTTPServerResponse& response) {
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