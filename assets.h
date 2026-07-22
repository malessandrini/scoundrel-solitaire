#ifndef ASSETS_H
#define ASSETS_H

#include <SFML/Graphics.hpp>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;


// convenience classes adding constructors to SFML classes

class MyText: public sf::Text {
public:
    MyText(const sf::Font &font, sf::String str, unsigned int charSize, sf::Color color):
        sf::Text(font, str, charSize)
    {   setFillColor(color); }
};


class MyRectangleShape: public sf::RectangleShape {
public:
    MyRectangleShape(sf::Vector2f size, sf::Color color, sf::Vector2f pos = {}):
        sf::RectangleShape(size)
    { setFillColor(color);  setPosition(pos); }
};


class MySprite: public sf::Sprite {
    static sf::Texture dummyTexture;
public:
    MySprite(): sf::Sprite(dummyTexture) {}
    using sf::Sprite::Sprite;
};


// Asset loader


class Assets {
public:
    Assets();
    MySprite bg, back, skull;
    std::vector<sf::Sprite> cards;  // 52 sprites
    sf::Font font;
protected:
    void load(sf::Texture&, std::string const&);
    fs::path basePath;
    sf::Texture t_bg, t_cards[4], t_back, t_skull;  // set of cards, one suit for image
};


#endif // ASSETS_H
