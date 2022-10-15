#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

#include <filesystem>
namespace fs = std::filesystem;

#include "../headers/TextBox.hpp"
#include "../headers/Util.hpp"
using namespace Util;

namespace Constants {
const sf::Color primary = sf::Color(230, 230, 230);
const sf::Color secondary = sf::Color(200, 200, 200);
const int margin = 10;
const int padding = 2;
const int iconSize = 16;
const int spacing[] = {0, 400, 600};
const int yStart = 70, headerHeight = 40;
} // namespace Constants

using namespace Constants;

sf::RenderWindow window(sf::VideoMode(740, 600), "Files++", sf::Style::Titlebar | sf::Style::Close);

void render(sf::Text text, TextBox textBox, sf::Sprite fileSprite, sf::Sprite folderSprite, sf::Sprite arrowSprite) {
    window.clear(primary);

    text.setStyle(sf::Text::Regular);
    text.setOrigin(0, 0);
    text.setString(textBox.getText());

    // Path bar
    arrowSprite.setPosition(margin, margin + padding);
    window.draw(arrowSprite);

    text.setPosition(margin * 2 + iconSize, margin);
    window.draw(text);

    int y = yStart;
    if (fs::is_directory(textBox.getText())) {
        fs::path dir = textBox.getText();

        for (fs::directory_entry const &item : fs::directory_iterator(dir)) {
            // Icon
            sf::Sprite sprite = fs::is_directory(item.path()) ? folderSprite : fileSprite;
            sprite.setPosition(margin, y + padding);
            window.draw(sprite);

            // Name
            text.setString(item.path().filename().string());
            text.setPosition(margin * 2 + iconSize, y);
            window.draw(text);

            // Size
            text.setString(prettyBytes(getPathSize(item.path()), 2));
            text.setPosition(spacing[1], y);
            window.draw(text);

            // Date modified
            text.setString(formatTime(item.last_write_time()));
            text.setPosition(spacing[2], y);
            window.draw(text);

            y += 20;
        }

        text.setStyle(sf::Text::Regular);

        // Footer
        int footerHeight = 32;
        sf::RectangleShape rectangle(sf::Vector2f(window.getSize().x, footerHeight));
        rectangle.setFillColor(secondary);
        rectangle.setPosition(0, window.getSize().y - footerHeight);
        window.draw(rectangle);

        text.setString(
            countItems(dir, "items") + " = " + countItems(dir, "folders") + " + " + countItems(dir, "files") + " | " + prettyBytes(getPathSize(dir), 2));
        text.setPosition(margin, window.getSize().y - text.getGlobalBounds().height * 2);
        window.draw(text);
    }

    // Header
    text.setStyle(sf::Text::Underlined);

    text.setString("Name");
    text.setPosition(margin, headerHeight);
    window.draw(text);

    text.setString("Size");
    text.setPosition(spacing[1], headerHeight);
    window.draw(text);

    text.setString("Date modified");
    text.setPosition(spacing[2], headerHeight);
    window.draw(text);

    text.setStyle(sf::Text::Regular);

    window.display();
};

int main(int argc, const char *argv[]) {
    sf::Image icon;
    if (!icon.loadFromFile("res/img/icon.png")) handleFileError();
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    sf::Font font;
    if (!font.loadFromFile("res/fonts/Helvetica.ttf")) handleFileError();

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::Black);

    fs::path startDir = argc == 2
                            ? fs::path{argv[1]}
                            : fs::current_path();

    TextBox textBox(startDir.string());

    sf::Texture fileIcon;
    if (!fileIcon.loadFromFile("res/img/file.png")) handleFileError();
    sf::Sprite fileSprite(fileIcon);

    sf::Texture folderIcon;
    if (!folderIcon.loadFromFile("res/img/folder.png")) handleFileError();
    sf::Sprite folderSprite(folderIcon);

    sf::Texture arrowIcon;
    if (!arrowIcon.loadFromFile("res/img/arrow.png")) handleFileError();
    sf::Sprite arrowSprite(arrowIcon);
    arrowSprite.setPosition(margin, margin + padding);

    render(text, textBox, fileSprite, folderSprite, arrowSprite);

    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {

            // Window closed
            case sf::Event::Closed: {
                window.close();
            } break;

            // Text input
            case sf::Event::TextEntered: {
                textBox.handleInput(event.text.unicode);
                render(text, textBox, fileSprite, folderSprite, arrowSprite);
            } break;

            // Window resized
            case sf::Event::Resized: {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            } break;

            // Key pressed
            case sf::Event::KeyPressed: {
                if (event.key.code == sf::Keyboard::Escape) window.close();
            } break;

            // Button
            case sf::Event::MouseButtonReleased: {
                if (arrowSprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    fs::path current = textBox.getText();
                    textBox.setText(current.parent_path().string());
                    render(text, textBox, fileSprite, folderSprite, arrowSprite);
                } else {
                    int my = event.mouseButton.y, y = yStart;
                    for (fs::directory_entry const &item : fs::directory_iterator(textBox.getText())) {
                        fs::path p = item.path();
                        if (my >= y && my <= y + 20) {
                            if (fs::is_directory(p)) {
                                textBox.setText(p.string());
                                render(text, textBox, fileSprite, folderSprite, arrowSprite);
                            } else {
                                std::string cmd = "start " + p.string();
                                std::system(cmd.c_str());
                            }
                        }
                        y += 20;
                    }
                }
                break;
            }
            }
        }
    }

    return EXIT_SUCCESS;
}