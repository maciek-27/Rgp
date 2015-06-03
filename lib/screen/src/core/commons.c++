#include "commons.h++"

using namespace Scr;

Vector::Vector(Sint _rows, Sint _cols)
	:rows(_rows), cols(_cols){;}

Position::Position(Uint _row, Uint _col)
    :row(_row), col(_col){;}

Position Position::operator+(const Position& pos)
{
	return Position(row + pos.row, col + pos.col);
}

Position Position::operator+(const Size& size)
{
	return Position(row + size.height, col + size.width);
}

Position Position::operator+(const Vector& vec)
{
	return Position(row + vec.rows, col + vec.cols);
}

Position& Position::operator+=(const Position& pos)
{
	row += pos.row;
	col += pos.col;
	return *this;
}

Position& Position::operator+=(const Size& size)
{
	row += size.height;
	col += size.width;
	return *this;
}

Position& Position::operator+=(const Vector& vec)
{
	row += vec.rows;
	col += vec.cols;
	return *this;
}

Position Position::operator-(const Position &pos)
{
	return Position(row - pos.row, col - pos.col);
}

Position Position::operator-(const Size& size)
{
	return Position(row - size.height, col - size.width);
}

Position Position::operator-(const Vector& vec)
{
	return Position(row - vec.rows, col - vec.cols);
}

Position& Position::operator-=(const Position& pos)
{
	row -= pos.row;
	col -= pos.col;
	return *this;
}

Position& Position::operator-=(const Size& size)
{
	row -= size.height;
	col -= size.width;
	return *this;
}

Position& Position::operator-=(const Vector& vec)
{
	row -= vec.rows;
	col -= vec.cols;
	return *this;
}

Size::Size(Uint _height, Uint _width)
    :height(_height),width(_width){;}

