/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
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

#include "kis_painting_assistants_decoration.h"

#include <cfloat>

#include <QList>
#include <QPointF>
#include <klocalizedstring.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include "kis_debug.h"
#include "KisDocument.h"
#include "kis_canvas2.h"

#include <QPainter>

struct KisPaintingAssistantsDecoration::Private {
    Private()
        : assistantVisible(false)
        , outlineVisible(false)
        , snapOnlyOneAssistant(true)
        , firstAssistant(0)
        , aFirstStroke(false)
    {}

    bool assistantVisible;
    bool outlineVisible;
    bool snapOnlyOneAssistant;
    KisPaintingAssistantSP firstAssistant;
    bool aFirstStroke;
    QColor m_assistantsColor;
    bool m_isEditingAssistants = false;

    KisCanvas2 * m_canvas = 0;
};

KisPaintingAssistantsDecoration::KisPaintingAssistantsDecoration(QPointer<KisView> parent) :
    KisCanvasDecoration("paintingAssistantsDecoration", parent),
    d(new Private)
{
    setAssistantVisible(true);
    setOutlineVisible(true);
    d->snapOnlyOneAssistant = true; //turn on by default.
}

KisPaintingAssistantsDecoration::~KisPaintingAssistantsDecoration()
{
    delete d;
}

void KisPaintingAssistantsDecoration::addAssistant(KisPaintingAssistantSP assistant)
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();
    if (assistants.contains(assistant)) return;

    assistants.append(assistant);

    view()->document()->setAssistants(assistants);
    setVisible(!assistants.isEmpty());
    emit assistantChanged();
}

void KisPaintingAssistantsDecoration::removeAssistant(KisPaintingAssistantSP assistant)
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();
    KIS_ASSERT_RECOVER_NOOP(assistants.contains(assistant));

    if (assistants.removeAll(assistant)) {
        view()->document()->setAssistants(assistants);
        setVisible(!assistants.isEmpty());
        emit assistantChanged();
    }
}

void KisPaintingAssistantsDecoration::removeAll()
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();
    assistants.clear();
    view()->document()->setAssistants(assistants);
    setVisible(!assistants.isEmpty());

    emit assistantChanged();
}

QPointF KisPaintingAssistantsDecoration::adjustPosition(const QPointF& point, const QPointF& strokeBegin)
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();

    if (assistants.empty()) return point;
    if (assistants.count() == 1) {
        if(assistants.first()->isSnappingActive() == true){
            QPointF newpoint = assistants.first()->adjustPosition(point, strokeBegin);
            // check for NaN
            if (newpoint.x() != newpoint.x()) return point;
            return newpoint;
        }
    }
    QPointF best = point;
    double distance = DBL_MAX;
    //the following tries to find the closest point to stroke-begin. It checks all assistants for the closest point//
    if(!d->snapOnlyOneAssistant){
        Q_FOREACH (KisPaintingAssistantSP assistant, assistants) {
            if(assistant->isSnappingActive() == true){//this checks if the assistant in question has it's snapping boolean turned on//
                QPointF pt = assistant->adjustPosition(point, strokeBegin);
                if (pt.x() != pt.x()) continue;
                double dist = qAbs(pt.x() - point.x()) + qAbs(pt.y() - point.y());
                if (dist < distance) {
                    best = pt;
                    distance = dist;
                }
            }
        }
    } else if (d->aFirstStroke==false) {
        Q_FOREACH (KisPaintingAssistantSP assistant, assistants) {
            if(assistant->isSnappingActive() == true){//this checks if the assistant in question has it's snapping boolean turned on//
                QPointF pt = assistant->adjustPosition(point, strokeBegin);
                if (pt.x() != pt.x()) continue;
                double dist = qAbs(pt.x() - point.x()) + qAbs(pt.y() - point.y());
                if (dist < distance) {
                    best = pt;
                    distance = dist;
                    d->firstAssistant = assistant;
                }
            }
        }
    } else if(d->firstAssistant) {
        //make sure there's a first assistant to begin with.//
        best = d->firstAssistant->adjustPosition(point, strokeBegin);
    } else {
        d->aFirstStroke=false;
    }
    //this is here to be compatible with the movement in the perspective tool.
    qreal dx = point.x() - strokeBegin.x(), dy = point.y() - strokeBegin.y();
    if (dx * dx + dy * dy >= 4.0) {
        // allow some movement before snapping
        d->aFirstStroke=true;
    }
    return best;
}

