#ifndef QTBOOGLE_H
#define QTBOOGLE_H

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QElapsedTimer>
#include <QEvent>
#include <QFile>
#include <QGridLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QRandomGenerator>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QTime>
#include <QTimer>
#include <random>


#include "libBoggle.h"

class qtBoogle : public QMainWindow
{
    Q_OBJECT

public:
    qtBoogle(QWidget *parent = nullptr);
    ~qtBoogle();
private:
    unsigned int m_l=370;  // dimension de la fenetre
    unsigned int m_L=280;  // dimension de la fenetre
    unsigned int m_totalTimeInSec = 60*3;  // dure du jeu
    unsigned int m_seed = 0;
    QPushButton* m_solvePushBtn;
    QPushButton* m_altViewPushBtn;
    QPushButton* m_playPushBtn;
    QPushButton* m_DureePushBtn;
    QLabel* m_legLabel;
    QLabel* m_workProofLabel;
    QCheckBox* m_RotateCheckBox;
    boggleSolver* m_bs;
    std::vector<char*> m_dicoContent;
    std::vector<QStackedWidget*> m_lesWidg;
    std::vector<QChar> m_lesQChars;  // donc entre 'A' et 'Z' ou '_'
    std::vector<QLineEdit*> m_lesQLineEdits;
    std::vector<QLabel*> m_lesQLabels;
    QString m_prefix;
    std::vector<QChar> m_Orient = {QChar('0'),QChar('1'),QChar('2'),QChar('3')};
    QProgressBar* m_progBar;
    QRandomGenerator* m_R;
    std::vector<QString> m_Lettres = {
        "NIGETV",
        "NDOEST",
        "ZDVNEA",
        "WULIER",
        "MAQOBJ",
        "MAROIS",
        "MADECP",
        "OAAITE",
        "HSFEIE",
        "TPSLEU",
        "LARECS",
        "ALIBTR",
        "NOUEKT",
        "RIFAXO",
        "YLGNUE",
        "HESINR"
       };

    //                                            METHODES
    void altView();
    void entreeChangee(unsigned int i);
    void play();
    void solve();
    void changeDuree();
    void tourne();
    QString imageFileName(unsigned int i);
    QString imageFileName(QChar c);
    unsigned int numero(QString);
    void QRandomPerm(std::vector<unsigned int> &P,unsigned int seed);
    void clearPlateau();
};

class MyPlainTextEdit : public QPlainTextEdit  // ajout du signal Closing quand on ferme
{
    Q_OBJECT
public:
    explicit MyPlainTextEdit(QWidget *parent = nullptr) : QPlainTextEdit(parent) {}
signals:
    void Closing(); // define new signal

protected:
    virtual void closeEvent(QCloseEvent *event) override
    {
        emit Closing(); // send signal before closing.
        QPlainTextEdit::closeEvent(event);
    }
};

#endif // QTBOOGLE_H
