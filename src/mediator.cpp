#include "mediator.h"

Mediator::Mediator()
{
    _mainWindow.reset(new QMainWindow());
    _userInterface.setupUi(_mainWindow.get());

    _sceneViewer.reset(new SceneViewer(&_userInterface));

    initViewer();
    initSignalSlot();
    initUserInterface();

    _mainWindow->show();
}

Mediator::~Mediator()
{

}

void Mediator::initViewer()
{
    _viewerLayout.reset(new QGridLayout());
    _viewerLayout->setMargin(0);
    _viewerLayout->addWidget(_sceneViewer.get());
    _userInterface.widgetScenePlayer->setLayout(_viewerLayout.get());
}

void Mediator::initSignalSlot()
{
    //File
    connect(_userInterface.actionQuit, SIGNAL(triggered()), _mainWindow.get(), SLOT(close()));
}

void Mediator::initUserInterface()
{
}