void KisPaintingAssistantsDecoration::endStroke()
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();

    d->aFirstStroke=false;
    Q_FOREACH (KisPaintingAssistantSP assistant, assistants) {
        assistant->endStroke();
    }
}

void KisPaintingAssistantsDecoration::drawDecoration(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter *converter,KisCanvas2* canvas)
{
    if (!canvas) {
        dbgFile<<"canvas does not exist in painting assistant decoration, you may have passed arguments incorrectly:"<<canvas;
    } else {
        d->m_canvas = canvas;
    }

    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();

    Q_FOREACH (KisPaintingAssistantSP assistant, assistants) {
        assistant->setAssistantColor(assistantsColor());

        // forcing to do a clean draw helps when we go from editing to painting
        bool useCache = true;
        if (d->m_isEditingAssistants) {
            useCache = false;
        }

        assistant->drawAssistant(gc, updateRect, converter, useCache, canvas, assistantVisibility(), outlineVisibility());


    }
}
//drawPreview//

QList<KisPaintingAssistantHandleSP> KisPaintingAssistantsDecoration::handles()
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();

    QList<KisPaintingAssistantHandleSP> hs;
    Q_FOREACH (KisPaintingAssistantSP assistant, assistants) {
        Q_FOREACH (const KisPaintingAssistantHandleSP handle, assistant->handles()) {
            if (!hs.contains(handle)) {
                hs.push_back(handle);
            }
        }
        Q_FOREACH (const KisPaintingAssistantHandleSP handle, assistant->sideHandles()) {
            if (!hs.contains(handle)) {
                hs.push_back(handle);
            }
        }
    }
    return hs;
}

QList<KisPaintingAssistantSP> KisPaintingAssistantsDecoration::assistants()
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();
    return assistants;
}

void KisPaintingAssistantsDecoration::setAssistantVisible(bool set)
{
    d->assistantVisible=set;
}

void KisPaintingAssistantsDecoration::setOutlineVisible(bool set)
{
    d->outlineVisible=set;
}

void KisPaintingAssistantsDecoration::setOnlyOneAssistantSnap(bool assistant)
{
    d->snapOnlyOneAssistant = assistant;
}

bool KisPaintingAssistantsDecoration::assistantVisibility()
{
    return d->assistantVisible;
}
bool KisPaintingAssistantsDecoration::outlineVisibility()
{
    return d->outlineVisible;
}
void KisPaintingAssistantsDecoration::uncache()
{
    QList<KisPaintingAssistantSP> assistants = view()->document()->assistants();

    Q_FOREACH (KisPaintingAssistantSP assistant, assistants) {
        assistant->uncache();
    }
}
void KisPaintingAssistantsDecoration::toggleAssistantVisible()
{
    setAssistantVisible(!assistantVisibility());
    uncache();
}

void KisPaintingAssistantsDecoration::toggleOutlineVisible()
{
    setOutlineVisible(!outlineVisibility());
}

QColor KisPaintingAssistantsDecoration::assistantsColor() {
    return d->m_assistantsColor;
}

void KisPaintingAssistantsDecoration::setAssistantsColor(QColor color)
{
    d->m_assistantsColor = color;
}

void KisPaintingAssistantsDecoration::activateAssistantsEditor()
{
    setVisible(true); // this turns on the decorations in general. we leave it on at this point
    d->m_isEditingAssistants = true;
}

void KisPaintingAssistantsDecoration::deactivateAssistantsEditor()
{
    if (!d->m_canvas) {
        return;
    }

    d->m_isEditingAssistants = false; // some elements are hidden when we aren't editing

    // draw decoration again in case some elements need to be show or hidden
    QPainter painter;
    painter.setPen(Qt::NoPen);
    painter.setBrush(assistantsColor());
    QRectF canvasSize = QRectF(QPointF(0, 0), QSizeF(d->m_canvas->image()->size()));
    drawDecoration(painter, canvasSize, d->m_canvas->coordinatesConverter(), d->m_canvas );
}

bool KisPaintingAssistantsDecoration::isEditingAssistants()
{
    return d->m_isEditingAssistants;
}

