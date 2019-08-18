/*
 * MassEffectModder
 *
 * Copyright (C) 2019 Pawel Kolodziejski
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "Gui/LayoutMeSelect.h"
#include "Gui/LayoutGameUtilities.h"
#include "Gui/MainWindow.h"
#include "Gui/MessageWindow.h"
#include "Helpers/MiscHelpers.h"
#include "Helpers/Logs.h"
#include "GameData.h"
#include "DLC.h"
#include "TOCFile.h"

LayoutGameUtilities::LayoutGameUtilities(MainWindow *window)
    : mainWindow(window)
{
    if (window == nullptr)
        CRASH();
    wnd = window;

    layoutId = MainWindow::kLayoutGameUtilities;

    auto ButtonCheckGameFiles = new QPushButton("Check Game Files");
    ButtonCheckGameFiles->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    ButtonCheckGameFiles->setMinimumWidth(kButtonMinWidth);
    ButtonCheckGameFiles->setMinimumHeight(kButtonMinHeight);
    QFont ButtonFont = ButtonCheckGameFiles->font();
    ButtonFont.setPointSize(kFontSize);
    ButtonCheckGameFiles->setFont(ButtonFont);
    connect(ButtonCheckGameFiles, &QPushButton::clicked, this, &LayoutGameUtilities::CheckGameFilesSelected);

    auto ButtonChangeGamePath = new QPushButton("Change Game Path");
    ButtonChangeGamePath->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    ButtonChangeGamePath->setMinimumWidth(kButtonMinWidth);
    ButtonChangeGamePath->setMinimumHeight(kButtonMinHeight);
    ButtonChangeGamePath->setFont(ButtonFont);
    connect(ButtonChangeGamePath, &QPushButton::clicked, this, &LayoutGameUtilities::ChangeGamePathSelected);

#if !defined(_WIN32)
    auto ButtonChangeUserPath = new QPushButton("Change User Path");
    ButtonChangeUserPath->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    ButtonChangeUserPath->setMinimumWidth(kButtonMinWidth);
    ButtonChangeUserPath->setMinimumHeight(kButtonMinHeight);
    ButtonChangeUserPath->setFont(ButtonFont);
    connect(ButtonChangeUserPath, &QPushButton::clicked, this, &LayoutGameUtilities::ChangeUserPathSelected);
#endif

    QPushButton *ButtonRepackGameFiles = nullptr;
    if (mainWindow->gameType != MeType::ME1_TYPE)
    {
        ButtonRepackGameFiles = new QPushButton("Repack Game Files");
        ButtonRepackGameFiles->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        ButtonRepackGameFiles->setMinimumWidth(kButtonMinWidth);
        ButtonRepackGameFiles->setMinimumHeight(kButtonMinHeight);
        ButtonRepackGameFiles->setFont(ButtonFont);
        connect(ButtonRepackGameFiles, &QPushButton::clicked, this, &LayoutGameUtilities::RepackGameFilesSelected);
    }

    QPushButton *ButtonUpdateTOCs = nullptr;
    QPushButton *ButtonExtractDLCs = nullptr;
    if (mainWindow->gameType == MeType::ME3_TYPE)
    {
        ButtonUpdateTOCs = new QPushButton("Update TOCs");
        ButtonUpdateTOCs->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        ButtonUpdateTOCs->setMinimumWidth(kButtonMinWidth);
        ButtonUpdateTOCs->setMinimumHeight(kButtonMinHeight);
        ButtonUpdateTOCs->setFont(ButtonFont);
        connect(ButtonUpdateTOCs, &QPushButton::clicked, this, &LayoutGameUtilities::UpdateTOCsSelected);

        ButtonExtractDLCs = new QPushButton("Extract DLCs");
        ButtonExtractDLCs->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        ButtonExtractDLCs->setMinimumWidth(kButtonMinWidth);
        ButtonExtractDLCs->setMinimumHeight(kButtonMinHeight);
        ButtonExtractDLCs->setFont(ButtonFont);
        connect(ButtonExtractDLCs, &QPushButton::clicked, this, &LayoutGameUtilities::ExtractDLCsSelected);
    }

    auto ButtonReturn = new QPushButton("Return");
    ButtonReturn->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    ButtonReturn->setMinimumWidth(kButtonMinWidth);
    ButtonReturn->setMinimumHeight(kButtonMinHeight / 2);
    ButtonReturn->setFont(ButtonFont);
    connect(ButtonReturn, &QPushButton::clicked, this, &LayoutGameUtilities::ReturnSelected);

    auto *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->addSpacing(PERCENT_OF_SIZE(MainWindow::kMinWindowWidth, 40));
    auto *verticalLayout = new QVBoxLayout();
    verticalLayout->setAlignment(Qt::AlignVCenter);
    verticalLayout->addWidget(ButtonCheckGameFiles, 1);
    verticalLayout->addWidget(ButtonChangeGamePath, 1);
#if !defined(_WIN32)
    verticalLayout->addWidget(ButtonChangeUserPath, 1);
#endif
    if (mainWindow->gameType != MeType::ME1_TYPE)
    {
        verticalLayout->addWidget(ButtonRepackGameFiles, 1);
    }
    if (mainWindow->gameType == MeType::ME3_TYPE)
    {
        verticalLayout->addWidget(ButtonUpdateTOCs, 1);
        verticalLayout->addWidget(ButtonExtractDLCs, 1);
    }
    verticalLayout->addSpacing(20);
    verticalLayout->addWidget(ButtonReturn, 1);
    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addSpacing(PERCENT_OF_SIZE(MainWindow::kMinWindowWidth, 40));
}

void LayoutGameUtilities::LockGui()
{
    foreach (QWidget *widget, this->findChildren<QWidget*>())
    {
        widget->setEnabled(false);
    }
}

void LayoutGameUtilities::UnlockGui()
{
    foreach (QWidget *widget, this->findChildren<QWidget*>())
    {
        widget->setEnabled(true);
    }
}

void LayoutGameUtilities::CheckGameFilesSelected()
{
}

void LayoutGameUtilities::ChangeGamePathSelected()
{
    LockGui();
    ConfigIni configIni{};
    g_GameData->Init(mainWindow->gameType, configIni);
    QString filter, exeSuffix;
    QString caption = "Please select the Mass Effect " +
            QString::number(static_cast<int>(mainWindow->gameType)) +
            " executable file";
    switch (mainWindow->gameType)
    {
    case MeType::ME1_TYPE:
        filter = "ME1 executable file (MassEffect.exe)";
        exeSuffix = "/Binaries";
        break;
    case MeType::ME2_TYPE:
        filter = "ME2 executable file (MassEffect2.exe)";
        exeSuffix = "/Binaries";
        break;
    case MeType::ME3_TYPE:
        filter = "ME3 executable file (MassEffect3.exe)";
        exeSuffix = "/Binaries/Win32";
        break;
    case MeType::UNKNOWN_TYPE:
        CRASH();
    }
    QString path = QFileDialog::getOpenFileName(this, caption,
                                                g_GameData->GamePath() + exeSuffix, filter);
    if (path.length() != 0 && QFile(path).exists())
    {
        if (mainWindow->gameType == MeType::ME3_TYPE)
            path = DirName(DirName(DirName(path)));
        else
            path = DirName(DirName(path));
        QString key = QString("ME%1").arg(static_cast<int>(mainWindow->gameType));
#if defined(_WIN32)
        configIni.Write(key, QString(path).replace(QChar('/'), QChar('\\'), Qt::CaseInsensitive), "GameDataPath");
#else
        configIni.Write(key, path, "GameDataPath");
#endif
        g_GameData->Init(mainWindow->gameType, configIni, true);
        QMessageBox::information(this, "Changing game path", "Game path changed to\n" + path);
    }
    else
    {
        QMessageBox::information(this, "Changing game path", "Game path NOT changed.");
    }
    UnlockGui();
}

void LayoutGameUtilities::ChangeUserPathSelected()
{
    LockGui();
    ConfigIni configIni{};
    g_GameData->Init(mainWindow->gameType, configIni);
    QString caption = "Please select the Mass Effect " +
            QString::number(static_cast<int>(mainWindow->gameType)) +
            " user configuration path";
    QString path = QFileDialog::getExistingDirectory(this, caption,
                                                     GameData::GameUserPath(mainWindow->gameType),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (path.length() != 0 && QDir(path).exists())
    {
        QString key = QString("ME%1").arg(static_cast<int>(mainWindow->gameType));
#if defined(_WIN32)
        configIni.Write(key, QString(path).replace(QChar('/'), QChar('\\'), Qt::CaseInsensitive), "GameUserPath");
#else
        configIni.Write(key, path, "GameUserPath");
#endif
        QString newPath = GameData::GameUserPath(mainWindow->gameType);
        QMessageBox::information(this, "Changing user configuration path",
                                 "User configuration path changed to\n" + newPath);
    }
    else
    {
        QMessageBox::information(this, "Changing user configuration path",
                                 "User configuration path NOT changed.");
    }
    UnlockGui();
}

void LayoutGameUtilities::RepackGameFilesSelected()
{
}

void LayoutGameUtilities::UpdateTOCsSelected()
{
    LockGui();
    mainWindow->statusBar()->showMessage("Updating TOC files...");

    ConfigIni configIni{};
    g_GameData->Init(MeType::ME3_TYPE, configIni);
    if (g_GameData->GamePath().length() == 0 || !QDir(g_GameData->GamePath()).exists())
    {
        mainWindow->statusBar()->clearMessage();
        QMessageBox::critical(this, "Updating TOC files", "Game data not found.");
        UnlockGui();
        return;
    }

    TOCBinFile::UpdateAllTOCBinFiles();
    mainWindow->statusBar()->clearMessage();
    QMessageBox::information(this, "Updating TOC files", "All TOC files updated.");
    UnlockGui();
}

void LayoutGameUtilities::ExtractDLCsSelected()
{
    LockGui();
    mainWindow->statusBar()->showMessage("Unpacking DLCs...");

    ConfigIni configIni{};
    g_GameData->Init(MeType::ME3_TYPE, configIni);
    if (g_GameData->GamePath().length() == 0 || !QDir(g_GameData->GamePath()).exists())
    {
        mainWindow->statusBar()->clearMessage();
        QMessageBox::critical(this, "Unpacking DLCs", "Game data not found.");
        UnlockGui();
        return;
    }

    g_logs->BufferClearErrors();
    g_logs->BufferEnableErrors(true);
    ME3DLC::unpackAllDLC(&LayoutGameUtilities::ExtractDlcCallback, mainWindow);
    g_logs->BufferEnableErrors(false);
    mainWindow->statusBar()->clearMessage();
    if (g_logs->BufferGetErrors() != "")
    {
        MessageWindow msg;
        msg.Show("Errors while unpacking DLCs", g_logs->BufferGetErrors());
    }
    else
    {
        QMessageBox::information(this, "Unpacking DLCs", "All DLCs unpacked.");
    }
    UnlockGui();
}

void LayoutGameUtilities::ExtractDlcCallback(void *handle, int progress)
{
    auto *win = static_cast<MainWindow *>(handle);
    win->statusBar()->showMessage(QString("Unpacking DLCs, progress: ") + QString::number(progress) + "%");
}

void LayoutGameUtilities::ReturnSelected()
{
    mainWindow->SwitchLayoutById(MainWindow::kLayoutModules);
    mainWindow->GetLayout()->removeWidget(this);
}
