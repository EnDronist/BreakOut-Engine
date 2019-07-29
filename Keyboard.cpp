/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Keyboard.cpp																		  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "Keyboard.h"
bool Keyboard::KeyIsPressed( unsigned char keycode ) const{
	return keystates[keycode];
}
Keyboard::Event Keyboard::ReadKey() {
	if( keybuffer.size() > 0u ) {
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	else {
		return Keyboard::Event();
	}
}
bool Keyboard::KeyIsEmpty() const {
	return keybuffer.empty();
}
char Keyboard::ReadChar() {
	if ( charbuffer.size() > 0u ) {
		unsigned char charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	else {
		return 0;
	}
}
bool Keyboard::CharIsEmpty() const {
	return charbuffer.empty();
}
void Keyboard::FlushKey() {
	keybuffer = std::queue<Event>();
}
void Keyboard::FlushChar() {
	charbuffer = std::queue<char>();
}
void Keyboard::Flush() {
	FlushKey();
	FlushChar();
}
void Keyboard::EnableAutorepeat() {
	autorepeatEnabled = true;
}
void Keyboard::DisableAutorepeat() {
	autorepeatEnabled = false;
}
bool Keyboard::AutorepeatIsEnabled() const {
	return autorepeatEnabled;
}
void Keyboard::OnKeyPressed( unsigned char keycode ) {
	keystates[ keycode ] = true;	
	keybuffer.push( Keyboard::Event( Keyboard::Event::Press,keycode ) );
	TrimBuffer( keybuffer );
}
void Keyboard::OnKeyReleased( unsigned char keycode ) {
	keystates[ keycode ] = false;
	keybuffer.push( Keyboard::Event( Keyboard::Event::Release,keycode ) );
	TrimBuffer( keybuffer );
}
void Keyboard::OnChar( char character ) {
	charbuffer.push( character );
	TrimBuffer( charbuffer );
}
template<typename T>
void Keyboard::TrimBuffer( std::queue<T>& buffer ) {
	while( buffer.size() > bufferSize )
	{
		buffer.pop();
	}
}
void Keyboard::ToString(std::string &object_text) const {
	// std::bitset<nKeys> keystates
	object_text += std::to_string(keystates.to_string().size()); object_text += " ";
	for (int i = 0; i < keystates.to_string().size(); i++) {
		object_text += keystates[i]?'1':'0';
	}
}
void Keyboard::FromString(std::string &object_text) {
	int char_count;
	// std::bitset<nKeys> keystates
	char_count = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
	for (int i = 0; i < char_count; i++) {
		this->keystates[i] = ((bool)atoi(object_text.substr(i, 1).c_str()) == true) ? true : false;
	}
	object_text.erase(0, size_t(char_count));
}