#include "mainwindow.h"
#include <QMessageBox>
#include <QMenuBar>
#include "logindlg.h"
#include <QIcon>
#include <QWidget>
#include <QMdiSubWindow>
#include <QString>
#include <QStandardItemModel>
#include <QTableView>
#include "scriptdlg.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QString("CDMA"));
    mdiArea = new QMdiArea(this);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    /*mdiArea->setAutoFillBackground(true);
        QPalette palette;
        palette.setBrush(QPalette::Background,QBrush(QPixmap("2.jpg")));
        mdiArea->setPalette(palette);*/  //mei you zuo yong

    mdiArea->setBackground(Qt::NoBrush);
    mdiArea->setStyleSheet("background-image: url(:/png/1.jpg);");
    // mdiArea->setStyleSheet("border-image:url(:/png/1.jpg);");

    setCentralWidget(mdiArea);

    setWindowIcon(QIcon(":/png/main.png"));
    CreatActions();
    CreatMenus();



}

MainWindow::~MainWindow()
{


}


void MainWindow::CreatMenus()
{
    adminMenu = menuBar()->addMenu("admin");
    adminMenu->addAction(loginAction);
    adminMenu->addAction(logoutAction);
    adminMenu->addSeparator();
    adminMenu->addAction(exitAction);


    dataMenu = menuBar()->addMenu("data");
    dataMenu->addAction(scriptAction);

    windowMenu = menuBar()->addMenu("window");
    windowMenu->addAction(cascadeAction);
    windowMenu->addAction(tileAction);


    helpMenu = menuBar()->addMenu("help");
    helpMenu->addAction(aboutAction);
}

void MainWindow::CreatActions()
{
    loginAction = new QAction(tr("login"),this);
    loginAction->setShortcut(tr("ctrl+u"));
    connect(loginAction, SIGNAL(triggered()), this, SLOT(on_login()));

    logoutAction = new QAction(tr("logout"), this);
    logoutAction->setShortcut(tr("ctrl+w"));
    connect(logoutAction,SIGNAL(triggered()),this,SLOT(on_logout()));

    exitAction = new QAction(tr("exit"), this);
    exitAction->setShortcut(tr("ctrl+q"));
    connect(exitAction,SIGNAL(triggered()),this,SLOT(on_exit()));

    scriptAction = new QAction("script",this);
    scriptAction->setEnabled(false);
    connect(scriptAction,SIGNAL(triggered()),this,SLOT(on_script()));

    cascadeAction = new QAction("cascade",this);
    connect(cascadeAction,SIGNAL(triggered()),this,SLOT(cascadeSubWindows()));

    tileAction = new QAction("tile",this);
    connect(tileAction,SIGNAL(triggered()),this,SLOT(tileSubWindows()));

    aboutAction = new QAction("about",this);
    connect(aboutAction,SIGNAL(triggered()),this,SLOT(on_about()));

}

void MainWindow::showSub()
{
    QWidget *wgt = new QWidget;
    wgt->setAttribute(Qt::WA_DeleteOnClose);
    wgt->setWindowTitle("subwgt");
    wgt->setStyleSheet("border-image:url(:/png/3.jpg);");
    mdiArea->addSubWindow(wgt);
    wgt->show();

    mdiArea->activeSubWindow()->resize( (width()-100) ,(height()-100)); //dang qian da xiao xiao 100



}

void MainWindow::cascadeSubWindows()
{
    //QMessageBox::information(this,"","cascade");
    mdiArea->cascadeSubWindows();
}

void MainWindow::tileSubWindows()
{
    //QMessageBox::information(this,"","tile");
    mdiArea->tileSubWindows();
}

void MainWindow::on_about()
{
    QMessageBox::about(this,"about","jlw 2017-09-24 22:12");
}

void MainWindow::on_login()
{
    //QMessageBox::information(this,"debug","Login OK!");
    loginDlg dlg(this);
    dlg.exec();
    if(dlg.isLogin)
    {

        int ret = db.sql_connect(dlg.hostip.toStdString().data(),dlg.userid.toStdString().data(),dlg.passwd.toStdString().data(),dlg.dbname.toStdString().data());
        if(ret == -1)
            QMessageBox::information(this,"login failed",db.getError());
        else
        {
            QMessageBox::information(this,"","login success");
            scriptAction->setEnabled(true);
        }
        //QString str = QString("%1%2%3%4").arg(dlg.userid).arg(dlg.passwd).arg(dlg.dbname).arg(dlg.hostip);
        //QMessageBox::information(this,"",str);
    }
}

void MainWindow::on_logout()
{
    //QMessageBox::information(this,"debug","Logout OK!");
    QMessageBox::StandardButton button = QMessageBox::question(this,"Tip","sure logout?",QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes)
        db.sql_disconnect();
    scriptAction->setEnabled(false);
}

void MainWindow::on_exit()
{
    this->close();
}

void MainWindow::on_script()
{
    //showSub();
    //showview();
    scriptDlg dlg;
    dlg.exec();
    if(dlg.isLogin)
    {
        script_msg(dlg.SQL.toStdString().data() );
    }

}

void MainWindow::script_msg(const char *SQL)
{
    bool ret = 0;
    QStandardItemModel *model = NULL;

    if( (strncmp(SQL,"select",6) == 0) || (strncmp(SQL,"SELECT",6) == 0) )
    {
        ret = db.sql_open(SQL , &model);
        //QStandardItemModel *model = new QStandardItemModel(5,3);
        QTableView *view1 = new QTableView;
        view1->setAttribute(Qt::WA_DeleteOnClose);
        view1->setModel(model);

        view1->setWindowTitle("view");
        view1->setStyleSheet("border-image:url(:/png/3.jpg);");
        mdiArea->addSubWindow(view1);
        view1->show();
        //mdiArea->currentSubWindow()->resize(width()-100,height()-100);
        mdiArea->activeSubWindow()->resize(width()-100,height()-100);

    }else
    {
        ret = db.sql_exec(SQL);
    }

    if(ret == 0 )
        QMessageBox::information(this,"","script sucess");
    else
        QMessageBox::information(this,"script failed",db.getError());




}



void MainWindow::closeEvent(QCloseEvent *event)
{
    //QMessageBox::information(this,"debug","closeEvent OK!");
    QMessageBox::StandardButton button = QMessageBox::question(this,"Tip","sure quit?",QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes)
        event->accept();
    else
        event->ignore();

}


void MainWindow::showview()
{
    //QStandardItemModel *model = new QStandardItemModel(5,3);
    QTableView *view1 = new QTableView;
    view1->setAttribute(Qt::WA_DeleteOnClose);
    //view1->setModel(model);
    //view1->setModel((*p));
    view1->setWindowTitle("view");
    view1->setStyleSheet("border-image:url(3.jpg);");
    mdiArea->addSubWindow(view1);
    view1->show();
    //mdiArea->currentSubWindow()->resize(width()-100,height()-100);
    mdiArea->activeSubWindow()->resize(width()-100,height()-100);
    /*model->setHeaderData(0,Qt::Horizontal,tr("Name"));
    model->setHeaderData(1,Qt::Horizontal,tr("sex"));
    model->setHeaderData(2,Qt::Horizontal,tr("age"));
    model->setHeaderData(0,Qt::Vertical,tr("001"));
    model->setHeaderData(1,Qt::Vertical,tr("002"));
    model->setHeaderData(2,Qt::Vertical,tr("003"));

    model->setData(model->index(0,0),"007");*/

}



















