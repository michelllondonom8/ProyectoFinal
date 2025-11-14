#include "portada.h"
#include "ui_mainwindow.h"
#include "nivel.h"

#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>
#include <QFont>
#include <QLabel>
#include <QDialog>

Portada::Portada(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    showFullScreen();
    audioOutput = new QAudioOutput(this);
    player = new QMediaPlayer(this);
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl("qrc:/sound/sounds/batalla.mp3"));
    audioOutput->setVolume(0.5);
    player->setLoops(QMediaPlayer::Infinite);
    player->play();

    setMenuAsCentral();
}

Portada::~Portada()
{
    delete ui;
}

void Portada::setMenuAsCentral()
{
    menuWidget_ = new QWidget(this);
    menuWidget_->setObjectName("centralWidget");

    QVBoxLayout *layout = new QVBoxLayout(menuWidget_);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *titulo = new QLabel(tr("IMPERIO ROMANO"), menuWidget_);
    titulo->setAlignment(Qt::AlignCenter);
    QFont f = titulo->font();
    f.setPointSize(36);
    f.setBold(true);
    titulo->setFont(f);
    titulo->setStyleSheet("color: rgb(255,215,0); text-shadow: 2px 2px 4px black;");

    QPushButton *btnJugar = new QPushButton(tr("Jugar"), menuWidget_);
    QPushButton *btnNiveles = new QPushButton(tr("Seleccionar Nivel"), menuWidget_);
    QPushButton *btnSalir = new QPushButton(tr("Salir"), menuWidget_);

    QList<QPushButton*> botones = {btnJugar, btnNiveles, btnSalir};
    for (auto *btn : botones) {
        btn->setFixedSize(250, 60);
        btn->setStyleSheet(
            "QPushButton {"
            "  background-color: rgba(0,0,0,170);"
            "  color: #FFFFFF;"
            "  border-radius: 12px;"
            "  font-size: 20px;"
            "  font-weight: bold;"
            "}"
            "QPushButton:hover { background-color: rgba(255,255,255,40); }"
            );
        layout->addWidget(btn, 0, Qt::AlignHCenter);
    }

    layout->setSpacing(25);
    layout->insertWidget(0, titulo, 0, Qt::AlignHCenter);

    menuWidget_->setStyleSheet(
        "QWidget#centralWidget {"
        "  border-image: url(:/images/images/portada.jpeg) 0 0 0 0 stretch stretch;"
        "}"
        );

    connect(btnJugar, &QPushButton::clicked, this, &Portada::onBtnJugarClicked);
    connect(btnNiveles, &QPushButton::clicked, this, &Portada::onBtnNivelesClicked);
    connect(btnSalir, &QPushButton::clicked, this, &Portada::close);

    setCentralWidget(menuWidget_);
}
void Portada::onBtnJugarClicked()
{
    nivelActual = 1;
    mostrarNivel(nivelActual);
}
void Portada::onBtnNivelesClicked()
{
    int seleccionado = mostrarDialogoSeleccionNivel();

    if (seleccionado >= 1 && seleccionado <= 3) {
        nivelActual = seleccionado;
        mostrarNivel(nivelActual);
    }
}
int Portada::mostrarDialogoSeleccionNivel()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Seleccionar nivel"));
    dlg.setMinimumSize(350, 260);

    QVBoxLayout *v = new QVBoxLayout(&dlg);

    QListWidget *list = new QListWidget(&dlg);
    list->addItem("Nivel 1 - Coliseo Romano");
    list->addItem("Nivel 2 - Templo Oscuro");
    list->addItem("Nivel 3 - Defensa de Roma");

    QPushButton *btnAceptar = new QPushButton(tr("Seleccionar"), &dlg);
    QPushButton *btnCancelar = new QPushButton(tr("Cancelar"), &dlg);

    v->addWidget(list);
    v->addStretch();
    v->addWidget(btnAceptar);
    v->addWidget(btnCancelar);

    QObject::connect(btnAceptar, &QPushButton::clicked, [&]() {
        if (!list->currentItem()) {
            QMessageBox::warning(&dlg, tr("Atención"), tr("Selecciona un nivel primero."));
            return;
        }
        dlg.accept();
    });

    QObject::connect(btnCancelar, &QPushButton::clicked, &dlg, &QDialog::reject);
    QObject::connect(list, &QListWidget::itemActivated, [&](QListWidgetItem*) {
        dlg.accept();
    });

    int result = dlg.exec();

    if (result == QDialog::Accepted)
        return list->currentRow() + 1;

    return 0; // cancelado
}
void Portada::mostrarNivel(int numero)
{
    player->pause();

    Nivel *nivel = new Nivel(numero, this);
    connect(nivel, &Nivel::nivelCompletado, this, &Portada::avanzarNivel);

    nivel->exec();
}
void Portada::avanzarNivel(int numero)
{
    if (numero < 3) {
        nivelActual = numero + 1;
        mostrarNivel(nivelActual);
    } else {
        QMessageBox::information(this, tr("Juego completado"),
                                 tr("¡Has completado todos los niveles!\n\nVolviendo al menú..."));
        volverAlMenu();
    }
}
void Portada::volverAlMenu()
{
    setCentralWidget(menuWidget_);
    player->play();
}
