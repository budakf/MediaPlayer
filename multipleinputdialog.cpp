#include "multipleinputdialog.h"

MultipleInputDialog::MultipleInputDialog(QStringList pLabels, QWidget *parent) : QDialog(parent){
    setLayout(pLabels);
}


void MultipleInputDialog::setLayout(QStringList pLabels){
    mMainLayout = new QFormLayout(this);

    foreach(QString label, pLabels) {
        QLineEdit* lineEdit = new QLineEdit(this);
        mLineEditFields.push_back(lineEdit);
        mMainLayout->addRow(label, lineEdit);
    }
    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                     QDialogButtonBox::Cancel, Qt::Horizontal, this);
    mMainLayout->addRow(mButtonBox);

    connect(mButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(mButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


QStringList MultipleInputDialog::getTexts(QStringList pLabels, QWidget *parent, QLineEdit::EchoMode echo,
                                          Qt::WindowFlags flags,Qt::InputMethodHints inputMethodHints){

    MultipleInputDialog* multipleDialog = new MultipleInputDialog(pLabels,parent);
    QStringList lines;

    if (multipleDialog->exec() == QDialog::Accepted){
        foreach(QLineEdit* lineEdit, multipleDialog->mLineEditFields){
            lines << lineEdit->text();
        }
    }
    qDebug()<<lines;
    return  lines;
}
