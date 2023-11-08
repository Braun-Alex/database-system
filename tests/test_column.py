from models.column import Column, IntCol, CharCol, StringCol


def test_column_default_validation():
    class TestColumn(Column):
        @staticmethod
        def validate(value):
            return value == "correct"

    col = TestColumn("test_type", "test_name", "correct")
    assert col.default == "correct"


def test_int_col_validation():
    col = IntCol("amount")
    assert col.validate(10)
    assert not col.validate("asdf")


def test_char_col_validation():
    col = CharCol("class")
    assert col.validate("A")
    assert not col.validate("BCD")


def test_string_col_validation():
    col = StringCol("name")
    assert col.validate("John Snow")
    assert not col.validate(True)
