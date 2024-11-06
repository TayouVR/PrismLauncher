// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2022 Jamie Mansfield <jmansfield@cadixdev.org>
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 *  Copyright (C) 2024 TheKodeToad <TheKodeToad@proton.me>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "MinecraftSettingsWidget.h"
#include <Application.h>

#include "settings/Setting.h"
#include "ui_MinecraftSettingsWidget.h"

MinecraftSettingsWidget::MinecraftSettingsWidget(InstancePtr instance, QWidget* parent)
    : QWidget(parent), m_instance(std::move(instance)), m_ui(new Ui::MinecraftSettingsWidget)
{
    m_ui->setupUi(this);

    if (m_instance == nullptr) {
        // Java
        m_ui->settingsTabs->removeTab(1);
        // Launch
        m_ui->settingsTabs->removeTab(2);
        m_ui->openGlobalJavaSettingsButton->setVisible(false);
    } else {
        m_ui->showGameTime->setText(tr("Show time &playing this instance"));
        m_ui->recordGameTime->setText(tr("&Record time playing this instance"));
        m_ui->showGlobalGameTime->hide();
        m_ui->showGameTimeWithoutDays->hide();

        m_ui->maximizedWarning->setText(
            tr("<span style=\" font-weight:600; color:#f5c211;\">Warning</span><span style=\" color:#f5c211;\">: The maximized option is "
               "not fully supported on this Minecraft version.</span>"));
    }

    m_ui->maximizedWarning->hide();

    connect(m_ui->maximizedCheckBox, &QCheckBox::toggled, this,
            [this](const bool value) { m_ui->maximizedWarning->setVisible(value && (m_instance == nullptr || !m_instance->isLegacy())); });

    loadSettings();
}

MinecraftSettingsWidget::~MinecraftSettingsWidget()
{
    delete m_ui;
}

