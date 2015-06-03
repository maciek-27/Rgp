//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2008 Damian Kaczmarek, Maciej Kaminski
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////

#ifndef __STYLESHEET_H__
#define __STYLESHEET_H__
#include <rexio/screen.h++>
#include <map>
#include <rexio/throw.h++>
namespace Scr
{
	namespace Tk
	{
		class Widget;

		//! CSS-like properties holder
		/*!
		  Stylesheet is a class which can hold different properties
		  for different classes. There are few value types supported.
		  It incomporates complete parser.
		*/
		class Stylesheet
		{			
		public:
			class Properties;
			struct Property;

			__DE(ParsingError, Exception);
			__DE(UnexpectedCharacter, ParsingError);
			__DE(BadValue, ParsingError);
			__DE(UnexpectedEndOfSheet, ParsingError);

		private:
			//! Type to bind class names to their properties
			typedef std::map<std::string, Properties *> ClassMap;
			//! Allows accessing properties of different classes
			ClassMap classes;

			/*!
			  \param valuestr unparsed value string
			  \return Property properly interpretted and converted valuestr

			  The function takes a crude string which is the following part
			  of CSS syntax:
			  \b property: \b value;
			  and converts it into the internal value holder.

			  \exception BadValue is throws if the valuestr cannot be parsed.
			  \exception Screen::InvalidUTF8 is thrown if an UTF-8 character
			  enclosed in single braces ' ' is not in correct UTF-8 format.
			*/
			Property ParseValue(const std::string& valuestr)
				throw(BadValue, Screen::InvalidUTF8);

		public:
			//! Type specifying Property value
			typedef enum {Style, Symbol, Number, String} PropertyType;

			//! Class holding multiple possible types of values.
			class Property {
			private:
//				Property();
				//! Current type.
				PropertyType type;
				union {
					//! Holding of DisplayStyle
					DisplayStyle *style;
					union {
						//! Holding of unicode character
						wchar_t symbol;
						//! Holding of integer
						Uint32 number;
					};
					//! Holding of string
					std::string *str;
				};				
			public:
				/*!
				  \param old				
				  Assign operator handling the allocated objects.
				*/
				const Property& operator=(const Property &old) {
					type = old.type;
					switch(type) {
					case Style:
						style = new DisplayStyle(*(old.style)); break;
					case String:
						str = new std::string(*(old.str)); break;
					case Symbol:
						symbol = old.symbol; break;
					case Number:
						number = old.number; break;
					}
					return *this;
				}
				/*!
				  \param old
				  Copy constructor handling the allocated objects.
				*/
				Property(const Property &old) {
					*this = old;
				}
				/*!
				  \param _style data to hold
				  Specialized constructor for holding DisplayStyle data.
				*/
				Property(const DisplayStyle& _style)
					:type(Style), style(new DisplayStyle(_style)) {;}
				/*!
				  \param _symbol data to hold
				  Specialized constructor for holding wchar_t data.
				*/
				Property( wchar_t _symbol) 
					:type(Symbol), symbol(_symbol) {;}
				/*!
				  \param _number data to hold
				  Specialized constructor for holding Uint32 data.
				*/
				Property( Uint32 _number) 
					:type(Number), number(_number) {;}
				/*!
				  \param _str data to hold
				  Specialized constructor for holding std::string data.
				*/
				Property(const std::string& _str) 
					:type(String), str(new std::string(_str)) {;}
				/*!
				  \return type of a Property
				*/
				PropertyType GetType() const throw() { return type;}

				__DE(WrongPropertyConversion, Exception);

				/*!
				  Autoconversion to DisplayStyle.
				*/
				operator DisplayStyle() const {
					if(type != Style) THROW(WrongPropertyConversion);					
					return DisplayStyle(*style);
				};
				/*!
				  Autoconversion to std::string..
				*/
				operator const std::string() const {
					if(type != String) THROW(WrongPropertyConversion);
					return *str;
				};
				/*!
				  Autoconversion to Uint32.
				*/
				operator Uint32() const {
					if(type != Number) THROW(WrongPropertyConversion);
					return number;
				};
				/*!
				  Autoconversion to wchar_t.
				*/
				operator wchar_t() const {
					if(type != Symbol) THROW(WrongPropertyConversion);
					return symbol;
				};

				/*!
				  Smart destructor, deleting type specific data.
				*/
				~Property() {
					if(type == Style)
						delete style;
					else if(type == String)
						delete str;
				}
			};
			class Properties {
			private:
				//! Type to bind different properties to their actual values.			   
				typedef std::map<std::string, Property *> PropertyMap;

				//! Allows accessing properties by property names
				PropertyMap properties;
			public:
				__DE(NoSuchProperty, Exception);
				
				/*!
				  \param propertyName

				  \exception NoSuchProperty is thrown if no such propertyName
				  has been defined.
				*/
				const Property& operator[](const std::string& propertyName)
					throw(NoSuchProperty);

				/*!
				  \param propertyName name
				  \param property value

				  Set the property value.
				*/
				void SetProperty(const std::string& propertyName,
								 const Property& property)throw();

				~Properties();

			};			
		   
		public:
			__DE(NoSuchClass, Exception);
			/*!
			  \param w widget to check
			  \param property
			  \return reference to found property			  
			  
			  Find certain property value for a widget.

			  \exception Properties::NoSuchProperty is thrown if no data
			  has been found.
			*/
			const Property& GetProperty(const Widget& w,
										const std::string& property)
				throw(Properties::NoSuchProperty);

			/*!
			  \param className
			  \param property
			  \param value
			  
			  Bind a certain vlaue to certain class's property.
			*/
			void SetProperty(const std::string& className,
							 const std::string& property,
							 const Property& value)throw();

			/*!
			  \param ss stream of CSS-like formatted data

			  Parses the specified buffer for later access.
			  
			  \exception ParsingError is thrown had the buffer was not
			  properly formatted.
			  \exception Screen::InvalidUTF8 is thrown if an UTF-8 character
			  enclosed in single braces ' ' is not in correct UTF-8 format.
			*/
			Stylesheet(std::istream &ss)throw(ParsingError, 
											  Screen::InvalidUTF8);

			~Stylesheet();
		};
	}
}

#endif // __STYLESHEET_H__
