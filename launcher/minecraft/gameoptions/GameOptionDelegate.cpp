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
#include "GameOptionDelegate.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

namespace {

enum WidgetType { slider, text, keybind, number, comboBox };

/// @brief gets the following layout for widgets in four QRects:
///     #------------#----------------------------#------------#----------#
///     | minus      | main widget                | plus       | reset to |
///     | (optional) | (slider, text field, etc.) | (optional) | default  |
///     #------------#----------------------------#------------#----------#
/// @param mainWidget
/// @param resetWidget
/// @param subWidget
/// @param addWidget
/// @param valueDisplayWidget
void getWidgetDimensions(QRect sourceWidget,
                         QRect& mainWidget,
                         QRect& resetWidget,
                         QRect& subWidget,
                         QRect& addWidget,
                         QRect& valueDisplayWidget,
                         WidgetType type)
{
    int smallButtonSize = type == slider ? 30 : 0;
    int valueDisplaySize = type == slider ? 50 : 0;
    int resetButtonSize = 30;

    // subtraction widget
    subWidget.setWidth(smallButtonSize);
    subWidget.setY(0);

    // addition widget
    addWidget.setWidth(smallButtonSize);
    addWidget.setY(30);

    // main widget
    mainWidget.setWidth(sourceWidget.width() - smallButtonSize * 2 - resetButtonSize - valueDisplaySize);
    mainWidget.setY(smallButtonSize);

    // reset widget
    resetWidget.setWidth(resetButtonSize);
    resetWidget.setY(sourceWidget.width() - resetButtonSize);

    // value display widget
    valueDisplayWidget.setWidth(valueDisplaySize);
    valueDisplayWidget.setY(sourceWidget.width() - resetButtonSize - valueDisplaySize);
}

void applyWidgetDimensions() {}

}  // namespace

QWidget* GameOptionDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    std::shared_ptr<GameOption> knownOption = contents->at(index.row()).knownOption;

    // define rects for widgets to be used in any type
    QRect mainWidget;
    QRect resetWidget;
    QRect subWidget;
    QRect addWidget;
    QRect valueDisplayWidget;

    switch (contents->at(index.row()).type) {
        case OptionType::String: {
            if (knownOption != nullptr && !knownOption->validValues.isEmpty()) {
                QComboBox* comboBox = new QComboBox(parent);
                for (auto value : knownOption->validValues) {
                    comboBox->addItem(value);
                }

                getWidgetDimensions(option.rect, mainWidget, resetWidget, subWidget, addWidget, valueDisplayWidget, WidgetType::comboBox);
                comboBox->setGeometry(mainWidget);
                makeResetButton(parent, knownOption, resetWidget);
                return comboBox;
            } else {
                QLineEdit* textField = new QLineEdit(parent);
                getWidgetDimensions(option.rect, mainWidget, resetWidget, subWidget, addWidget, valueDisplayWidget, WidgetType::text);
                textField->setGeometry(mainWidget);
                return textField;
            }
        }
        case OptionType::Int: {
            if (knownOption != nullptr && (knownOption->getIntRange().max != 0 || knownOption->getIntRange().min != 0)) {
                QSlider* slider = new QSlider(parent);
                slider->setMinimum(knownOption->getIntRange().min);
                slider->setMaximum(knownOption->getIntRange().max);
                slider->setOrientation(Qt::Horizontal);
                getWidgetDimensions(option.rect, mainWidget, resetWidget, subWidget, addWidget, valueDisplayWidget, WidgetType::slider);
                slider->setGeometry(mainWidget);
                makeResetButton(parent, knownOption, resetWidget);
                return slider;
            } else {
                QSpinBox* intInput = new QSpinBox(parent);
                getWidgetDimensions(option.rect, mainWidget, resetWidget, subWidget, addWidget, valueDisplayWidget, WidgetType::number);
                intInput->setGeometry(mainWidget);
                return intInput;
            }
        }
        case OptionType::Bool: {
            QCheckBox* checkBox = new QCheckBox(parent);
            getWidgetDimensions(option.rect, mainWidget, resetWidget, subWidget, addWidget, valueDisplayWidget, WidgetType::text);
            checkBox->setGeometry(mainWidget);
            makeResetButton(parent, knownOption, resetWidget);
            return checkBox;
        }
        case OptionType::Float: {
            if (knownOption != nullptr && (knownOption->getFloatRange().max != 0 || knownOption->getFloatRange().min != 0)) {
                QSlider* slider = new QSlider(parent);
                slider->setMinimum(knownOption->getFloatRange().min * 100);
                slider->setMaximum(knownOption->getFloatRange().max * 100);
                slider->setOrientation(Qt::Horizontal);
                getWidgetDimensions(option.rect, mainWidget, resetWidget, subWidget, addWidget, valueDisplayWidget, WidgetType::slider);
                slider->setGeometry(mainWidget);
                makeResetButton(parent, knownOption, resetWidget);
                return slider;
            } else {
                QDoubleSpinBox* floatInput = new QDoubleSpinBox(parent);
                getWidgetDimensions(option.rect, mainWidget, resetWidget, subWidget, addWidget, valueDisplayWidget, WidgetType::number);
                floatInput->setGeometry(mainWidget);
                return floatInput;
            }
        }
        case OptionType::KeyBind: {
            QKeySequenceEdit* keySequenceEdit = new QKeySequenceEdit(parent);
            getWidgetDimensions(option.rect, mainWidget, resetWidget, subWidget, addWidget, valueDisplayWidget, WidgetType::keybind);
            keySequenceEdit->setGeometry(mainWidget);
            makeResetButton(parent, knownOption, resetWidget);
            return keySequenceEdit;
        }
        default:
            break;
    }
};

void GameOptionDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    std::shared_ptr<GameOption> knownOption = contents->at(index.row()).knownOption;

    switch (contents->at(index.row()).type) {
        case OptionType::String: {
            QComboBox* comboBox;
            if ((comboBox = dynamic_cast<QComboBox*>(editor)) != nullptr) {
                comboBox->setCurrentIndex(comboBox->findText(contents->at(index.row()).value));
            } else {
                ((QLineEdit*)editor)->setText(contents->at(index.row()).value);
            }
            return;
        }
        case OptionType::Int: {
            QSlider* slider;
            if ((slider = dynamic_cast<QSlider*>(editor)) != nullptr) {
                slider->setValue(contents->at(index.row()).intValue);
            } else {
                ((QSpinBox*)editor)->setValue(contents->at(index.row()).intValue);
            }
            return;
        }
        case OptionType::Bool: {
            ((QCheckBox*)editor)->setText(contents->at(index.row()).value);
            ((QCheckBox*)editor)->setChecked(contents->at(index.row()).boolValue);
            return;
        }
        case OptionType::Float: {
            QSlider* slider;
            if ((slider = dynamic_cast<QSlider*>(editor)) != nullptr) {
                slider->setValue(contents->at(index.row()).floatValue * 100);
            } else {
                ((QDoubleSpinBox*)editor)->setValue(contents->at(index.row()).floatValue);
            }
            return;
        }
        case OptionType::KeyBind: {
            // TODO: fall back to QLineEdit if keybind can't be represented, like some mods do (by using another key input API)
            // TODO: mouse binding? If not possible, fall back as well maybe?
            Qt::Key key;
            for (auto& keyBinding : *keybindingOptions) {
                // this could become a std::find_if eventually, if someone wants to bother making it that.
                if (keyBinding->minecraftKeyCode == contents->at(index.row()).value) {
                    key = keyBinding->qtKeyCode.keyboardKey;
                }
            }
            ((QKeySequenceEdit*)editor)->setKeySequence(QKeySequence(key));
            return;
        }
        default:
            return;
    }
};

void GameOptionDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // define rects for widgets to be used in any type
    QRect mainWidget;
    QRect resetWidget;
    QRect subWidget;
    QRect addWidget;
    QRect valueDisplayWidget;

    getWidgetDimensions(option.rect, mainWidget, resetWidget, subWidget, addWidget, valueDisplayWidget, WidgetType::slider);
    editor->setGeometry(mainWidget);
};

void GameOptionDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    std::shared_ptr<GameOption> knownOption = contents->at(index.row()).knownOption;

    switch (contents->at(index.row()).type) {
        case OptionType::String: {
            QComboBox* comboBox;
            if ((comboBox = dynamic_cast<QComboBox*>(editor)) != nullptr) {
                contents->at(index.row()).value = comboBox->currentText();
            } else {
                contents->at(index.row()).value = ((QLineEdit*)editor)->text();
            }
            return;
        }
        case OptionType::Int: {
            QSlider* slider;
            if ((slider = dynamic_cast<QSlider*>(editor)) != nullptr) {
                contents->at(index.row()).intValue = slider->value();
            } else {
                contents->at(index.row()).intValue = ((QSpinBox*)editor)->value();
            }
            return;
        }
        case OptionType::Bool: {
            ((QCheckBox*)editor)->setText(contents->at(index.row()).value);
            contents->at(index.row()).boolValue = ((QCheckBox*)editor)->isChecked();
            return;
        }
        case OptionType::Float: {
            QSlider* slider;
            if ((slider = dynamic_cast<QSlider*>(editor)) != nullptr) {
                contents->at(index.row()).floatValue = slider->value() / 100.0f;
            } else {
                contents->at(index.row()).floatValue = ((QDoubleSpinBox*)editor)->value();
            }
            return;
        }
        case OptionType::KeyBind: {
            QKeySequenceEdit* keySequenceEdit = (QKeySequenceEdit*)editor;

            QString minecraftKeyCode;
            for (auto& keyBinding : *keybindingOptions) {
                // this could become a std::find_if eventually, if someone wants to bother making it that.
                if (keyBinding->qtKeyCode.keyboardKey == keySequenceEdit->keySequence()[0].key() ||
                    keyBinding->qtKeyCode.mouseButton == keySequenceEdit->keySequence()[0].key()) {
                    minecraftKeyCode = keyBinding->minecraftKeyCode;
                }
            }
            contents->at(index.row()).value = minecraftKeyCode;
            return;
        }
        default:
            return;
    }
}

QPushButton* GameOptionDelegate::makeResetButton(QWidget* parent, std::shared_ptr<GameOption>& knownOption, QRect rect)
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
