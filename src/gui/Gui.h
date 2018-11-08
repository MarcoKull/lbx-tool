#ifndef GUI_H
#define GUI_H

#include <QApplication>

#include "Window.h"

namespace LbxToolGui {

    static void show(std::string path) {
        // initialize qt application
        int argc = 0;
        QApplication app(argc, NULL);

        // create main window
        Window win;
        win.show();

        // open given path
        if (path.size() > 0) {
            win.open(path);
        }

        // show window
        app.exec();
    }

    static void show() {
        show("");
    }

}

#endif /* GUI_H */

