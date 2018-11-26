#include <QFileInfo>
#include <QMessageBox>

#include "Model.h"

using namespace LbxToolGui;

Model::Model(QObject* _parent) : QAbstractTableModel(_parent) {
    lbx = new LbxFile();
    isViewOffsets = false;
}

Model::~Model() {
    delete lbx;
}

int Model::rowCount(const QModelIndex& /*_parent*/) const {
    return lbx->size();
}

int Model::columnCount(const QModelIndex& /*_parent*/) const {
    if (isViewOffsets) {
        return 3;
    }
    return 2;
}

QVariant Model::headerData(int _section, Qt::Orientation _orientation, int _role) const {
    if (_role != Qt::DisplayRole) {
        return QVariant();
    }

    if (_orientation == Qt::Horizontal) {
        if (isViewOffsets) {
            switch (_section) {
                case 0:
                    return tr("Information");
                case 1:
                    return tr("Offset");
                case 2:
                    return tr("Size");
                default:
                    return QVariant();
            }
        } else {
            switch (_section) {
                case 0:
                    return tr("Information");
                case 1:
                    return tr("Size");
                default:
                    return QVariant();
            }
        }
    }
    return _section + 1;
}

Qt::ItemFlags Model::flags(const QModelIndex& index) const {
    return QAbstractTableModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QVariant Model::data(const QModelIndex& _index, int _role) const {
    if (_role != Qt::DisplayRole) {
        return QVariant();
    }

    if (isViewOffsets) {
        switch (_index.column()) {
            case 0:
                return QString::fromStdString(desc.at((size_t) _index.row()));
            case 1:
                return QString::number(lbx->offset((uint16_t) _index.row()));
            case 2:
                return QString::number(lbx->at((uint16_t) _index.row()).second);
            default:
                return QVariant();
        }
    } else {
        switch (_index.column()) {
            case 0:
                return QString::fromStdString(desc.at((size_t) _index.row()));
            case 1:
                return QString::number(lbx->at((uint16_t) _index.row()).second);
            default:
                return QVariant();
        }
    }
}

void Model::remove(uint16_t _index) {
    beginRemoveRows(QModelIndex(), _index, _index);
    lbx->remove(_index);
    desc.erase(desc.begin() + _index);
    endRemoveRows();
}

void Model::insert(uint16_t _index, char* _data, uint32_t _size, std::string _desc) {
    beginInsertRows(QModelIndex(), _index, _index);
    lbx->insert(_index,{_data, _size});
    desc.insert(desc.begin() + _index, _desc);
    endInsertRows();
}

void Model::error(std::string _msg) {
    QMessageBox::critical(NULL, "error", QString::fromStdString("<b>Error:</b><br><br>" + _msg + "."));
}

bool Model::open(std::string _path) {
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

bool Model::save() {
    try {
        lbx->save();
        return true;
    } catch (std::exception& e) {
        error(e.what());
        return false;
    }
}

bool Model::save(std::string _path) {
    try {
        lbx->save(_path);
        return true;
    } catch (std::exception& e) {
        error(e.what());
        return false;
    }
}

bool Model::getIsViewOffsets() {
    return isViewOffsets;
}

void Model::setIsViewOffsets(bool _isViewOffsets) {
    beginResetModel();
    isViewOffsets = _isViewOffsets;
    endResetModel();
}
