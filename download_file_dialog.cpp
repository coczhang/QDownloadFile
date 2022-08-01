#include "download_file_dialog.h"

#include <QMessageBox>
#include <windows.h>
#include <QStandardPaths>
#include <QFileInfo>
#include <QProcess>
#include <QDir>
#include <QTextStream>
#include <QDebug>

DownloadFileDialog::DownloadFileDialog(QWidget *parent)
    : QWidget(parent)
{
    init_dialog_layout();
    init_dialog_connect();
}

DownloadFileDialog::~DownloadFileDialog()
{

}

void DownloadFileDialog::init_dialog_layout()
{
    this->setFixedSize(800, 600);

    m_line_download_url = new QLineEdit(this);
    m_line_download_url->setGeometry(10, 10, 650, 40);
    m_line_download_url->setStyleSheet("QLineEdit{font-family: Microsoft YaHei; font-size: 20px; color: black}");

    m_btn_download = new QPushButton("下载", this);
    m_btn_download->setGeometry(m_line_download_url->pos().x() + m_line_download_url->width() + 10,
                              m_line_download_url->pos().y(), 100, 40);

    m_text_log = new QTextEdit(this);
    m_text_log->setGeometry(m_line_download_url->pos().x(),
                            m_line_download_url->pos().y() + m_line_download_url->height() + 20, 780, 520);
    m_text_log->setStyleSheet("QTextEdit{font-family: Microsoft YaHei; font-size: 16px; color: black}");

}

void DownloadFileDialog::init_dialog_connect()
{
    connect(m_btn_download, &QPushButton::clicked, this, &DownloadFileDialog::download_file);
}

void DownloadFileDialog::download_file()
{
    m_text_log->clear();
    QString download_url = m_line_download_url->text();
    if (download_url.isEmpty()) {
        QMessageBox msg_box(QMessageBox::NoIcon, "警告", "请输入下载链接", QMessageBox::Ok, NULL);
        msg_box.exec();

        return;
    }

    QString file_name = "index.html";
    QString desktop_dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString file_path = desktop_dir + "/" + file_name;
    QFileInfo file_info(file_path);
    int count = 0;
    QDir dir(desktop_dir);
    QFileInfoList file_list = dir.entryInfoList();
    foreach(QFileInfo info, file_list) {
        if (info.fileName().indexOf(file_info.fileName().section('.', 0, 0), 0) != -1 &&
                info.fileName().indexOf(file_info.suffix(), file_info.fileName().length() - file_info.suffix().length()) != -1) {
           count++;
        }
    }

    if (count > 0) {
        QString name_info = file_name.left(file_name.length() - file_info.suffix().length()-1);
        file_name = QString("%1(%2)%3%4").arg(name_info).arg(count).arg(".").arg(file_info.suffix());
    }

    QString command  = QString("wget -O \"%1/%2\" --no-check-certificate \"%3\"").arg(desktop_dir).arg(file_name).arg(download_url);
    //QProcess::execute(command);
    int ret = download_command(command.toLatin1().data());
    if (ret) {
        QFile file("download_file.log");
        file.open(QIODevice::ReadOnly);
        QTextStream text_stream(&file);
        QString file_info = text_stream.readAll();
        m_text_log->setText(file_info);
        file.remove();
        file.close();
    }
}

int DownloadFileDialog::download_command(const char *command)
{
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    HANDLE h = CreateFile(L"download_file.log",
        FILE_WRITE_DATA,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &sa,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL ret = FALSE;
    DWORD flags = CREATE_NO_WINDOW;

    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = NULL;
    si.hStdError = h;
    si.hStdOutput = h;

    wchar_t cmd[1024] = {0};
    mbstowcs(cmd, command, strlen(command) + 1);
    ret = CreateProcess(NULL, cmd, NULL, NULL, TRUE, flags, NULL, NULL, &si, &pi);
    if (ret)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(h);

        return 1;
    }
    CloseHandle(h);

    return -1;
}
