#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    readSettings();
    qDebug() << path;

    actSave = new QAction("Сохранить");

    readXMLFileAndBuildTree();

    changesSaved = true;

    connect(ui->lineEdit, &QLineEdit::textEdited, this, &Widget::search);
    connect(ui->lineEdit, &QLineEdit::textEdited, this, &Widget::antiSearch);
    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &Widget::currentRodentShow);
    connect(ui->treeWidget, &CustomTreeWidget::onAddClickedSignal, this, &Widget::addNewRodentSlot);
    connect(ui->treeWidget, &CustomTreeWidget::onDeleteClickedSignal, this, &Widget::deleteRodentSlot);
    connect(ui->treeWidget, &CustomTreeWidget::onEditClickedSignal, this, &Widget::editRodentSlot);

    menuBar = new QMenuBar();

    actSave->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    QAction *actChange = new QAction("Выбрать файл");
    QAction *actNew = new QAction("Создать новый файл");

    connect(actSave, &QAction::triggered, this, &Widget::saveChanges);
    connect(actChange, &QAction::triggered, this, &Widget::changePath);
    connect(actNew, &QAction::triggered, this, &Widget::createNewFile);
    menuBar->addAction(actSave);
    menuBar->addAction(actChange);
    menuBar->addAction(actNew);
    ui->verticalLayout_8->insertWidget(0, menuBar);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::search(QString searchStr)
{
    qDebug() << searchStr;
    if(rootItem == nullptr){
        return;
    }

    rootItem->setHidden(!searchStr.isEmpty());

    if(searchStr.isEmpty()){
        return;
    }
    for(rodentHashIt = rodentsHash.begin(); rodentHashIt != rodentsHash.end(); ++rodentHashIt){
        if(rodentHashIt.key().contains(searchStr, Qt::CaseInsensitive) && !searchHub.contains(rodentHashIt.key(), Qt::CaseInsensitive)){
            QTreeWidgetItem* rodentItem = buildRodentTree(rodentHashIt.value());
            ui->treeWidget->addTopLevelItem(rodentItem);
            searchHub.emplace_back(rodentHashIt.key());
        }
    }
}

void Widget::antiSearch(QString searchStr)
{
    if(rootItem != nullptr){
        if(searchStr.isEmpty()){
            while(ui->treeWidget->topLevelItemCount() != 1){
                delete ui->treeWidget->topLevelItem(1);
            }
            searchHub.clear();
        }
        else{
            for(int i = 1; i < ui->treeWidget->topLevelItemCount(); ++i){
                if(!ui->treeWidget->topLevelItem(i)->text(0).contains(searchStr, Qt::CaseInsensitive)){
                    for(int j = 0; j < searchHub.size(); j++){
                        if(searchHub[j] == ui->treeWidget->topLevelItem(i)->text(0)){
                           searchHub.removeAt(j);
                           break;
                        }
                    }
                    deleteLater.append(ui->treeWidget->topLevelItem(i));
                }
            }
            for(QTreeWidgetItem* item : deleteLater){
                delete item;
            }
            deleteLater.clear();
        }
    }
}

void Widget::addNewRodentSlot(QTreeWidgetItem *item, int pos)
{
    RodentChangeForm dlg(this, &rodentsHash);

    switch( dlg.exec() ) {
    case QDialog::Accepted:{
        qDebug() << "принято";
        if(pos == -1){
            addNewRodentAfterSuborder(dlg.getRodent(), item);
        }
        else if(searchHub.empty()){
            addNewRodent(pos, dlg.getRodent(), item);
        }
        else{
            QString searchName = item->text(0);

            QTreeWidgetItemIterator it(ui->treeWidget);
            while (*it) {
                QTreeWidgetItem *originalItem = *it;
                if (originalItem->text(0) == searchName) {
                    addNewRodent(pos, dlg.getRodent(), originalItem);
                    break;
                }
                ++it;
            }
        }
        changesSaved = false;
        break;
    }
    case QDialog::Rejected:
        qDebug() << "Rejected";
        break;
    default:
        qDebug() << "Unexpected";
    }
}

