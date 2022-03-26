//********************************************************
/// @brief 滚动区域，支持设置吸顶部件
/// @author y974183789@gmail.com
/// @date 2022/02/22
/// @note setCeilingWidget 设置吸顶部件，会清空之前设置的
///       addCeilingWidget 添加要吸顶的部件;不清空旧的
/// @version 1.0.0
//********************************************************

#pragma once

#include <QtWidgets/QScrollArea>
#include <QtCore/QMap>

class MScrollArea : public QScrollArea {
    Q_OBJECT

public:
    MScrollArea(QWidget* parent = nullptr);
    ~MScrollArea();

    //设置悬浮部件
    void setFloatingWidget(QWidget* w, QLayout* lay);
    void addFloatingWidget(QWidget* w, QLayout* lay);

protected:
    virtual void scrollContentsBy(int dx, int dy) override;

private:
    //调整悬浮部件位置
    void adjustFloatingWidget();
    //重置
    void resetFloatingWidget();

private:
    QMap<QWidget*, QLayout*> m_mapCeiling; //需要吸顶效果的部件
    QWidget* m_pwFloating;//当前吸顶的部件指针
    QWidget* m_pwPlaceholder;//占位窗口

};



