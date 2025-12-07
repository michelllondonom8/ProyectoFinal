#ifndef PORTADA_H
#define PORTADA_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>

class Nivel;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Portada : public QMainWindow
{
    Q_OBJECT

public:
    explicit Portada(QWidget *parent = nullptr);
    ~Portada();

private slots:
    void onBtnJugarClicked();
    void onBtnNivelesClicked();
    void avanzarNivel(int numero);
    void volverAlMenu();

private:
    void setMenuAsCentral();
    void mostrarNivel(int numero);
    int mostrarDialogoSeleccionNivel();

    Ui::MainWindow *ui;
    QWidget *menuWidget_;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    int nivelActual;
    Nivel *nivelActivo;  // NUEVO: Para mantener referencia al nivel actual
};

#endif // PORTADA_H