void MinecraftSettingsWidget::loadSettings()
{
    const SettingsObjectPtr settings = getSettings();

    // Miscellaneous
    m_ui->miscellaneousSettingsBox->setCheckable(m_instance != nullptr);
    m_ui->miscellaneousSettingsBox->setChecked(settings->get("OverrideMiscellaneous").toBool());
    m_ui->closeAfterLaunchCheck->setChecked(settings->get("CloseAfterLaunch").toBool());
    m_ui->quitAfterGameStopCheck->setChecked(settings->get("QuitAfterGameStop").toBool());

    // Console
    m_ui->consoleSettingsBox->setCheckable(m_instance != nullptr);
    m_ui->consoleSettingsBox->setChecked(m_instance == nullptr || settings->get("OverrideConsole").toBool());
    m_ui->showConsoleCheck->setChecked(settings->get("ShowConsole").toBool());
    m_ui->autoCloseConsoleCheck->setChecked(settings->get("AutoCloseConsole").toBool());
    m_ui->showConsoleErrorCheck->setChecked(settings->get("ShowConsoleOnError").toBool());

    // Window Size
    m_ui->windowSizeGroupBox->setCheckable(m_instance != nullptr);
    m_ui->windowSizeGroupBox->setChecked(m_instance == nullptr || settings->get("OverrideWindow").toBool());
    m_ui->windowSizeGroupBox->setChecked(settings->get("OverrideWindow").toBool());
    m_ui->maximizedCheckBox->setChecked(settings->get("LaunchMaximized").toBool());
    m_ui->windowWidthSpinBox->setValue(settings->get("MinecraftWinWidth").toInt());
    m_ui->windowHeightSpinBox->setValue(settings->get("MinecraftWinHeight").toInt());

    // Custom commands
    m_ui->customCommands->initialize(m_instance != nullptr, m_instance == nullptr || settings->get("OverrideCommands").toBool(),
                                     settings->get("PreLaunchCommand").toString(), settings->get("WrapperCommand").toString(),
                                     settings->get("PostExitCommand").toString());

    // Environment variables
    m_ui->environmentVariables->initialize(m_instance != nullptr, m_instance == nullptr || settings->get("OverrideEnv").toBool(),
                                           settings->get("Env").toMap());

    // Workarounds
    m_ui->nativeWorkaroundsGroupBox->setCheckable(m_instance != nullptr);
    m_ui->nativeWorkaroundsGroupBox->setChecked(m_instance == nullptr || settings->get("OverrideNativeWorkarounds").toBool());
    m_ui->useNativeGLFWCheck->setChecked(settings->get("UseNativeGLFW").toBool());
    m_ui->lineEditGLFWPath->setText(settings->get("CustomGLFWPath").toString());
#ifdef Q_OS_LINUX
    m_ui->lineEditGLFWPath->setPlaceholderText(APPLICATION->m_detectedGLFWPath);
#else
    m_ui->lineEditGLFWPath->setPlaceholderText(tr("Path to %1 library file").arg(BuildConfig.GLFW_LIBRARY_NAME));
#endif
    m_ui->useNativeOpenALCheck->setChecked(settings->get("UseNativeOpenAL").toBool());
    m_ui->lineEditOpenALPath->setText(settings->get("CustomOpenALPath").toString());
#ifdef Q_OS_LINUX
    m_ui->lineEditOpenALPath->setPlaceholderText(APPLICATION->m_detectedOpenALPath);
#else
    m_ui->lineEditOpenALPath->setPlaceholderText(tr("Path to %1 library file").arg(BuildConfig.OPENAL_LIBRARY_NAME));
#endif

    // Performance
    m_ui->perfomanceGroupBox->setCheckable(m_instance != nullptr);
    m_ui->perfomanceGroupBox->setChecked(m_instance == nullptr || settings->get("OverridePerformance").toBool());
    m_ui->enableFeralGamemodeCheck->setChecked(settings->get("EnableFeralGamemode").toBool());
    m_ui->enableMangoHud->setChecked(settings->get("EnableMangoHud").toBool());
    m_ui->useDiscreteGpuCheck->setChecked(settings->get("UseDiscreteGpu").toBool());
    m_ui->useZink->setChecked(settings->get("UseZink").toBool());

#if !defined(Q_OS_LINUX)
    m_ui->settingsTabs->setTabVisible(ui->settingsTabs->indexOf(ui->performancePage), false);
#endif

    if (!(APPLICATION->capabilities() & Application::SupportsGameMode)) {
        m_ui->enableFeralGamemodeCheck->setDisabled(true);
        m_ui->enableFeralGamemodeCheck->setToolTip(tr("Feral Interactive's GameMode could not be found on your system."));
    }

    if (!(APPLICATION->capabilities() & Application::SupportsMangoHud)) {
        m_ui->enableMangoHud->setDisabled(true);
        m_ui->enableMangoHud->setToolTip(tr("MangoHud could not be found on your system."));
    }

    // Miscellanous
    m_ui->gameTimeGroupBox->setCheckable(m_instance != nullptr);
    m_ui->gameTimeGroupBox->setChecked(m_instance == nullptr || settings->get("OverrideGameTime").toBool());
    m_ui->showGameTime->setChecked(settings->get("ShowGameTime").toBool());
    m_ui->recordGameTime->setChecked(settings->get("RecordGameTime").toBool());
    m_ui->showGlobalGameTime->setChecked(m_instance != nullptr && settings->get("ShowGlobalGameTime").toBool());
    m_ui->showGameTimeWithoutDays->setChecked(m_instance != nullptr && settings->get("ShowGameTimeWithoutDays").toBool());

    m_ui->legacySettingsGroupBox->setCheckable(m_instance != nullptr);
    m_ui->legacySettingsGroupBox->setChecked(m_instance == nullptr || settings->get("OverrideLegacySettings").toBool());
    m_ui->onlineFixes->setChecked(settings->get("OnlineFixes").toBool());
}

