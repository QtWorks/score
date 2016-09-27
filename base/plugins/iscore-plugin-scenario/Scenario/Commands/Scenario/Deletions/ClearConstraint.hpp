#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <QByteArray>
#include <QMap>
#include <QVector>
#include <Scenario/Tools/dataStructures.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore_plugin_scenario_export.h>


struct DataStreamInput;
struct DataStreamOutput;
namespace Scenario
{
class ConstraintModel;
class ConstraintViewModel;
class RackModel;
namespace Command
{
/**
         * @brief The ClearConstraint class
         *
         * Removes all the processes and the rackes of a constraint.
         */
class ISCORE_PLUGIN_SCENARIO_EXPORT ClearConstraint final : public iscore::SerializableCommand
{
        ISCORE_COMMAND_DECL(ScenarioCommandFactoryName(), ClearConstraint, "Clear a constraint")
        public:
            ClearConstraint(const ConstraintModel& constraintPath);
        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput&) const override;
        void deserializeImpl(DataStreamOutput&) override;

    private:
        ConstraintSaveData m_constraintSaveData;
};
}
}
