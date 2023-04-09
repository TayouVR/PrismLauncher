// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2023 Tayou <tayou@gmx.net>
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
 */

#include "GameOptionWidgetKeyBind.h"
#include "ui_GameOptionWidgetKeyBind.h"

GameOptionWidgetKeyBind::GameOptionWidgetKeyBind(QWidget* parent, std::shared_ptr<GameOption> knownOption) : GameOptionWidget(parent), ui(new Ui::GameOptionWidgetKeyBind)
{
    ui->setupUi(this);

    ui->resetButton->setMaximumWidth(ui->resetButton->height());
    ui->resetButton->setToolTip(QString(tr("Default Value: %1")).arg(knownOption->getDefaultString()));
}

GameOptionWidgetKeyBind::~GameOptionWidgetKeyBind()
{
    delete ui;
}

void GameOptionWidgetKeyBind::setEditorData(GameOptionItem optionItem) {
    /*for (auto& keyBinding : *keybindingOptions) {
        // this could become a std::find_if eventually, if someone wants to bother making it that.
        if (keyBinding->minecraftKeyCode == contents[row].knownOption->getDefaultString()) {
            return keyBinding->displayName;  // + " (" + contents[row].knownOption->getDefaultString() + ")";
        }
    }

    return contents[row].knownOption->getDefaultString();*/
    ui->keySequenceEdit->setKeySequence(optionItem.value);
}
