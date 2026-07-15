#ifndef ASSETS_H
#define ASSETS_H

#include <SFML/Graphics.hpp>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;


class Assets {
public:
    Assets();
    // resources
    sf::Texture bg;  // background
    std::vector<sf::Sprite> cards;  // 52 sprites
protected:
    void load(sf::Texture&, std::string const&);
    fs::path basePath;
    sf::Texture t_cards[4];  // set of cards, one suit for image
};


#endif // ASSETS_H
