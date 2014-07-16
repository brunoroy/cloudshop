#include "mediator.h"

#include <QMessageBox>

Mediator::Mediator()
{
    _mainWindow.reset(new QMainWindow());
    _userInterface.setupUi(_mainWindow.get());

    _sceneViewer.reset(new SceneViewer(&_userInterface, &_transformsDialog, &_clippingDialog));
    _sceneViewer->stopAnimation();

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
    _userInterface.widgetSceneViewer->setLayout(_viewerLayout.get());
}

void Mediator::initSignalSlot()
{
    //File
    connect(_userInterface.actionImportGeometry, SIGNAL(triggered()), this, SLOT(importGeometry()));
    connect(_userInterface.actionImportTexture, SIGNAL(triggered()), this, SLOT(importTexture()));
    connect(_userInterface.actionExportCurrentFrame, SIGNAL(triggered()), this, SLOT(exportCurrentFrame()));
    connect(_userInterface.actionExportAllFrames, SIGNAL(triggered()), this, SLOT(exportAllFrames()));
    connect(_userInterface.actionQuit, SIGNAL(triggered()), this, SLOT(quit()));

    //Tools
    connect(_userInterface.actionMatch, SIGNAL(triggered(bool)), this, SLOT(setMatching(bool)));
    connect(_userInterface.actionMerge, SIGNAL(triggered()), this, SLOT(merge()));
    connect(_userInterface.actionShapeClipping, SIGNAL(triggered(bool)), this, SLOT(setClipping(bool)));
    connect(_userInterface.actionSurfaceReconstruction, SIGNAL(triggered()), this, SLOT(surfaceReconstruction()));
    connect(_userInterface.actionTexturePanel, SIGNAL(triggered(bool)), this, SLOT(showTexturePanel(bool)));

    //View
    connect(_userInterface.actionModeCloud, SIGNAL(triggered()), this, SLOT(setViewModeCloud()));
    connect(_userInterface.actionModeMesh, SIGNAL(triggered()), this, SLOT(setViewModeMesh()));

    //Help
    connect(_userInterface.actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    //Player
    connect(_userInterface.hsCurrentFrame, SIGNAL(valueChanged(int)), this, SLOT(changeCurrentFrame(int)));
    connect(_userInterface.bPlayPause, SIGNAL(clicked()), this, SLOT(doPlayPause()));
}

void Mediator::initUserInterface()
{
    //_progressBar.reset(new CustomProgressBar());
    //_progressBar->setFormat(QString("%p%"));
    //_userInterface.widgetProgressBar->layout()->addWidget(_progressBar.get());
    _userInterface.widgetProgressBar->hide();

    _userInterface.hsCurrentFrame->setMinimum(1);
    _userInterface.eCurrentFrame->setText(QString::number(_userInterface.hsCurrentFrame->minimum()));
    _userInterface.bPlayPause->setIcon(QIcon(PATH_PLAY_ICON));
    _userInterface.widgetScenePlayer->hide();

    _viewModeGroup = new QActionGroup(_mainWindow.get());
    _viewModeGroup->addAction(_userInterface.actionModeCloud);
    _viewModeGroup->addAction(_userInterface.actionModeMesh);
    _userInterface.actionModeCloud->setChecked(true);

    _userInterface.dockWidgetTexture->hide();
}

void Mediator::toggleProgressBar(const int max)
{
    if (_userInterface.widgetProgressBar->isHidden())
    {
        _userInterface.widgetProgressBar->show();
        _userInterface.progressBar->setValue(0);
        _userInterface.progressBar->setMaximum(max);
    }
    else
    {
        _userInterface.widgetProgressBar->hide();
        _userInterface.progressBar->setValue(0);
        _userInterface.progressBar->setMaximum(100);
    }
}

void Mediator::setViewModeCloud()
{
    _sceneViewer->setViewMode(0);
}

void Mediator::setViewModeMesh()
{
    _sceneViewer->setViewMode(1);
}

void Mediator::toggleScenePlayer()
{
    int frameCount = _sceneViewer->getScenePlayer()->getFrameCount();
    _userInterface.hsCurrentFrame->setMaximum(frameCount);
    _userInterface.widgetScenePlayer->show();
}

void Mediator::changeCurrentFrame(const int currentFrame)
{
    if (_sceneViewer->getScenePlayer()->isPaused())
    {
        _sceneViewer->getScenePlayer()->setCurrentFrame(currentFrame-1);
        _sceneViewer->update();
    }

    _userInterface.eCurrentFrame->setText(QString::number(currentFrame));
}

void Mediator::doPlayPause()
{
    if (_sceneViewer->getScenePlayer()->isPaused())
    {
        _sceneViewer->startAnimation();
        _sceneViewer->getScenePlayer()->play();
        _userInterface.bPlayPause->setIcon(QIcon(PATH_PAUSE_ICON));
    }
    else
    {
        _sceneViewer->stopAnimation();
        _sceneViewer->getScenePlayer()->pause();
        _userInterface.bPlayPause->setIcon(QIcon(PATH_PLAY_ICON));
    }
}

void Mediator::setMatching(bool value)
{
    _sceneViewer->setMatching(value);
}

void Mediator::importGeometry()
{
    QString geometryFolder = QFileDialog::getExistingDirectory(_mainWindow.get(), "Browse geometry folder", "geo/");
    if (!geometryFolder.isEmpty())
    {
        QDir* geometryFolderContent = new QDir(geometryFolder);
        geometryFolderContent->setNameFilters(QStringList(QString("*").append(PLY_FILE_EXTENSION)));
        QStringList geometryFiles = geometryFolderContent->entryList(QDir::Files);

        if (!geometryFiles.isEmpty())
        {
            _sceneViewer->initScenePlayer(geometryFiles.size());
            toggleProgressBar(geometryFiles.size());
            for (uint i = 0; i < geometryFiles.size(); ++i)
            {
                std::string path(geometryFolder.toStdString());
                path.append("/").append(geometryFiles.at(i).toStdString());
                //std::clog << path << std::endl;
                _sceneViewer->importGeometry(path);
                _sceneViewer->update();
                _userInterface.progressBar->setValue(_userInterface.progressBar->value()+1);
                //_progressBar->updateValue();
            }
            toggleProgressBar();
            toggleScenePlayer();
            _sceneViewer->startAnimation();
            QString message = QString("%1 files loaded.").arg(QString::number(geometryFiles.size()));
            _userInterface.statusBar->showMessage(message, 3000);
            //std::clog <<  << std::endl;
        }
        else
            std::clog << geometryFolder.toStdString() << " is empty." << std::endl;
    }
}

void Mediator::importTexture()
{
    QString textureFolder = QFileDialog::getExistingDirectory(_mainWindow.get(), "Browse texture folder", "tex/");
    if (!textureFolder.isEmpty())
    {
        QDir* textureFolderContent = new QDir(textureFolder);
        textureFolderContent->setNameFilters(QStringList(QString("*").append(PNG_FILE_EXTENSION)));
        QStringList textureFiles = textureFolderContent->entryList(QDir::Files);

        if (!textureFiles.isEmpty())
        {
            toggleProgressBar(textureFiles.size());
            for (uint i = 0; i < textureFiles.size(); ++i)
            {
                std::string path(textureFolder.toStdString());
                path.append("/").append(textureFiles.at(i).toStdString());
                _sceneViewer->importTexture(path);
                //_sceneViewer->importGeometry(path);
                _userInterface.progressBar->setValue(_userInterface.progressBar->value()+1);
            }
            toggleProgressBar();
            QString message = QString("%1 files loaded.").arg(QString::number(textureFiles.size()));
            _userInterface.statusBar->showMessage(message, 3000);
        }
        else
            std::clog << textureFolder.toStdString() << " is empty." << std::endl;
    }
}

void Mediator::merge()
{
    _sceneViewer->mergeObjects();
    setMatching(false);
    _userInterface.actionMatch->setEnabled(false);
}

void Mediator::setClipping(bool value)
{
    _sceneViewer->setShapeClipping(value);
}

void Mediator::exportCurrentFrame()
{
    QString exportFilename = QFileDialog::getSaveFileName(_mainWindow.get(), "Export geometry", "output/");
    if (!exportFilename.isEmpty())
        _sceneViewer->exportGeometry(exportFilename.toStdString(), _sceneViewer->getScenePlayer()->getCurrentFrame());
}

void Mediator::exportAllFrames()
{
    QString exportFilename = QFileDialog::getSaveFileName(_mainWindow.get(), "Export geometry", "output/");
    if (!exportFilename.isEmpty())
    {
        QCoreApplication::processEvents();
        toggleProgressBar(_sceneViewer->getScenePlayer()->getFrameCount());
        _sceneViewer->exportGeometry(exportFilename.toStdString());
        toggleProgressBar();
    }
}

void Mediator::showTexturePanel(bool value)
{
    _userInterface.dockWidgetTexture->setVisible(value);
}

void Mediator::surfaceReconstruction()
{
    //std::shared_ptr<SurfaceReconstruction> surfaceReconstruction;
    //surfaceReconstruction.reset(new SurfaceReconstruction());

    //uint currentFrame = _sceneViewer->getScenePlayer()->getCurrentFrame();
    _sceneViewer->surfaceReconstruction();
    _userInterface.actionModeMesh->setEnabled(true);
    _userInterface.actionModeMesh->setChecked(true);
}

void Mediator::about()
{
    QMessageBox msgBox;
    msgBox.setText("This file is part of the Cloudshop project.\n\nCopyright (C) 2014 Bruno Roy\n\nThis program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA");
    msgBox.exec();
}

void Mediator::quit()
{
    _mainWindow->close();
}
