#ifndef LBXTOOLGUI_WINDOW_H
#define LBXTOOLGUI_WINDOW_H

#include <QAction>
#include <QItemSelection>
#include <QLabel>
#include <QMainWindow>

#include <lbx.h>

#include "Command.h"
#include "Model.h"

namespace LbxToolGui {

    class Window : public QMainWindow {
        Q_OBJECT

    public:
        Window();
        bool open(std::string path);
        bool openNew(std::string path);

    private:
        Window* newWindow();
        void exec(Command* cmd);
        void onDo();
        void onSelect();
        bool save(std::string path, std::pair<char*, uint32_t>* content);
        std::pair<char*, uint32_t> read(std::string path);
        bool hasSelection();
        char* copy(char* data, uint32_t size);
        uint16_t selection();
        
        Model model;
        QItemSelectionModel* sel;

        QAction actAbout;
        QAction actCopy;
        QAction actCut;
        QAction actDelete;
        QAction actExtract;
        QAction actExtractAll;
        QAction actInsert;
        QAction actNew;
        QAction actOpen;
        QAction actPaste;
        QAction actQuit;
        QAction actRedo;
        QAction actSave;
        QAction actSaveAs;
        QAction actUndo;
        QAction actUnknown0;
        QAction actUnknown1Open;
        QAction actUnknown1Save;
        QAction actViewOffsets;

        QLabel statusPath;
        QLabel statusFiles;
        
        char* copyData;
        std::string copyDesc;
        uint32_t copySize;

        std::vector<Command*> cmd;
        size_t cmdCurrent;
        size_t cmdSaved;

    private slots:
        void onActivated(const QModelIndex& index);
        void onContextMenu(const QPoint& pos);
        void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

        void onAbout();
        void onCopy();
        void onCut();
        void onDelete();
        void onExtract();
        void onExtractAll();
        void onInsert();
        void onNew();
        void onOpen();
        void onPaste();
        void onQuit();
        void onRedo();
        void onSave();
        void onSaveAs();
        void onUndo();
        void onUnknown0();
        void onUnknown1Open();
        void onUnknown1Save();
        void onViewOffsets();
    };
}

#endif /* LBXTOOLGUI_WINDOW_H */

