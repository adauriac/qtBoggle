#include "qtBoggle.h"
#include <QApplication>
#include <QPushButton>
#include <QGridLayout>
#include <QStackedWidget>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QDebug>
#include <QTextEdit>
#include <QFile>
#include <QTransform>
#include <QTime>
#include <random>
#include <QElapsedTimer>
#include <QTimer>
#include <QThread>
#include <QProgressBar>
#include <QTextBrowser>
#include <QInputDialog>
#include <QRadioButton>
#include <QScrollBar>
#include <QPlainTextEdit>

#define MSG(msg){QMessageBox msgBox;msgBox.setText(msg);msgBox.exec();}

qtBoogle::qtBoogle(QWidget *parent)
    : QMainWindow(parent)
{
    m_prefix = ":/resources/";
    QRandomGenerator rand;
    QGridLayout *mainLayout = new QGridLayout(parent);
    for(uint i=0;i<4*4;i++)
    {
        QStackedWidget *aux = new QStackedWidget;
        //MyQStackedWidget* aux = new MyQStackedWidget;
        QLabel* lab = new QLabel;
        QLineEdit *linE = new QLineEdit;
        // creation du stacked widget
        aux->addWidget(linE);  // donc le mineEdit est pour index=0
        aux->addWidget(lab);
        aux->setCurrentIndex(0);
        aux->setMinimumSize(51,51);
        aux->setMaximumSize(51,51);
        // configuration du label (en fait l'image)
        //uint k = rand.bounded(26);
        lab->setStyleSheet("background-image: url('/"+imageFileName('_')+"')");
        lab->resize(51,51);
        // configuration du lineedit
        linE->setText("");
        linE->setMaxLength(1);
        linE->resize(51,51);
        //connect(linE,&QLineEdit::textChanged,this,[this, i]{entreeChangee(i);});
        connect(linE,&QLineEdit::textEdited,this,[this, i]{entreeChangee(i);});
        // ajout du stacked widget
        mainLayout->addWidget(aux,i/4+1,i%4);
        m_lesWidg.push_back(aux);
        m_lesQChars.push_back('_');
        m_lesQLabels.push_back(lab);
        m_lesQLineEdits.push_back(linE);
    }
    // ajout des boutons
    //QPushButton *altView = new QPushButton("Plus beau");
    //m_altView = altView;
    QPushButton *play = new QPushButton("Jouer");
    QPushButton *solve = new QPushButton("Solve");
    m_solvePushBtn = solve;
    m_playBushBtn = play;
    play->setMaximumSize(60,30);
    play->setMinimumSize(70,35);
    solve->setMaximumSize(60,30);
    solve->setMinimumSize(70,35);
    //mainLayout->addWidget(altView,0,4);
    QLabel *leg= new QLabel("vous pouvez remplir la grille ou ");
    m_leg = leg;
    leg->setFrameStyle(QFrame::Box);
    mainLayout->addWidget(leg,0,0,1,4);
    mainLayout->addWidget(play,0,4);
    mainLayout->addWidget(solve,1,4);
    solve->setEnabled(false);
    solve->setVisible(false);
    //connect(altView,&QPushButton::clicked,this,&qtBoogle::altView);
    connect(play,&QPushButton::clicked,this,&qtBoogle::play);
    connect(solve,&QPushButton::clicked,this,&qtBoogle::solve);

    QPushButton* pbDuree = new QPushButton(QString::number(m_totalTimeInSec)+" sec.");
    m_pbDuree = pbDuree;
    connect(pbDuree,&QPushButton::clicked,this,&qtBoogle::changeDuree);
    mainLayout->addWidget(pbDuree,4,4);
    QCheckBox* chbRotate = new QCheckBox("Rotation active");
    m_chbRotate = chbRotate;
    //connect(chbRotate,&QCheckBox::clicked,this,&qtBoogle::tourne);
    mainLayout->addWidget(chbRotate,3,4);

    QProgressBar* progBar = new QProgressBar();
    m_progBar = progBar;
    progBar->setVisible(false);
    mainLayout->addWidget(progBar,5,0,1,4);

    QLabel* workProof = new QLabel("Je bosse");
    m_workProof = workProof;
    mainLayout->addWidget(workProof,5,4);
    workProof->setStyleSheet("background-color: red");
    workProof->setVisible(false);

    QWidget *w = new QWidget();
    // Set the outer layout as a main layout of the widget
    w->setLayout(mainLayout);
    w->setWindowTitle("BOGGLE");
    w->show();
}    // FIN qtBoogle::qtBoogle(QWidget *parent)
// ****************************************************************************

