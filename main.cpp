#include "download_file_dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DownloadFileDialog w;
    w.show();

    return a.exec();
}
