#include<QDebug>
#include<tank.h>
#include "tank2.h"
#include<bullet.h>
#include<QTimer>
#include<QKeyEvent>
#include<QGraphicsScene>
#include<config.h>

Tank2::Tank2(const QPixmap &pixmap,MyScene *scene):QGraphicsPixmapItem(pixmap),tank_hp(100),speed(TANK_SPEED),energy(0),eCount(0),harm(0)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    b[0]=new Bullet(scene,this,-1);
    b[1]=new Bullet(scene,this,-1);
    b[2]=new Bullet(scene,this,-1);
    bulletNum=3;
    shootCD=true;
    bulletTurn=0;
    parent=scene;
    setPos(700,227);
    destroyed=false;
    unkillable=false;
    outofwar = false;
    recover_remaintime=5;
    tank_hp = TANK_HP;
    energy = 0;
}

void Tank2::destroy()
{
    parent->painthp(2,100,0);
    tank_hp=0;
    destroyed=true;
    unkillable=true;
    parent->removeItem(this);
    QTimer::singleShot(3000, [this]() {
        setPos(700,227);
        setRotation(0);
        parent->painthp(2,0,100);
        tank_hp=100;
        energy=fmax(energy-50,0);
        parent->paintenergy(2,100,energy);
        destroyed=false;
        parent->addItem(this);
        QList<QGraphicsItem *> collidingItems = this->collidingItems();
        for (QGraphicsItem *item : collidingItems)
        {
            if (item->type() == Tank::Type)
                ((Tank*)item)->destroy();
        }
        QTimer::singleShot(3000, [this]() {
            unkillable=false;
        });
    });
}

bool Tank2::collision()
{
    QList<QGraphicsItem *> collidingItems = this->collidingItems();
    if (!collidingItems.isEmpty())
        return true;
    return false;
}

void Tank2::turnLeft()
{
    setTransformOriginPoint(boundingRect().center());
    setRotation(rotation() - speed);
    if(collision())
        turnRight();
}
void Tank2::turnRight()
{
    setTransformOriginPoint(boundingRect().center());
    setRotation(rotation() + speed);
    if(collision())
        turnLeft();
}
void Tank2::goForward()
{
    qreal angle = rotation() * M_PI / 180;
    setPos(x() - speed*qCos(angle), y() - speed*qSin(angle));
    if(collision())
        goBack();
}
void Tank2::goBack()
{
    qreal angle = rotation() * M_PI / 180;
    setPos(x() + speed*qCos(angle), y() + speed*qSin(angle));
    if(collision())
        goForward();
}

void Tank2::shoot()
{
    if(shootCD&&bulletNum>0&&!destroyed)
    {
        shootCD=false;
        bulletNum--;
        resettimer();
        b[bulletTurn]->shoot(x()+18+qCos(rotation() * M_PI / 180+M_PI)*28,y()+10+qSin(rotation() * M_PI / 180+M_PI)*28,rotation() * M_PI / 180+M_PI);
        bulletTurn=(bulletTurn+1)%3;
        QTimer::singleShot(300, [this]() {
            shootCD=true;
        });
    }
}

void Tank2::beHarmed()
{
    if(harm==0)
        return;
    if(!unkillable)
    {
        parent->painthp(2,tank_hp,fmax(0,tank_hp-harm));
        tank_hp-=harm;
        if(harm>0)
            resettimer();
        if(tank_hp<=0)
            destroy();
    }
    harm=0;
}

void Tank2::addEnergy()
{
    if(eCount==0)
        return;
    parent->paintenergy(2,energy,fmin(100,energy+0.001*eCount));
    energy=fmin(100,energy+0.001*eCount);
    eCount=0;
}

void Tank2::handletimeout()
{
    recover_remaintime --;
    if(recover_remaintime==0)
    {
        outofwar = true;
        recover();
    }
    else if(outofwar)
    {
        recover();
    }
}
void Tank2::resettimer()
{
    recover_remaintime = 5;
    outofwar=false;
}
void Tank2::recover()
{
    if(tank_hp<TANK_HP)
    {
        parent->painthp(2,tank_hp,tank_hp+fmin(RECOVER_HP,TANK_HP-tank_hp));
        tank_hp += fmin(RECOVER_HP,TANK_HP-tank_hp);
    }
}