qtBoogle::~qtBoogle()
{
}    // FIN qtBoogle::qtBoogle(QWidget *parent)
// ****************************************************************************

void qtBoogle::solve()
{
    m_workProof->setVisible(true);
    m_solvePushBtn->setVisible(false);
    m_playBushBtn->setVisible(false);
    m_leg->setVisible(false);
    m_pbDuree->setVisible(false);
    m_chbRotate->setVisible(false);
    QString dico = m_prefix+"bigdict";
    dico = "/home/dauriac/games/qtBoggle/qtBoggle/resources/bigdict";
    std::string dicoStr = dico.toStdString();
    boggleSolver* bs = new boggleSolver(dicoStr);
    if (bs->status != "OK")
    {
        MSG(dico + " " + QString::fromStdString(bs->status));
        qApp->exit(1);
        exit(1);
    }
    m_bs = bs;
    QString data = "";
    for(uint i=0;i<4*4;i++)
        data += m_lesQChars[i];
    std::string dataStr = data.toStdString();
    std::string ans = bs->solve(dataStr);   // ICI ON RESOUD
    QString qAns(ans.c_str());
    MyPlainTextEdit* showAns = new MyPlainTextEdit();
    showAns->setPlainText(qAns);
    connect(showAns,&MyPlainTextEdit::Closing,this,&qtBoogle::clearPlateau);
    showAns->show();
    m_progBar->reset();
    m_workProof->setVisible(false);
}    // FIN qtBoogle::solve()
// ****************************************************************************

void qtBoogle::clearPlateau()
{
    for(uint i=0;i<4*4;i++)
    {
        m_lesQChars[i]='_';
        m_lesQLineEdits[i]->setText("");
        m_lesWidg[i]->setCurrentIndex(0);
    }
    m_solvePushBtn->setVisible(true);
    m_playBushBtn->setVisible(true);
    m_leg->setVisible(true);
    m_pbDuree->setVisible(true);
    m_chbRotate->setVisible(true);
}    // void qtBoogle::clearPlateau()
// ****************************************************************************

