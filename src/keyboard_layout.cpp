#include "keyboard_layout.h"

#include <string>

using namespace std;

KeyboardLayout::KeyboardLayout(ConfigParse *config)
{
    this->config = config;
    this->setCurrentLayout();
}

KeyboardLayout::~KeyboardLayout()
{
    delete this->config;
}

void KeyboardLayout::setCurrentLayout()
{
    if (this->config == NULL)
    {
        this->_layout = Default;
        return;
    }
    std::vector<string> sections = this->config->getSections();
    string layout = this->config->getValue("default", "Layout");
    if (layout.compare("emacs") != 0)
    {
        this->_layout = Default;
    }
    else
    {
        this->_layout = Emacs;
    }
}

KeyboardLayout::Layout KeyboardLayout::layout()
{
    return this->_layout;
}
