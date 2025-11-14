#pragma once
#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>

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
    void mostrarNivel(int numero);
    void avanzarNivel(int numero);
    void volverAlMenu();

private:
    Ui::MainWindow *ui;

    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    int nivelActual = 1;
    QWidget *menuWidget_ = nullptr;

    void setMenuAsCentral();
    int mostrarDialogoSeleccionNivel();
};
