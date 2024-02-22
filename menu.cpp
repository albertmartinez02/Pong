#include "menu.h"

MenuItem::MenuItem(sf::Font f, sf::Text t) {
	font = f;
	text = t;
}

sf::Font MenuItem::getFont() const{
	return font;
}

sf::Text MenuItem::getText() const {
	return text;
}