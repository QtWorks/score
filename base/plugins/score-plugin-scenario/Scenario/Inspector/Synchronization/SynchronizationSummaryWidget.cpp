// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "SynchronizationSummaryWidget.hpp"

#include <score/document/DocumentContext.hpp>
#include <score/widgets/TextLabel.hpp>

#include <QLabel>

#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <Scenario/Inspector/SelectionButton.hpp>
#include <State/Expression.hpp>

namespace Scenario
{
SynchronizationSummaryWidget::SynchronizationSummaryWidget(
    const SynchronizationModel& object,
    const score::DocumentContext& doc,
    QWidget* parent)
    : QWidget(parent)
    , m_selectionDispatcher{doc.selectionStack}
    , m_lay{this}
{
  auto eventBtn
      = SelectionButton::make("", &object, m_selectionDispatcher, this);

  m_lay.addWidget(new TextLabel{object.metadata().getName()}, 0, 0, 1, 3);
  m_lay.addWidget(new TextLabel{object.date().toString()}, 0, 3, 1, 3);
  m_lay.addWidget(eventBtn, 0, 6, 1, 1);

  if (!object.expression().toString().isEmpty())
  {
    auto cond = new TextLabel{object.expression().toString()};
    cond->setWordWrap(true);
    m_lay.addWidget(cond, 1, 1, 1, 6);
  }
}
}