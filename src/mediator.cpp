#include "mediator.h"

#include <QMessageBox>

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
    connect(_userInterface.actionImportGeometry, SIGNAL(triggered()), this, SLOT(importGeometry()));
    connect(_userInterface.actionQuit, SIGNAL(triggered()), this, SLOT(quit()));

    //Help
    connect(_userInterface.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
}

void Mediator::initUserInterface()
{
}

void Mediator::importGeometry()
{
    QString geometryFolder = QFileDialog::getExistingDirectory(_mainWindow.get(), "Browse geometry folder", "geo/");
    if (!geometryFolder.isEmpty())
    {
        QDir* geometryFolderContent = new QDir(geometryFolder);
        geometryFolderContent->setNameFilters(QStringList(QString("*").append(PLY_FILE_EXTENSION)));
        QStringList geometryFiles = geometryFolderContent->entryList(QDir::Files);

        for (uint i = 0; i < geometryFiles.size(); ++i)
            std::clog << "file: " << geometryFiles.at(i).toStdString() << std::endl;
    }
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
