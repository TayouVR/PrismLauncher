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
#include "Background.h"

#include <Json.h>

static bool readManifestJson(const QString& path, QString& name, QString& id, QString& defaultImagePath)
{
    QFileInfo pathInfo(path);
    if (pathInfo.exists() && pathInfo.isFile()) {
        try {
            auto doc = Json::requireDocument(path, "Background manifest file");
            const QJsonObject root = doc.object();

            // name and ID
            name = Json::requireString(root, "name", "Background name");
            id = Json::requireString(root, "id", "Background ID");

            // default image
            defaultImagePath = Json::requireString(root, "imagePath", "Path to Image");

            // list of image variations and their conditions
            auto imageVariantsRoot = Json::requireObject(root, "variants", "Seasonal Variants");
            for each (object variant in Json::ensureArray(imageVariantsRoot)) {

            }

            // TODO continue ripping this out and writing the proper logic here!
            auto readColor = [&](QString colorName) -> QColor {
                auto colorValue = Json::ensureString(imageVariantsRoot, colorName, QString());
                if (!colorValue.isEmpty()) {
                    QColor color(colorValue);
                    if (!color.isValid()) {
                        themeWarningLog() << "Color value" << colorValue << "for" << colorName << "was not recognized.";
                        return QColor();
                    }
                    return color;
                }
                return QColor();
            };
            auto readAndSetColor = [&](QPalette::ColorRole role, QString colorName) {
                auto color = readColor(colorName);
                if (color.isValid()) {
                    palette.setColor(role, color);
                } else {
                    themeDebugLog() << "Color value for" << colorName << "was not present.";
                }
            };

            // palette
            readAndSetColor(QPalette::Window, "Window");
            readAndSetColor(QPalette::WindowText, "WindowText");
            readAndSetColor(QPalette::Base, "Base");
            readAndSetColor(QPalette::AlternateBase, "AlternateBase");
            readAndSetColor(QPalette::ToolTipBase, "ToolTipBase");
            readAndSetColor(QPalette::ToolTipText, "ToolTipText");
            readAndSetColor(QPalette::Text, "Text");
            readAndSetColor(QPalette::Button, "Button");
            readAndSetColor(QPalette::ButtonText, "ButtonText");
            readAndSetColor(QPalette::BrightText, "BrightText");
            readAndSetColor(QPalette::Link, "Link");
            readAndSetColor(QPalette::Highlight, "Highlight");
            readAndSetColor(QPalette::HighlightedText, "HighlightedText");

            // fade
            fadeColor = readColor("fadeColor");
            fadeAmount = Json::ensureDouble(imageVariantsRoot, "fadeAmount", 0.5, "fade amount");

        } catch (const Exception& e) {
            themeWarningLog() << "Couldn't load manifest json: " << e.cause();
            return false;
        }
    } else {
        themeDebugLog() << "No manifest json present.";
        return false;
    }
    return true;
}

Background::Background(QFileInfo manifest) {
}

Background::Background(QString name, QFileInfo manifest) {}

QString Background::getDefaultBackground() {}

QString Background::getBackground() {}
