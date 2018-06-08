#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QTimer>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QDebug>

#define Interval 20
#define Speed    5

class Github : public QGraphicsPixmapItem
{
public:
    Github(qreal width) {
        setPos(QRandomGenerator::global()->bounded(width), 0);
        setPixmap(QPixmap(":/GitHub-Mark-32px.png"));
    }
    virtual ~Github() {}
};

class Bullet : public QGraphicsEllipseItem
{
public:
    Bullet(const QPointF& startAt) {
        setRect(0, 0, 5, 8);
        setPos(startAt);
        setBrush(QBrush(Qt::black));
    }
    virtual ~Bullet() {}
};

class Fighter : public QGraphicsPixmapItem
{
public:
    Fighter() {
        setActive(false);
        setPixmap(QPixmap(":/3Mzz9.jpg"));
    }
    virtual ~Fighter() {}

    Bullet* attack() {
        const QSizeF& s = boundingRect().size()/2;
        return new Bullet(pos() + QPointF(s.width(), s.height()));
    }
};

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    Scene() {
        connect(&mClock, &QTimer::timeout, this, &Scene::handler);
        addItem(&mFighter);
    }
    virtual ~Scene() {}

    void start() {
        mFighter.setActive(true);
        mFighter.show();
        mClock.start(Interval);
    }

public slots:
    void handler() {
        QVector<Bullet*> bullets;
        QVector<Github*> githubs;
        foreach (QGraphicsItem* item, items()) {
            Bullet* b = qgraphicsitem_cast<Bullet*>(item);
            Github* g = dynamic_cast<Github*>(item);
            if(b != Q_NULLPTR) {
                if(b->y() > 0) {
                    b->moveBy(0, -3 * Speed);
                    bullets.push_back(b);
                } else {
                    removeItem(b);
                    delete b;
                }
            } else if (g != Q_NULLPTR) {
                if(g->y() < height()) {
                    g->moveBy(0, Speed);
                    githubs.push_back(g);
                } else {
                    removeItem(g);
                    delete g;
                }
            }
        }

        QSet<QGraphicsItem*> dead;
        foreach (Bullet* b, bullets) {
            foreach (Github* g, githubs) {
                if(g != Q_NULLPTR && b != Q_NULLPTR)
                    if(g->collidesWithItem(b)) {
                        bullets.removeOne(b);
                        githubs.removeOne(g);
                        dead.insert(b);
                        dead.insert(g);
                    }
            }
        }
        foreach (QGraphicsItem* item, dead) {
            removeItem(item);
            delete item;
        }

        addGithub();
    }
protected:
    void keyPressEvent(QKeyEvent* e) override {
        if(mFighter.isActive())
        switch (e->key()) {
        case Qt::Key_Up:
            mFighter.moveBy(0, -Speed);
            break;
        case Qt::Key_Down:
            mFighter.moveBy(0, Speed);
            break;
        case Qt::Key_Left:
            mFighter.moveBy(-Speed, 0);
            break;
        case Qt::Key_Right:
            mFighter.moveBy(Speed, 0);
            break;
        case Qt::Key_X:
            addBullet();
            break;
        default:
            break;
        }
    }
private:
    QTimer              mClock;
    Fighter            mFighter;
    int                 mG;

    void addBullet() {
        addItem(mFighter.attack());
    }

    void addGithub() {
        if(mG > 20){
            addItem(new Github(width()));
            mG = 0;
        } else {
            mG++;
        }
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QGraphicsView   mView;
    Scene           mScene;
protected:
    void resizeEvent(QResizeEvent* e);
};

#endif // MAINWINDOW_H
