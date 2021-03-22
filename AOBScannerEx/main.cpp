#include "stdafx.h"

#include "CMainWindow.hpp"

int main(int argc, char *argv[])
{
#ifdef _WIN32
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif

    Q_INIT_RESOURCE(AOBScannerEx);

    QCoreApplication::setOrganizationName("Japanese1337hackers");
    QCoreApplication::setOrganizationDomain("jpn.hackers.com");
    QCoreApplication::setApplicationName("AOBScannerEx");

    QApplication a(argc, argv);
    CMainWindow m;
    m.show();
    return a.exec();
}
