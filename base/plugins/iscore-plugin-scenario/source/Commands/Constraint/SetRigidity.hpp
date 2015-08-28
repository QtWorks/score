#pragma once
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/ModelPath.hpp>

#include <tests/helpers/ForwardDeclaration.hpp>
#include <ProcessInterface/TimeValue.hpp>
class ConstraintModel;
namespace Scenario
{
    namespace Command
    {
        /**
         * @brief The SetRigidity class
         *
         * Sets the rigidity of a constraint
         */
        class SetRigidity : public iscore::SerializableCommand
        {
                ISCORE_COMMAND_DECL_OBSOLETE("SetRigidity", "SetRigidity")
#include <tests/helpers/FriendDeclaration.hpp>

            public:
                ISCORE_SERIALIZABLE_COMMAND_DEFAULT_CTOR_OBSOLETE(SetRigidity, "ScenarioControl")
                SetRigidity(
                    Path<ConstraintModel>&& constraintPath,
                    bool rigid);

                virtual void undo() override;
                virtual void redo() override;

            protected:
                virtual void serializeImpl(QDataStream&) const override;
                virtual void deserializeImpl(QDataStream&) override;

            private:
                Path<ConstraintModel> m_path;

                bool m_rigidity {};

                // Unused if the constraint was rigid
                TimeValue m_oldMinDuration;
                TimeValue m_oldMaxDuration;
        };
    }
}
