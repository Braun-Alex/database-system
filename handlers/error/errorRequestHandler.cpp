#include "errorRequestHandler.h"

void NotFoundRequestHandler::handleRequest(HTTPServerRequest& request,
                                           HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Delete attribute\" from %s",
                             request.clientAddress().toString());

    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    response.setKeepAlive(true);
    response.set("access-control-allow-origin", "*");
    response.setStatus(HTTPResponse::HTTP_NOT_FOUND);

    Poco::JSON::Object attributes;
    attributes.set("API endpoint", request.getURI());
    attributes.set("Result of request", "Not found");
    std::ostream& answer = response.send();
    attributes.stringify(answer);

    response.send();
}