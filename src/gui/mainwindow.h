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
 * @file mainwindow.h
 * @authors Gschwind, Weber, Schoch, Niederberger
 *
 * @brief Mainwindow for the project linescaleGUI
 *
 */

#pragma once
#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include "../deviceCommunication/commMaster.h"
#include "../notification/notification.h"
#include "dialogabout.h"
#include "dialogconnect.h"
#include "dialogdebug.h"
#include "plotWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private:
    /**
     * @brief Open the github project page
     *
     */
    void openGitHubLink(void);

    /**
     * @brief Show or hide the logWindow
     *
     * Trigger the logWindow based on the action in the menubar.
     */
    void showLog(void);

    /**
     * @brief Slot triggered by the button "resetPeak"
     *
     * This slot sets the value on the right sidebar to zero and sends
     * the command to the connected device.
     */
    void sendResetPeak();

   private slots:
    /**
     * @brief Receive new force from CommMaster
     *
     * This slot updates the peak and current value of the right side bar
     * It also updates the bool `MainWindow::reading` keeping track of the status of the
     * connection
     *
     * @param value Current force reading in the unit of the device
     */
    void receiveNewForce(float time, float value);

    /**
     * @brief Toggle the GUI elements on connection
     *
     * Some GUI elements are only relevant / usable if a device is connected.
     * This slot toggles this elements if a change in the connection was
     * signaled by the CommMaster.
     *
     * @param connected Current connection state
     */
    void toggleActions(bool connected);

    /**
     * @brief Start or stop the readings
     *
     * Send the command to the connected device. If the host receives a new
     * reading, the bool `MainWindow::reading` will be enabled by
     * `MainWindow::receiveNewForce`.
     * If the host terminates the stream, the bool will be set to false after
     * a delay. This is to prevent buffered data from setting the bool to true.
     */
    void triggerReadings();
    void redrawPlot();

   private:
    Ui::MainWindow* ui;
    comm::CommMaster* comm;
    DialogAbout* dAbout;
    DialogDebug* dDebug;
    DialogConnect* dConnect;
    Notification* notification;
    Plot* plot;
    float maxValue = 0;
    bool reading = false;  ///< Tracks whether the host reads data or not
    QTimer* plotTimer;
};

#endif  // MAINWINDOW_H_
