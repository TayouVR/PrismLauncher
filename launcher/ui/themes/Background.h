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

struct ImageVariation {
    QString imagePath;
};

class Background {
   public:
    Background(QFileInfo manifest);
     
    /// @brief Simple ctor, if the user just wants to use one simple image file for the background
    /// @param name this is the ID and display name
    /// @param imageFile 
    Background(QString name, QFileInfo imageFile);

    QString getDefaultImage();

    /// @brief get all image IDs in this background object
    /// @return list of IDs
    QStringList getAvaliableImages();

    /// @brief Returns image based on conditions defined in manifest
    /// @param id Optional, if you need a specific image.
    /// @return resource/file path of the image
    QString getImage(QString id = "");

    QString displayName;
    QString id;

   private:
    QList<ImageVariation> imageVariants;
    QString defaultImage;
};