void Widget::addNewRodent(bool pos, Rodent *newRodent, QTreeWidgetItem *&item)
{
    QTreeWidgetItem* parentItem = item->parent();
    QTreeWidgetItem* newRodentItem;

    newRodent->suborderRus = item->parent()->text(0);
    newRodent->setSuborderEng();
    rodentsHash.insert(newRodent->name, newRodent);

    newRodentItem = buildRodentTree(rodentsHash.value(newRodent->name));
    int newItemIndex = parentItem->indexOfChild(item) + pos;
    parentItem->insertChild(newItemIndex, newRodentItem);
}

void Widget::addNewRodentAfterSuborder(Rodent *newRodent, QTreeWidgetItem *&item)
{
    newRodent->suborderRus = item->text(0);
    qDebug() << item->text(0);
    newRodent->setSuborderEng();
    rodentsHash.insert(newRodent->name, newRodent);

    item->addChild(buildRodentTree(newRodent));
}

void Widget::editRodentSlot(QTreeWidgetItem *item)
{
    RodentChangeForm dlg(rodentsHash.value(item->text(0)), &rodentsHash);
    switch( dlg.exec() ) {
    case QDialog::Accepted:{
        if(*rodentsHash.value(item->text(0)) != *dlg.getRodent()){
            if(searchHub.empty()){
                editRodent(dlg.getRodent(), item);
                currentRodentShow(item);
            }
            else{
                QString searchName = item->text(0);

                QTreeWidgetItemIterator it(ui->treeWidget);
                while (*it) {
                    QTreeWidgetItem *originalItem = *it;

                    if (originalItem->text(0) == searchName) {
                        editRodent(dlg.getRodent(), item, originalItem);
                        currentRodentShow(item);
                        break;
                    }
                    ++it;
                }
            }
            changesSaved = false;
        }
        break;
    }
    case QDialog::Rejected:
        qDebug() << "Rejected";
        break;
    default:
        qDebug() << "Unexpected";
    }
}


void Widget::editRodent(Rodent* editedRodent, QTreeWidgetItem *&item, QTreeWidgetItem *originalItem)
{
    Rodent* oldRodent = rodentsHash.take(item->text(0));
    editedRodent->suborderEng = oldRodent->suborderEng;
    editedRodent->suborderRus = oldRodent->suborderRus;
    delete oldRodent;
    rodentsHash.insert(editedRodent->name, editedRodent);
    item->setText(0, editedRodent->name);
    item->child(0)->setText(0, editedRodent->name);
    item->child(1)->setText(0, editedRodent->description);
    item->child(2)->setText(0, editedRodent->habitat);
    item->child(3)->setText(0, QString::number(editedRodent->weight));
    item->child(4)->setText(0, QString::number(editedRodent->maximum_age));
    item->child(5)->setText(0, editedRodent->image_path);

    if(originalItem != nullptr){
        originalItem->setText(0, editedRodent->name);
        originalItem->child(0)->setText(0, editedRodent->name);
        originalItem->child(1)->setText(0, editedRodent->description);
        originalItem->child(2)->setText(0, editedRodent->habitat);
        originalItem->child(3)->setText(0, QString::number(editedRodent->weight));
        originalItem->child(4)->setText(0, QString::number(editedRodent->maximum_age));
        originalItem->child(5)->setText(0, editedRodent->image_path);
    }
}

void Widget::saveChanges()
{
    qDebug() << "сохранено";
    /*
    // if(isDarkMode){
    //     isDarkMode = false;
    //     ui->frame->setStyleSheet("background-color:lightgrey;"
    //                              "color:black;"
    //                              "border-radius:10px;");
    //     ui->treeWidget->setStyleSheet("QTreeWidget{"
    //                                         "background-color:lightgrey;"
    //                                         "color:black;"
    //                                         "border-radius:10px;}"
    //                                   "QHeaderView::section {"
    //                                         "background-color: white;}");
    //     ui->frame_2->setStyleSheet("background-color:lightgrey;"
    //                                "color:black;"
    //                                "border-radius:10px;");
    //     menuBar->setStyleSheet("background-color:lightgrey;"
    //                            "color:black;");
    // }
    // else{
    //     isDarkMode = true;
    //     ui->frame->setStyleSheet("background-color:black;"
    //                              "color:white;"
    //                              "border-radius:10px;");
    //     ui->treeWidget->setStyleSheet("QTreeWidget{"
    //                                         "background-color:black;"
    //                                         "color: white;"
    //                                         "border-radius:10px;}"
    //                                   "QHeaderView::section {"
    //                                         "background-color: black;"
    //                                         "color:white;}");
    //     ui->frame_2->setStyleSheet("background-color:black;"
    //                                "color:white;"
    //                                "border-radius:10px;");
    //     menuBar->setStyleSheet("background-color:black;"
    //                            "color:white;");
    // }
    */
    saveXMLFile();
    changesSaved = true;
}

