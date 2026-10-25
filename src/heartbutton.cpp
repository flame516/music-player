#include "heartbutton.h"

#include <QPainter>
#include <QPainterPath>

HeartButton::HeartButton(QWidget *parent)
    : QPushButton(parent)
{
    setFixedSize(40, 40);
    setCursor(Qt::PointingHandCursor);
}

void HeartButton::setFavorite(bool favorite)
{
    if (m_isFavorite != favorite) {
        m_isFavorite = favorite;
        update(); // 触发重绘
    }
}

void HeartButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
    
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    
    const QRect rect = this->rect().adjusted(8, 8, -8, -8);
    const QPoint center = rect.center();
    const qreal size = qMin(rect.width(), rect.height()) * 0.6;
    
    // 绘制标准爱心形状
    QPainterPath heart;
    
    // 从底部中心点开始
    heart.moveTo(center.x(), center.y() + size * 0.3);
    
    // 左半部分（两个贝塞尔曲线形成左半心形）
    heart.cubicTo(center.x(), center.y(),
                 center.x() - size * 0.5, center.y() - size * 0.3,
                 center.x() - size * 0.5, center.y() + size * 0.1);
    heart.cubicTo(center.x() - size * 0.5, center.y() + size * 0.6,
                 center.x(), center.y() + size * 0.9,
                 center.x(), center.y() + size);
    
    // 右半部分（对称）
    heart.cubicTo(center.x(), center.y() + size * 0.9,
                 center.x() + size * 0.5, center.y() + size * 0.6,
                 center.x() + size * 0.5, center.y() + size * 0.1);
    heart.cubicTo(center.x() + size * 0.5, center.y() - size * 0.3,
                 center.x(), center.y(),
                 center.x(), center.y() + size * 0.3);
    
    if (m_isFavorite) {
        // 实心红色爱心
        p.setBrush(QColor("#ff5252"));
        p.setPen(Qt::NoPen);
        p.fillPath(heart, QColor("#ff5252"));
    } else {
        // 空心爱心（灰色边框）
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#455a64"), 2));
        p.strokePath(heart, QPen(QColor("#455a64"), 2));
    }
}

