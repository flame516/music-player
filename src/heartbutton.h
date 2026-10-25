#ifndef HEARTBUTTON_H
#define HEARTBUTTON_H

#include <QPushButton>

class HeartButton : public QPushButton
{
    Q_OBJECT
public:
    explicit HeartButton(QWidget *parent = nullptr);
    
    void setFavorite(bool favorite);
    bool isFavorite() const { return m_isFavorite; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_isFavorite = false;
};

#endif // HEARTBUTTON_H

