/******************************************************************************
 * Copyright (C) 2022 by Gschwind, Weber, Schoch, Niederberger                *
 *                                                                            *
 * This file is part of linescaleGUI.                                         *
 *                                                                            *
 * LinescaleGUI is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * LinescaleGUI is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the               *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with linescaleGUI. If not, see <http://www.gnu.org/licenses/>.       *
 ******************************************************************************/
/**
 * @file mainwindow.cpp
 * @authors Gschwind, Weber, Schoch, Niederberger
 *
 */

#include "mainwindow.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTimer>
#include "../deviceCommunication/command.h"
#include "../notification/notification.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    notification = new Notification(ui->textBrowserLog);
    comm = new comm::CommMaster();

    dAbout = new DialogAbout(this);
    dDebug = new DialogDebug(comm, this);
    dConnect = new DialogConnect(comm, this);
    ui->widgetConnection->setCommunicationMaster(comm);

    // menu actions
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, dAbout, &DialogAbout::show);
    connect(ui->actionGitHub, &QAction::triggered, this, &MainWindow::openGitHubLink);
    connect(ui->actionDebug, &QAction::triggered, dDebug, &DialogDebug::show);
    connect(ui->actionShowLog, &QAction::triggered, this, &MainWindow::showLog);

    // Tool bar actions
    connect(ui->actionConnect, &QAction::triggered, dConnect, &DialogConnect::show);
    connect(ui->actionDisconnect, &QAction::triggered, this, [=] { comm->removeConnection(); });
    connect(ui->actionStartStop, &QAction::triggered, this, &MainWindow::triggerReadings);

    // Buttons next to readings
    connect(ui->btnResetPeak, &QPushButton::pressed, this, &MainWindow::sendResetPeak);

    // updates from CommMaster
    connect(comm, &comm::CommMaster::newForceMaster, this, &MainWindow::receiveNewForce);
    connect(comm, &comm::CommMaster::changedStateMaster, this, &MainWindow::toggleActions);

    // disable wait for close, automatic close after main window close
    dAbout->setAttribute(Qt::WA_QuitOnClose, false);
    dDebug->setAttribute(Qt::WA_QuitOnClose, false);
    dConnect->setAttribute(Qt::WA_QuitOnClose, false);

    // Set default log visibility to match the actionShowLog button
    showLog();

    // Init actions in the toolbar, deactivate actions that require a connected device
    toggleActions(false);
}

MainWindow::~MainWindow() {
    delete comm;
    delete ui;
    delete notification;
}

void MainWindow::openGitHubLink(void) {
    QDesktopServices::openUrl(QUrl("https://github.com/linescaleGUI/linescaleGUI"));
}

void MainWindow::showLog(void) {
    bool isChecked = ui->actionShowLog->isChecked();
    ui->textBrowserLog->setVisible(isChecked);
}

void MainWindow::sendResetPeak() {
    comm->sendData(command::RESETPEAK);
    maxValue = 0;
    receiveNewForce(0);
}

void MainWindow::triggerReadings() {
    if (!reading) {
        notification->push("Start reading");
        comm->sendData(command::REQUESTONLINE);
    } else {
        QTimer::singleShot(10, [=] { reading = false; });
        notification->push("Stop reading");
        comm->sendData(command::DISCONNECTONLINE);
    }
}

void MainWindow::receiveNewForce(float value) {
    reading = true;
    if (value >= maxValue) {
        maxValue = value;
        ui->lblPeakForce->setText(QString("%1 kN").arg(value, 3, 'f', 2));
    }
    ui->lblCurrentForce->setText(QString("%1 kN").arg(value, 0, 'f', 2));
}

void MainWindow::toggleActions(bool connected) {
    ui->actionDisconnect->setEnabled(connected);
    ui->actionStartStop->setEnabled(connected);
    ui->actionConnect->setEnabled(!connected);
    ui->widgetConnection->setEnabled(connected);
}
