//********************************************************
/// @brief 滚动区域，支持设置吸顶部件
/// @author y974183789@gmail.com
/// @date 2022/02/22
/// @note setCeilingWidget 设置吸顶部件，会清空之前设置的
///       addCeilingWidget 添加要吸顶的部件;不清空旧的
/// @version 1.0.0
//********************************************************

#include "MScrollArea.h"
#include <QtWidgets/QLayout>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QScroller>
#include <QtCore/QDebug>

MScrollArea::MScrollArea(QWidget* parent)
: QScrollArea(parent)
, m_pwCeiling(Q_NULLPTR)
, m_pwPlaceholder(Q_NULLPTR)
{
}

MScrollArea::~MScrollArea()
{

}

void MScrollArea::setCeilingWidget(QWidget *w, QLayout* lay) {
    if(Q_NULLPTR == w || (Q_NULLPTR != m_pwCeiling && m_pwCeiling != w)) {
        //置位空时或者悬浮部件变化，需要做下还原
        resetCeilingWidget();
        m_pwCeiling = Q_NULLPTR;
    }
    m_mapCeiling.clear();
    if(Q_NULLPTR == w) {
        return;
    }
    addCeilingWidget(w, lay);
}

void MScrollArea::addCeilingWidget(QWidget* w, QLayout* lay) {
    m_pwCeiling = w;
    m_mapCeiling[w] = lay;
    if(Q_NULLPTR == m_pwPlaceholder) {
        m_pwPlaceholder = new QWidget(this);
        m_pwPlaceholder->hide();
    }
}

void MScrollArea::scrollContentsBy(int dx, int dy){
    QScrollArea::scrollContentsBy(dx,dy);
    adjustCeilingWidget();
}

void MScrollArea::resetCeilingWidget() {
    auto pCurrentLay = m_mapCeiling.value(m_pwCeiling);
    if (Q_NULLPTR == m_pwCeiling || Q_NULLPTR == pCurrentLay) {
        return;
    }
    if(-1 != pCurrentLay->indexOf(m_pwPlaceholder)
    && -1 == pCurrentLay->indexOf(m_pwCeiling)) {
        //把占位部件置换出来
        pCurrentLay->replaceWidget(m_pwPlaceholder, m_pwCeiling, Qt::FindDirectChildrenOnly);
        m_pwPlaceholder->hide();
    }
}

void MScrollArea::adjustCeilingWidget() {
    if(Q_NULLPTR == m_pwPlaceholder) {
        return;
    }
    if (Q_NULLPTR == m_pwCeiling || m_mapCeiling.isEmpty()) {
        return;
    }
    auto pCurrentLay = m_mapCeiling.value(m_pwCeiling);
    if(Q_NULLPTR == pCurrentLay) {
        return;
    }

    QPoint pt(horizontalScrollBar()->value(), verticalScrollBar()->value());
    //当前左上角坐标
    //查找里当前距离左上角最近的吸顶
    auto y = m_pwPlaceholder->mapTo(this, m_pwPlaceholder->geometry().topLeft()).y();
    auto minOffsetY = INT_MAX;
    QWidget* willShowCeiling = m_pwCeiling;
    for (auto ceiling : m_mapCeiling.keys()) {
        if (Q_NULLPTR != ceiling) {
            if(m_pwCeiling == ceiling) {
                y = m_pwPlaceholder->mapTo(this, m_pwPlaceholder->geometry().topLeft()).y();
            } else {
                y = ceiling->mapTo(this, ceiling->geometry().topLeft()).y();
            }
            if(y >= pt.y()) {
                //部件在左下角下方，不需要吸顶
                continue;
            }
            if(qAbs(pt.y() - y) < minOffsetY) {
                willShowCeiling = ceiling;
            }
            minOffsetY = qMin(minOffsetY, qAbs(pt.y() - y));
        }
    }
    if(willShowCeiling != m_pwCeiling) {
        resetCeilingWidget();
        m_pwCeiling = willShowCeiling;
    }
    m_pwPlaceholder->setFixedSize(m_pwCeiling->size());
    bool bCeiling = false;
    if(-1 != pCurrentLay->indexOf(m_pwPlaceholder)) {
        //占位部件的可见高度和实际高度不一致时，需要显示悬浮部件
        bCeiling = m_pwPlaceholder->visibleRegion().boundingRect().height() != m_pwPlaceholder->height();
        y = m_pwPlaceholder->mapTo(this, m_pwPlaceholder->geometry().topLeft()).y();

    } else if(-1 != pCurrentLay->indexOf(m_pwCeiling)) {
        //待悬浮部件的可见高度和实际高度不一致时，需要显示悬浮部件
        bCeiling = m_pwCeiling->visibleRegion().boundingRect().height() != m_pwCeiling->height();
        y = m_pwCeiling->mapTo(this, m_pwCeiling->geometry().topLeft()).y();
    }
    //待悬浮部件的左上角Y坐标小于滚动区域的左上角Y时，才需要吸顶
    if (bCeiling && y < pt.y()) {
        pCurrentLay->replaceWidget(m_pwCeiling, m_pwPlaceholder, Qt::FindDirectChildrenOnly);
        m_pwPlaceholder->show();
        //悬浮部件调整位置
        auto margins = pCurrentLay->contentsMargins();
        //左上角坐标
        m_pwCeiling->move(pt + QPoint(margins.left(), 0));
        m_pwCeiling->raise();
    } else {
        //不需要悬浮，则把悬浮部件还原至固定位置
        pCurrentLay->replaceWidget(m_pwPlaceholder, m_pwCeiling, Qt::FindDirectChildrenOnly);
        m_pwPlaceholder->hide();
    }
}
