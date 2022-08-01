#ifndef DOWNLOAD_FILE_DIALOG_H
#define DOWNLOAD_FILE_DIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

class DownloadFileDialog : public QWidget
{
    Q_OBJECT

public:
    DownloadFileDialog(QWidget *parent = 0);
    ~DownloadFileDialog();

private slots:
    void download_file();

private:
    void init_dialog_layout();
    void init_dialog_connect();
    int download_command(const char *command);

private:
    QLineEdit *m_line_download_url;
    QPushButton *m_btn_download;
    QTextEdit *m_text_log;
};
#endif
