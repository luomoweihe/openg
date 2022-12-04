//
// Created by Alex L on 2022/11/30.
//

#ifndef OPENGL3_MOUSEKEYBOARD_H
#define OPENGL3_MOUSEKEYBOARD_H

#endif //OPENGL3_MOUSEKEYBOARD_H
void mouseButton(int button, int state, int , int y);
void mouseMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void selectMain(int choice);
void mouseWheel(int button, int dir, int x, int y);

struct menuEntryStruct
{
    const char *label;
    char key;
};

enum TransMode{
    Translate,
    Rotate,
    Scale,
    WheelControl,
    Auto,
    ViewTransform
    //Camera
};


static menuEntryStruct mainMenu[] = {
        "Reset", '0',
        "Rotate", 'r',
        "Translate", 't',
        "Scale", 's',
        "lines/polygons", '1',
        "Auto", 'a',
        "front", '2',
        "left", '4',
        "right", '6',
        "behind", '8',
        "top", '5',
        "quit", 27, //ESC 键（ASCII: 27）
};