void Widget::deleteRodentSlot(QTreeWidgetItem *item)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление");
    msgBox.setText("Вы действительно хотите удалить " + item->text(0) + "?");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    switch(msgBox.exec()){
        case QMessageBox::Yes:{
            if(searchHub.empty()){
                rodentsHash.remove(item->text(0));
                delete item;
            }
            else{
                QString searchName = item->text(0);

                QTreeWidgetItemIterator it(ui->treeWidget);
                while (*it) {
                    QTreeWidgetItem *originaltItem = *it;
                    if (originaltItem->text(0) == searchName) {
                        qDebug() << originaltItem->text(0);
                        rodentsHash.remove(item->text(0));
                        delete originaltItem;
                        delete item;
                        for(int j = 0; j < searchHub.size(); j++){
                            if(searchHub[j] == searchName){
                                searchHub.removeAt(j);
                                break;
                            }
                        }
                        break;
                    }
                    ++it;
                }
            }
            changesSaved = false;
            break;
        }
        case QMessageBox::Cancel:{
            break;
        }
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    qDebug() << changesSaved;
    saveSettings();
    if(!changesSaved){
        QMessageBox msgBox;
        msgBox.setText("Сохранить изменения в " + ui->treeWidget->headerItem()->text(0) + "?");
        msgBox.setWindowTitle("Сохранитю изменения");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);

        switch(msgBox.exec()){
            case QMessageBox::Yes:{
                saveChanges();
                qDebug() << "да";
                event->accept();
                break;
            }
            case QMessageBox::No:{
                qDebug() << "нет";
                event->accept();
                break;
                }
            case QMessageBox::Cancel:{
                qDebug() << "отмена";
                event->ignore();
                break;
            }
        }

    }
}

bool Widget::saveChangesDialog()
{
    QMessageBox msgBox;
    msgBox.setText("Сохранить изменения в " + ui->treeWidget->headerItem()->text(0) + "?");
    msgBox.setWindowTitle("Сохранить изменения?");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    switch(msgBox.exec()){
            case QMessageBox::Yes:{
                saveChanges();
                qDebug() << "да";
                return 1;
                break;
            }
            case QMessageBox::No:{
                qDebug() << "нет";
                return 1;
                break;
            }
            case QMessageBox::Cancel:{
                qDebug() << "отмена";
                return 0;
                break;
            }
        }
}

void Widget::createNewFile()
{
    QString newPath = QFileDialog::getSaveFileName(nullptr, "Создать файл", QDir::currentPath(), "XML Files (*.xml)");

    if(!newPath.isEmpty())
    {
        QFile file(newPath);
        if(file.open(QIODevice::WriteOnly))
        {
            QXmlStreamWriter writer;
            writer.setDevice(&file);
            writer.setAutoFormatting(true);
            writer.writeStartDocument();
            writer.writeStartElement("rodents");

            writer.writeStartElement("suborder");
            writer.writeAttribute("name_eng", "Hystricomorpha");
            writer.writeAttribute( "name_rus",  QString::fromUtf8("Дикобразообразные"));
            writer.writeEndElement();

            writer.writeStartElement("suborder");
            writer.writeAttribute("name_eng", "Myomorpha");
            writer.writeAttribute( "name_rus", QString::fromUtf8("Мышеобразные"));
            writer.writeEndElement();

            writer.writeStartElement("suborder");
            writer.writeAttribute("name_eng", "Anomaluromorpha");
            writer.writeAttribute( "name_rus", QString::fromUtf8("Шипохвостообразные"));
            writer.writeEndElement();

            writer.writeStartElement("suborder");
            writer.writeAttribute("name_eng", "Sciuromorpha");
            writer.writeAttribute( "name_rus", QString::fromUtf8("Белкообразные"));
            writer.writeEndElement();

            writer.writeStartElement("suborder");
            writer.writeAttribute("name_eng", "Castorimorpha");
            writer.writeAttribute( "name_rus", QString::fromUtf8("Боброобразные"));
            writer.writeEndElement();

            writer.writeEndElement();
            writer.writeEndDocument();
            file.close();
        }
        else
        {
            qDebug() << "Error: Cannot open file for writing";
        }
    }
}

