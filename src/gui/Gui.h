#ifndef GUI_H
#define GUI_H

#include <QApplication>

#include "Window.h"

namespace LbxToolGui {

    static int show(int argc, char** argv) {
        // initialize qt application
        int ac = 0;
        QApplication app(ac, NULL);

        // create main window
        Window win;
        win.show();

        // open file paths
        if (argc > 0) {
            if (win.open(argv[0])) {
                for (int i = 1; i < argc; ++i) {
                    if (!win.openNew(argv[i])) {
                        break;
                    }
                }
            }
        }

        // show window
        return app.exec();
        return 0;
    }

    static int show(char* path) {
        return show(1, &path);
    }

    static int show() {
        return show(0, NULL);
    }

}

#endif /* GUI_H */

