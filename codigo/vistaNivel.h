#ifndef VISTANIVEL_H
#define VISTANIVEL_H
#include <QGraphicsView>
#include <QElapsedTimer>

class QGraphicsScene;
class QTimer;
class QKeyEvent;

class vistaNivel:public QGraphicsView
{
    Q_OBJECT
private:
    QTimer* reloj;
    QElapsedTimer cronometro;
    double acumulado;
    bool pausado;
private slots:
    void avanzar();
protected:
    //la escena la hacen las hijas
    QGraphicsScene* escena;

    explicit vistaNivel(QWidget* parent = nullptr);

    //ni idea como describir
    virtual void avanzarLogica(double dt) = 0;
    virtual void dibujar(double alfa) = 0;
    virtual void alPulsar(int tecla) = 0;
    virtual void alSoltar(int tecla) = 0;
    virtual void alPerderFoco();

    void terminar(bool gano);
    //los controles
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;
public:
    //"momentos"
    void iniciar();
    void pausar();
    double getAcumulado() const;
    bool getPausado() const;
signals:
    void terminado(bool gano);
};

#endif // VISTANIVEL_H
