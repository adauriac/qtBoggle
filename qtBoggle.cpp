#include "qtBoggle.h"
/*
#include <QApplication>
#include <QPushButton>
#include <QStackedWidget>
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
*/
#define MSG(msg){QMessageBox msgBox;msgBox.setText(msg);msgBox.exec();}

qtBoogle::qtBoogle(QWidget *parent)
    : QMainWindow(parent)
{
    m_prefix = ":/resources/";
    QGridLayout *mainLayout = new QGridLayout(parent);
    for(unsigned int i=0;i<4*4;i++)
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
        //unsigned int k = rand.bounded(26);
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
    QPushButton *play = new QPushButton("Jouer");
    QPushButton *solve = new QPushButton("Solve");
    m_solvePushBtn = solve;
    m_playPushBtn = play;
    play->setMaximumSize(60,30);
    play->setMinimumSize(70,35);
    solve->setMaximumSize(60,30);
    solve->setMinimumSize(70,35);
    QLabel *leg= new QLabel("vous pouvez remplir la grille ou ");
    m_legLabel = leg;
    leg->setFrameStyle(QFrame::Box);
    mainLayout->addWidget(leg,0,0,1,4);
    mainLayout->addWidget(play,0,4);
    mainLayout->addWidget(solve,1,4);
    solve->setEnabled(false);
    solve->setVisible(false);
    connect(play,&QPushButton::clicked,this,&qtBoogle::play);
    connect(solve,&QPushButton::clicked,this,&qtBoogle::solve);

    QPushButton* pbDuree = new QPushButton(QString::number(m_totalTimeInSec)+" sec.");
    m_DureePushBtn = pbDuree;
    connect(pbDuree,&QPushButton::clicked,this,&qtBoogle::changeDuree);
    mainLayout->addWidget(pbDuree,4,4);
    mainLayout->setColumnMinimumWidth(4, 120);  // Pour ne pas tout changer la mise en page qd les boutons non visibles
    QCheckBox* chbRotate = new QCheckBox("Rotation active");
    m_RotateCheckBox = chbRotate;
    mainLayout->addWidget(chbRotate,3,4);

    QProgressBar* progBar = new QProgressBar();
    m_progBar = progBar;
    progBar->setVisible(false);
    mainLayout->addWidget(progBar,5,0,1,4);

    QWidget *w = new QWidget();
    // Set the outer layout as a main layout of the widget
    w->setLayout(mainLayout);
    w->setWindowTitle("BOGGLE");
    w->setMinimumSize(QSize(m_l,m_L));
    w->setMaximumSize(QSize(m_l,m_L));
    w->show();

}    // FIN qtBoogle::qtBoogle(QWidget *parent)
// ****************************************************************************

qtBoogle::~qtBoogle()
{
}    // FIN qtBoogle::qtBoogle(QWidget *parent)
// ****************************************************************************

void qtBoogle::solve()
{
    m_solvePushBtn->setVisible(false);
    m_playPushBtn->setVisible(false);
    m_legLabel->setVisible(false);
    m_DureePushBtn->setVisible(false);
    m_RotateCheckBox->setVisible(false);
    QString dico = m_prefix+"bigdict";
    QFile F(dico);
    if (!F.exists())
    {
        MSG(dico + " does not exists");
        qApp->exit(0);
        exit(0);
    }
    if (!F.open(QFile::ReadOnly))
    {
        MSG(dico + " pas ouvrable");
        qApp->exit(0);
        exit(0);
    }
    if (m_dicoContent.size()==0)
    {
      for(;;)
        {
            char line[32];
            F.readLine(line,32);
            // on enleve les repetitions
            uint nn = m_dicoContent.size();
            if (nn==0)
            {
                m_dicoContent.push_back(strdup(line));
                continue;
            }
            char *lll = m_dicoContent[nn-1];
            if (strcmp(line,lll)!=0)
                m_dicoContent.push_back(strdup(line));
            if (F.atEnd())
                break;
        }
    }
    boggleSolver* bs = new boggleSolver(m_dicoContent);
    if (bs->status != "OK")
    {
        MSG(dico + " " + QString::fromStdString(bs->status));
        qApp->exit(1);
        exit(1);
    }
    m_bs = bs;
    QString data = "";
    for(unsigned int i=0;i<4*4;i++)
        data += m_lesQChars[i];
    std::string dataStr = data.toStdString();
    std::string ans = bs->solve(dataStr);   // ICI ON RESOUD
    QString qAns(ans.c_str());
    MyPlainTextEdit* showAns = new MyPlainTextEdit();
    showAns->setReadOnly(true);
    showAns->setPlainText(qAns);
    connect(showAns,&MyPlainTextEdit::Closing,this,&qtBoogle::clearPlateau);
    showAns->show();
    m_progBar->reset();
}    // FIN qtBoogle::solve()
// ****************************************************************************

void qtBoogle::clearPlateau()
{
    for(unsigned int i=0;i<4*4;i++)
    {
        m_lesQChars[i]='_';
        m_lesQLineEdits[i]->setText("");
        m_lesWidg[i]->setCurrentIndex(0);
    }
 //   m_solvePushBtn->setVisible(true);
    m_solvePushBtn->setText("solve");
    m_playPushBtn->setVisible(true);
    m_legLabel->setVisible(true);
    m_DureePushBtn->setVisible(true);
    m_RotateCheckBox->setVisible(true);
}    // void qtBoogle::clearPlateau()
// ****************************************************************************

