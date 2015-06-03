#include <iostream>
#include"screen.h++"
#include"screenbase.h++"
#include"subscreen.h++"

using namespace Scr;

Scr::ScreenBase::ScreenBase()
    :Screen(),aPoint(0,0)
{}

Uint Scr::ScreenBase::GetY()const throw()
{
    return aPoint.row;
}
Uint Scr::ScreenBase::GetX()const throw()
{
    return aPoint.col;
}

void Scr::ScreenBase::AddText(const std::string & text, Uint cols)         
			throw(PrintOutOfRange, IllegalCharacter)
{
	AddText(text.c_str(),cols);
}