void MinecraftSettingsWidget::saveSettings()
{
    SettingsObjectPtr settings = getSettings();
    SettingsObject::Lock lock(settings);

    // Miscellaneous
    bool miscellaneous = m_instance == nullptr || m_ui->miscellaneousSettingsBox->isChecked();

    if (m_instance != nullptr)
        settings->set("OverrideMiscellaneous", miscellaneous);

    if (miscellaneous) {
        settings->set("CloseAfterLaunch", m_ui->closeAfterLaunchCheck->isChecked());
        settings->set("QuitAfterGameStop", m_ui->quitAfterGameStopCheck->isChecked());
    } else {
        settings->reset("CloseAfterLaunch");
        settings->reset("QuitAfterGameStop");
    }

    // Console
    bool console = m_instance == nullptr || m_ui->consoleSettingsBox->isChecked();

    if (m_instance != nullptr)
        settings->set("OverrideConsole", console);

    if (console) {
        settings->set("ShowConsole", m_ui->showConsoleCheck->isChecked());
        settings->set("AutoCloseConsole", m_ui->autoCloseConsoleCheck->isChecked());
        settings->set("ShowConsoleOnError", m_ui->showConsoleErrorCheck->isChecked());
    } else {
        settings->reset("ShowConsole");
        settings->reset("AutoCloseConsole");
        settings->reset("ShowConsoleOnError");
    }

    // Window Size
    bool window = m_instance == nullptr || m_ui->windowSizeGroupBox->isChecked();

    if (m_instance != nullptr)
        settings->set("OverrideWindow", window);

    if (window) {
        settings->set("LaunchMaximized", m_ui->maximizedCheckBox->isChecked());
        settings->set("MinecraftWinWidth", m_ui->windowWidthSpinBox->value());
        settings->set("MinecraftWinHeight", m_ui->windowHeightSpinBox->value());
    } else {
        settings->reset("LaunchMaximized");
        settings->reset("MinecraftWinWidth");
        settings->reset("MinecraftWinHeight");
    }

    // Custom Commands
    bool custcmd = m_instance == nullptr || m_ui->customCommands->checked();

    if (m_instance != nullptr)
        settings->set("OverrideCommands", custcmd);

    if (custcmd) {
        settings->set("PreLaunchCommand", m_ui->customCommands->prelaunchCommand());
        settings->set("WrapperCommand", m_ui->customCommands->wrapperCommand());
        settings->set("PostExitCommand", m_ui->customCommands->postexitCommand());
    } else {
        settings->reset("PreLaunchCommand");
        settings->reset("WrapperCommand");
        settings->reset("PostExitCommand");
    }

    // Environment Variables
    auto env = m_instance == nullptr || m_ui->environmentVariables->override();

    if (m_instance != nullptr)
        settings->set("OverrideEnv", env);

    if (env)
        settings->set("Env", m_ui->environmentVariables->value());
    else
        settings->reset("Env");

    // Workarounds
    bool workarounds = m_instance == nullptr || m_ui->nativeWorkaroundsGroupBox->isChecked();

    if (m_instance != nullptr)
        settings->set("OverrideNativeWorkarounds", workarounds);

    if (workarounds) {
        settings->set("UseNativeGLFW", m_ui->useNativeGLFWCheck->isChecked());
        settings->set("CustomGLFWPath", m_ui->lineEditGLFWPath->text());
        settings->set("UseNativeOpenAL", m_ui->useNativeOpenALCheck->isChecked());
        settings->set("CustomOpenALPath", m_ui->lineEditOpenALPath->text());
    } else {
        settings->reset("UseNativeGLFW");
        settings->reset("CustomGLFWPath");
        settings->reset("UseNativeOpenAL");
        settings->reset("CustomOpenALPath");
    }

    // Performance
    bool performance = m_instance == nullptr || m_ui->perfomanceGroupBox->isChecked();

    if (m_instance != nullptr)
        settings->set("OverridePerformance", performance);

    if (performance) {
        settings->set("EnableFeralGamemode", m_ui->enableFeralGamemodeCheck->isChecked());
        settings->set("EnableMangoHud", m_ui->enableMangoHud->isChecked());
        settings->set("UseDiscreteGpu", m_ui->useDiscreteGpuCheck->isChecked());
        settings->set("UseZink", m_ui->useZink->isChecked());
    } else {
        settings->reset("EnableFeralGamemode");
        settings->reset("EnableMangoHud");
        settings->reset("UseDiscreteGpu");
        settings->reset("UseZink");
    }

    // Game time
    bool gameTime = m_instance == nullptr || m_ui->gameTimeGroupBox->isChecked();

    if (m_instance != nullptr)
        settings->set("OverrideGameTime", gameTime);

    if (gameTime) {
        settings->set("ShowGameTime", m_ui->showGameTime->isChecked());
        settings->set("RecordGameTime", m_ui->recordGameTime->isChecked());
    } else {
        settings->reset("ShowGameTime");
        settings->reset("RecordGameTime");
    }

    if (m_instance == nullptr) {
        settings->set("ShowGlobalGameTime", m_ui->showGlobalGameTime->isChecked());
        settings->set("ShowGameTimeWithoutDays", m_ui->showGameTimeWithoutDays->isChecked());
    }

    bool overrideLegacySettings = m_instance == nullptr || m_ui->legacySettingsGroupBox->isChecked();

    if (m_instance != nullptr)
        settings->set("OverrideLegacySettings", overrideLegacySettings);

    if (overrideLegacySettings) {
        settings->set("OnlineFixes", m_ui->onlineFixes->isChecked());
    } else {
        settings->reset("OnlineFixes");
    }

    if (m_instance != nullptr)
        m_instance->updateRuntimeContext();
}

SettingsObjectPtr MinecraftSettingsWidget::getSettings() const
{
    if (m_instance != nullptr)
        return m_instance->settings();

    return APPLICATION->settings();
}
