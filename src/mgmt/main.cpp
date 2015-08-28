#include "mgmt/adhashmgmt.h"
#include "common/xpm_images.h"
#include <QApplication>
//---------------------------------------------------------------------------
// AdHash
// (c) 2015 Razen M. AlHarbi. Web site: http://cloud.kaust.edu.sa/Pages/adhash.aspx
//
// This work is licensed under the Creative Commons
// Attribution-Noncommercial-Share Alike 3.0 Unported License. To view a copy
// of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300,
// San Francisco, California, 94105, USA.
//---------------------------------------------------------------------------
using namespace std;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSplashScreen *splash_screen = new QSplashScreen;
    splash_screen->setPixmap(QPixmap(splash));
    splash_screen->show();

    AdHashMgmt w;
    w.move(QApplication::desktop()->screen()->rect().center() - w.rect().center());
    QTimer::singleShot(1000, splash_screen, SLOT(close()));
    QTimer::singleShot(1000, &w, SLOT(show()));
    return a.exec();
}