void Widget::saveXMLFile()
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for reading saveXML";
        return;
    }
    QXmlStreamWriter writer;
    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("rodents");
    QTreeWidgetItem *item;
    Rodent *saveRodent;
    QTreeWidgetItemIterator it(ui->treeWidget);
    while (*it) {
        item = *it;
        if(ui->treeWidget->indexOfTopLevelItem(item) > 0){
            break;
        }
        if(item->parent() == rootItem){
            switch (enumConventer(item->text(0))) {
            case Hystricomorpha:
                writer.writeStartElement("suborder");
                writer.writeAttribute("name_eng", "Hystricomorpha");
                writer.writeAttribute( "name_rus",  QString::fromUtf8("Дикобразообразные"));
                break;
            case Myomorpha:
                writer.writeEndElement();
                writer.writeStartElement("suborder");
                writer.writeAttribute("name_eng", "Myomorpha");
                writer.writeAttribute( "name_rus", QString::fromUtf8("Мышеобразные"));
                break;
            case Anomaluromorpha:
                writer.writeEndElement();
                writer.writeStartElement("suborder");
                writer.writeAttribute("name_eng", "Anomaluromorpha");
                writer.writeAttribute( "name_rus", QString::fromUtf8("Шипохвостообразные"));
                break;
            case Sciuromorpha:
                writer.writeEndElement();
                writer.writeStartElement("suborder");
                writer.writeAttribute("name_eng", "Sciuromorpha");
                writer.writeAttribute( "name_rus", QString::fromUtf8("Белкообразные"));
                break;
            case Castorimorpha:
                writer.writeEndElement();
                writer.writeStartElement("suborder");
                writer.writeAttribute("name_eng", "Castorimorpha");
                writer.writeAttribute( "name_rus", QString::fromUtf8("Боброобразные"));
                break;
            default:
                break;
            }
        }
        if(rodentsHash.contains(item->text(0)) && item->childCount() != 0){
            saveRodent = rodentsHash.value(item->text(0));
            writer.writeStartElement("rodent");
            writer.writeAttribute("name", saveRodent->name);

            writer.writeStartElement("name");
            writer.writeCharacters(saveRodent->name);
            writer.writeEndElement();

            writer.writeStartElement("description");
            writer.writeCharacters(saveRodent->description);
            writer.writeEndElement();

            writer.writeStartElement("habitat");
            writer.writeCharacters(saveRodent->habitat);
            writer.writeEndElement();

            writer.writeStartElement("weight");
            writer.writeCharacters(QString::number(saveRodent->weight));
            writer.writeEndElement();

            writer.writeStartElement("maximum_age");
            writer.writeCharacters(QString::number(saveRodent->maximum_age));
            writer.writeEndElement();

            writer.writeStartElement("image_path");
            writer.writeCharacters(saveRodent->image_path);
            writer.writeEndElement();

            writer.writeEndElement();
        }
        ++it;
    }
    writer.writeEndElement();
    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
}

void Widget::readSettings()
{
    QFile file("settings.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open settings file for reading";
        return;
    }

    QXmlStreamReader reader;
    reader.setDevice(&file);
    QXmlStreamAttributes attr;
    while(!reader.atEnd()){
        if(reader.isStartElement()){
            if(reader.name().toString() == "path"){
                path = reader.readElementText();
            }
        }
        reader.readNext();
    }
    file.close();
}

