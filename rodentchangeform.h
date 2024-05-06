#ifndef RODENTCHANGEFORM_H
#define RODENTCHANGEFORM_H

#include <QWidget>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include "rodent.h"
namespace Ui {
class RodentChangeForm;
}

class RodentChangeForm : public QDialog
{
    Q_OBJECT

public:
    explicit RodentChangeForm(QWidget *parent = nullptr, QHash<QString, Rodent*>* = nullptr);
    RodentChangeForm(Rodent*, QHash<QString, Rodent*>*);
    ~RodentChangeForm();

    Rodent* getRodent();
    QString getRodentName() const;
    QString editedRodentName;
private:
    Ui::RodentChangeForm *ui;
    QHash<QString, Rodent*>* rodentHash;

    void nameChecker();
private slots:
    void openFileDialog();

    // QDialog interface
};

#endif // RODENTCHANGEFORM_H
