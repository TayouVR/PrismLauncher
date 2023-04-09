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
#pragma once

#include <QString>
#include <QStyledItemDelegate>
#include <QWidget>
#include <QPushButton>

#include "minecraft/gameoptions/GameOptions.h"

class GameOptionWidget : public QWidget {
   public:
    GameOptionWidget(QWidget* parent) : QWidget(parent){};
    virtual void setEditorData(GameOptionItem optionItem) = 0;

   protected:
    QPushButton* makeResetButton(QWidget* parent, std::shared_ptr<GameOption>& knownOption, QRect rect)
    {
        if (knownOption != nullptr) {
            QPushButton* resetButton = new QPushButton(parent);
            resetButton->setText("↺");
            resetButton->setGeometry(rect);
            resetButton->setToolTip(tr("Default Value: ") + knownOption->getDefaultString());
            return resetButton;
        }
        return nullptr;
    }
};