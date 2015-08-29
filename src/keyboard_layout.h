#ifndef KEYBOARDLAYOUT_H
#define KEYBOARDLAYOUT_H

#include "config_parse.h"

class KeyboardLayout
{
public:
    KeyboardLayout(ConfigParse *config);
    ~KeyboardLayout();

    enum Layout
    {
        Default,
        Emacs
    };

    Layout layout();
private:
    ConfigParse *config;
    Layout _layout;
    void setCurrentLayout();
};

#endif
