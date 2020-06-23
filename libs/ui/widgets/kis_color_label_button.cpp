#include "kis_color_label_button.h"

#include <QStylePainter>
#include <QStyleOption>
#include <QMimeData>

#include "kis_global.h"
#include "kis_debug.h"
#include "krita_container_utils.h"

struct KisColorLabelButton::Private {
    QColor color;
};

KisColorLabelButton::KisColorLabelButton(QColor color, QWidget *parent) : QAbstractButton(parent), m_d(new Private())
{
    setCheckable(true);
    setChecked(true);
    m_d->color = color;
}

KisColorLabelButton::~KisColorLabelButton()
{

}

void KisColorLabelButton::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QStylePainter painter(this);
    QStyleOption styleOption;
    styleOption.initFrom(this);

    if (isDown() || isChecked()){
        styleOption.state |= QStyle::State_On;
    }

    // Draw fill..
    QRect fillRect = kisGrowRect(rect(), -2);
    fillRect.width();
    if (m_d->color.alpha() > 0) {
        QColor fillColor = m_d->color;

        if (!isChecked()) {
            fillColor.setAlpha(32);
        }

        painter.fillRect(fillRect, fillColor);
    } else {
        // draw an X for no color for the first item
        const int shortestEdge = std::min(fillRect.width(), fillRect.height());
        const int longestEdge = std::max(fillRect.width(), fillRect.height());
        bool horizontalIsShortest = (shortestEdge == fillRect.width());
        QRect srcRect = horizontalIsShortest ?
                    fillRect.adjusted(0, (longestEdge / 2) - (shortestEdge / 2), 0, (shortestEdge / 2) - (longestEdge / 2)) :
                    fillRect.adjusted((longestEdge / 2) - (shortestEdge / 2), 0, (shortestEdge / 2) - (longestEdge / 2), 0);
        QRect crossRect = kisGrowRect(srcRect, -1);

        QColor shade = styleOption.palette.text().color();

        if (!isChecked()) {
            shade.setAlpha(64);
        }

        painter.setPen(QPen(shade, 2));
        painter.drawLine(crossRect.topLeft(), crossRect.bottomRight());
        painter.drawLine(crossRect.bottomLeft(), crossRect.topRight());
    }
}

QSize KisColorLabelButton::sizeHint() const
{
    return QSize(16,32);
}

void KisColorLabelButton::nextCheckState()
{
    KisColorLabelButtonGroup* colorLabelButtonGroup = dynamic_cast<KisColorLabelButtonGroup*>(group());
    if (colorLabelButtonGroup && (colorLabelButtonGroup->viableCheckedButtonCount() > 1 || !isChecked())) {
        setChecked(!isChecked());
    } else {
        setChecked(isChecked());
    }
}

KisColorLabelButtonGroup::KisColorLabelButtonGroup(QObject *parent)
    : QButtonGroup(parent)
{
    connect(this, SIGNAL(buttonToggled(QAbstractButton*,bool)), this, SLOT(slotRegisterButtonState(QAbstractButton*,bool)));
}

KisColorLabelButtonGroup::~KisColorLabelButtonGroup() {

}

QList<QAbstractButton *> KisColorLabelButtonGroup::viableButtons() {
    QList<QAbstractButton*> viableButtons = this->buttons();
    KritaUtils::filterContainer(viableButtons, [](QAbstractButton* btn){
        return (btn->isVisible());
    });
    return viableButtons;
}

QList<QAbstractButton *> KisColorLabelButtonGroup::checkedViableButtons() {
    QList<QAbstractButton*> checkedButtons = viableButtons();
    KritaUtils::filterContainer(checkedButtons, [](QAbstractButton* btn){
       return (btn->isChecked());
    });

    return checkedButtons;
}

int KisColorLabelButtonGroup::viableCheckedButtonCount() {
    return checkedViableButtons().count();
}

void KisColorLabelButtonGroup::reset()
{
    Q_FOREACH( QAbstractButton* btn, buttons()) {
        btn->setChecked(true);
    }
}
