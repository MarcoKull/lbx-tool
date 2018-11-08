#include <QFileInfo>
#include <QMessageBox>

#include "Model.h"

LbxToolGui::Model::Model(QObject* _parent) : QAbstractTableModel(_parent) {
    lbx = new LbxFile();
}

LbxToolGui::Model::~Model() {
    delete lbx;
}

int LbxToolGui::Model::rowCount(const QModelIndex& /*_parent*/) const {
    return lbx->size();
}

int LbxToolGui::Model::columnCount(const QModelIndex& /*_parent*/) const {
    return 2;
}

QVariant LbxToolGui::Model::headerData(int _section, Qt::Orientation _orientation, int _role) const {
    if (_role != Qt::DisplayRole) {
        return QVariant();
    }

    if (_orientation == Qt::Horizontal) {
        switch (_section) {
            case 0:
                return tr("Information");
            case 1:
                return tr("Size");
            default:
                return QVariant();
        }
    }
    return _section + 1;
}

Qt::ItemFlags LbxToolGui::Model::flags(const QModelIndex& index) const {
    return QAbstractTableModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QVariant LbxToolGui::Model::data(const QModelIndex& _index, int _role) const {
    if (_role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (_index.column()) {
        case 0:
            return QString::fromStdString(desc.at((size_t)_index.row()));
        case 1:
            return QString::number(lbx->at((uint16_t) _index.row()).size);
        default:
            return QVariant();
    }
}

void LbxToolGui::Model::remove(uint16_t _index) {
    beginRemoveRows(QModelIndex(), _index, _index);
    lbx->remove(_index);
    desc.erase(desc.begin() + _index);
    endRemoveRows();
}

void LbxToolGui::Model::insert(uint16_t _index, char* _data, uint32_t _size, std::string _desc) {
    beginInsertRows(QModelIndex(), _index, _index);
    lbx->insert(_index, {_data, _size});
    desc.insert(desc.begin() + _index, _desc);
    endInsertRows();
}

void LbxToolGui::Model::error(std::string _msg) {
    QMessageBox::critical(NULL, "error", QString::fromStdString("<b>Error:</b><br><br>" + _msg + "."));
}

bool LbxToolGui::Model::open(std::string _path) {
    beginResetModel();
    bool ret = true;
    try {
        lbx->open(_path);
        desc.clear();
        std::string name = QFileInfo(QString::fromStdString(lbx->path())).fileName().toStdString();
        for (uint16_t i = 0; i < lbx->size(); ++i) {
            desc.push_back(name + " [" + QString::number(i + 1).toStdString() + "]");
        }
    } catch (std::exception& e) {
        error(e.what());
        ret = false;
    }
    endResetModel();
    return ret;
}

bool LbxToolGui::Model::save() {
    try {
        lbx->save();
        return true;
    } catch (std::exception& e) {
        error(e.what());
        return false;
    }
}

bool LbxToolGui::Model::save(std::string _path) {
    try {
        lbx->save(_path);
        return true;
    } catch (std::exception& e) {
        error(e.what());
        return false;
    }
}