void qtBoogle::play()
{
    // le temps est totale est m_totalTimeInSec
    bool tourne = m_RotateCheckBox->isChecked();
    m_RotateCheckBox->setVisible(false);
    m_solvePushBtn->setVisible(false);
    m_legLabel->setVisible(false);
    m_playPushBtn->setVisible(false);
    m_DureePushBtn->setVisible(false);
    if (m_seed==0)  // si seed==0 on prend le nb de milliseconds depuis the epoch
    {
        QElapsedTimer el;
        qint64 q = el.elapsed();
        m_seed = (unsigned int)q;
    }
    QRandomGenerator R(m_seed);
    m_R = &R;
    std::vector<unsigned int> P={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    for (unsigned int i= P.size()-1;i!=1;i--)
    {
       unsigned int j = R.bounded(i); // aleatoire 0<=j<i
       unsigned int aux = P[i];
       P[i] = P[j];
       P[j]= aux;
    }
    for(unsigned int i=0;i<4*4;i++)
    {
        QString leDe = m_Lettres[P[i]];   // choix du de de la case i
        unsigned int face = R.bounded(6);  // parmi les 6 face (pas exactement juste car 6 n'est pas 2**k)
        QChar lettre =  leDe[face]; // lettre elle meme
        m_lesQChars[i] = lettre;
    }
    // affichage des caracteres visibles avec orientation
    std::vector<unsigned int> orients(4*4);
    for(unsigned int i=0;i<4*4;i++)
        orients[i] = R.bounded(4);
    QProgressBar qpb;
    std::vector<unsigned int> permRot,permRot2; // Pour vaire le qart de tour
    for(unsigned int i=0;i<4*4;i++)
    {
        permRot.push_back(i);
        permRot2.push_back(i);
    }
#define ROTE(k) (4*((k)%4) + (3-(k/4)))
    m_progBar->setVisible(true);
    m_progBar->setRange(0,m_totalTimeInSec);
    m_progBar->reset();
    //for (unsigned int coup = 0;coup < 4; coup++)
    QElapsedTimer timer;
    timer.start();
    for (unsigned int tic = 0;tic < m_totalTimeInSec; tic++)
    {
        m_progBar->setValue(tic);
        unsigned int coup = (4*tic)/m_totalTimeInSec;  // 0, 1 2 ou 3 suivant le quart de coups ou l'on est
        for(unsigned int i=0;i<4*4;i++)
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
            for(unsigned int i=0;i<4*4;i++)
                permRot2[i] = ROTE(permRot[i]);
            for(unsigned int i=0;i<4*4;i++)
                permRot[i] = permRot2[i];
        } // fin repermutation
    } // fin for(coup=0;coup<4
    m_progBar->setValue(m_totalTimeInSec);
    m_seed = 0; // pour nouvel appel different de celui-ci
    MSG("Fini ! \nClique OK et laisse moi quelques secondes de réflexion");
    m_solvePushBtn->setVisible(true);
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
        for(unsigned int k =0;k<4*4;k++)
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
        m_altViewPushBtn->setText("Plus beau");
        for(unsigned int k =0;k<4*4;k++)
        {

            m_lesWidg[k]->setCurrentIndex(0);
        }
    }
}    // FIN void qtBoogle::altView()
// ****************************************************************************************

void qtBoogle::entreeChangee(unsigned int i)
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
   for(unsigned int i=0;i<4*4;i++) // peut-on ebabler solve
       if (!(('A'<=m_lesQChars[i]) && (m_lesQChars[i]<='Z')))
           return; // il manque une lettre
   // ici on enable
    m_solvePushBtn->setEnabled(true);
    m_solvePushBtn->setVisible(true);
    altView();
}    // FIN void qtBoogle::entreeChangee(unsigned int i)
// ****************************************************************************************

QString qtBoogle::imageFileName(unsigned int i)
{
    if (i>=26)
        return "";
    return m_prefix+QChar(65+i)+".png";
}    // FIN QString qtBoogle::imageFileName(unsigned int i)
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

unsigned int qtBoogle::numero(QString name)
{
    if (!name.contains(m_prefix))
        return -1;
    if (!name.contains(".png"))
        return -1;
    QString aux  = name.replace(m_prefix,"");
    QChar c = aux[0];
    unsigned int ans = c.digitValue();
    return ans;
}    //FIN unsigned int qtBoogle::numero(QString name)
// ****************************************************************************************

void qtBoogle::changeDuree()
{
    bool ok;
    int i = QInputDialog::getInt(this,"","duree en secondes:", 180, 1, 60*100, 1, &ok);
    m_totalTimeInSec = i;
    m_DureePushBtn->setText(QString::number(m_totalTimeInSec)+" sec.");
}    // void qtBoogle::changeDuree()
// ****************************************************************************************

void qtBoogle::QRandomPerm(std::vector<unsigned int> &P,unsigned int seed)
{
    if (seed==0)
    {
        QElapsedTimer el;
        qint64 q = el.elapsed();
        seed = (unsigned int)q;
    }
    QRandomGenerator R(seed);
    for (unsigned int i= P.size()-1;i!=1;i--)
    {
       unsigned int j = R.bounded(i); // aleatoire 0<=j<i
       unsigned int aux = P[i];
       P[i] = P[j];
       P[j]= aux;
    }
}    // FIN void QRandomPerm(std::vector<unsigned int> &P)
// ****************************************************************************************



