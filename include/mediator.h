#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QObject>
#include <QGridLayout>
#include <QFileDialog>
#include <QDir>
#include <QProgressBar>

#include "sceneViewer.h"

class CustomProgressBar : public QProgressBar
{
public:
    CustomProgressBar() {}
    ~CustomProgressBar() {}

    void updateValue(const int value = 1) {this->setValue(this->value()+value);}
};

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
    //File
    void importGeometry();
    void quit();

    //Help
    void about();

private:
    std::shared_ptr<QMainWindow> _mainWindow;
    std::shared_ptr<SceneViewer> _sceneViewer;
    std::shared_ptr<QGridLayout> _viewerLayout;
    Ui_MainWindow _userInterface;

    std::shared_ptr<CustomProgressBar> _progressBar;

    void toggleProgressBar(const int max = 100);
};

#endif // MEDIATOR_H
