// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QBoxLayout>
#include <QPushButton>
#include <Scenario/Commands/Synchronization/SetTrigger.hpp>
#include <algorithm>
#include <score/widgets/MarginLess.hpp>

#include "TriggerInspectorWidget.hpp"
#include <Inspector/InspectorWidgetBase.hpp>
#include <QInputDialog>
#include <QMenu>
#include <Scenario/Commands/Synchronization/TriggerCommandFactory/TriggerCommandFactoryList.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <Scenario/Inspector/Expression/ExpressionEditorWidget.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/model/path/Path.hpp>

namespace Scenario
{

TriggerInspectorWidget::TriggerInspectorWidget(
    const score::DocumentContext& doc,
    const Command::TriggerCommandFactoryList& fact,
    const SynchronizationModel& object,
    Inspector::InspectorWidgetBase* parent)
    : QWidget{parent}
    , m_triggerCommandFactory{fact}
    , m_model{object}
    , m_parent{parent}
    , m_menu{[&] { return m_model.expression(); }, this}
{
  auto triglay = new score::MarginLess<QHBoxLayout>{this};

  m_exprEditor = new ExpressionEditorWidget{doc, this};
  connect(
      m_exprEditor, &ExpressionEditorWidget::editingFinished, this,
      &TriggerInspectorWidget::on_triggerChanged);
  con(m_model, &SynchronizationModel::triggerChanged, m_exprEditor,
      &ExpressionEditorWidget::setExpression);

  m_addTrigBtn = new QPushButton{tr("Enable trigger")};

  m_exprEditor->setMenu(m_menu.menu);

  triglay->addWidget(m_addTrigBtn);
  triglay->addWidget(m_exprEditor);

  on_triggerActiveChanged();

  connect(
      m_addTrigBtn, &QPushButton::released, this,
      &TriggerInspectorWidget::createTrigger);

  connect(
      m_menu.deleteAction, &QAction::triggered, this,
      &TriggerInspectorWidget::removeTrigger);
  con(m_menu, &ExpressionMenu::expressionChanged, this, [=] (const QString&
                                                                str) {
    auto trig = State::parseExpression(str);
    if (!trig)
    {
      trig = State::defaultTrueExpression();
    }

    if (*trig != m_model.expression())
    {
      auto cmd = new Scenario::Command::SetTrigger{m_model, std::move(*trig)};
      m_parent->commandDispatcher()->submitCommand(cmd);
    }
  });
  con(m_model, &SynchronizationModel::activeChanged, this,
      &TriggerInspectorWidget::on_triggerActiveChanged);
}

void TriggerInspectorWidget::on_triggerChanged()
{
  auto trig = m_exprEditor->expression();

  if (trig != m_model.expression())
  {
    auto cmd = new Scenario::Command::SetTrigger{m_model, std::move(trig)};
    m_parent->commandDispatcher()->submitCommand(cmd);
  }
}

void TriggerInspectorWidget::createTrigger()
{
  m_exprEditor->setVisible(true);
  m_addTrigBtn->setVisible(false);

  auto cmd = m_triggerCommandFactory.make(
      &Scenario::Command::TriggerCommandFactory::make_addTriggerCommand,
      m_model);
  if (cmd)
    m_parent->commandDispatcher()->submitCommand(cmd);
}

void TriggerInspectorWidget::removeTrigger()
{
  m_exprEditor->setVisible(false);
  m_addTrigBtn->setVisible(true);

  auto cmd = m_triggerCommandFactory.make(
      &Scenario::Command::TriggerCommandFactory::make_removeTriggerCommand,
      m_model);
  if (cmd)
    m_parent->commandDispatcher()->submitCommand(cmd);
}

void TriggerInspectorWidget::on_triggerActiveChanged()
{
  bool v = m_model.active();
  m_exprEditor->setVisible(v);
  m_addTrigBtn->setVisible(!v);
}

void TriggerInspectorWidget::updateExpression(const State::Expression& expr)
{
  m_exprEditor->setExpression(expr);
}
}