import PySimpleGUI as sg
import Pyro5.api

from models import column
from pyro_client.views import (
    enter_uri_view,
    create_db_view,
    add_table_view,
    add_column_view,
    add_row_view,
    delete_row_view,
    delete_table_view,
    change_row_view,
    change_row_detailed_view,
    save_db_view,
    open_db_view,
    join_tables_view
)

sg.theme("SandyBeach")

danger_button = {
    'button_color': 'red'
}

green_button = {
    'button_color': 'green'
}

sg.set_options(font=("Andale Mono", 14))

layout = [
    [sg.Button("Підключитися"), sg.Button("Створити базу даних", **green_button), sg.Button("Завантажити базу даних"),
     sg.Button("Зберегти базу даних")],
    [
        sg.Text("Поточна база даних:"),
        sg.Text(key="-CURRENT-DB-"),
        sg.Text(key="-ERRORS-", text_color="red"),
    ],
    [
        sg.Text("Таблиці:"),
        sg.Button("Додати атрибут", **green_button),
        sg.Text("Вміст:", pad=((145, 0), (0, 0))),
        sg.Push(),
        sg.Button("Сполучити таблиці"),
    ],
    [
        sg.Listbox([], size=(23, 16), key="-TABLE-LIST-", enable_events=True),
        sg.Multiline(key="-TABLE-DATA-", size=(80, 16), font=("Andale Mono", 15)),
    ],
    [
        sg.Button("Створити таблицю", **green_button),
        sg.Button("Вилучити таблицю", **danger_button),
        sg.Button("Вставити рядок", **green_button),
        sg.Button("Вилучити рядок", **danger_button),
        sg.Button("Змінити рядок"),
    ],
]


def parse_value(val, type_: str):
    if type_ == "int":
        return int(val)
    if type_ == "real":
        return float(val)

    return val


LOCAL_DATA = {
    "selected_table": None,
    "tables": {},
    "selected_table_cols": {},
    "selected_table_rows": [],
}

DB_MANAGER = None

window = sg.Window("Фрагментарна система управління табличними базами даних", layout)

while True:
    try:
        event, values = window.read()
        should_re_render = True
        print(event, values)
        if event == sg.WIN_CLOSED:
            DB_MANAGER.unregister_db()
            break

        if event == "Підключитися":
            event, values = enter_uri_view()
            if event == "Підключитися":
                DB_MANAGER = Pyro5.api.Proxy(values["-URI-"])
                should_re_render = False

        if event == "Створити базу даних":
            event, values = create_db_view()
            if event == "Створити":
                DB_MANAGER.create_database(values["-DB-NAME-"])

        elif event == "Створити таблицю":
            event, values = add_table_view()
            if event == "Створити":
                table_name = values["-TABLE-NAME-"]
                table_uri = DB_MANAGER.add_table(table_name)
                LOCAL_DATA["selected_table"] = Pyro5.api.Proxy(table_uri)
                LOCAL_DATA["tables"][table_name] = LOCAL_DATA["selected_table"]

        elif event == "Вилучити таблицю":
            event, values = delete_table_view()
            if event == "Вилучити":
                DB_MANAGER.delete_table(values["-TABLE-NAME-"])
                LOCAL_DATA["selected_table"] = None

        elif event == "Додати атрибут":
            event, values = add_column_view(
                column_choices=column.COLUMN_TYPE_CHOICES,
                table_name=LOCAL_DATA["selected_table"].name,
            )
            if event == "Додати":
                new_column = None
                column_type = values["-COLUMN-TYPE-"]
                column_name = values["-COLUMN-NAME-"]
                col = Pyro5.api.Proxy(
                    DB_MANAGER.add_column(
                        values["-TABLE-NAME-"], column_type, column_name
                    )
                )
                LOCAL_DATA["selected_table"] = LOCAL_DATA["tables"][
                    values["-TABLE-NAME-"]
                ]
                LOCAL_DATA["selected_table_cols"][col.name] = col

        elif event == "Вставити рядок":
            table = DB_MANAGER.get_table(LOCAL_DATA["selected_table"].name)
            event, values = add_row_view(
                table.columns[1:],
            )

            if event == "Вставити":
                for column_name in list(values):
                    correct_column_name = column_name.split(":")[0]
                    col = LOCAL_DATA["selected_table_cols"][correct_column_name]
                    values[correct_column_name] = parse_value(
                        values.pop(column_name), col.type
                    )

                row_uri = DB_MANAGER.add_row(table_name=table.name, data=values)
                LOCAL_DATA["selected_table_rows"].append(Pyro5.api.Proxy(row_uri))

        elif event == "Вилучити рядок":
            event, values = delete_row_view()
            if event == "Вилучити":
                DB_MANAGER.delete_row(
                    LOCAL_DATA["selected_table"].name, int(values["-ROW-INDEX-"])
                )

        elif event == "Змінити рядок":
            table = DB_MANAGER.get_table(LOCAL_DATA["selected_table"].name)
            rows = LOCAL_DATA["selected_table_rows"]
            event, values = change_row_view()

            if event == "Змінити":
                index_to_change = int(values["-ROW-INDEX-"])
                print("Рядки", table.rows)
                change_event, change_values = change_row_detailed_view(
                    table.columns[1:], rows[index_to_change].values
                )

                if change_event == "Змінити":
                    for column_name in list(change_values):
                        correct_column_name = column_name.split(":")[0]
                        col = LOCAL_DATA["selected_table_cols"][correct_column_name]
                        change_values[correct_column_name] = parse_value(
                            change_values.pop(column_name), col.type
                        )

                    DB_MANAGER.change_row(
                        table_name=table.name, index=index_to_change, data=change_values
                    )

        elif event == "Сполучити таблиці":
            event, values = join_tables_view()
            if event == "Сполучити":
                view = Pyro5.api.Proxy(
                    DB_MANAGER.join_tables(
                        values["-FIRST-TABLE-NAME-"],
                        values["-SECOND-TABLE-NAME-"],
                        values["-FIELD-NAME-"]
                    )
                )
                window["-TABLE-DATA-"].update(view.to_str())
                should_re_render = False

        elif event == "-TABLE-LIST-":
            if values["-TABLE-LIST-"]:
                LOCAL_DATA["selected_table"] = LOCAL_DATA["tables"][
                    values["-TABLE-LIST-"][0]
                ]

        elif event == "Зберегти базу даних":
            event, values = save_db_view()

            if event == "Зберегти":
                DB_MANAGER.save_database(values["-DB-SAVE-PATH-"])
                sg.popup("Базу даних успішно збережено!")

        elif event == "Завантажити базу даних":
            event, values = open_db_view()

            if event == "Завантажити":
                DB_MANAGER.open_database(values["-DB-OPEN-PATH-"])
                sg.popup("Базу даних успішно завантажено!")
                LOCAL_DATA["selected_table"] = None

        if should_re_render:
            window["-CURRENT-DB-"].update(DB_MANAGER.db.name)
            window["-TABLE-LIST-"].update(list(DB_MANAGER.db.tables))

            if LOCAL_DATA["selected_table"]:
                table = LOCAL_DATA["selected_table"]
                window["-TABLE-DATA-"].update(table.to_str())
            else:
                window["-TABLE-DATA-"].update("")
            window["-ERRORS-"].update("")

    except Exception as error:
        window["-ERRORS-"].update(error)


window.close()
