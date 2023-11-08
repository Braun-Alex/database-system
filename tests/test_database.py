import pytest

from models.database import Database


def test_add_table():
    db = Database("test_db")
    db.add_table("test_table")
    assert db.tables_count == 1

    db.add_table("another_table")
    assert db.tables_count == 2


def test_get_table():
    db = Database("test_db")
    table_name = "test_table"

    db.add_table(table_name)
    assert db.get_table(table_name).name == table_name


def test_delete_table():
    db = Database("test_db")
    table_name = "test_table"
    db.add_table(table_name)
    assert db.tables_count == 1

    db.delete_table(table_name)
    assert db.tables_count == 0
    with pytest.raises(KeyError):
        db.get_table(table_name)
