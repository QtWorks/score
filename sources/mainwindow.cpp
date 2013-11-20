/*
Copyright: LaBRI / SCRIME

Authors : Jaime Chao, Clément Bossut (2013-2014)

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "timeevent.hpp"
#include "graphicstimebox.hpp"
#include "itemtypes.hpp"

#include <QMouseEvent>
#include <QActionGroup>
#include <QGraphicsView>
#include <QStateMachine>
#include <QPointF>
#include <QGraphicsLineItem>
#include <QTimer>
#include <QFinalState>

const qint16 OFFSET_INCREMENT = 5;

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow), _scene(NULL)
{
  ui->setupUi(this);
  setWindowTitle(tr("%1").arg(QApplication::applicationName()));

  createGraphics();
  createActionGroups();
  createStates();
  createTransitions();
  createConnections();

  QTimer::singleShot(0, _stateMachine, SLOT(start())); /// Using a single shot timer to ensure that the window is fully constructed before we start processing
}

void MainWindow::createGraphics()
{
  _view = ui->graphicsView;

 /*! @todo uncomment when Timebox achieved
  * _mainProcess = new GraphicsTimeBox();
  * Q_CHECK_PTR(_mainProcess);
  * _scene = _mainProcess->scene();
  */

  _scene = new QGraphicsScene(this);
  Q_CHECK_PTR(_scene);
  _view->setScene(_scene);
}

void MainWindow::createActionGroups() /// @todo Faire un stateMachine dédié pour gestion de la actionBar à la omnigraffle
{
  // GraphicsItems relative's actions
  ui->actionAddTimeEvent->setData(EventItemType);
  ui->actionAddTimeBox->setData(BoxItemType);

  _mouseActionGroup = new QActionGroup(this); // actiongroup keeping all mouse relatives actions
  _mouseActionGroup->addAction(ui->actionAddTimeEvent);
  _mouseActionGroup->addAction(ui->actionAddTimeBox);
  /// @bug QActionGroup always return 0 in checkedAction() if we set m_mouseActionGroup->setExclusive(false);

  // Mouse cursor relative's actions
  ui->actionMouse->setData(QGraphicsView::NoDrag);
  ui->actionScroll->setData(QGraphicsView::ScrollHandDrag);
  ui->actionSelect->setData(QGraphicsView::RubberBandDrag);

  _mouseActionGroup->addAction(ui->actionMouse);
  _mouseActionGroup->addAction(ui->actionScroll);
  _mouseActionGroup->addAction(ui->actionSelect);

  ui->actionMouse->setChecked(true);
}

void MainWindow::createStates()
{
  _stateMachine = new QStateMachine(this);

  _initialState = new QState();
  _initialState->assignProperty(this, "objectName", tr("mainWindow"));
  _initialState->assignProperty(this, "currentFullView", qVariantFromValue((void *)_mainProcess)); /// @todo Peut etre trop compliqué pour pas grand chose. sinon http://blog.bigpixel.ro/2010/04/storing-pointer-in-qvariant/
  _initialState->assignProperty(_mouseActionGroup, "enabled", true);
  _stateMachine->addState(_initialState);
  _stateMachine->setInitialState(_initialState);

  // creating a new top-level state
  _normalState = new QState();
  _editionState = new QState(_normalState);
  _editionState->assignProperty(_mouseActionGroup, "enabled", true);

  /// @todo create a state when changing the _currenFullView. do it history state or parallel (because can occur during execution or editing)
  _executionState = new QState(_normalState);
  _executionState->assignProperty(_mouseActionGroup, "enabled", false);

  _runningState = new QState(_executionState);
  _pausedState = new QState(_executionState);
  _stoppedState = new QState(_executionState);
  _executionState->setInitialState(_runningState);

  _normalState->setInitialState(_editionState);
  _stateMachine->addState(_normalState);

  _finalState = new QFinalState(); /// @todo gérer le final state et la suppression d'objets graphiques
  _stateMachine->addState(_finalState);
}

