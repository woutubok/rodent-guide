#include "rodentchangeform.h"
#include "ui_rodentchangeform.h"

RodentChangeForm::RodentChangeForm(QWidget *parent, QHash<QString, Rodent *> *hash)
    : QDialog(parent)
    , ui(new Ui::RodentChangeForm)
{
    ui->setupUi(this);
    ui->acceptButton->setDefault(true);

    this->rodentHash = hash;

    connect(ui->acceptButton, &QPushButton::clicked, this, &RodentChangeForm::nameChecker);
    connect(ui->rejectButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->fileButton, &QPushButton::clicked, this, &RodentChangeForm::openFileDialog);
}

RodentChangeForm::RodentChangeForm(Rodent * editedRodent, QHash<QString, Rodent *> *hash)
    : RodentChangeForm()
{
    this->rodentHash = hash;
    editedRodentName = editedRodent->name;

    ui->nameLineEdit->setText(editedRodent->name);
    ui->weightSpinBox->setValue(editedRodent->weight);
    ui->ageSpinBox->setValue(editedRodent->maximum_age);
    ui->habitatTextEdit->setText(editedRodent->habitat);
    ui->descriptionTextEdit->setText(editedRodent->description);
    ui->imageLineEdit->setText(editedRodent->image_path);
}

RodentChangeForm::~RodentChangeForm()
{
    delete ui;
}

Rodent *RodentChangeForm::getRodent()
{
    Rodent* newRodent = new Rodent;
    newRodent->name = ui->nameLineEdit->text();
    newRodent->weight = ui->weightSpinBox->value();
    newRodent->maximum_age = ui->ageSpinBox->value();
    newRodent->habitat = ui->habitatTextEdit->toPlainText();
    newRodent->description = ui->descriptionTextEdit->toPlainText();
    newRodent->image_path = ui->imageLineEdit->text();
    return newRodent;
}

QString RodentChangeForm::getRodentName() const
{
    return ui->nameLineEdit->text();
}


void RodentChangeForm::openFileDialog()
{
    QString newPath(QFileDialog::getOpenFileName(this, "Выбрать картинку"));
    if(!newPath.isEmpty()){
        ui->imageLineEdit->setText(newPath);
    }
}
void RodentChangeForm::nameChecker()
{
    if(ui->nameLineEdit->text().isEmpty()){
        QMessageBox::critical(nullptr, "Ошибка", "Имя не может быть пустым");
    }
    else if(rodentHash == nullptr){
        accept();
    }
    else if(ui->nameLineEdit->text() == editedRodentName){
        accept();
    }
    else if(rodentHash->contains(ui->nameLineEdit->text())){
        QMessageBox::critical(nullptr, "Ошибка", "Данное имя уже существует");
    }
    else{
        accept();
    }
}
