#ifndef COMMAND_H
#define COMMAND_H

#include "Model.h"

namespace LbxToolGui {

    class Command {
    public:
        Command(Model* model);
        virtual ~Command();

        virtual void exec() = 0;
        virtual void undo() = 0;
        virtual std::string toString() = 0;

    protected:
        Model* model;
    };

    class CommandDeleteInsert : public Command {
    public:
        CommandDeleteInsert(Model* model, uint16_t index);
        virtual ~CommandDeleteInsert();

    protected:
        void doDelete();
        void doInsert();
        uint16_t index;
        std::pair<char*, uint32_t> content;
        std::string desc;
    };

    class CommandDelete : public CommandDeleteInsert {
    public:
        CommandDelete(Model* model, uint16_t index);
        virtual ~CommandDelete();

        virtual void exec();
        virtual void undo();
        virtual std::string toString();
    };

    class CommandInsert : public CommandDeleteInsert {
    public:
        CommandInsert(Model* model, uint16_t index, char* data, uint32_t size, std::string desc);
        virtual ~CommandInsert();

        virtual void exec();
        virtual void undo();
        virtual std::string toString();
    };

    class CommandUnknown0 : public Command {
    public:
        CommandUnknown0(Model* model, uint16_t value);
        virtual ~CommandUnknown0();

        virtual void exec();
        virtual void undo();
        virtual std::string toString();

    private:
        uint16_t value;
    };

    class CommandUnknown1 : public Command {
    public:
        CommandUnknown1(Model* model, char* data, uint32_t size);
        virtual ~CommandUnknown1();

        virtual void exec();
        virtual void undo();
        virtual std::string toString();

    private:
        char* data;
        uint32_t size;
    };
}
#endif /* COMMAND_H */

