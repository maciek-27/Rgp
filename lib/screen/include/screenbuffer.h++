#ifndef __SCREENBUFFER_H__
#define __SCREENBUFFER_H__

#include <vector>
#include "screen.h++"

namespace Scr
{
    
    class ScreenBuffer;
    //! \brief character to be displayed with all
    //! it's properties
    class ScreenCharacter
    {
    public:
		/*!
		  \param _c character UNICODE code
		  \param _style colour etc.
		*/
		ScreenCharacter(Uint _c, const DisplayStyle & _style);
		/*!
		  \param other right-hand operand
		  
		  Assignment operator copies  character and all it's properties
		*/
		ScreenCharacter & operator=(const ScreenCharacter & other);
		/*!
		  \param other right-hand operand
		  
		  Comparison operator returns true if colour and character match
		*/
		bool operator==(const ScreenCharacter & other);
		/*!
		  \param other right-hand operand
		*/
		bool operator!=(const ScreenCharacter & other);
		DisplayStyle style;
		Uint c;
    };

    //! \brief single row of ScreenBuffer object (which may
    //! contain more rows)
    /*!
      Class implements single row of characters, so it encapsulates std::vector.
	*/
    class ScreenRow
    {
    protected:
		std::vector<ScreenCharacter> characters;
		/*!
		  \param width number of characters
		  \param character initial content
		*/
		ScreenRow(Uint width, 
				  const ScreenCharacter & character=
				  ScreenCharacter(' ',
								  DisplayStyle(
									  Fg::White, Fg::Dark,Bg::Black)));

		/*!
		  \param newWidth new width of specific row.
		  \param character if new width is greater, than current,
		  additional fields will be filled with this specific character
		  \note declared as protected function to prevent changing
		  width outside of ScreenBuffer, and therefore to assure, that
		  buffer will be rectangular (equal width for each row).
		*/
		void Resize(Uint newWidth, const ScreenCharacter & character);

    public:
	
		/*!
		  \param other right-hand operand

		  copy content of one buffer to second one. If size differs,
		  target is resized to match source.
		*/
		ScreenRow & operator=(const ScreenRow & other);
		/*!
		  \param i index

		  Array element access operator returns reference to the
		  specific character.
		*/
		ScreenCharacter & operator[] (Uint i)
			{
				return characters[i];
			}
		/*!
		  \param other right-hand operand

		  Comparison for equal compares each character, and returns
		  true if no difference found
		*/
		bool operator==(const ScreenRow & other);
		/*!
		  \param other right-hand operand

		  Comparison for equal compares each character, and returns
		  true if any difference found
		*/
		bool operator!=(const ScreenRow & other);
		Uint GetWidth()const; // get number of columns in row
		friend class ScreenBuffer;
    };

    //! \brief buffer of characters, supporting colours
    //! and unicode.
    /*!
      Class represents character buffer.
	*/
    class ScreenBuffer
    {
    protected:
		typedef std::vector<ScreenRow> RowVector;
    private:
		RowVector rows;
    public:
		/*!
		  \param  _rows initial height of screen buffer
		  \param columns initial width of screen buffer
		  \param character initial fill of screen buffer (by default
		  plain black background (filled with space))
  
		  \note buffer size may be changed runtime.
		*/
		ScreenBuffer(Uint _rows, Uint columns,
					 const ScreenCharacter & character=
					 ScreenCharacter(' ',
									 DisplayStyle(
										 Fg::White, Fg::Dark,Bg::Black)));
		/*!
		  \param _i row number (0..height-1)
		  \return reference to specific row
		  \note no range checking, and no exception-connected
		  warranties for this function. 
		*/
		ScreenRow & operator[] (Uint _i)
			{
				return rows[_i];
			}

		/*!
		  \param other right-hand operand

		  Assign other screen to this one. Function copies whole
		  contents, so complexity is O(width*height).
		*/
		ScreenBuffer & operator=(const ScreenBuffer & other);

		/*!
		  \param other right-hand operand

		  \return true if size of each buffer is equal, each character
		  equals  its counterpart on second buffer, both in terms of
		  unicode value and colour.
		*/
		bool operator==(const ScreenBuffer & other);

		/*!
		  \param other right-hand operand

		  \return true if any difference occours between two screens.
		*/
		bool operator!=(const ScreenBuffer & other);
		/*!
		  \param newHeight new height of screen buffer
		  \param newWidth new width of screen buffer
		  \param character character, to fill new rows or colums (if
		  their number grows) with.
		*/
		void Resize(Uint newHeight,
					Uint newWidth,
					const ScreenCharacter & character=
					ScreenCharacter(' ',
									DisplayStyle(
										Fg::White, Fg::Dark,Bg::Black)));

		/*!
		  \return current height of buffer (number of rows)
		*/
		Uint GetHeight() const; // get number of rows
		/*!
		  \return current width of buffer (number of characters in each row)
		*/
		Uint GetWidth() const; // get number of columns in row

		/*!
		  \param character character

		  Function fills whole buffer with specific character.
		*/
		void Fill(const ScreenCharacter & character);//assign new
													 //content

    };    
}

#endif // __SCREENBUFFER_H__
