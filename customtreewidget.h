#ifndef CUSTOMTREEWIDGET_H
#define CUSTOMTREEWIDGET_H

#include <QTreeWidget>
#include <QMenu>
#include <QMouseEvent>
class CustomTreeWidget : public QTreeWidget
{
    Q_OBJECT
    enum Position{
        BEFORE,
        AFTER
    };

public:
    CustomTreeWidget(QWidget *parent = nullptr);
    QTreeWidgetItem* currentItem;
private:
    void showContextMenu(const QPoint &pos);

private slots:

signals:
    void onAddClickedSignal(QTreeWidgetItem* item, int pos);
    void onDeleteClickedSignal(QTreeWidgetItem* item);
    void onEditClickedSignal(QTreeWidgetItem* item);
};

#endif // CUSTOMTREEWIDGET_H
