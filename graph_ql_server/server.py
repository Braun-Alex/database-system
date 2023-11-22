from ariadne import QueryType, MutationType, make_executable_schema, load_schema_from_path, graphql_sync
from flask import Flask, request, jsonify
from models.db_manager import DBManager
from models.column import StringCol, IntCol

type_defs = load_schema_from_path("schema.graphql")

query = QueryType()
mutation = MutationType()
db_manager = DBManager()


@mutation.field("createDatabase")
def resolve_create_database(_, info, name):
    db_manager.create_database(name)
    return {"success": True, "message": f"Базу даних '{name}' створено"}


@mutation.field("addTable")
def resolve_add_table(_, info, name):
    db_manager.add_table(name)
    return {"success": True, "message": f"Таблицю '{name}' додано до бази даних"}


@mutation.field("addColumn")
def resolve_add_column(_, info, table_name, column_name, column_type):
    if column_type == "String":
        column = StringCol(column_name)
    elif column_type == "Int":
        column = IntCol(column_name)
    else:
        return {"success": False, "message": "Тип атрибуту є коректним"}

    db_manager.add_column(table_name, column)
    return {"success": True, "message": f"Атрибут '{column_name}' додано до таблиці '{table_name}'"}


@mutation.field("addRow")
def resolve_add_row(_, info, table_name, data):
    db_manager.add_row(table_name, data)
    return {"success": True, "message": f"Рядок додано до таблиці '{table_name}'."}


@query.field("joinTables")
def resolve_join_tables(_, info, first_table_name, second_table_name, field_name):
    joined_table = db_manager.join_tables(first_table_name, second_table_name, field_name)
    return {
        "name": joined_table.name,
        "columns": [{"name": col.name, "column_type": col.type} for col in joined_table._columns],
        "rows": [{"values": row.values} for row in joined_table._rows]
    }


schema = make_executable_schema(type_defs, query, mutation)

app = Flask(__name__)


@app.route("/graphql", methods=["GET"])
def graphql_playground():
    return '''
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset=utf-8/>
      <title>GraphQL Playground</title>
      <link rel="stylesheet" href="//cdn.jsdelivr.net/npm/graphql-playground-react@1.7.20/build/static/css/index.css" />
    </head>
    <body>
      <div id="root"></div>
      <script src="//cdn.jsdelivr.net/npm/graphql-playground-react@1.7.20/build/static/js/middleware.js"></script>
      <script>window.addEventListener('load', function (event) {
          GraphQLPlayground.init(document.getElementById('root'), {
            endpoint: '/graphql'
          })
        })</script>
    </body>
    </html>
    ''', 200


@app.route("/graphql", methods=["POST"])
def graphql_server():
    data = request.get_json()
    success, result = graphql_sync(schema, data, context_value=request)
    status_code = 200 if success else 400
    return jsonify(result), status_code


if __name__ == '__main__':
    app.run(debug=True)
