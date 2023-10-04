#include "tableRequestHandlers.h"

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;
using namespace Poco::Data;
using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

void TableCreateRequestHandler::handleRequest(HTTPServerRequest& request,
                                              HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Create table\" from %s",
                                 request.clientAddress().toString());

    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    response.setKeepAlive(true);
    response.set("access-control-allow-origin", "*");

    HTMLForm form(request, request.stream());
    auto databaseName = form.find("databaseName");

    auto tableName = form.find("tableName");

    auto columns = form.find("columns");

    RegularExpression attributesRegex(R"(^[a-zA-Z]+\:\s(int)+$|^[a-zA-Z]+\:\s(double)+$
    |^[a-zA-Z]+\:\s(char\([1-9]+\))+$|^[a-zA-Z]+\:\s(varchar)+$|^[a-zA-Z]+\:\s(date)+$
    |^[a-zA-Z]+\:\s(date_interval)+$)");

    std::vector<std::pair<std::string, std::string>> tableColumns;
    std::map<std::string, std::string> numberTypes = {
            {"int", "bigint"},
            {"real", "double precision"}
    };

    if (databaseName == form.end() || tableName == form.end() || columns == form.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        std::string columnPairs = columns->second;
        auto iterator = columnPairs.begin(),
        previousIterator = iterator;
        while (iterator != columnPairs.end()) {
            previousIterator = iterator;
            iterator = std::find(iterator, columnPairs.end(), '|');
            auto column = std::string(previousIterator, iterator);

            if (iterator != columnPairs.end()) {
                ++iterator;
            }

            if (!attributesRegex.match(column)) {
                response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
                response.send();
                return;
            }

            auto columnName = form.find(std::string(
                    column.begin(),
                    std::find(column.begin(), column.end(), ':')));

            tableColumns.emplace_back(
                columnName->first,
                columnName->second
            );
        }

        for (auto& [name, type]: tableColumns) {
            if (numberTypes.find(name) != numberTypes.end()) {
                type = numberTypes[name];
            }
        }

        std::string query = "CREATE TABLE " + tableName->second + "(";

        for (int i = 0; i < tableColumns.size() - 1; i++) {
            query += tableColumns[i].first;
            query += " ";
            query += tableColumns[i].second;
            query += " NOT NULL, ";
        }

        query += tableColumns.back().first;
        query += " ";
        query += tableColumns.back().second;
        query += " NOT NULL)";

        PostgreSQL::Connector::registerConnector();
        Poco::Data::Session session(Poco::Data::PostgreSQL::Connector::KEY,
                                    "host=localhost port=5432 user=alex_braun "
                                    "password=AlAzazaAl123 dbname=" + databaseName->second);

        session << query, now;

        response.setStatus(HTTPResponse::HTTP_OK);
    }

    response.send();
}

void TableShowRequestHandler::handleRequest(HTTPServerRequest& request,
                                            HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Show table\" from %s",
                                 request.clientAddress().toString());

    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    response.setKeepAlive(true);
    response.set("access-control-allow-origin", "*");

    const std::string& uri = request.getURI();
    auto iterator = uri.begin() + 10;
    std::string databaseName;
    while (*iterator != '/') {
        databaseName += *iterator;
        ++iterator;
    }

    iterator += 12;
    std::string tableName;
    while (iterator != uri.end()) {
        tableName += *iterator;
        ++iterator;
    }

    response.setStatus(HTTPResponse::HTTP_OK);

    PostgreSQL::Connector::registerConnector();
    Poco::Data::Session session(Poco::Data::PostgreSQL::Connector::KEY,
                                "host=localhost port=5432 user=alex_braun "
                                "password=AlAzazaAl123 dbname=" + databaseName);

    Statement select(session);
    select << "SELECT * FROM " + tableName, now;
    RecordSet rs(select);

    Poco::JSON::Object result;
    result.set("Database", databaseName);
    result.set("Table", tableName);

    size_t columns = rs.columnCount();
    std::vector<std::string> columnNames;
    for (size_t i = 0; i < columns; ++i) {
        columnNames.push_back(rs.columnName(i));
    }

    result.set("Columns", columnNames);

    std::vector<Poco::JSON::Object> rows;

    for (RecordSet::Iterator row = rs.begin(); row != rs.end(); ++row) {
        Poco::JSON::Object rowData;
        for (size_t i = 0; i < columns; ++i) {
            rowData.set(rs.columnName(i), row->get(i));
        }
        rows.push_back(rowData);
    }

    result.set("Rows", rows);

    std::ostream& answer = response.send();
    result.stringify(answer);
}

void TableRenameRequestHandler::handleRequest(HTTPServerRequest& request,
                                              HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Rename table\" from %s",
                                 request.clientAddress().toString());

    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    response.setKeepAlive(true);
    response.set("access-control-allow-origin", "*");

    HTMLForm form(request, request.stream());
    auto databaseName = form.find("databaseName"),
    tablePreviousName = form.find("tablePreviousName"),
    tableNewName = form.find("tableNewName");

    if (databaseName == form.end() || tablePreviousName == form.end() ||
    tableNewName == form.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        PostgreSQL::Connector::registerConnector();
        Poco::Data::Session session(Poco::Data::PostgreSQL::Connector::KEY,
                                    "host=localhost port=5432 user=alex_braun "
                                    "password=AlAzazaAl123 dbname=" + databaseName->second);

        session << "ALTER TABLE " + tablePreviousName->second + " RENAME TO " +
                   tableNewName->second, now;

        response.setStatus(HTTPResponse::HTTP_OK);
    }

    response.send();
}

void TableDeleteRequestHandler::handleRequest(HTTPServerRequest& request,
                                              HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Delete table\" from %s",
                                 request.clientAddress().toString());

    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    response.setKeepAlive(true);
    response.set("access-control-allow-origin", "*");

    HTMLForm form(request, request.stream());
    auto databaseName = form.find("databaseName"),
    tableName = form.find("tableName");

    if (databaseName == form.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        PostgreSQL::Connector::registerConnector();
        Poco::Data::Session session(Poco::Data::PostgreSQL::Connector::KEY,
                                    "host=localhost port=5432 user=alex_braun "
                                    "password=AlAzazaAl123 dbname=" + databaseName->second);

        session << "DROP TABLE IF EXISTS " + tableName->second, now;

        response.setStatus(HTTPResponse::HTTP_OK);
    }

    response.send();
}