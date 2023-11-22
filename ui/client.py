import PySimpleGUI as sg

from models import column
from models.db_manager import DBManager
from ui.views import (
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
    [sg.Button("Створити базу даних", **green_button), sg.Button("Відкрити базу даних"),
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


DB_MANAGER = DBManager()
LOCAL_DATA = {"selected_table": None}

window = sg.Window("Фрагментарна система управління табличними базами даних", layout)

while True:
    try:
        event, values = window.read()
        should_re_render = True
        print(event, values)
        if event == sg.WIN_CLOSED:
            break

        if event == "Створити базу даних":
            event, values = create_db_view()
            if event == "Створити":
                DB_MANAGER.create_database(values["-DB-NAME-"])

        elif event == "Створити таблицю":
            event, values = add_table_view()
            if event == "Створити":
                DB_MANAGER.add_table(values["-TABLE-NAME-"])
                LOCAL_DATA["selected_table"] = values["-TABLE-NAME-"]

        elif event == "Вилучити таблицю":
            event, values = delete_table_view()
            if event == "Вилучити":
                DB_MANAGER.delete_table(values["-TABLE-NAME-"])
                LOCAL_DATA["selected_table"] = None

        elif event == "Додати атрибут":
            event, values = add_column_view(
                column_choices=column.COLUMN_TYPE_CHOICES,
                table_name=LOCAL_DATA["selected_table"],
            )

            if event == "Додати":
                column_type = values["-COLUMN-TYPE-"]
                one_input_columns = {
                    "int": column.IntCol,
                    "real": column.RealCol,
                    "char": column.CharCol,
                    "string": column.StringCol,
                    "date": column.DateCol,
                    "date_interval": column.DateIntervalCol
                }

                new_column = one_input_columns[column_type](values["-COLUMN-NAME-"])
                DB_MANAGER.add_column(values["-TABLE-NAME-"], new_column)
                LOCAL_DATA["selected_table"] = values["-TABLE-NAME-"]

        elif event == "Вставити рядок":
            table = DB_MANAGER.get_table(LOCAL_DATA["selected_table"])
            event, values = add_row_view(
                table.columns[1:],
            )

            if event == "Вставити":
                for column_name in list(values):
                    correct_column_name = column_name.split(":")[0]
                    col = table.get_column_by_name(correct_column_name)
                    values[correct_column_name] = parse_value(
                        values.pop(column_name), col.type
                    )

                DB_MANAGER.add_row(table_name=table.name, data=values)

        elif event == "Вилучити рядок":
            event, values = delete_row_view()
            if event == "Вилучити":
                DB_MANAGER.delete_row(
                    LOCAL_DATA["selected_table"], int(values["-ROW-INDEX-"])
                )

        elif event == "Змінити рядок":
            table = DB_MANAGER.get_table(LOCAL_DATA["selected_table"])
            event, values = change_row_view()
            if event == "Змінити":
                index_to_change = int(values["-ROW-INDEX-"])
                change_event, change_values = change_row_detailed_view(
                    table.columns[1:],
                    table.get_row(index_to_change).values,
                )
                if change_event == "Змінити":
                    for column_name in list(change_values):
                        correct_column_name = column_name.split(":")[0]
                        col = table.get_column_by_name(correct_column_name)
                        change_values[correct_column_name] = parse_value(
                            change_values.pop(column_name), col.type
                        )

                    DB_MANAGER.change_row(
                        table_name=table.name, index=index_to_change, data=change_values
                    )

        elif event == "Сполучити таблиці":
            event, values = join_tables_view()
            if event == "Сполучити":
                view = DB_MANAGER.join_tables(
                    values["-FIRST-TABLE-NAME-"],
                    values["-SECOND-TABLE-NAME-"],
                    values["-FIELD-NAME-"]
                )
                window["-TABLE-DATA-"].update(str(view))
                should_re_render = False

        elif event == "-TABLE-LIST-":
            if values["-TABLE-LIST-"]:
                LOCAL_DATA["selected_table"] = values["-TABLE-LIST-"][0]

        elif event == "Зберегти базу даних":
            event, values = save_db_view()

            if event == "Зберегти":
                DB_MANAGER.save_database(values["-DB-SAVE-PATH-"])
                sg.popup("Базу даних успішно збережено!")

        elif event == "Відкрити базу даних":
            event, values = open_db_view()

            if event == "Відкрити":
                DB_MANAGER.open_database(values["-DB-OPEN-PATH-"])
                sg.popup("Базу даних успішно завантажено!")
                LOCAL_DATA["selected_table"] = None

        if should_re_render:
            window["-CURRENT-DB-"].update(DB_MANAGER.db.name)
            window["-TABLE-LIST-"].update(list(DB_MANAGER.db.tables))

            if LOCAL_DATA["selected_table"]:
                table = DB_MANAGER.get_table(LOCAL_DATA["selected_table"])
                window["-TABLE-DATA-"].update(str(table))
            else:
                window["-TABLE-DATA-"].update("")
            window["-ERRORS-"].update("")

    except Exception as error:
        window["-ERRORS-"].update(error)

window.close()
