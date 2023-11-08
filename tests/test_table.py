import pytest

from models.column import IntCol, StringCol, CharCol
from models.table import Table


def test_add_row():
    table = Table("test")

    assert table.rows_count == 0

    with pytest.raises(ValueError):
        table.add_row({"amount": 1})

    table.add_column(IntCol("amount"))
    table.add_row({"amount": 10})
    assert table.get_row(0).values == [10]

    with pytest.raises(TypeError):
        table.add_row({"amount": "hello"})


def test_join_tables():
    first_table = Table("vegetables")
    first_table.add_column(StringCol("owner"))
    first_table.add_column(StringCol("vegetable_name"))
    first_table.add_column(IntCol("count_of_vegetables"))

    first_table.add_row({"owner": "alex", "vegetable_name": "carrot", "count_of_vegetables": 3})

    second_table = Table("fruits")
    second_table.add_column(StringCol("owner"))
    second_table.add_column(StringCol("fruit_name"))
    second_table.add_column(IntCol("count_of_fruits"))

    second_table.add_row({"owner": "alex", "fruit_name": "orange", "count_of_fruits": 29})

    view = first_table.join_tables(second_table, "owner")

    assert view.columns_count == 5
    assert view.rows_count == 1


def test_delete_row():
    table = Table("test")
    table.add_column(IntCol("amount"))

    table.add_row({"amount": 10})
    table.add_row({"amount": 20})
    table.add_row({"amount": 30})

    assert table.rows_count == 3

    table.delete_row(1)

    assert table.rows_count == 2
    assert table.get_row(0).values[0] == 10
    assert table.get_row(1).values[0] == 30

    with pytest.raises(IndexError):
        table.delete_row(10)
