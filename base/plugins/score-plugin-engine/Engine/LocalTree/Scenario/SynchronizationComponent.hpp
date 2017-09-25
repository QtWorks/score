#pragma once
#include <Engine/LocalTree/LocalTreeComponent.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>

namespace Engine
{
namespace LocalTree
{
class Synchronization final : public CommonComponent
{
  COMMON_COMPONENT_METADATA("104e4446-b09f-4bf6-92ef-0fe360397066")
public:
  Synchronization(
      ossia::net::node_base& parent,
      const Id<score::Component>& id,
      Scenario::SynchronizationModel& event,
      DocumentPlugin& doc,
      QObject* parent_comp);
};
}
}