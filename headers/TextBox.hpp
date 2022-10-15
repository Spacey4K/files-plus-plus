#include <string>

class TextBox {
    std::string text;

  public:
    TextBox(std::string deafultText) {
        setText(deafultText);
    }
    void handleInput(int unicode);
    void setText(std::string newText);
    std::string getText();
};