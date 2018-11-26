#include "Window.h"

#include <fstream>

#include <QCloseEvent>
#include <QFileDialog>
#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTableView>
#include <iostream>

using namespace LbxToolGui;

Window::Window() : model(this) {
    copyData = NULL;
    cmdCurrent = 0;
    cmdSaved = 0;
    resize(450, 300);

    // set icon
    setWindowIcon(QIcon(":lbx-tool.png"));

    // create status bar
    statusBar()->addPermanentWidget(&statusPath, 1);
    statusBar()->addPermanentWidget(&statusFiles, 0);

    // create center table
    QTableView* table = new QTableView(this);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setModel(&model);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    setCentralWidget(table);

    sel = table->selectionModel();
    connect(table, &QWidget::customContextMenuRequested, this, &Window::onContextMenu);
    connect(table, &QTableView::activated, this, &Window::onActivated);
    connect(sel, &QItemSelectionModel::selectionChanged, this, &Window::onSelectionChanged);

    // generate actions
    actAbout.setText("&About");
    actAbout.setShortcut(QKeySequence::HelpContents);
    connect(&actAbout, &QAction::triggered, this, &Window::onAbout);

    actCopy.setText("&Copy");
    actCopy.setShortcut(QKeySequence::Copy);
    connect(&actCopy, &QAction::triggered, this, &Window::onCopy);

    actCut.setText("&Cut");
    actCut.setShortcut(QKeySequence::Cut);
    connect(&actCut, &QAction::triggered, this, &Window::onCut);

    actDelete.setText("&Delete");
    actDelete.setShortcut(QKeySequence::Delete);
    connect(&actDelete, &QAction::triggered, this, &Window::onDelete);

    actExtract.setText("&Extract...");
    actExtract.setShortcut(tr("Ctrl+E"));
    connect(&actExtract, &QAction::triggered, this, &Window::onExtract);

    actExtractAll.setText("Extract &All...");
    actExtractAll.setShortcut(tr("Ctrl+A"));
    connect(&actExtractAll, &QAction::triggered, this, &Window::onExtractAll);

    actInsert.setText("&Insert...");
    actInsert.setShortcut(tr("Ctrl+I"));
    connect(&actInsert, &QAction::triggered, this, &Window::onInsert);

    actNew.setText("&New");
    actNew.setShortcut(QKeySequence::New);
    connect(&actNew, &QAction::triggered, this, &Window::onNew);

    actOpen.setText("&Open...");
    actOpen.setShortcut(QKeySequence::Open);
    connect(&actOpen, &QAction::triggered, this, &Window::onOpen);

    actPaste.setText("&Paste");
    actPaste.setShortcut(QKeySequence::Paste);
    actPaste.setEnabled(false);
    connect(&actPaste, &QAction::triggered, this, &Window::onPaste);

    actQuit.setText("&Quit");
    actQuit.setShortcut(QKeySequence::Quit);
    connect(&actQuit, &QAction::triggered, this, &Window::onQuit);

    actRedo.setText("&Redo");
    actRedo.setShortcut(QKeySequence::Redo);
    connect(&actRedo, &QAction::triggered, this, &Window::onRedo);

    actSave.setText("&Save");
    actSave.setShortcut(QKeySequence::Save);
    connect(&actSave, &QAction::triggered, this, &Window::onSave);

    actSaveAs.setText("&SaveAs...");
    actSaveAs.setShortcut(QKeySequence::SaveAs);
    connect(&actSaveAs, &QAction::triggered, this, &Window::onSaveAs);

    actUndo.setText("&Undo");
    actUndo.setShortcut(QKeySequence::Undo);
    connect(&actUndo, &QAction::triggered, this, &Window::onUndo);

    actUnknown0.setText("Unknown&0");
    connect(&actUnknown0, &QAction::triggered, this, &Window::onUnknown0);

    actUnknown1Open.setText("Open...");
    connect(&actUnknown1Open, &QAction::triggered, this, &Window::onUnknown1Open);

    actUnknown1Save.setText("Save...");
    connect(&actUnknown1Save, &QAction::triggered, this, &Window::onUnknown1Save);

    // create menues
    QMenu* menuFile = menuBar()->addMenu("&File");
    menuFile->addAction(&actNew);
    menuFile->addSeparator();
    menuFile->addAction(&actOpen);
    menuFile->addAction(&actSave);
    menuFile->addAction(&actSaveAs);
    menuFile->addSeparator();
    menuFile->addAction(&actExtractAll);
    menuFile->addSeparator();
    menuFile->addAction(&actQuit);

    QMenu* menuEdit = menuBar()->addMenu("&Edit");
    menuEdit->addAction(&actUndo);
    menuEdit->addAction(&actRedo);
    menuEdit->addSeparator();
    menuEdit->addAction(&actCopy);
    menuEdit->addAction(&actPaste);
    menuEdit->addAction(&actCut);
    menuEdit->addAction(&actDelete);
    menuEdit->addSeparator();
    menuEdit->addAction(&actExtract);
    menuEdit->addAction(&actInsert);

    QMenu* menuExtra = menuBar()->addMenu("E&xtra");
    menuExtra->addAction(&actUnknown0);

    QMenu* menuExtraUnknown1 = menuExtra->addMenu("Unknown&1");
    menuExtraUnknown1->addAction(&actUnknown1Open);
    menuExtraUnknown1->addAction(&actUnknown1Save);

    QMenu* menuHelp = menuBar()->addMenu("&Help");
    menuHelp->addAction(&actAbout);

    // set window to 'no selection'
    onDo();
    onSelect();
}