void Widget::saveSettings()
{
    QFile file("settings.xml");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for reading";
        return;
    }
    QXmlStreamWriter writer;
    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("settings");
    writer.writeStartElement("path");
    writer.writeCharacters(path);
    writer.writeEndElement();
    writer.writeEndElement();
    writer.writeEndElement();
}


void Widget::changePath()
{
    if(!changesSaved){
        if(!saveChangesDialog())
            return;
    }
    QString newPath(QFileDialog::getOpenFileName(this, "Выбрать файл", QDir::currentPath(),"XML Files (*.xml)"));
    if(!newPath.isEmpty() && newPath != path){
        QFile file(newPath);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QXmlStreamReader reader;
            reader.setDevice(&file);
            reader.readNext();
            reader.readNext();
            if(reader.name().toString() == "rodents"){
                file.close();
                path = newPath;
                readXMLFileAndBuildTree();
            }
            else{
                QMessageBox::critical(nullptr, "Ошибка чтения файла", "Не удалось распарсить файл");
                file.close();
            }
        }
        else{
            qDebug() << "ошибка открытия файла в changePath";
        }
    }
}

void Widget::deleteTreeItem(QTreeWidgetItem *item)
{
    if(item) {
        QList<QTreeWidgetItem*> children = item->takeChildren();
        for(QTreeWidgetItem* child : children) {
            deleteTreeItem(child);
        }
        delete item;
    }
}


void Widget::readXMLFileAndBuildTree()
{

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open rodents file for reading buildtree";
        actSave->setEnabled(false);
        return;
    }
    actSave->setEnabled(true);

    QFileInfo fileInfo(path);
    ui->treeWidget->setHeaderLabel(fileInfo.fileName());

    if(rootItem != nullptr){
        deleteTreeItem(rootItem);
        rootItem = nullptr;
    }

    rodentsHash.clear();

    rootItem = new QTreeWidgetItem;
    rootItem->setText(0, "Грызуны");
    QTreeWidgetItem* HystricomorphaItem = new QTreeWidgetItem(QStringList() << "Дикобразообразные");
    QTreeWidgetItem* MyomorphaItem = new QTreeWidgetItem(QStringList() << "Мышеобразные");
    QTreeWidgetItem* AnomaluromorphaItem = new QTreeWidgetItem(QStringList() << "Шипохвостообразные");
    QTreeWidgetItem* SciuromorphaItem = new QTreeWidgetItem(QStringList() << "Белкообразные");
    QTreeWidgetItem* CastorimorphaItem = new QTreeWidgetItem(QStringList() << "Боброобразные");
    rootItem->addChild(HystricomorphaItem);
    rootItem->addChild(MyomorphaItem);
    rootItem->addChild(AnomaluromorphaItem);
    rootItem->addChild(SciuromorphaItem);
    rootItem->addChild(CastorimorphaItem);

    QXmlStreamReader reader;
    reader.setDevice(&file);
    QXmlStreamAttributes attr;
    QString suborderEng;
    QString suborderRus;
    Rodent *rodentBuffer = nullptr;
    int i = 0;

    while(!reader.atEnd()){
        if(reader.isStartElement()){
            if(reader.name().toString() == "suborder"){
                attr = reader.attributes();
                suborderEng = attr.value("name_eng").toString();
                suborderRus = attr.value("name_rus").toString();
            }
            else if(reader.name().toString() == "rodent"){
                rodentBuffer = new Rodent(suborderEng, suborderRus);
            }
            else if(reader.name().toString() == "name"){
                rodentBuffer->name = reader.readElementText();
                rodentsHash.insert(rodentBuffer->name, rodentBuffer);
            }
            else if(reader.name().toString() == "description"){
                rodentBuffer->description = reader.readElementText();
            }
            else if(reader.name().toString() == "habitat"){
                rodentBuffer->habitat = reader.readElementText();
            }
            else if(reader.name().toString() == "weight"){
                rodentBuffer->weight = reader.readElementText().toFloat();
            }
            else if(reader.name().toString() == "maximum_age"){
                rodentBuffer->maximum_age = reader.readElementText().toFloat();
            }
            else if(reader.name().toString() == "image_path"){
                rodentBuffer->image_path = reader.readElementText();
            }
        }

        if(reader.name().toString() == "rodent" && reader.isEndElement()){
            i++;
            switch(enumConventer(suborderEng)){
            case Hystricomorpha:
                HystricomorphaItem->addChild(buildRodentTree(rodentBuffer));
                break;
            case Myomorpha:
                MyomorphaItem->addChild(buildRodentTree(rodentBuffer));
                break;
            case Anomaluromorpha:
                AnomaluromorphaItem->addChild(buildRodentTree(rodentBuffer));
                break;
            case Sciuromorpha:
                SciuromorphaItem->addChild(buildRodentTree(rodentBuffer));
                break;
            case Castorimorpha:
                CastorimorphaItem->addChild(buildRodentTree(rodentBuffer));
                break;
            default:
                break;
            }
        }

        reader.readNext();
    }
    qDebug() << i;
    ui->treeWidget->insertTopLevelItem(0, rootItem);
    ui->lineEdit->setText("");
    antiSearch("");
    if(rootItem->child(0)->child(0)){
        currentRodentShow(rootItem->child(0)->child(0));
    }
    file.close();
}


