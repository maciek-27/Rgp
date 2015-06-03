#include "screenbuffer.h++"
#include <cstring>
#include <cstdlib>
#include <iostream>
using namespace Scr;

/////////////////////////////////////////////////
//
//   ScreenCharacter

ScreenCharacter::ScreenCharacter(Uint _c, const DisplayStyle & _style)
    :style(_style),c(_c){;}

ScreenCharacter & ScreenCharacter::operator=(const ScreenCharacter & other)
{
    c=other.c;
    style=other.style;
    return *this;
}

bool ScreenCharacter::operator==(const ScreenCharacter & other)
{
    return (c == other.c) && (style == other.style);
}

bool ScreenCharacter::operator!=(const ScreenCharacter & other)
{
    return (c != other.c) || (style != other.style);
}

//   ScreenRow
ScreenRow::ScreenRow(Uint width,
					 const ScreenCharacter & character)
    :characters(width,character){;}

void ScreenRow::Resize(Uint newWidth, const ScreenCharacter & character)
{
    characters.resize(newWidth,character);
}

ScreenRow & ScreenRow::operator=(const ScreenRow & other)
{
    characters=other.characters;
    return *this;
}

bool ScreenRow::operator==(const ScreenRow & other)
{    
    if (characters.size()!=other.characters.size())
	return false;
    std::vector<ScreenCharacter>::iterator i = characters.begin();
    std::vector<ScreenCharacter>::const_iterator j = other.characters.begin();
    for ( ; i!= characters.end(); i++,j++)
	if ( !((*i) == (*j)) )
	    return false;

    return true;
}

bool ScreenRow::operator!=(const ScreenRow & other)
{    
    if (characters.size()!=other.characters.size())
	return true;
    std::vector<ScreenCharacter>::iterator i = characters.begin();
    std::vector<ScreenCharacter>::const_iterator j = other.characters.begin();
    for ( ; i!= characters.end(); i++,j++)
	if ( !((*i) == (*j)) )
	    return true;

    return false;
}

Uint ScreenRow::GetWidth() const
{
    return characters.size();
}

////////////////////////////////////////////////////////////////////////////
//
//   ScreenBuffer

ScreenBuffer::ScreenBuffer(Uint _rows, Uint columns,
			   const ScreenCharacter & character)
    :rows(_rows,ScreenRow(columns,character)){;}

ScreenBuffer & ScreenBuffer::operator=(const ScreenBuffer & other)
{
    rows=other.rows;
    return *this;
}

bool ScreenBuffer::operator==(const ScreenBuffer & other)
{
    if (rows.size()!=other.rows.size())
	return false;
    std::vector<ScreenRow>::iterator i = rows.begin();
    std::vector<ScreenRow>::const_iterator j = other.rows.begin();
    for ( ; i!= rows.end(); i++,j++)
	if ( !((*i) == (*j)) )
	    return false;   
    return true;
}

bool ScreenBuffer::operator!=(const ScreenBuffer & other)
{
    if (rows.size()!=other.rows.size())
	return true;
    std::vector<ScreenRow>::iterator i = rows.begin();
    std::vector<ScreenRow>::const_iterator j = other.rows.begin();
    for ( ; i!= rows.end(); i++,j++)
	if ( !((*i) == (*j)) )
	    return true;   
    return false;
}

void ScreenBuffer::Resize(Uint newHeight,
			  Uint newWidth,
			  const ScreenCharacter & character)
{
    
    for (std::vector<ScreenRow>::iterator i = rows.begin();
	 i!= rows.end(); i++)
	i->Resize(newWidth,character);

    if (newHeight < GetHeight())
	rows.erase(rows.end()-(GetHeight()-newHeight),rows.end());
    else if (newHeight > GetHeight())
	rows.insert(rows.end(),newHeight-GetHeight(),
		    ScreenRow(newWidth,character));
}

Uint ScreenBuffer::GetWidth()const
{
    return rows[0].GetWidth();
}

Uint ScreenBuffer::GetHeight() const
{
    return rows.size();
}

void ScreenBuffer::Fill(const ScreenCharacter & character)
{
    rows[0].characters.assign(GetWidth(),character);
    rows.assign(GetHeight(),rows[0]);
}
