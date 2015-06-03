#include "stylesheet.h++"
#include "widget.h++"
#include "../../screen/include/utf8.h++"
#include <cstring>
#include <cstdio>
#include <cctype>
#include<cstdlib>
using namespace Scr;
using namespace Scr::Tk;

const Stylesheet::Property&
Stylesheet::Properties::operator[](const std::string &propertyName)
	throw(NoSuchProperty)
{
	Property *tmp = properties[propertyName];
	if(!tmp) {
		THROW(NoSuchProperty);
	}
	return *tmp;
}

void Stylesheet::Properties::SetProperty(const std::string& propertyName,
										 const Property& property)throw()
{
	Property *tmp = properties[propertyName];
	if(!tmp) {
		tmp = new Property(property);
		properties[propertyName] = tmp;
	}
	else
		*tmp = property;
}

Stylesheet::Properties::~Properties() {
	PropertyMap::iterator i = properties.begin();
	while(i != properties.end()) {
		delete (*i).second;
		i++;
	}
}

static std::string num2str(Uint num)throw()
{
	std::stringstream strstream;
	strstream << num;
	return strstream.str();
}

Stylesheet::Property
Stylesheet::ParseValue(const std::string& valuestr)
	throw(BadValue, Screen::InvalidUTF8)
{
	bool trimwhite = true;
	for(Uint i = 0;i< valuestr.length();i++) {
		if(trimwhite && isspace(valuestr[i]))
			continue;
		if(valuestr[i] == '"') {
			for(int j = valuestr.length();j--;) {
				if(valuestr[j] == '"') {
					if((Uint)j == i)
						THROW(BadValue);
					return Property(valuestr.substr(i+1, j - i - 1));					
				}
			}
		}
		if(valuestr[i] == '\'') {
			i++;
			const char *str = (valuestr.c_str()) + i;
			Uint len = CharLengthUTF8(str);
			if(valuestr[i + len] != '\'')
				THROW(BadValue);
			
			return Property(DecodeUTF8(&str));
		}
		if(isdigit(valuestr[i]) || valuestr[i] == '-') {
			int number;
			sscanf(valuestr.c_str() + i, "%i", &number);
			return Property(static_cast<Uint32>(atoi(valuestr.c_str())));
		}
		else if(isalpha(valuestr[i])) {
			
			Fg::Color fg;
			Fg::Style style;
			Bg::Color bg;

#define COLOR(name, target, ns) \
(strncasecmp(valuestr.c_str() + i, #name, strlen(#name)) == 0) \
	target = ns::name, i+=strlen(#name)
			if COLOR(System, fg, Fg);
			else if COLOR(Transparent, fg, Fg);
			else if COLOR(Black, fg, Fg);
			else if COLOR(Red, fg, Fg);
			else if COLOR(Green, fg, Fg);
			else if COLOR(Yellow, fg, Fg);
			else if COLOR(Blue, fg, Fg);
			else if COLOR(Magenta, fg, Fg);
			else if COLOR(Cyan, fg, Fg);
			else if COLOR(White, fg, Fg);
			else THROW(BadValue);
			
			if(i++ == valuestr.length())
			   THROW(BadValue);
			
			if COLOR(Bright, style, Fg);
			else if COLOR(Dark, style, Fg);
			else THROW(BadValue);
			
			if(i++ == valuestr.length())
				THROW(BadValue);

			if COLOR(System, bg, Bg);
			else if COLOR(Transparent, bg, Bg);
			else if COLOR(Black, bg, Bg);
			else if COLOR(Red, bg, Bg);
			else if COLOR(Green, bg, Bg);
			else if COLOR(Yellow, bg, Bg);
			else if COLOR(Blue, bg, Bg);
			else if COLOR(Magenta, bg, Bg);
			else if COLOR(Cyan, bg, Bg);
			else if COLOR(White, bg, Bg);
			else THROW(BadValue);
			
			return Property(DisplayStyle(fg, style, bg));
		}
	}
	THROW(BadValue);
}

Stylesheet::Stylesheet(std::istream &ss)
	throw(ParsingError, Screen::InvalidUTF8)
{
//	std::istringstream ss(str);

	Uint linecnt = 0;
	std::string line;	

	typedef enum {Out, ReadClass, WaitBlock, ReadBlock, Comment} ParseState;
	ParseState state = Out;
	ParseState prevstate;
	typedef enum {WaitProperty, ReadProperty, WaitValue, ReadValue} BlockState;
	BlockState substate = WaitProperty;
	BlockState prevsubstate;

	
#define COMMENT_CHECK \
if(line[i] == '/') { \
	i++; \
	if(i < line.length()) { \
		if(line[i] == '/') { \
			goto endofline; \
		} \
		else if(line[i] == '*') { \
			i++; \
			prevstate = state; \
			prevsubstate = substate; \
			state = Comment; \
			goto reread; \
		} \
		else \
			THROWP(UnexpectedCharacter, \
				   ", " + num2str(linecnt) + " " + num2str(i)); \
	} \
    i--; \
}

	std::string className;
	std::string propertyName;
	std::string propertyValue;
	bool colonfound = false;

	while (std::getline(ss, line)) {
		Uint i;
		linecnt++;
		for(i = 0;i<line.length();i++) {
		reread:

			if(state == Out) {
				for(;i<line.length();i++) {					
					if(isspace(line[i]))
						continue;
					else if(isalnum(line[i]) || line[i] == '_') {
						state = ReadClass;
						goto reread;
					}
					else 
						COMMENT_CHECK;
					THROWP(UnexpectedCharacter, 
					", " + num2str(linecnt) + ":" + num2str(i)
						+ " Unexpected character in target specifier.");
				}
			}
			else if(state == ReadClass) {
				for(;i<line.length();i++) {
					if(isalnum(line[i]) || line[i] == '_' || line[i] == '#'){
						className += line[i];
						continue;
					}
					else if(isspace(line[i])) {
						state = WaitBlock;
						goto reread;
					}
					else if(line[i] == '{') {
						state = ReadBlock;
						i++;
						goto reread;
					}
					else
						COMMENT_CHECK;
					THROWP(UnexpectedCharacter,
						   ", " + num2str(linecnt) + ":" + num2str(i)
						   + " Unexpected character in target specifier.");
				}
			}
			else if(state == WaitBlock) {
				for(;i<line.length();i++) {

					if(isspace(line[i]))
						continue;
					else if(line[i] == '{') {
						state = ReadBlock;
						i++;
						goto reread;
					}
					else
						COMMENT_CHECK;
					THROWP(UnexpectedCharacter,
						   ", " + num2str(linecnt) + ":" + num2str(i)
						   + " Unexpected character after target specifier.");
				}
			}
			else if(state == ReadBlock) {
				bool inQuote = false;

				for(;i<line.length();i++) {
					
					if(!inQuote)
						COMMENT_CHECK;

					if(line[i] == '}') {
						className = "";
						i++;
						state = Out;
						if(substate != WaitProperty)
							THROWP(UnexpectedCharacter,
								   ", " + num2str(linecnt) + ":" + num2str(i)
								   + " Unexpected end of block(forgot a semicolon?).");
						colonfound = false;
						propertyName = "";
						propertyValue = "";
						goto reread;
					}
					if(substate == WaitProperty) {
						if(isspace(line[i]))
							continue;
						else if(isalnum(line[i]) || line[i] == '_')
							substate = ReadProperty;
						else
							THROWP(UnexpectedCharacter,
								   ", " + num2str(linecnt) + ":" + num2str(i)
								   + " Unexpected character while expecting property.");
							
					}
					if(substate == ReadProperty) {
						if(isalnum(line[i]) || line[i] == '_')
							propertyName += line[i];
						else if(isspace(line[i]) || line[i] == ':') {
							substate = WaitValue;
							colonfound = false;
						}
						else
							THROWP(UnexpectedCharacter,
								   ", " + num2str(linecnt) + ":" + num2str(i)
								   + " Unexpected character while reading property.");
					}
					if(substate == WaitValue) {
						if(isspace(line[i]))
							continue;
						else if(line[i] == ':' && !colonfound)
							colonfound = true;
						else if(colonfound &&
								isprint(line[i]))
							substate = ReadValue;
						else
							THROWP(UnexpectedCharacter,
								   ", " + num2str(linecnt) + ":" + num2str(i)
								   + " Unexpected character while expecting value.");
					}
					if(substate == ReadValue) {
						if(line[i] == '"')
							inQuote = !inQuote;

						if(line[i] == ';' && !inQuote) {
							substate = WaitProperty;
							
							SetProperty(className, propertyName,
										ParseValue(propertyValue));
							
							propertyName = "";
							propertyValue = "";
							colonfound = false;
							inQuote = false;
						}
						else
							propertyValue += line[i];						
					}

				}
			}
			else if(state == Comment) {
				for(;i<line.length();i++) {
					if(line[i] == '*') {
						i++;
						if(i < line.length()) {
							if(line[i] == '/') {
								state = prevstate;
								substate = prevsubstate;
								i++;
								goto reread;
							}
						}
					}
				}
			}
		}
	endofline:;
	}
	if(state != Out)
		THROW(UnexpectedEndOfSheet);
}

const Stylesheet::Property& Stylesheet::GetProperty(const Widget& w,
										 const std::string& property)
	throw(Properties::NoSuchProperty)
{
	Properties* tmp;
	
	const Widget::ClassHierarchy &cs = const_cast<Widget &>(w).Hierarchy();
	
	// walk through the whole class hierarchy
	for(Uint i = 0;i < cs.size();i++) {
		if(w.objectName != "") {
			tmp = classes[cs[i] + "#" + w.objectName];
			if(tmp) {
				try {
					return (*tmp)[property];
				}
				catch(Properties::NoSuchProperty) {
					;
				}
			}
		}
		tmp = classes[cs[i]];
		if(tmp) {
			try { 
				return (*tmp)[property];
			}
			catch(Properties::NoSuchProperty) {
				continue;
			}
		}
	}
	THROW(Properties::NoSuchProperty);
}

void Stylesheet::SetProperty(const std::string& className,
							 const std::string& property,
							 const Property& value) throw()
{
	Properties *tmp = classes[className];
	if(!tmp) {
		tmp = new Properties();
		classes[className] = tmp;
		
	}
	tmp->SetProperty(property, value);
}

Stylesheet::~Stylesheet() {
	ClassMap::iterator i = classes.begin();
	while(i != classes.end()) {
		delete (*i).second;
		i++;
	}
}
