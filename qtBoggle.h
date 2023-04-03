#ifndef QTBOOGLE_H
#define QTBOOGLE_H

#include <QStackedWidget>
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QRandomGenerator>
#include <QHoverEvent>
#include <QEvent>
#include <QCheckBox>
#include <QDebug>
#include <stdio.h>
#include <QProgressBar>
#include <QRadioButton>
#include <QPlainTextEdit>
#include "libBoggle.h"

class qtBoogle : public QMainWindow
{
    Q_OBJECT

public:
    qtBoogle(QWidget *parent = nullptr);
    ~qtBoogle();
private:
    void altView();
    void entreeChangee(uint i);
    void play();
    void solve();
    void changeDuree();
    void tourne();
    QString imageFileName(uint i);
    QString imageFileName(QChar c);
    uint numero(QString);
    void QRandomPerm(std::vector<uint> &P,uint seed);
    uint m_seed=0;
    QPushButton* m_solvePushBtn;
    QPushButton* m_altView;
    QPushButton* m_playBushBtn;
    boggleSolver* m_bs;
    QLabel* m_leg;
    QLabel* m_workProof;
    QPushButton* m_pbDuree;
    QCheckBox* m_chbRotate;
    uint m_totalTimeInSec=60*3;
    std::vector<QStackedWidget*> m_lesWidg;
    std::vector<QChar> m_lesQChars;  // donc entre 'A' et 'Z' ou '_'
    std::vector<QLineEdit*> m_lesQLineEdits;
    std::vector<QLabel*> m_lesQLabels;
    QString m_prefix;
    std::vector<QChar> m_Orient = {QChar('0'),QChar('1'),QChar('2'),QChar('3')};
    QProgressBar* m_progBar;
    QRandomGenerator* m_R;
    void clearPlateau();
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
};

class MyQStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit MyQStackedWidget(QWidget *parent = 0);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    bool event(QEvent * e);

    void hoverEnter(QHoverEvent * event);
    void hoverLeave(QHoverEvent * event);

signals:

public slots:

private:
    bool m_active;
};

class Worker : public QObject {
    Q_OBJECT
public:
    Worker(QLabel* qrb){m_qrb=qrb;};
    ~Worker(){};
public slots:
    void process();
signals:
//    void finished();
 //   void error(QString err);
private:
    // add your variables here
    QLabel* m_qrb;
};

class MyPlainTextEdit : public QPlainTextEdit
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