bool Window::open(std::string _path) {
    bool ret = model.open(_path);
    cmdCurrent = 0;
    cmdSaved = 0;
    for (size_t i = 0; i < cmd.size(); ++i) {
        delete cmd[i];
    }
    cmd.clear();
    onDo();
    onSelect();
    return ret;
}

bool Window::openNew(std::string _path) {
    Window* w = new Window();
    w->show();
    return w->open(_path);
}

void Window::exec(Command* _cmd) {
    // delete commands after the current one
    while (cmdCurrent != cmd.size()) {
        delete cmd[cmd.size() - 1];
        cmd.pop_back();
    }

    // add and run command
    cmd.push_back(_cmd);
    ++cmdCurrent;
    _cmd->exec();

    // run trigger
    onDo();
}

void Window::onDo() {
    actUndo.setEnabled(cmdCurrent > 0);
    actRedo.setEnabled(cmdCurrent < cmd.size());
    actSave.setEnabled(cmdCurrent != cmdSaved && model.lbx->path().size() > 0);
    actExtractAll.setEnabled(model.lbx->size() > 0);

    if (cmdCurrent == 0) {
        actUndo.setText("&Undo");
    } else {
        actUndo.setText(QString::fromStdString("&Undo '" + cmd[cmdCurrent - 1]->toString() + "'"));
    }

    if (cmdCurrent == cmd.size()) {
        actRedo.setText("&Redo");
    } else {
        actRedo.setText(QString::fromStdString("&Redo '" + cmd[cmdCurrent]->toString() + "'"));
    }

    if (model.lbx->path().size() == 0) {
        setWindowTitle("lbx-tool");
    } else {
        QString title = QFileInfo(QString::fromStdString(model.lbx->path())).fileName();
        title = "lbx-tool - " + title;
        if (cmdCurrent != cmdSaved) {
            title = "~" + title;
        }
        setWindowTitle(title);
    }

    if (model.lbx->path().size() == 0) {
        statusPath.setText("New File");
    } else {
        statusPath.setText(QString::fromStdString(" " + model.lbx->path()));
    }
    statusFiles.setText(QString("%1 Files").arg(model.lbx->size()));
}

void Window::onContextMenu(const QPoint& _pos) {
    QMenu menu(this);
    menu.addAction(&actCopy);
    menu.addAction(&actPaste);
    menu.addAction(&actCut);
    menu.addAction(&actDelete);
    menu.addSeparator();
    menu.addAction(&actExtract);
    menu.addAction(&actInsert);
    menu.exec(mapToGlobal(_pos), 0);
}

void Window::onSelect() {
    bool s = hasSelection();
    actCopy.setEnabled(s);
    actCut.setEnabled(s);
    actDelete.setEnabled(s);
    actExtract.setEnabled(s);
}

char* Window::copy(char* _data, uint32_t _size) {
    char* d = new char[_size];
    for (size_t i = 0; i < _size; ++i) {
        d[i] = _data[i];
    }
    return d;
}

bool Window::save(std::string _path, std::pair<char*, uint32_t>* _content) {
    std::ofstream o(_path);
    if (!o.good()) {
        Model::error("could not open '" + _path + "'.");
        return false;
    }
    o.write(_content->first, _content->second);
    return true;
}

std::pair<char*, uint32_t> Window::read(std::string _path) {
    std::pair<char*, uint32_t> p;
    p.first = NULL;
    p.second = 0;

    std::ifstream f(_path, std::ifstream::ate);
    if (!f.good()) {
        Model::error("could not open '" + _path + "'.");
        return p;
    }

    p.second = (uint32_t) f.tellg();
    f.seekg(0, std::ifstream::beg);
    p.first = new char[p.second];
    f.read(p.first, p.second);
    return p;
}

void Window::onActivated(const QModelIndex& /* _index */) {
    onExtract();
}

void Window::onSelectionChanged(const QItemSelection& /* _selected */, const QItemSelection& /* _deselected */) {
    onSelect();
}

