#include "Position.h"

Position::Position(int r, char c) {
    row = r;
    column = c;
}

int Position::get_row() {
    return row;
}

char Position::get_column() {
    return column;
}
