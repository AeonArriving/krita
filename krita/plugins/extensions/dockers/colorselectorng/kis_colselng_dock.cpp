/*
 *  Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
 *  Copyright (c) 2010 Adam Celarek <kdedev at xibo dot at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_colselng_dock.h"

#include <klocale.h>
#include <KoCanvasBase.h>

#include "kis_colselng_widget.h"
#include "kis_canvas_resource_provider.h"

#include <KoColorSpaceRegistry.h>

KisColSelNgDock::KisColSelNgDock()
        : QDockWidget()
        , m_canvas(0)
{
    m_colorSelectorNgWidget = new KisColSelNgWidget(this);

    setWidget(m_colorSelectorNgWidget);
    m_colorSelectorNgWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    connect(m_colorSelectorNgWidget, SIGNAL(colorChanged(const QColor&)),
//            this, SLOT(colorChangedProxy(const QColor&)));

    setWindowTitle(i18n("Color Selector Ng"));
}

void KisColSelNgDock::setCanvas(KoCanvasBase * canvas)
{
//    m_canvas = canvas;
//    connect(m_canvas->resourceManager(), SIGNAL(resourceChanged(int, const QVariant&)),
//            this, SLOT(resourceChanged(int, const QVariant&)));
//    m_colorSelectorNgWidget->setQColor(m_canvas->resourceManager()->foregroundColor().toQColor());
}

void KisColSelNgDock::colorChangedProxy(const QColor& c)
{
//    if (m_canvas)
//        m_canvas->resourceManager()->setForegroundColor(KoColor(c , KoColorSpaceRegistry::instance()->rgb8()));
}

void KisColSelNgDock::resourceChanged(int key, const QVariant& v)
{
//    if (key == KoCanvasResource::ForegroundColor) {
//        m_colorSelectorNgWidget->setQColor(v.value<KoColor>().toQColor());
//    }
}

#include "kis_colselng_dock.moc"
