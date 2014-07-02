#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QObject>
#include <QGridLayout>
#include <QFileDialog>
#include <QDir>

#include "sceneViewer.h"

class Mediator : public QObject
{
Q_OBJECT
public:
    Mediator();
    ~Mediator();

    void initViewer();
    void initSignalSlot();
    void initUserInterface();

public slots:
    void about();
    void quit();

private:
    std::shared_ptr<QMainWindow> _mainWindow;
    std::shared_ptr<SceneViewer> _sceneViewer;
    std::shared_ptr<QGridLayout> _viewerLayout;
    Ui_MainWindow _userInterface;
};

#endif // MEDIATOR_H
