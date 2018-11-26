#ifndef LBXTOOLGUI_MODEL_H
#define LBXTOOLGUI_MODEL_H

#include <QAbstractTableModel>
#include <lbx.h>

namespace LbxToolGui {

    class Model : public QAbstractTableModel {
        Q_OBJECT

    public:
        Model(QObject* parent);
        ~Model();

        bool open(std::string path);
        bool save();
        bool save(std::string path);
        void setIsViewOffsets(bool isViewOffsets);
        bool getIsViewOffsets();
        void remove(uint16_t index);
        void insert(uint16_t index, char* data, uint32_t size, std::string desc);

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;

        LbxFile* lbx;
        std::vector<std::string> desc;

        static void error(std::string msg);
        
    private:
        bool isViewOffsets;

    };

}

#endif /* LBXTOOLGUI_MODEL_H */
