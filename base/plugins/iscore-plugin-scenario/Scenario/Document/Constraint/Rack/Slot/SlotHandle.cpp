#include <Process/Style/ScenarioStyle.hpp>
#include <QCursor>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QPoint>
#include <qnamespace.h>

#include "SlotHandle.hpp"
#include <Scenario/Document/Constraint/Rack/Slot/SlotPresenter.hpp>
#include <Scenario/Document/Constraint/Rack/Slot/SlotView.hpp>


class QWidget;

namespace Scenario
{
SlotHandle::SlotHandle(const SlotView& slotView, QQuickPaintedItem* parent)
    : GraphicsItem{parent}
    , m_slotView{slotView}
    , m_width{slotView.width()}
{
  //this->setCacheMode(QQuickPaintedItem::NoCache);
  this->setCursor(Qt::SizeVerCursor);
  m_pen.setWidth(0);
}

QRectF SlotHandle::boundingRect() const
{
  return {0, -handleHeight() / 2., m_width, handleHeight()};
}

void SlotHandle::paint(
    QPainter* painter)
{
  m_pen.setBrush(ScenarioStyle::instance().ProcessViewBorder.getColor());
  painter->setPen(m_pen);
  painter->setBrush(m_pen.color());

  painter->drawLine(0, -handleHeight() / 2., m_width, -handleHeight() / 2.);
}

void SlotHandle::mousePressEvent(QMouseEvent* event)
{
  m_slotView.presenter.pressed(mapToScene(event->localPos()));
}

void SlotHandle::mouseMoveEvent(QMouseEvent* event)
{
  m_slotView.presenter.moved(mapToScene(event->localPos()));
}

void SlotHandle::mouseReleaseEvent(QMouseEvent* event)
{
  m_slotView.presenter.released(mapToScene(event->localPos()));
}
}