void MainWindow::createTransitions()
{
  _initialState->addTransition(_initialState, SIGNAL(propertiesAssigned()), _normalState);
  _editionState->addTransition(ui->playButton, SIGNAL(clicked()), _runningState);
  _runningState->addTransition(ui->playButton, SIGNAL(clicked()), _pausedState);
  _runningState->addTransition(ui->stopButton, SIGNAL(clicked()), _stoppedState);
  _pausedState->addTransition(ui->playButton, SIGNAL(clicked()), _runningState);
  _pausedState->addTransition(ui->stopButton, SIGNAL(clicked()), _stoppedState);
  _stoppedState->addTransition(_stoppedState, SIGNAL(propertiesAssigned()), _editionState);
  _normalState->addTransition(this, SIGNAL(suppress()), _finalState);
}

void MainWindow::createConnections()
{
  connect(ui->graphicsView, SIGNAL(mousePressAddItem(QPointF)), this, SLOT(addItem(QPointF)));
  connect(_mouseActionGroup, SIGNAL(triggered(QAction*)), ui->graphicsView, SLOT(mouseDragMode(QAction*)));
  connect(ui->graphicsView, SIGNAL(mousePosition(QPointF)), this, SLOT(setMousePosition(QPointF)));
}

void MainWindow::setDirty(bool on)
{
    setWindowModified(on);
    updateUi();
}

void MainWindow::updateUi()
{
 /// @todo Update actions to reflect application state
 // ui->actionSelect->setChecked();
}

void MainWindow::addItem(QPointF pos)
{
  QAction *action = _mouseActionGroup->checkedAction();
  Q_ASSERT(action);
  if(action != ui->actionAddTimeEvent && action != ui->actionAddTimeBox) { /// @todo  pas très découplé mais à cause de la galère des groupActions, regarder signalMapper
      return;
    }

  qint32 type = action->data().toInt(); // we recover the data associated with the action (see createActionGroups())
  QObject *item = NULL;

  Q_ASSERT(type);
  if (type == EventItemType) {
      item = new TimeEvent(pos, 0);
    }
  else if(type == BoxItemType) {

      /*! @todo uncomment when Timebox is achieved
      item = new TimeboxModel(pos, 200, 300);
      QGraphicsItem* graphicItem = qobject_cast<QGraphicsItem*>(item);

      TimeboxHeader *plugin = new TimeboxHeader(graphicItem); // create and position a plugin according to his parent (graphicItem)
      new TimeboxStoreyBar(graphicItem);

      _scene->clearSelection(); /// @todo Faut-il vraiment garder la QGScene parent dans la classe gTP ? si OUI la renommer parentQGScene.
      _scene->addItem(graphicItem);
      graphicItem->setSelected(true);
      */
    }
  ui->actionMouse->setChecked(true); /// @todo Pas joli, à faire dans la méthode dirty ou  dans un stateMachine

  Q_CHECK_PTR(item);
  if(item) {
      connectItem(item);
      setDirty(true);
    }
}

void MainWindow::connectItem(QObject *item)
{
    connect(item, SIGNAL(dirty()), this, SLOT(setDirty()));

    const QMetaObject *metaObject = item->metaObject();
    if (metaObject->indexOfSignal("playOrPauseButtonClicked()") > -1)
      connect(ui->playButton, SIGNAL(clicked()), item, SIGNAL(playOrPauseButtonClicked())); /// @todo rename playButton to playOrPauseButton and transform it
    if (metaObject->indexOfSignal("stopButtonClicked()") > -1)
      connect(ui->stopButton, SIGNAL(clicked()), item, SIGNAL(stopButtonClicked()));
    if(metaObject->indexOfSignal("headerClicked()") > -1)
      connect(item, SIGNAL(headerClicked()), ui->graphicsView, SLOT(graphicItemEnsureVisible()));
//    if (metaObject->indexOfProperty("running") > -1) /// @todo change play button to play/pause
//      connect(ui->playButton, SIGNAL(clicked()), item, SLOT(setrunning(true;)));
//    if (metaObject->indexOfProperty("stopped"))
//      connect(ui->stopButton, SIGNAL(clicked()), item, SLOT(setstopped(false;)));
}

void MainWindow::setMousePosition(QPointF point)
{
  statusBar()->showMessage(QString("position : %1 %2").arg(point.x()).arg(point.y()));
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
  QMainWindow::mousePressEvent(event);
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
  QMainWindow::mouseMoveEvent(event);
}

void MainWindow::setcurrentFullView(GraphicsTimeBox* arg)
{
  if (_currentFullView != arg) {
      _currentFullView = arg;
      emit currentFullViewChanged(arg);
    }
}
