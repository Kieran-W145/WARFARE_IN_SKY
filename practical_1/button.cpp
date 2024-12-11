#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

class Button {
public:
    Button(const sf::Vector2f& position, const std::string& text, sf::Font& font) {
        button.setSize(sf::Vector2f(200.f, 50.f));  // Size of the button
        button.setPosition(position);
        button.setFillColor(sf::Color::Blue);  // Button color

        label.setFont(font);
        label.setString(text);
        label.setCharacterSize(24);
        label.setFillColor(sf::Color::White);
        sf::FloatRect textRect = label.getLocalBounds();
        label.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        label.setPosition(position.x + button.getSize().x / 2.0f, position.y + button.getSize().y / 2.0f);
    }

    void render(sf::RenderWindow& window) {
        window.draw(button);
        window.draw(label);
    }

    bool isClicked(const sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)) && sf::Mouse::isButtonPressed(sf::Mouse::Left);
    }

private:
    sf::RectangleShape button;
    sf::Text label;
};