bool Window::hasSelection() {
    return sel->selection().size() == 1;
}

uint16_t Window::selection() {
    return (uint16_t) sel->selection()[0].indexes().at(0).row();
}

void Window::onAbout() {
    QMessageBox::about(this, "about lbx-tool", "This is free software licensed under the GNU General Public License version 3.<br>For more information visit <a href=\"https://github.com/MarcoKull/lbx-tool\">the project's github page</a>.");
}

void Window::onCopy() {
    uint16_t s = selection();
    delete[] copyData;
    copySize = model.lbx->at(s).second;
    copyData = copy(model.lbx->at(s).first, copySize);
    copyDesc = model.desc[s] + " (copy)";
    actPaste.setEnabled(true);
    actPaste.setText(QString::fromStdString("&Paste '" + model.desc[s] + "'"));
}

void Window::onCut() {
    onCopy();
    onDelete();
}

void Window::onDelete() {
    exec(new CommandDelete(&model, selection()));
}

void Window::onExtract() {
    std::string path = QFileDialog::getSaveFileName(
            this,
            "Save File",
            QDir::currentPath() + QDir::separator() + QString::fromStdString(model.desc.at(selection()) + ".bin"),
            "All Files(*.*)").toStdString();

    if (path.size() > 0) {
        save(path, &model.lbx->at(selection()));
    }
}

void Window::onExtractAll() {
    std::string path = QString(QFileDialog::getExistingDirectory(this, "Select Target Directory", QDir::currentPath()) + QDir::separator()).toStdString();
    if (path.size() > 0) {
        for (uint16_t i = 0; i < model.lbx->size(); ++i) {
            save(path + model.desc[i] + ".bin", &model.lbx->at(i));
        }
    }

}

void Window::onInsert() {
    std::string path = QFileDialog::getOpenFileName(
            this,
            "Open File",
            QDir::currentPath(),
            "All Files(*.*)").toStdString();

    if (path.size() > 0) {
        std::pair<char*, uint32_t> p = read(path);
        if (p.first != NULL) {
            uint16_t sel = model.lbx->size();
            if (hasSelection()) {
                sel = selection();
            }

            exec(new CommandInsert(&model, sel, p.first, p.second, QFileInfo(QString::fromStdString(path)).fileName().toStdString()));
        }
    }
}

void Window::onNew() {
    Window* w = new Window();
    w->show();
}

void Window::onOpen() {
    std::string path = QFileDialog::getOpenFileName(
            this,
            "Open LBX File",
            QDir::currentPath(),
            "LBX Files(*.lbx *.LBX)").toStdString();

    if (path.size() > 0) {
        open(path);
    }
}

void Window::onPaste() {
    uint16_t i;
    if (hasSelection()) {
        i = selection();
    } else {
        i = model.lbx->size();
    }
    exec(new CommandInsert(&model, i, copy(copyData, copySize), copySize, copyDesc));
}

void Window::onQuit() {
    close();
}

void Window::onRedo() {
    cmd[cmdCurrent++]->exec();
    onDo();
}

void Window::onSave() {
    if (model.save()) {
        cmdSaved = cmdCurrent;
        onDo();
    }
}

void Window::onSaveAs() {
    std::string path = QFileDialog::getSaveFileName(
            this,
            "Save LBX File",
            QFileInfo(QString::fromStdString(model.lbx->path())).dir().path(),
            "LBX Files(*.lbx *.LBX)").toStdString();

    if (path.size() > 0) {
        if (model.save(path)) {
            cmdSaved = cmdCurrent;
            onDo();
        }
    }
}

void Window::onUndo() {
    cmd[--cmdCurrent]->undo();
    onDo();
}

void Window::onUnknown0() {
    bool ok;
    int u0 = QInputDialog::getInt(this, "Unknown0", "A value with the size of two byte.<br>Most of the time it is zero, but its meaning is unknown.", model.lbx->unknown0(), 0, UINT16_MAX, 1, &ok);
    if (ok) {
        exec(new CommandUnknown0(&model, (uint16_t) u0));
    }
}

void Window::onUnknown1Open() {
    std::string path = QFileDialog::getOpenFileName(
            this,
            "Open File",
            QDir::currentPath(),
            "All Files(*.*)").toStdString();

    if (path.size() > 0) {
        std::pair<char*, uint32_t> p = read(path);
        if (p.first != NULL) {
            exec(new CommandUnknown1(&model, p.first, p.second));
        }
    }
}

void Window::onUnknown1Save() {
    std::string path = QFileDialog::getSaveFileName(
            this,
            "Save File",
            QDir::currentPath() + QDir::separator() + "unknown1.bin",
            "All Files(*.*)").toStdString();

    if (path.size() > 0) {
        save(path, &model.lbx->unknown1());
    }
}