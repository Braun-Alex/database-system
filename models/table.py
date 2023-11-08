from __future__ import annotations
from Pyro5.api import expose

from typing import Any

from tabulate import tabulate

from .column import Column
from .row import Row


def remove_duplicates(lst):
    unique_list = []
    for item in lst:
        if item not in unique_list:
            unique_list.append(item)
    return unique_list


@expose
class Table:
    def __init__(self, name: str) -> None:
        self._name = name
        self._columns: list[Column] = []
        self._rows: list[Row] = []

    @property
    def name(self):
        return self._name

    @property
    def rows(self):
        return [[index] + row.values for index, row in enumerate(self._rows)]

    @property
    def columns(self):
        return ("index: int",) + tuple(
            f"{column.name}: {column.type}" for column in self._columns
        )

    @property
    def rows_count(self):
        return len(self._rows)

    @property
    def columns_count(self):
        return len(self._columns)

    def __str__(self):
        return f"Таблиця: {self.name}\n" + self._str_columns_and_rows()

    def to_str(self):
        return str(self)

    def _str_columns_and_rows(self) -> str:
        if len(self._columns) == 0:
            return ""

        return tabulate(
            self.rows,
            self.columns,
            tablefmt="orgtbl",
        )

    def _get_column_names(self) -> tuple[str]:
        return tuple(column.name for column in self._columns)

    def _check_column_name_already_exists(self, new_column_name: str) -> bool:
        return new_column_name in self._get_column_names()

    def add_column(self, column: Column) -> Column:
        if self._check_column_name_already_exists(column.name):
            raise ValueError(
                f"Атрибут з ім'ям '{column.name}' вже існує у таблиці!"
            )

        self._columns.append(column)
        self._add_default_values_to_all_existing_rows(column)
        return column

    def _add_default_values_to_all_existing_rows(self, column: Column) -> None:
        for row in self._rows:
            row.values.append(column.default)

    def _validate_row_data(self, data: dict[str, Any]) -> None:
        if len(data) == 0:
            raise ValueError("Рядок таблиці не може бути порожнім!")

        columns_names = self._get_column_names()
        if not set(data.keys()).issubset(set(columns_names)):
            raise ValueError(
                f"Некоректні імена атрибутів: {tuple(data.keys())} не є підмножиною {columns_names}!"
            )

    def add_row(self, data: dict[str, Any]) -> Row:
        self._validate_row_data(data)

        row = []

        for column in self._columns:
            value_to_add = data.get(column.name, None)

            if not value_to_add:
                row.append(column.default)
                continue

            column.validate_or_error(value_to_add)

            row.append(value_to_add)

        created_row = Row(row)
        self._rows.append(created_row)
        return created_row

    def get_row(self, index: int) -> Row:
        if not (0 <= index < len(self._rows)):
            raise IndexError(f"Рядок з індексом '{index}' не існує!")

        return self._rows[index]

    def get_column_by_name(self, name: str) -> Column:
        return next(column for column in self._columns if column.name == name)

    def change_row(self, index: int, data: dict) -> None:
        row = self.get_row(index)
        self._validate_row_data(data)

        for column_name, new_column_value in data.items():
            column = self.get_column_by_name(column_name)
            column.validate_or_error(new_column_value)

        for column_name, new_column_value in data.items():
            column_index = self._get_column_names().index(column_name)
            row[column_index] = new_column_value

    def delete_row(self, index: int) -> Row:
        """Delete row by index, and return it"""
        row = self.get_row(index)
        del self._rows[index]
        return row

    def join_tables(self, other: Table, field_name: str) -> Table:
        if field_name not in self._get_column_names():
            raise ValueError(f"Поле '{field_name}' не існує в таблиці '{self.name}'!")
        if field_name not in other._get_column_names():
            raise ValueError(f"Поле '{field_name}' не існує в таблиці '{other.name}'!")

        joined_table = Table(f"{self.name}_joined_with_{other.name}")

        for column in self._columns + other._columns:
            if not joined_table._check_column_name_already_exists(column.name):
                joined_table.add_column(column)

        for self_row in self._rows:
            self_value = self_row.values[self._get_column_names().index(field_name)]

            for other_row in other._rows:
                other_value = other_row.values[other._get_column_names().index(field_name)]

                if self_value == other_value:
                    joined_values = remove_duplicates(self_row.values + other_row.values)
                    joined_table.add_row(dict(zip(joined_table._get_column_names(), joined_values)))

        return joined_table
