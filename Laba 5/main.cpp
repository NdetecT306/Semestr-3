#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <locale>
#include <codecvt>
#include <functional>
using namespace std;
using namespace sf;
class IGameController {// Интерфейс для получателей команд
public:
    virtual ~IGameController() = default;
    virtual void startNewRound(int playerChoice) = 0;
    virtual void playerMakeChoice(int playerIndex, int choice) = 0;
    virtual void restartGame() = 0;
    virtual void prepareForDuel(int participant1, int participant2) = 0;
};
class GameCommand {
public:
    virtual ~GameCommand() = default;
    virtual void execute() = 0;
};
class StartNewRoundCommand : public GameCommand {
private:
    IGameController* controller;
    int playerChoice;
public:
    StartNewRoundCommand(IGameController* controller, int choice) : controller(controller), playerChoice(choice) {}
    void execute() override {
        if (controller) controller->startNewRound(playerChoice);
    }
};
class PlayerChoiceCommand : public GameCommand {
private:
    IGameController* controller;
    int playerIndex;
    int choice;
public:
    PlayerChoiceCommand(IGameController* controller, int playerIndex, int choice) : controller(controller), playerIndex(playerIndex), choice(choice) {}
    void execute() override {
        if (controller) controller->playerMakeChoice(playerIndex, choice);
    }
};
class RestartGameCommand : public GameCommand {
private:
    IGameController* controller;
public:
    RestartGameCommand(IGameController* controller) : controller(controller) {}
    void execute() override {
        if (controller) controller->restartGame();
    }
};
class PrepareDuelCommand : public GameCommand {
private:
    IGameController* controller;
    int participant1;
    int participant2;
public:
    PrepareDuelCommand(IGameController* controller, int p1, int p2) : controller(controller), participant1(p1), participant2(p2) {}
    void execute() override {
        if (controller) controller->prepareForDuel(participant1, participant2);
    }
};
class GameCommandInvoker { //Управляет историей команд и их выполнением
private:
    vector<unique_ptr<GameCommand>> commandHistory;
    size_t currentIndex;
public:
    GameCommandInvoker() : currentIndex(0) {}
    void executeCommand(unique_ptr<GameCommand> command) {
        if (currentIndex < commandHistory.size()) {
            commandHistory.resize(currentIndex);
        }
        command->execute();
        commandHistory.push_back(std::move(command));
        currentIndex = commandHistory.size();
    }
    void redo() {
        if (currentIndex < commandHistory.size()) {
            commandHistory[currentIndex]->execute();
            currentIndex++;
        }
    }
    void clearHistory() {
        commandHistory.clear();
        currentIndex = 0;
    }
};
// Игровой элемент глобально
class GameElement {
protected:
    string name; // Элемент
    string textureFile; // .jpg
    vector<int> beatsInd; // Кого побеждает 
    vector<int> losesToInd;// Кому проигрывает
public:
    GameElement(const string& name, const string& textureFile, const vector<int>& beats, const vector<int>& losesTo)
        : name(name), textureFile(textureFile), beatsInd(beats), losesToInd(losesTo) {}
    virtual ~GameElement() = default;
    virtual unique_ptr<class Card> createCard(const Vector2f& position, const Vector2f& size, const Color& bgColor) = 0;
    string getName() const { return name; }
    string getTextureFile() const { return textureFile; }
    bool beats(int elementIndex) const { // Победа
        for (int ind : beatsInd) {
            if (ind == elementIndex) return true;
        }
        return false;
    }
    bool losesTo(int elementIndex) const { // Проигрыш
        for (int ind : losesToInd) {
            if (ind == elementIndex) return true;
        }
        return false;
    }
};
// Карточка
class Card {
private:
    RectangleShape shape;
    Sprite sprite;
    Texture texture;
    bool isThrown;
    Vector2f origPos;
    Vector2f targetPos;
    Vector2f origSize;
    Vector2f targetSize;
    float animationSpeed;
    bool isHovered;
    int elementInd;
    string elementName;
    string textureFile;
    bool textureLoaded;
public:
    Card(const Vector2f& position, const Vector2f& size, const Color& bgColor, int elemIndex, const string& name, const string& texFile) 
        : isThrown(false), animationSpeed(8.0f), isHovered(false), elementInd(elemIndex), elementName(name), textureFile(texFile), 
          textureLoaded(false) {
        shape = RectangleShape(size);
        shape.setFillColor(bgColor);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(Color::Black);
        shape.setPosition(position);
        origPos = position;
        origSize = size;
        targetPos = position;
        targetSize = size;
    }
    bool loadTexture() {
        if (textureLoaded) {
            return true;
        }
        if (texture.loadFromFile(textureFile)) {
            sprite.setTexture(texture, true);
            textureLoaded = true;
            updateSpritePosition();
            return true;
        }
        vector<string> paths = {"./" + textureFile};
        for (const auto& path : paths) {
            if (texture.loadFromFile(path)) {
                sprite.setTexture(texture, true);
                textureLoaded = true;
                updateSpritePosition();
                return true;
            }
        }
        return false;
    }
    void createDefaultTexture() {
        if (textureLoaded) return;
        updateSpritePosition();
    }
    void initializeTexture() {
        if (!loadTexture()) {
            createDefaultTexture();
        }
    }
    void throwTo(const Vector2f& position, const Vector2f& size) {
        isThrown = true;
        targetPos = position;
        targetSize = size;
    }
    void update(float deltaTime) {
        Vector2f currentPos = shape.getPosition();
        Vector2f currentSize = shape.getSize();
        Vector2f newPos = currentPos + (targetPos - currentPos) * animationSpeed * deltaTime;
        Vector2f newSize = currentSize + (targetSize - currentSize) * animationSpeed * deltaTime;
        shape.setPosition(newPos);
        shape.setSize(newSize);
        updateSpritePosition();
    }
    void updateSpritePosition() {
        if (!textureLoaded) return;
        FloatRect spriteBounds = sprite.getLocalBounds();
        Vector2f shapeSize = shape.getSize();
        float scale;
        if (isThrown) {
            scale = min((shapeSize.x - 20) / spriteBounds.width, (shapeSize.y - 40) / spriteBounds.height);
        } else {
            scale = min((shapeSize.x - 20) / spriteBounds.width, (shapeSize.y - 40) / spriteBounds.height);
        }
        sprite.setScale(scale, scale);
        FloatRect scaledBounds = sprite.getGlobalBounds();
        sprite.setPosition(
            shape.getPosition().x + (shapeSize.x - scaledBounds.width) / 2,
            shape.getPosition().y + (shapeSize.y - scaledBounds.height) / 2
        );
    }
    void setHovered(bool hovered) {
        isHovered = hovered;
        if (!isThrown) {
            targetPos.y = hovered ? origPos.y - 20 : origPos.y;
        }
    }
    bool contains(const Vector2f& point) const {
        return shape.getGlobalBounds().contains(point);
    }
    void draw(RenderWindow& window) {
        window.draw(shape);
        if (textureLoaded && sprite.getTexture()) {
            window.draw(sprite);
        }
    }
    void reset() {
        isThrown = false;
        targetPos = origPos;
        targetSize = origSize;
    }
    void resetToOriginalPosition() {
        isThrown = false;
        shape.setPosition(origPos);
        shape.setSize(origSize);
        targetPos = origPos;
        targetSize = origSize;
        updateSpritePosition();
    }
    RectangleShape& getShape() { return shape; }
    bool isCardThrown() const { return isThrown; }
    Vector2f getPosition() const { return shape.getPosition(); }
    Vector2f getSize() const { return shape.getSize(); }
    int getElementIndex() const { return elementInd; }
    string getElementName() const { return elementName; }
    bool isTextureLoaded() const { return textureLoaded; }
    Vector2f getOriginalPosition() const { return origPos; }
    Vector2f getOriginalSize() const { return origSize; }
};
// Наши объекты
class Rock : public GameElement {
public:
    Rock() : GameElement("Камень", "rock.jpg", {1, 3}, {2, 4}) {}
    unique_ptr<Card> createCard(const Vector2f& position, const Vector2f& size, const Color& bgColor) override {
        return make_unique<Card>(position, size, bgColor, 0, "Камень", "rock.jpg");
    }
};
class Scissors : public GameElement {
public:
    Scissors() : GameElement("Ножницы", "scissors.jpg", {2, 3}, {0, 4}) {}
    unique_ptr<Card> createCard(const Vector2f& position, const Vector2f& size, const Color& bgColor) override {
        return make_unique<Card>(position, size, bgColor, 1, "Ножницы", "scissors.jpg");
    }
};
class Paper : public GameElement {
public:
    Paper() : GameElement("Бумага", "paper.jpg", {0, 4}, {1, 3}) {}
    unique_ptr<Card> createCard(const Vector2f& position, const Vector2f& size, const Color& bgColor) override {
        return make_unique<Card>(position, size, bgColor, 2, "Бумага", "paper.jpg");
    }
};
class Lizard : public GameElement {
public:
    Lizard() : GameElement("Ящерица", "lizard.jpg", {2, 4}, {0, 1}) {}
    unique_ptr<Card> createCard(const Vector2f& position, const Vector2f& size, const Color& bgColor) override {
        return make_unique<Card>(position, size, bgColor, 3, "Ящерица", "lizard.jpg");
    }
};
class Spock : public GameElement {
public:
    Spock() : GameElement("Спок", "spock.jpg", {0, 1}, {2, 3}) {}
    unique_ptr<Card> createCard(const Vector2f& position, const Vector2f& size, const Color& bgColor) override {
        return make_unique<Card>(position, size, bgColor, 4, "Спок", "spock.jpg");
    }
};
// Правила игры 
class GameRules {
private:
    vector<unique_ptr<GameElement>> elements;
public:
    GameRules() {
        elements.push_back(make_unique<Rock>());
        elements.push_back(make_unique<Scissors>());
        elements.push_back(make_unique<Paper>());
        elements.push_back(make_unique<Lizard>());
        elements.push_back(make_unique<Spock>());
    }
    bool beats(int attacker, int defender) const {
        if (attacker < 0 || attacker >= elements.size() || defender < 0 || defender >= elements.size()) {
            return false;
        }
        return elements[attacker]->beats(defender);
    }
    string getElementName(int index) const {
        if (index >= 0 && index < elements.size()) {
            return elements[index]->getName();
        }
        return "Неизвестно";
    }
    string getElementTextureFile(int index) const {
        if (index >= 0 && index < elements.size()) {
            return elements[index]->getTextureFile();
        }
        return "";
    }
    unique_ptr<Card> createCardForElement(int index, const Vector2f& position, const Vector2f& size, const Color& bgColor) {
        if (index >= 0 && index < elements.size()) {
            return elements[index]->createCard(position, size, bgColor);
        }
        return nullptr;
    }
    int getNumberOfElements() const {
        return elements.size();
    }
};
// Игрок
class Player {
protected:
    vector<unique_ptr<Card>> cards;
    int choice;
    bool isHuman;
    Color color;
    float startX;
    float startY;
    string playerName;
    int score;
public:
    Player(float x, float y, const Color& playerColor, const string& name, bool human = false) 
        : startX(x), startY(y), color(playerColor), isHuman(human), choice(-1), playerName(name), score(0) {}
    virtual ~Player() = default;
    void initializeCards(GameRules& rules) {
        Vector2f cardSize(120, 180);
        float cardSpacing = 20;
        for (int i = 0; i < rules.getNumberOfElements(); i++) {
            Vector2f position(startX + i * (cardSize.x + cardSpacing), startY);            
            auto card = rules.createCardForElement(i, position, cardSize, color);
            if (card) {
                card->initializeTexture();
                cards.push_back(std::move(card));
            }
        }
    }
    virtual void makeChoice(int selected = -1) {
        if (selected >= 0 && selected < static_cast<int>(cards.size())) {
            choice = selected;
        } else if (!isHuman) {
            static random_device rd;
            static mt19937 gen(rd());
            uniform_int_distribution<> dist(0, cards.size() - 1);
            choice = dist(gen);
        }
    }
    void throwCardToPosition(int positionIndex) {
        if (choice >= 0 && choice < static_cast<int>(cards.size())) {
            Vector2f positions[] = {
                Vector2f(600, 250),  // Игрок - перед
                Vector2f(800, 250),  // Бот1 - лево верх
                Vector2f(1000, 250)  // Бот2 - право верх
            };
            if (positionIndex >= 0 && positionIndex < 3) {
                cards[choice]->throwTo(positions[positionIndex], Vector2f(140, 200));
            }
        }
    }
    void update(float deltaTime, const Vector2f& mousePos = Vector2f()) {
        for (auto& card : cards) {
            card->update(deltaTime);       
            if (isHuman && !card->isCardThrown()) {
                card->setHovered(card->contains(mousePos));
            }
        }
    }
    void draw(RenderWindow& window) {
        for (auto& card : cards) {
            card->draw(window);
        }
    }
    void reset() {
        choice = -1;
        for (auto& card : cards) {
            card->reset();
        }
    }
    void resetToOriginalPositions() {
        choice = -1;
        for (auto& card : cards) {
            card->resetToOriginalPosition();
        }
    }
    void resetChoiceOnly() {
        choice = -1;
    }
    void incrementScore() { score++; }
    int getScore() const { return score; }
    int getChoice() const { return choice; }
    Card& getCard(int index) { 
        if (index >= 0 && index < static_cast<int>(cards.size())) {
            return *cards[index];
        }
        throw out_of_range("Invalid card index");
    }
    Card* getCardPtr(int index) { 
        if (index >= 0 && index < static_cast<int>(cards.size())) {
            return cards[index].get();
        }
        return nullptr;
    }
    bool isPlayerHuman() const { return isHuman; }
    string getName() const { return playerName; }
    int getNumberOfCards() const { return static_cast<int>(cards.size()); }
    void resetScore() { score = 0; }
    void setChoice(int newChoice) { choice = newChoice; }
    bool hasMadeChoice() const { return choice != -1; }
};
// Бот
class Bot : public Player {
public:
    Bot(float x, float y, const Color& botColor, const string& name) : Player(x, y, botColor, name, false) {}
};
// Анимация креста
class AnimatedCross {
private:
    RectangleShape line1;
    RectangleShape line2;
    float progress;
    bool active;
    Clock animationClock;
    Vector2f position;
    Vector2f size;
public:
    AnimatedCross() : progress(0.0f), active(false) {
        line1.setSize(Vector2f(0, 6));
        line2.setSize(Vector2f(0, 6));
        line1.setFillColor(Color::Red);
        line2.setFillColor(Color::Red);
        line1.setOrigin(0, 3);
        line2.setOrigin(0, 3);
    }
    void start(const Vector2f& pos, const Vector2f& cardSize) {
        position = pos;
        size = cardSize;
        progress = 0.0f;
        active = true;
        animationClock.restart();   
        Vector2f center = pos + cardSize / 2.0f;
        line1.setPosition(center);
        line2.setPosition(center);
        line1.setRotation(45);
        line2.setRotation(-45);
    }
    void update() {
        if (!active) return;   
        float elapsed = animationClock.getElapsedTime().asSeconds();
        progress = min(elapsed / 0.5f, 1.0f);
        float diagonal = std::sqrt(size.x * size.x + size.y * size.y);
        float currentLength = progress * diagonal * 0.8f;
        line1.setSize(Vector2f(currentLength, 6));
        line2.setSize(Vector2f(currentLength, 6));
        line1.setOrigin(currentLength / 2, 3);
        line2.setOrigin(currentLength / 2, 3);
        if (progress >= 1.0f) {
            active = false;
        }
    }
    void draw(RenderWindow& window) {
        if (progress > 0) {
            window.draw(line1);
            window.draw(line2);
        }
    }
    void reset() {
        progress = 0.0f;
        active = false;
        line1.setSize(Vector2f(0, 6));
        line2.setSize(Vector2f(0, 6));
    }
    bool isActive() const { return active; }
};
// Сама игра 
class Game : public IGameController {
private:
    RenderWindow window;
    GameRules rules;
    unique_ptr<Player> player;
    unique_ptr<Bot> bot1;
    unique_ptr<Bot> bot2;
    AnimatedCross crossPlayer;
    AnimatedCross crossBot1;
    AnimatedCross crossBot2;
    Clock revealClock;
    Clock duelStartClock;
    Font font;
    Text scoreTextPlayer;
    Text scoreTextBot1;
    Text scoreTextBot2;
    Text victoryText;
    Text restartText;
    Text duelText;
    Text instructionText;
    bool roundInProgress;
    bool roundFinished;
    bool gameOver;
    bool isDuelMode;
    bool preparingForDuel;
    vector<int> duelParticipants;
    wstring winnerName;
    const int WINNING_SCORE = 5;
    bool waitingForPlayerChoiceInDuel;
    bool choicesRevealed;
    int duelRoundCount;
    GameCommandInvoker commandInvoker;
public:
    Game() : 
        window(VideoMode(1700, 700), L"Камень Ножницы Бумага Ящерица Спок", Style::Default),
        roundInProgress(false),
        roundFinished(false),
        gameOver(false),
        isDuelMode(false),
        preparingForDuel(false),
        winnerName(L""),
        waitingForPlayerChoiceInDuel(false),
        choicesRevealed(false),
        duelRoundCount(0) {
        window.setVerticalSyncEnabled(true);
        player = make_unique<Player>(150, 450, Color::White, "Игрок", true);
        bot1 = make_unique<Bot>(150, 50, Color(100, 100, 200), "Бот 1");
        bot2 = make_unique<Bot>(1000, 50, Color(200, 100, 100), "Бот 2");
        player->initializeCards(rules);
        bot1->initializeCards(rules);
        bot2->initializeCards(rules);
        loadFont();
        scoreTextPlayer.setFont(font);
        scoreTextPlayer.setCharacterSize(28);
        scoreTextPlayer.setFillColor(Color::White);
        scoreTextPlayer.setPosition(1400, 600);
        scoreTextBot1.setFont(font);
        scoreTextBot1.setCharacterSize(28);
        scoreTextBot1.setFillColor(Color(100, 100, 200));
        scoreTextBot1.setPosition(1400, 630);
        scoreTextBot2.setFont(font);
        scoreTextBot2.setCharacterSize(28);
        scoreTextBot2.setFillColor(Color(200, 100, 100));
        scoreTextBot2.setPosition(1400, 660);
        victoryText.setFont(font);
        victoryText.setCharacterSize(60);
        victoryText.setFillColor(Color::Yellow);
        victoryText.setStyle(Text::Bold);
        restartText.setFont(font);
        restartText.setCharacterSize(30);
        restartText.setFillColor(Color::Cyan);
        restartText.setString(L"Нажмите ПРОБЕЛ для новой игры");
        duelText.setFont(font);
        duelText.setCharacterSize(30);
        duelText.setFillColor(Color::Yellow);
        updateScoreDisplay();
    }
    void run() {
        Clock clock;
        while (window.isOpen()) {
            float deltaTime = clock.restart().asSeconds();
            processEvents();
            update(deltaTime);
            render();
        }
    }
    void startNewRound(int playerChoice) override {
        roundInProgress = true;
        roundFinished = false;
        isDuelMode = false;
        preparingForDuel = false;
        duelParticipants.clear();
        waitingForPlayerChoiceInDuel = false;
        choicesRevealed = false;
        duelRoundCount = 0;
        crossPlayer.reset();
        crossBot1.reset();
        crossBot2.reset();
        player->makeChoice(playerChoice);
        bot1->makeChoice();
        bot2->makeChoice();
        revealClock.restart();
    }
    void playerMakeChoice(int playerIndex, int choice) override {
        if (playerIndex == 0) {
            player->makeChoice(choice);
        } else if (playerIndex == 1) {
            bot1->makeChoice(choice);
        } else if (playerIndex == 2) {
            bot2->makeChoice(choice);
        }
    }
    void restartGame() override {
        gameOver = false;
        winnerName = L"";
        player->resetScore();
        bot1->resetScore();
        bot2->resetScore();
        player->resetToOriginalPositions();
        bot1->resetToOriginalPositions();
        bot2->resetToOriginalPositions();
        crossPlayer.reset();
        crossBot1.reset();
        crossBot2.reset();
        roundInProgress = false;
        roundFinished = false;
        isDuelMode = false;
        preparingForDuel = false;
        duelParticipants.clear();
        waitingForPlayerChoiceInDuel = false;
        choicesRevealed = false;
        duelRoundCount = 0;
        updateScoreDisplay();
        commandInvoker.clearHistory();
    }
    void prepareForDuel(int participant1, int participant2) override {
        preparingForDuel = true;
        isDuelMode = false;
        duelParticipants = {participant1, participant2};
        duelStartClock.restart();
        duelRoundCount = 1;
        player->resetChoiceOnly();
        bot1->resetChoiceOnly();
        bot2->resetChoiceOnly();
        player->resetToOriginalPositions();
        bot1->resetToOriginalPositions();
        bot2->resetToOriginalPositions();
        crossPlayer.reset();
        crossBot1.reset();
        crossBot2.reset();
        string names[] = {"Игрок", "Бот 1", "Бот 2"};
        wstring duelMsg = L"ПОДГОТОВКА К ДУЭЛИ: " + stringToWstring(names[participant1]) + L" vs " + stringToWstring(names[participant2]);
        duelText.setString(duelMsg);
        FloatRect textRect = duelText.getLocalBounds();
        duelText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        duelText.setPosition(850, 320);
    }
private:
    void loadFont() {
        vector<string> fontPaths = {
            "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        };
        bool fontLoaded = false;
        for (const auto& path : fontPaths) {
            if (font.loadFromFile(path)) {
                fontLoaded = true;
                break;
            }
        }
        if (!fontLoaded) {
            font = Font();
        }
    }
    wstring stringToWstring(const std::string& str) {
        try {
            wstring_convert<codecvt_utf8<wchar_t>> converter;
            return converter.from_bytes(str);
        } catch (...) {
            wstring wstr;
            for (char c : str) {
                wstr += static_cast<wchar_t>(c);
            }
            return wstr;
        }
    }
    void updateScoreDisplay() {
        wstring playerStr = L"Игрок: " + to_wstring(player->getScore());
        wstring bot1Str = L"Бот 1: " + to_wstring(bot1->getScore());
        wstring bot2Str = L"Бот 2: " + to_wstring(bot2->getScore());
        if (player->getScore() >= WINNING_SCORE) playerStr += L" ★";
        if (bot1->getScore() >= WINNING_SCORE) bot1Str += L" ★";
        if (bot2->getScore() >= WINNING_SCORE) bot2Str += L" ★";
        scoreTextPlayer.setString(playerStr);
        scoreTextBot1.setString(bot1Str);
        scoreTextBot2.setString(bot2Str);
    }
    void checkGameOver() {
        if (player->getScore() >= WINNING_SCORE || 
            bot1->getScore() >= WINNING_SCORE || 
            bot2->getScore() >= WINNING_SCORE) {
            gameOver = true;
            int maxScore = max({player->getScore(), bot1->getScore(), bot2->getScore()});
            if (player->getScore() == maxScore) {
                winnerName = L"Игрок";
            }
            else if (bot1->getScore() == maxScore) {
                winnerName = L"Бот 1";
            } 
            else if (bot2->getScore() == maxScore) {
                winnerName = L"Бот 2";
            }
            wstring victoryMsg = L"ПОБЕДИТЕЛЬ: " + winnerName + L"!\n";
            victoryText.setString(victoryMsg);
            FloatRect textRect = victoryText.getLocalBounds();
            victoryText.setOrigin(textRect.left + textRect.width / 2.0f,textRect.top + textRect.height / 2.0f);
            victoryText.setPosition(850, 250);
            FloatRect restartRect = restartText.getLocalBounds();
            restartText.setOrigin(restartRect.left + restartRect.width / 2.0f,restartRect.top + restartRect.height / 2.0f);
            restartText.setPosition(850, 350);
        }
    }
    void processEvents() {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            if (event.type == Event::KeyPressed && gameOver) {
                if (event.key.code == Keyboard::Space) {
                    commandInvoker.executeCommand(make_unique<RestartGameCommand>(this));
                }
                else if (event.key.code == Keyboard::Y && event.key.control) {
                    commandInvoker.redo();
                }
            }
            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                if (event.mouseButton.button == Mouse::Left && !gameOver) {
                    if (isDuelMode && waitingForPlayerChoiceInDuel) {
                        // Игрок выбирает карту для дуэли
                        for (int i = 0; i < player->getNumberOfCards(); i++) {
                            if (player->getCard(i).contains(mousePos)) {
                                commandInvoker.executeCommand(
                                    make_unique<PlayerChoiceCommand>(this, 0, i)
                                );
                                waitingForPlayerChoiceInDuel = false;
                                // Боты делают выбор сразу
                                if (find(duelParticipants.begin(), duelParticipants.end(), 1) != duelParticipants.end()) {
                                    commandInvoker.executeCommand(
                                        make_unique<PlayerChoiceCommand>(this, 1, -1)
                                    );
                                }
                                if (find(duelParticipants.begin(), duelParticipants.end(), 2) != duelParticipants.end()) {
                                    commandInvoker.executeCommand(
                                        make_unique<PlayerChoiceCommand>(this, 2, -1)
                                    );
                                }
                                // Сразу выбрасываем карты для дуэли
                                throwCardsForDuel();
                                revealClock.restart();
                                break;
                            }
                        }
                    } 
                    else if (!roundInProgress && !isDuelMode && !preparingForDuel) {
                        // Обычный выбор карты для нового раунда
                        for (int i = 0; i < player->getNumberOfCards(); i++) {
                            if (player->getCard(i).contains(mousePos)) {
                                commandInvoker.executeCommand(
                                    make_unique<StartNewRoundCommand>(this, i)
                                );
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    void throwCardsForDuel() {
        if (duelParticipants.size() != 2) return;
        // Если дуэль между игроком и ботом 1
        if ((duelParticipants[0] == 0 && duelParticipants[1] == 1) || (duelParticipants[0] == 1 && duelParticipants[1] == 0)) {
            player->throwCardToPosition(0);  
            bot1->throwCardToPosition(1);    
        }
        // Если дуэль между игроком и ботом 2
        else if ((duelParticipants[0] == 0 && duelParticipants[1] == 2) || (duelParticipants[0] == 2 && duelParticipants[1] == 0)) {
            player->throwCardToPosition(0);  
            bot2->throwCardToPosition(2);    
        }
        // Если дуэль между ботами
        else if ((duelParticipants[0] == 1 && duelParticipants[1] == 2) || (duelParticipants[0] == 2 && duelParticipants[1] == 1)) {
            bot1->throwCardToPosition(1);    
            bot2->throwCardToPosition(2);    
        }
    }
    void update(float deltaTime) {
        if (gameOver) return;
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        player->update(deltaTime, mousePos);
        bot1->update(deltaTime);
        bot2->update(deltaTime);
        crossPlayer.update();
        crossBot1.update();
        crossBot2.update();
        if (preparingForDuel) {
            float elapsed = duelStartClock.getElapsedTime().asSeconds();
            if (elapsed > 1.0f) {
                preparingForDuel = false;
                startDuelAfterPreparation();
            }
        }
        else if (roundInProgress && !roundFinished) {
            if (!choicesRevealed) {
                float elapsed = revealClock.getElapsedTime().asSeconds();
                if (elapsed > 0.5f) {
                    if (isDuelMode) {
                    } else {
                        player->throwCardToPosition(0);
                        bot1->throwCardToPosition(1);
                        bot2->throwCardToPosition(2);
                    }
                    choicesRevealed = true;
                }
            }
            if (choicesRevealed) {
                float elapsed = revealClock.getElapsedTime().asSeconds();
                if (elapsed > 1.5f) {
                    determineRoundResult();
                }
            }
        }
        if (roundFinished && revealClock.getElapsedTime().asSeconds() > 3.0f) {
            resetRound();
        }
    }
    void render() {
        window.clear(Color(30, 70, 30));
        bot1->draw(window);
        bot2->draw(window);
        player->draw(window);
        crossPlayer.draw(window);
        crossBot1.draw(window);
        crossBot2.draw(window);
        window.draw(scoreTextPlayer);
        window.draw(scoreTextBot1);
        window.draw(scoreTextBot2);
        if (!roundInProgress && !gameOver && !isDuelMode && !preparingForDuel) {
            window.draw(instructionText);
        }
        if ((isDuelMode || preparingForDuel) && roundInProgress) {
            window.draw(duelText);
        }
        if (gameOver) {
            RectangleShape overlay(Vector2f(1700, 700));
            overlay.setFillColor(Color(0, 0, 0, 180));
            window.draw(overlay);
            window.draw(victoryText);
            window.draw(restartText);
        }
        window.display();
    }
    void startDuelAfterPreparation() {
        isDuelMode = true;
        roundInProgress = true;
        roundFinished = false;
        choicesRevealed = false;
        crossPlayer.reset();
        crossBot1.reset();
        crossBot2.reset();
        if (duelParticipants[0] == 0 || duelParticipants[1] == 0) {
            // Игрок участвует в дуэли
            player->resetChoiceOnly();
            waitingForPlayerChoiceInDuel = true;
            string names[] = {"Игрок", "Бот 1", "Бот 2"};
            wstring duelMsg = L"ДУЭЛЬ: " + stringToWstring(names[duelParticipants[0]]) + L" vs " + stringToWstring(names[duelParticipants[1]]) + 
                              L"\nИгрок, выберите карту!";
            duelText.setString(duelMsg);
            FloatRect textRect = duelText.getLocalBounds();
            duelText.setOrigin(textRect.left + textRect.width / 2.0f,textRect.top + textRect.height / 2.0f);
            duelText.setPosition(850, 320);
        } else {
            waitingForPlayerChoiceInDuel = false;
            bot1->resetChoiceOnly();
            bot2->resetChoiceOnly();
            if (duelParticipants[0] == 1 || duelParticipants[1] == 1) {
                bot1->makeChoice();
            }
            if (duelParticipants[0] == 2 || duelParticipants[1] == 2) {
                bot2->makeChoice();
            }
            string names[] = {"Игрок", "Бот 1", "Бот 2"};
            wstring duelMsg = L"ДУЭЛЬ: " + stringToWstring(names[duelParticipants[0]]) + L" vs " + stringToWstring(names[duelParticipants[1]]);
            duelText.setString(duelMsg);
            FloatRect textRect = duelText.getLocalBounds();
            duelText.setOrigin(textRect.left + textRect.width / 2.0f,textRect.top + textRect.height / 2.0f);
            duelText.setPosition(850, 320);
            throwCardsForDuel();
            revealClock.restart();
        }
    }
    void determineRoundResult() {
        if (gameOver) return;
        int playerChoice = player->getChoice();
        int bot1Choice = bot1->getChoice();
        int bot2Choice = bot2->getChoice();
        if (isDuelMode) {
            if (duelParticipants.size() == 2) {
                int p1 = duelParticipants[0];
                int p2 = duelParticipants[1];
                int choice1 = (p1 == 0) ? playerChoice : (p1 == 1) ? bot1Choice : bot2Choice;
                int choice2 = (p2 == 0) ? playerChoice : (p2 == 1) ? bot1Choice : bot2Choice;
                if (choice1 == -1 || choice2 == -1) {
                    return; 
                }
                // Обработка ситуации, когда выбраны одинаковые карты в дуэли
                if (choice1 == choice2) {
                    // Сбрасываем выборы и начинаем новую дуэль с теми же участниками
                    if (p1 == 0) player->resetChoiceOnly();
                    else if (p1 == 1) bot1->resetChoiceOnly();
                    else if (p1 == 2) bot2->resetChoiceOnly();
                    if (p2 == 0) player->resetChoiceOnly();
                    else if (p2 == 1) bot1->resetChoiceOnly();
                    else if (p2 == 2) bot2->resetChoiceOnly();
                    // Возвращаем карты на исходные позиции
                    player->resetToOriginalPositions();
                    bot1->resetToOriginalPositions();
                    bot2->resetToOriginalPositions();
                    crossPlayer.reset();
                    crossBot1.reset();
                    crossBot2.reset();
                    startDuelAfterPreparation();
                    return;
                } 
                else if (rules.beats(choice1, choice2)) {
                    awardWinner(p1);
                    animateCrossForLoser(p2);
                } else {
                    awardWinner(p2);
                    animateCrossForLoser(p1);
                }
                checkGameOver();
                endDuel();
            }
            return;
        }
        if (playerChoice == -1 || bot1Choice == -1 || bot2Choice == -1) {
            return;
        }
        int winner = determineThreePlayerWinner(playerChoice, bot1Choice, bot2Choice);
        if (winner != -1) {
            awardWinner(winner);
            animateCrossesForThreePlayers(winner, playerChoice, bot1Choice, bot2Choice);
            checkGameOver();
            roundFinished = true;
            return;
        }
        if (playerChoice == bot1Choice && bot1Choice == bot2Choice) {
            // Ничья для всех - ничего не делаем
        } 
        else if (playerChoice == bot1Choice) {
            if (rules.beats(bot2Choice, playerChoice)) {
                awardWinner(2);
                animateCrossForLoser(0);
                animateCrossForLoser(1);
            } else {
                commandInvoker.executeCommand(
                    make_unique<PrepareDuelCommand>(this, 0, 1)
                );
                return;
            }
        } 
        else if (playerChoice == bot2Choice) {
            if (rules.beats(bot1Choice, playerChoice)) {
                awardWinner(1);
                animateCrossForLoser(0);
                animateCrossForLoser(2);
            } else {
                commandInvoker.executeCommand(
                    make_unique<PrepareDuelCommand>(this, 0, 2)
                );
                return;
            }
        } 
        else if (bot1Choice == bot2Choice) {
            if (rules.beats(playerChoice, bot1Choice)) {
                awardWinner(0);
                animateCrossForLoser(1);
                animateCrossForLoser(2);
            } else {
                commandInvoker.executeCommand(
                    make_unique<PrepareDuelCommand>(this, 1, 2)
                );
                return;
            }
        }
        checkGameOver();
        roundFinished = true;
    }
    int determineThreePlayerWinner(int playerChoice, int bot1Choice, int bot2Choice) {
        if (rules.beats(playerChoice, bot1Choice) && rules.beats(playerChoice, bot2Choice)) {
            return 0;
        }
        if (rules.beats(bot1Choice, playerChoice) && rules.beats(bot1Choice, bot2Choice)) {
            return 1;
        }
        if (rules.beats(bot2Choice, playerChoice) && rules.beats(bot2Choice, bot1Choice)) {
            return 2;
        }
        return -1;
    }
    void awardWinner(int winnerIndex) {
        if (winnerIndex == 0) {
            player->incrementScore();
        } else if (winnerIndex == 1) {
            bot1->incrementScore();
        } else if (winnerIndex == 2) {
            bot2->incrementScore();
        }
        updateScoreDisplay();
    }
    void animateCrossForLoser(int loserIndex) {
        Card* card = nullptr;
        if (loserIndex == 0) {
            int choice = player->getChoice();
            if (choice >= 0) {
                card = player->getCardPtr(choice);
            }
        } else if (loserIndex == 1) {
            int choice = bot1->getChoice();
            if (choice >= 0) {
                card = bot1->getCardPtr(choice);
            }
        } else if (loserIndex == 2) {
            int choice = bot2->getChoice();
            if (choice >= 0) {
                card = bot2->getCardPtr(choice);
            }
        }
        if (card) {
            if (loserIndex == 0) {
                crossPlayer.start(card->getPosition(), card->getSize());
            } else if (loserIndex == 1) {
                crossBot1.start(card->getPosition(), card->getSize());
            } else if (loserIndex == 2) {
                crossBot2.start(card->getPosition(), card->getSize());
            }
        }
    }
    void animateCrossesForThreePlayers(int winnerIndex, int playerChoice, int bot1Choice, int bot2Choice) {
        if (winnerIndex == 0) {
            Card* card1 = bot1->getCardPtr(bot1Choice);
            Card* card2 = bot2->getCardPtr(bot2Choice);
            if (card1) crossBot1.start(card1->getPosition(), card1->getSize());
            if (card2) crossBot2.start(card2->getPosition(), card2->getSize());
        } else if (winnerIndex == 1) {
            Card* card1 = player->getCardPtr(playerChoice);
            Card* card2 = bot2->getCardPtr(bot2Choice);
            if (card1) crossPlayer.start(card1->getPosition(), card1->getSize());
            if (card2) crossBot2.start(card2->getPosition(), card2->getSize());
        } else if (winnerIndex == 2) {
            Card* card1 = player->getCardPtr(playerChoice);
            Card* card2 = bot1->getCardPtr(bot1Choice);
            if (card1) crossPlayer.start(card1->getPosition(), card1->getSize());
            if (card2) crossBot1.start(card2->getPosition(), card2->getSize());
        }
    }
    void endDuel() {
        isDuelMode = false;
        waitingForPlayerChoiceInDuel = false;
        roundFinished = true;
        if (!gameOver) {
            checkGameOver();
        }
    }
    void resetRound() {
        if (gameOver) return;
        roundInProgress = false;
        roundFinished = false;
        isDuelMode = false;
        preparingForDuel = false;
        duelParticipants.clear();
        waitingForPlayerChoiceInDuel = false;
        choicesRevealed = false;
        duelRoundCount = 0;
        player->reset();
        bot1->reset();
        bot2->reset();
        crossPlayer.reset();
        crossBot1.reset();
        crossBot2.reset();
    }
};
int main() {
    srand(static_cast<unsigned int>(std::time(nullptr)));
    locale::global(locale("ru_RU.UTF-8"));
    Game game;
    game.run();
    return 0;
}
