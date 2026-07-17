#ifndef MAINGAME_H
#define MAINGAME_H


#include "assets.h"
#include "cards.h"
#include <SFML/Graphics.hpp>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <functional>


extern std::mutex guiMutexEvent, guiMutexDraw;
extern std::condition_variable guiCv;
extern std::optional<sf::Event> guiEvent;
extern std::vector<std::function<void()>> drawFunctions;  // to be managed by game loop


class MainGame {
public:
    MainGame(sf::RenderWindow&, Assets&);
    void run();
    bool isDone = false, mustQuit = false;  // to be checked by main thread
protected:
    // geometry information
    const sf::Vector2f posDeck{62, 60}, posRoom[4]{{318, 60}, {484, 60}, {650, 60}, {816, 60}},
        szCard{133, 200}, posAvoid{525, 280}, szAvoid{200, 50};
    const sf::FloatRect rectHealt{{64, 412}, {128, 128}};
    // geometry and data for dialog box
    const sf::Vector2f posDlg{250, 400}, szDlg{500, 340}, szBtn{130, 30},
        posBtn1{250, 700}, posBtn2{400, 700}, posCancel{550, 700};
    std::string dlgText, dlgBtn1, dlgBtn2;
    bool dlgCancel;
    // graphics objects
    sf::RenderWindow &window;
    sf::View view;
    Assets &assets;
    sf::Sprite spriteBg, spriteBack;
    sf::Text txtDeck, txtAvoid, txtHealth, txtDialog, txtBtn1, txtBtn2, txtCancel;
    sf::RectangleShape rectAvoid{szAvoid}, rectDlg{szDlg}, rectBtn1{szBtn}, rectBtn2{szBtn}, rectCancel{szBtn};
    //
    sf::Event waitEvent();  // automatically manages resize by calling onResize()
    enum class UserInput { Card, Avoid };
    std::pair<UserInput,int> getInput();
    void onResize(sf::Vector2u);
    void drawTable();
    void drawAvoid();
    void drawDialog();
    static void matchAspectRatio(sf::View &view, sf::Vector2u winSize);
    void center(sf::Text&, const sf::FloatRect&) const;
    void center(sf::Text&, const sf::Shape&) const;
    int currentCards() const;
    void setupDialog(std::string const &text, std::string const &btn1, std::string const &btn2, bool cancel);
    // game state
    Deck deck;
    int health = 20;
    std::optional<Card> room[4], weapon, lastMonster;
    bool avoidedLast = false;
};


#endif // MAINGAME_H
