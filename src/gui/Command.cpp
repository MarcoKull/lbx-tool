#include "Command.h"

// abstract base class

LbxToolGui::Command::Command(Model* _model) {
    model = _model;
}

LbxToolGui::Command::~Command() {

}

// abstract delete/insert class

LbxToolGui::CommandDeleteInsert::CommandDeleteInsert(Model* _model, uint16_t _index) : Command(_model) {
    index = _index;
}

LbxToolGui::CommandDeleteInsert::~CommandDeleteInsert() {
    delete[] content.first;
}

void LbxToolGui::CommandDeleteInsert::doInsert() {
    model->insert(index, content.first, content.second, desc);
    content.first = NULL;
}

void LbxToolGui::CommandDeleteInsert::doDelete() {
    desc = model->desc[index];
    content = model->lbx->at(index);
    model->lbx->at(index).first = NULL;
    model->remove(index);
}

// delete

LbxToolGui::CommandDelete::CommandDelete(Model* _model, uint16_t _index) : CommandDeleteInsert(_model, _index) {
}

LbxToolGui::CommandDelete::~CommandDelete() {
}

void LbxToolGui::CommandDelete::exec() {
    doDelete();
}

void LbxToolGui::CommandDelete::undo() {
    doInsert();
}

std::string LbxToolGui::CommandDelete::toString() {
    return "delete " + desc;
}

// insert

LbxToolGui::CommandInsert::CommandInsert(Model* _model, uint16_t _index, char* _data, uint32_t _size, std::string _desc) : CommandDeleteInsert(_model, _index) {
    content.first = _data;
    content.second = _size;
    desc = _desc;
}

LbxToolGui::CommandInsert::~CommandInsert() {

}

void LbxToolGui::CommandInsert::exec() {
    doInsert();
}

void LbxToolGui::CommandInsert::undo() {
    doDelete();
}

std::string LbxToolGui::CommandInsert::toString() {
    return "insert " + desc;
}

// unknown0

LbxToolGui::CommandUnknown0::CommandUnknown0(Model* _model, uint16_t _value) : Command(_model) {
    value = _value;
}

LbxToolGui::CommandUnknown0::~CommandUnknown0() {

}

void LbxToolGui::CommandUnknown0::exec() {
    uint16_t v = model->lbx->unknown0();
    model->lbx->unknown0() = value;
    value = v;
}

void LbxToolGui::CommandUnknown0::undo() {
    exec();
}

std::string LbxToolGui::CommandUnknown0::toString() {
    return "changing unknown0";
}

// unknown1

LbxToolGui::CommandUnknown1::CommandUnknown1(Model* _model, char* _data, uint32_t _size) : Command(_model) {
    data = _data;
    size = _size;
}

LbxToolGui::CommandUnknown1::~CommandUnknown1() {
    delete[] data;
}

void LbxToolGui::CommandUnknown1::exec() {
    char* d = model->lbx->unknown1().first;
    uint32_t s = model->lbx->unknown1().second;
    model->lbx->unknown1().first = data;
    model->lbx->unknown1().second = size;
    data = d;
    size = s;
}

void LbxToolGui::CommandUnknown1::undo() {
    exec();
}

std::string LbxToolGui::CommandUnknown1::toString() {
    return "changing unknown1";
}
