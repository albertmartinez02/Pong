#include <SFML/Graphics.hpp>
class MenuItem {

public:
	MenuItem(sf::Font, sf::Text);

	sf::Font getFont() const;

	sf::Text getText() const;

private:
	sf::Font font;

	sf::Text text;
};