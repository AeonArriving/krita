/*
 * This file is part of Krita
 *
 * Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
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

#ifndef KIS_BLUR_FILTER_H
#define KIS_BLUR_FILTER_H

#include "kis_filter.h"

class KisUnsharpFilter : public KisFilter
{
    public:
        KisUnsharpFilter();
    public:
        virtual void process(const KisPaintDeviceSP src, const QPoint& srcTopLeft, KisPaintDeviceSP dst, const QPoint& dstTopLeft, const QSize& size, KisFilterConfiguration* config);
        static inline KoID id() { return KoID("unsharpmask", i18n("Unsharp Mask")); };
        virtual bool supportsPainting() { return true; }
        virtual bool supportsPreview() { return true; }
        virtual bool supportsIncrementalPainting() { return false; }
        virtual bool supportsAdjustmentLayers() { return false; }
        virtual ColorSpaceIndependence colorspaceIndependence() { return FULLY_INDEPENDENT; };
    public:
        virtual KisFilterConfigWidget * createConfigurationWidget(QWidget* parent, KisPaintDeviceSP dev);
        virtual KisFilterConfiguration* configuration(QWidget*);
};

#endif
