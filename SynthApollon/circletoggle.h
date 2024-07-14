#ifndef CIRCLETOGGLE_H
#define CIRCLETOGGLE_H

//#include "qpainter.h"
#include <QPushButton>
#include <QPainter>

/**
 * Cette classe est utilisé pour afficher la toggle rouge/vert des contrôles de synthétiseur.
 *
 *
 */

class CircleToggle : public QPushButton {
    Q_OBJECT
public:
    CircleToggle(QWidget* parent = nullptr)
        : QPushButton{parent}
    {
        setCheckable(true);
    }

    bool isPressed() const {
        return isChecked();
    }

    void hideToggle(bool visible) {
        if (!visible) {
            QWidget::show();
        } else {
            QWidget::hide();
        }
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(isChecked() ? Qt::green : Qt::red);
        painter.drawEllipse(rect());
    }

};

#endif // CIRCLETOGGLE_H
