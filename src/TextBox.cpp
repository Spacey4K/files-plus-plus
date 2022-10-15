#include "../headers/TextBox.hpp"

#include <algorithm>
#include <string>

const int DELETE = 8;
const int ENTER = 13;

std::string TextBox::getText() {
    return this->text;
}

void TextBox::setText(std::string newText) {
    std::replace(newText.begin(), newText.end(), '\\', '/');

    this->text = newText;
}

void TextBox::handleInput(int unicode) {
    std::string currentText = getText();
    if (unicode == DELETE) {
        setText(currentText.substr(0, currentText.size() - 1));
    } else {
        setText(currentText + char(unicode));
    }
}
