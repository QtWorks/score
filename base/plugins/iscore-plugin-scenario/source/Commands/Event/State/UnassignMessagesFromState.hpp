#pragma once
#include <iscore/command/SerializableCommand.hpp>

namespace Scenario
{
    namespace Command
    {
        class UnassignMessagesFromState : public iscore::SerializableCommand
        {
                ISCORE_COMMAND_DECL_OBSOLETE("UnassignMessagesFromState", "UnassignMessagesFromState")
            public:
                ISCORE_SERIALIZABLE_COMMAND_DEFAULT_CTOR_OBSOLETE(UnassignMessagesFromState, "ScenarioControl")
                virtual void undo() override;
                virtual void redo() override;

            protected:
                virtual void serializeImpl(QDataStream&) const override;
                virtual void deserializeImpl(QDataStream&) override;
        };
    }
}
