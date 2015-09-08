#pragma once
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <ProcessInterface/TimeValue.hpp>
#include <ProcessInterface/ExpandMode.hpp>
#include "Tools/dataStructures.hpp"
#include "Process/ScenarioModel.hpp"
#include "Process/Algorithms/StandardDisplacementPolicy.hpp"
#include "Process/Algorithms/VerticalMovePolicy.hpp"

class EventModel;
class TimeNodeModel;
class ConstraintModel;
class ConstraintViewModel;
class RackModel;
class ScenarioModel;
struct ElementsProperties;

#include <tests/helpers/ForwardDeclaration.hpp>


namespace Scenario
{
    namespace Command
    {
        template<class DisplacementPolicy>
        class MoveEvent2 : public iscore::SerializableCommand
        {
                // No ISCORE_COMMAND here since it's a template.

#include <tests/helpers/FriendDeclaration.hpp>
            public:
                static const char * commandName()
                {
                    static QByteArray name = QString{"MoveEvent2_%1"}.arg(DisplacementPolicy::name()).toLatin1();
                    return name.constData();
                }
                static QString description()
                {
                    return QObject::tr("Move Event 2 With %1").arg(DisplacementPolicy::name());
                }
                static auto static_uid()
                {
                    using namespace std;
                    hash<string> fn;
                    return fn(std::string(commandName()));
                }
                ISCORE_SERIALIZABLE_COMMAND_DEFAULT_CTOR_OBSOLETE(MoveEvent2, "ScenarioControl")
                /**
                 * @brief MoveEvent2
                 * @param scenarioPath
                 * @param eventId
                 * @param newDate
                 * !!! in the future it would be better to give directly the delta time of the mouse displacement  !!!
                 * @param mode
                 */
                MoveEvent2(
                    Path<ScenarioModel>&& scenarioPath,
                    const Id<EventModel>& eventId,
                    const TimeValue& newDate,
                    ExpandMode mode)
                :
                SerializableCommand {"ScenarioControl", commandName(), description()},
                m_path {std::move(scenarioPath)},
                m_mode{mode}
                {
                    m_eventId = eventId;
                    m_initialDate = newDate;
                    m_oldDate = m_initialDate;

                    update(
                            m_path,
                            eventId,
                            newDate,
                            m_mode);

                }
        void
        update(
                const Path<ScenarioModel>& scenarioPath,
                const Id<EventModel>& eventId,
                const TimeValue& newDate,
                ExpandMode mode)
        {
            TimeValue deltaDate = newDate - m_oldDate;
            m_oldDate = newDate;

            auto& scenario = m_path.find();

            //NOTICE: multiple event displacement functionnality already available this is "retro" compatibility
            QVector <Id<TimeNodeModel>> draggedElements;
            draggedElements.push_back(scenario.event(eventId).timeNode());// retrieve corresponding timenode and store it in array

            DisplacementPolicy::computeDisplacement(scenario, draggedElements, deltaDate, m_savedElementsProperties);

        }
                virtual
                void
                undo() override
                {
                    auto& scenario = m_path.find();

                    m_oldDate = m_initialDate;

                    bool useNewValues{false};

                    DisplacementPolicy::updatePositions(
                                scenario,
                                [&] (Process& p, const TimeValue& t){ p.expandProcess(m_mode, t); },
                                m_savedElementsProperties,
                                useNewValues);

                    updateEventExtent(m_eventId, scenario);

                }
                virtual
                void
                redo() override
                {
                    auto& scenario = m_path.find();

                    bool useNewValues{true};

                    DisplacementPolicy::updatePositions(
                                scenario,
                                [&] (Process& p, const TimeValue& t){ p.expandProcess(m_mode, t); },
                                m_savedElementsProperties,
                                useNewValues);

                    updateEventExtent(m_eventId, scenario);
                }



                const Path<ScenarioModel>& path() const
                { return m_path; }

            protected:

                virtual
                void
                serializeImpl(QDataStream& s) const override
                {
                    s << m_savedElementsProperties
                      << m_path
                      << m_eventId
                      << m_initialDate
                      << m_oldDate
                      << (int)m_mode;
                }

                virtual
                void
                deserializeImpl(QDataStream& s) override
                {
                    int mode;
                    s >> m_savedElementsProperties
                      >> m_path
                      >> m_eventId
                      >> m_initialDate
                      >> m_oldDate
                      >> mode;

                      m_mode = static_cast<ExpandMode>(mode);
                }

                private:
            private:
                ElementsProperties m_savedElementsProperties;
                Path<ScenarioModel> m_path;

                Id<EventModel> m_eventId;
                TimeValue m_oldDate; //used to compute the deltaTime
                TimeValue m_initialDate; //used to compute the deltaTime and respect undo behavior

                ExpandMode m_mode{ExpandMode::Scale};
        };

    }
}