QTreeWidgetItem* Widget::buildRodentTree(Rodent* rodent){
    QTreeWidgetItem* rodentItem = new QTreeWidgetItem(QStringList() << rodent->name);
    rodentItem->addChild(new QTreeWidgetItem(QStringList() << rodent->name));
    rodentItem->addChild(new QTreeWidgetItem(QStringList() << rodent->description.left(30) + "..."));
    rodentItem->addChild(new QTreeWidgetItem(QStringList() << rodent->habitat.left(30) + "..."));
    rodentItem->addChild(new QTreeWidgetItem(QStringList() << QString::number(rodent->weight)));
    rodentItem->addChild(new QTreeWidgetItem(QStringList() << QString::number(rodent->maximum_age)));
    rodentItem->addChild(new QTreeWidgetItem(QStringList() << rodent->image_path));
    return rodentItem;
}


int Widget::enumConventer(const QString &suborder)
{
    if(suborder == "Hystricomorpha" || suborder == "Дикобразообразные")
        return 0;
    else if(suborder == "Myomorpha" || suborder == "Мышеобразные")
        return 1;
    else if(suborder == "Anomaluromorpha" || suborder == "Шипохвостообразные")
        return 2;
    else if(suborder == "Sciuromorpha" || suborder == "Белкообразные")
        return 3;
    else
        return 4;
}

QPixmap& Widget::roundedPixmap(QString& path)
{
    QPixmap pic(path);
    if(pic.isNull()){
        pic.load(":/img/img/No-Image-Placeholder-darken.png");
    }

    int radius = 8;
    QPixmap *canvas = new QPixmap(ui->PictureLbl->size());
    canvas->fill(Qt::transparent);
    pic = pic.scaled(ui->PictureLbl->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QPainter painter(canvas);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect(0.0, 0.0, ui->PictureLbl->width(), ui->PictureLbl->height());
    painter.setBrush(QBrush(pic));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, radius, radius);

    return *canvas;
}


void Widget::currentRodentShow(QTreeWidgetItem *item)
{
    QString itemText = item->text(0);
    if(rodentsHash.contains(itemText)){
        ui->nameLbl->setText(rodentsHash.value(itemText)->name);
        ui->habitatTextEdit->setText(rodentsHash.value(itemText)->habitat);
        ui->weightLbl->setText(QString::number(rodentsHash.value(itemText)->weight));
        ui->maximum_ageLbl->setText(QString::number(rodentsHash.value(itemText)->maximum_age));
        ui->descriptionTextEdit->setText(rodentsHash.value(itemText)->description);
        ui->suborderLblRus->setText(rodentsHash.value(itemText)->suborderRus);
        ui->suborderLblEng->setText("(" + rodentsHash.value(itemText)->suborderEng + ")");
        ui->PictureLbl->setPixmap(roundedPixmap(rodentsHash.value(itemText)->image_path));
    }
}







