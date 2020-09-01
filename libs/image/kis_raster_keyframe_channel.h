/*
 *  Copyright (c) 2015 Jouni Pentikäinen <joupent@gmail.com>
 *  Copyright (c) 2020 Emmet O'Neill <emmetoneill.pdx@gmail.com>
 *  Copyright (c) 2020 Eoin O'Neill <eoinoneill1991@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _KIS_RASTER_KEYFRAME_CHANNEL_H
#define _KIS_RASTER_KEYFRAME_CHANNEL_H

#include "kis_keyframe_channel.h"


class KRITAIMAGE_EXPORT KisRasterKeyframe : public KisKeyframe
{
public:
    KisRasterKeyframe(KisPaintDeviceWSP paintDevice);
    KisRasterKeyframe(KisPaintDeviceWSP paintDevice, int premadeFrameID);
    ~KisRasterKeyframe() override;

    bool hasContent();
    QRect bounds();
    int frameID() const;

    KisKeyframeSP duplicate(KisKeyframeChannel *channel = 0) override;

private:
    KisRasterKeyframe(const KisRasterKeyframe &rhs);

    int m_frameId;
    KisPaintDeviceWSP m_paintDevice;
};


class KRITAIMAGE_EXPORT KisRasterKeyframeChannel : public KisKeyframeChannel
{
    Q_OBJECT

public:
    KisRasterKeyframeChannel(const KoID& id, const KisPaintDeviceWSP paintDevice, KisNodeWSP parent);
    KisRasterKeyframeChannel(const KoID& id, const KisPaintDeviceWSP paintDevice, const KisDefaultBoundsBaseSP bounds);
    KisRasterKeyframeChannel(const KisRasterKeyframeChannel &rhs, KisNodeWSP newParent, const KisPaintDeviceWSP newPaintDevice);
    ~KisRasterKeyframeChannel() override;

    /**
     * Return the ID of the active frame at a given time. The active frame is
     * defined by the keyframe at the given time or the last keyframe before it.
     * @param time
     * @return active frame id
     */
    Q_DECL_DEPRECATED int frameIdAt(int time) const;

    /**
     * Copy the active frame at given time to target device.
     * @param keyframe keyframe to copy from
     * @param targetDevice device to copy the frame to
     */
    Q_DECL_DEPRECATED void fetchFrame(KisKeyframeSP keyframe, KisPaintDeviceSP targetDevice);

    void fetchFrame(int time, KisPaintDeviceSP targetDevice);

    /**
     * Copy the content of the sourceDevice into a new keyframe at given time
     * @param time position of new keyframe
     * @param sourceDevice source for content
     * @param parentCommand parent command used for stacking
     */
    void importFrame(int time, KisPaintDeviceSP sourceDevice, KUndo2Command *parentCommand);

    QRect frameExtents(KisKeyframeSP keyframe);

    QString frameFilename(int frameId) const;

    /**
     * When choosing filenames for frames, this will be appended to the node filename
     */
    void setFilenameSuffix(const QString &suffix);

    QDomElement toXML(QDomDocument doc, const QString &layerFilename) override;
    void loadXML(const QDomElement &channelNode) override;

    void setOnionSkinsEnabled(bool value);
    bool onionSkinsEnabled() const;

    KisPaintDeviceWSP paintDevice();

private:

    QRect affectedRect(int time) const override;

    void saveKeyframe(KisKeyframeSP keyframe, QDomElement keyframeElement, const QString &layerFilename) override;
    QPair<int, KisKeyframeSP> loadKeyframe(const QDomElement &keyframeNode) override;

    KisKeyframeSP createKeyframe() override;

    void setFrameFilename(int frameId, const QString &filename);
    QString chooseFrameFilename(int frameId, const QString &layerFilename);
    int frameId(KisKeyframeSP keyframe) const;
    int frameId(const KisKeyframe *keyframe) const;

    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
