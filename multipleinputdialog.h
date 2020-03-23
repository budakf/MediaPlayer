#ifndef MULTIPLEINPUTDIALOG_H
#define MULTIPLEINPUTDIALOG_H

#include <QTimer>
#include <QDebug>
#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>

#include <QString>
#include <QStringList>

class MultipleInputDialog : public QDialog{
    Q_OBJECT

public:
    MultipleInputDialog(QStringList pFieldNames, QWidget* parent=nullptr);
    static QStringList getTexts(QStringList, QWidget* parent=nullptr, QLineEdit::EchoMode echo = QLineEdit::Normal,
                                Qt::WindowFlags flags = Qt::WindowFlags(),
                                Qt::InputMethodHints inputMethodHints = Qt::ImhNone);
private:
    QList<QLineEdit*> mLineEditFields;
    QFormLayout* mMainLayout;
    QDialogButtonBox* mButtonBox;

    void setLayout(QStringList pFieldNames);

public slots:
//    void accept() override;
//    void reject() override;

signals:


};

#endif // MULTIPLEINPUTDIALOG_H