void qtBoogle::play()
{
    // le temps est totale est m_totalTimeInSec
    bool tourne = m_chbRotate->isChecked();
    m_solvePushBtn->setVisible(false);
    if (m_seed==0)  // si seed==0 on prend le nb de milliseconds depuis the epoch
    {
        QElapsedTimer el;
        qint64 q = el.elapsed();
        m_seed = (uint)q;
    }
    QRandomGenerator R(m_seed);
    m_R = &R;
    std::vector<uint> P={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    for (uint i= P.size()-1;i!=1;i--)
    {
       uint j = R.bounded(i); // aleatoire 0<=j<i
       uint aux = P[i];
       P[i] = P[j];
       P[j]= aux;
    }
    for(uint i=0;i<4*4;i++)
    {
        QString leDe = m_Lettres[P[i]];   // choix du de de la case i
        uint face = R.bounded(6);  // parmi les 6 face (pas exactement juste car 6 n'est pas 2**k)
        QChar lettre =  leDe[face]; // lettre elle meme
        m_lesQChars[i] = lettre;
    }
    // affichage des caracteres visibles avec orientation
    std::vector<uint> orients(4*4);
    for(uint i=0;i<4*4;i++)
        orients[i] = R.bounded(4);
    QProgressBar qpb;
    std::vector<uint> permRot,permRot2; // Pour vaire le qart de tour
    for(uint i=0;i<4*4;i++)
    {
        permRot.push_back(i);
        permRot2.push_back(i);
    }
#define ROTE(k) (4*((k)%4) + (3-(k/4)))
    m_progBar->setVisible(true);
    m_progBar->setRange(0,m_totalTimeInSec);
    m_progBar->reset();
    //for (uint coup = 0;coup < 4; coup++)
    QElapsedTimer timer;
    timer.start();
    for (uint tic = 0;tic < m_totalTimeInSec; tic++)
    {
        m_progBar->setValue(tic);
        uint coup = (4*tic)/m_totalTimeInSec;  // 0, 1 2 ou 3 suivant le quart de coups ou l'on est
        for(uint i=0;i<4*4;i++)
        {
            QChar Orient = tourne ?  m_Orient[(orients[i]+coup)%4] : m_Orient[orients[i]];
            QString fileName = tourne ? m_prefix + m_lesQChars[permRot[i]] + Orient + ".png": m_prefix + m_lesQChars[i] + Orient + ".png";
            if (1)
            {
                QFile F(fileName);
                if (!F.exists())
                {
                    MSG(fileName + " n'existe pas");
                    qApp->exit();
                }
            }
                m_lesQLabels[i]->setStyleSheet("background-image: url('" + fileName + "')");
                m_lesWidg[i]->setCurrentIndex(1);
        }

        while (1)
            if (timer.elapsed() >= 1000*tic ) // car unite = millisecondes
                break;
            else
                QCoreApplication::processEvents();
        if ((4*(tic+1))/m_totalTimeInSec != coup) // repermutation
        {
            // permutation corespondant a la rotation (on pourrait faire mieux)
            for(uint i=0;i<4*4;i++)
                permRot2[i] = ROTE(permRot[i]);
            for(uint i=0;i<4*4;i++)
                permRot[i] = permRot2[i];
        } // fin repermutation
    } // fin for(coup=0;coup<4
    m_progBar->setValue(m_totalTimeInSec);
    m_seed = 0; // pour nouvel appel different de celui-ci
    m_solvePushBtn->setVisible(true);
    m_solvePushBtn->setEnabled(true);
    MSG("fini");
    m_progBar->setVisible(false);
    solve();
}    // FIN void qtBoogle::play()
// ********************************************************************************************

void qtBoogle::altView()
    {
    if  (m_lesWidg[0]->currentIndex()==0) // tous les setackedwidhets montrent la mm chose
    {
        // on est lineEdit et on passera label (photo)
        //m_altView->setText("Editable");
        for(uint k =0;k<4*4;k++)
        {
            QChar Orient = m_Orient[0]; // orientation normal
            QString fileName = m_prefix + m_lesQChars[k] + Orient + ".png";
            if (1)
            {
                QFile F(fileName);
                if (!F.exists())
                {
                    MSG(fileName + " n'existe pas");
                    qApp->exit();
                }
            }
            m_lesQLabels[k]->setStyleSheet("background-image: url('" + fileName + "')");
            m_lesWidg[k]->setCurrentIndex(1);
        }
    }
    else
    {
        m_altView->setText("Plus beau");
        for(uint k =0;k<4*4;k++)
        {

            m_lesWidg[k]->setCurrentIndex(0);
        }
    }
}    // FIN void qtBoogle::altView()
// ****************************************************************************************

void qtBoogle::entreeChangee(uint i)
{
   QChar newChar = m_lesQLineEdits[i]->text()[0];
   if (newChar.unicode()==0)
       return;
   if (newChar==QChar(8))
       MSG("vu bs");
   QChar oldChar = m_lesQChars[i];
   newChar = newChar.toUpper();
   //MSG("old " + QString(oldChar)+" new "+ QString(newChar)+" "+QString::number(newChar.unicode()));
   if (!(('A'<=newChar)&&(newChar<='Z')))
   {
       // pas OK on remet l'ancien charactere
       QString x;
       if (oldChar==QChar('_'))
           x = "";
       else
           x = oldChar;
       m_lesQLineEdits[i]->setText(x);
       return;
   }
   // ici c'est OK on accepte
   m_lesQChars[i] = newChar;
   m_lesQLineEdits[i]->setText(newChar);
   if (i!=4*4-1)
       m_lesQLineEdits[i+1]->setFocus();
   for(uint i=0;i<4*4;i++) // peut-on ebabler solve
       if (!(('A'<=m_lesQChars[i]) && (m_lesQChars[i]<='Z')))
           return; // il manque une lettre
   // ici on enable
    m_solvePushBtn->setEnabled(true);
    m_solvePushBtn->setVisible(true);
    altView();
}    // FIN void qtBoogle::entreeChangee(uint i)
// ****************************************************************************************

QString qtBoogle::imageFileName(uint i)
{
    if (i>=26)
        return "";
    return m_prefix+QChar(65+i)+".png";
}    // FIN QString qtBoogle::imageFileName(uint i)
// ****************************************************************************************

void qtBoogle::tourne()
{

}    // FIN qtBoogle::tourne()
// ****************************************************************************************

QString qtBoogle::imageFileName(QChar c)
{
    return m_prefix+c+".png";
}    // FIN QString qtBoogle::imageFileName(QChar c)
// ****************************************************************************************

uint qtBoogle::numero(QString name)
{
    if (!name.contains(m_prefix))
        return -1;
    if (!name.contains(".png"))
        return -1;
    QString aux  = name.replace(m_prefix,"");
    QChar c = aux[0];
    uint ans = c.digitValue();
    return ans;
}    //FIN uint qtBoogle::numero(QString name)
// ****************************************************************************************

void qtBoogle::changeDuree()
{
    bool ok;
    int i = QInputDialog::getInt(this,"","duree en secondes:", 180, 1, 60*100, 1, &ok);
    m_totalTimeInSec = i;
    m_pbDuree->setText(QString::number(m_totalTimeInSec)+" sec.");
}    // void qtBoogle::changeDuree()
// ****************************************************************************************

void qtBoogle::QRandomPerm(std::vector<uint> &P,uint seed)
{
    if (seed==0)
    {
        QElapsedTimer el;
        qint64 q = el.elapsed();
        seed = (uint)q;
    }
    QRandomGenerator R(seed);
    for (uint i= P.size()-1;i!=1;i--)
    {
       uint j = R.bounded(i); // aleatoire 0<=j<i
       uint aux = P[i];
       P[i] = P[j];
       P[j]= aux;
    }
}    // FIN void QRandomPerm(std::vector<uint> &P)
// ****************************************************************************************

MyQStackedWidget::MyQStackedWidget(QWidget *parent) :
    QStackedWidget(parent)
{
    setAttribute(Qt::WA_Hover);
    //fprintf(stderr,"Createur Stacked\n");
}    // FIN MyQStackedWidget::MyQStackedWidget(QWidget *parent) :
// ****************************************************************************************

bool MyQStackedWidget::event(QEvent* e)
{
    switch(e->type())
    {
    case QEvent::HoverEnter:
        hoverEnter(static_cast<QHoverEvent*>(e));
        return true;
    case QEvent::HoverLeave:
        hoverLeave(static_cast<QHoverEvent*>(e));
        return true;
    default:
        break;
    }
    return QWidget::event(e);
}    // FIN
// ****************************************************************************

void MyQStackedWidget::enterEvent(QEvent* e)
{
    //fprintf(stderr,"enterEvent MyQStackedWidget\n");
}    // FIN void MyQStackedWidget::enterEvent(QEvent* e)
// ****************************************************************************

void MyQStackedWidget::leaveEvent(QEvent* e)
{
    //fprintf(stderr,"leaveEvent MyQStackedWidget\n");
}    // FIN void MyQStackedWidget::leaveEvent(QEvent* e)
// ****************************************************************************

void MyQStackedWidget::hoverEnter(QHoverEvent* event)
{
    //fprintf(stderr,"hoverEnter MyQStackedWidget\n");
    if (m_active)
    {
        m_active = false;
        this->setCurrentIndex(1); // active le textEdit
        //int i = (this->currentIndex()+1)%2;
        //this->setCurrentIndex(i);
    }
    m_active = true;
}    // FIN void MyQStackedWidget::hoverEnter(QHoverEvent* event)
// ****************************************************************************

void MyQStackedWidget::hoverLeave(QHoverEvent* event)
{
    if (m_active)
    {
        m_active = false;
        this->setCurrentIndex(0); // active the label
        //int i = (this->currentIndex()+1)%2;
        //this->setCurrentIndex(i);
    }
    m_active = true;
}    // FIN void MyQStackedWidget::hoverEnter(QHoverEvent* event)
// ****************************************************************************

void Worker::process()
{ // Process. Start processing data.
    // fait blinker le radio button
    while (true)
    {
        m_qrb->setVisible(true);
        //QTest::qWait(250);
        m_qrb->setVisible(false);
    }
//    emit finished();
}    // FIN void void Worker::process()
// ****************************************************************************


