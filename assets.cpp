#include "assets.h"
#include <stdexcept>


Assets::Assets() {
    load(bg, "bg.png");
    for (int s = 0; s < 4; ++s) load(t_cards[s], "cards" + std::to_string(s) + ".png");
    load(back, "back.png");
    load(skull, "skull.png");
    // compose the 52 sprites from textures
    cards.reserve(52);
    for (int s = 0; s < 4; ++s)
        for (int i = 0; i < 13; ++i)
            cards.emplace_back(t_cards[s], sf::IntRect({133 * i, 0}, {133, 200}));
    // font
    font = sf::Font(basePath / "DejaVuSans.ttf");
}


void Assets::load(sf::Texture &tex, const std::string &name) {
    if (basePath.empty()) {
        // try to discover where assets are
        for (const std::string p: {"./assets", "../assets", "/usr/share/games/scoundrel-solitaire", "/usr/share/scoundrel-solitaire"})
            if (fs::exists(fs::path(p) / name)) {
                basePath = p;
                break;
            }
    }
    if (basePath.empty() || !tex.loadFromFile(basePath / name)) throw std::runtime_error("Asset " + name + " not found");
}
