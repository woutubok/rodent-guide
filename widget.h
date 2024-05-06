#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtXml>
#include <QTreeWidget>
#include <QPainter>
#include <QHash>
#include <QVector>
#include <QCloseEvent>
#include <QMessageBox>
#include <QShortcut>
#include <QMenuBar>
#include <QMenu>
#include "rodent.h"
#include "rodentchangeform.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    enum Suborders{
        Hystricomorpha,
        Myomorpha,
        Anomaluromorpha,
        Sciuromorpha,
        Castorimorpha
    };

    Ui::Widget *ui;
    QTreeWidgetItem *rootItem = nullptr;
    QTreeWidgetItem *searchItem;
    QList<QTreeWidgetItem*> deleteLater;
    QHash<QString, Rodent*> rodentsHash;
    QHash<QString, Rodent*>::iterator rodentHashIt;
    QVector<QString> searchHub;
    QString path;
    QMenuBar *menuBar;
    QAction *actSave;
    bool changesSaved;
    bool isDarkMode = 0;
    void readXMLFileAndBuildTree();
    QTreeWidgetItem* buildRodentTree(Rodent*);
    int enumConventer(const QString&);
    QPixmap& roundedPixmap(QString& path);
    void addNewRodent(bool pos, Rodent *newRodent, QTreeWidgetItem *&item);
    void addNewRodentAfterSuborder(Rodent *newRodent, QTreeWidgetItem *&item);
    void editRodent(Rodent *editedRodent, QTreeWidgetItem *&item, QTreeWidgetItem *originalItem = nullptr);
    void saveXMLFile();
    void readSettings();
    void deleteTreeItem(QTreeWidgetItem*);
    void saveSettings();
    bool saveChangesDialog();

private slots:
    void currentRodentShow(QTreeWidgetItem* item);
    void search(QString searchStr);
    void antiSearch(QString searchStr);
    void addNewRodentSlot(QTreeWidgetItem* item, int pos);
    void deleteRodentSlot(QTreeWidgetItem* item);
    void editRodentSlot(QTreeWidgetItem* item);
    void saveChanges();
    void changePath();
    void createNewFile();

protected:
    void closeEvent(QCloseEvent *event);
};
#endif // WIDGET_H
