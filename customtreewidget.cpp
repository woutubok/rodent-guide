#include "customtreewidget.h"

CustomTreeWidget::CustomTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTreeWidget::customContextMenuRequested, this, &CustomTreeWidget::showContextMenu);
    //connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
}

void CustomTreeWidget::showContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = itemAt(pos);
    if (!item)
        return;
    else if(item->parent() == this->topLevelItem(0)){
        QMenu menu(this);
        QAction *action1 = menu.addAction("Добавить");
        connect(action1, &QAction::triggered, [this, item]() {
            qDebug() << "Action добавить triggered for item: " << item->text(0);
            emit onAddClickedSignal(item, -1);
        });
        menu.exec(viewport()->mapToGlobal(pos));
    }
    else if (this->topLevelItem(0)->isHidden() && item->childCount()){
            QMenu menu(this);
            QAction *action1 = menu.addAction("Редактировать");
            QAction *action2 = menu.addAction("Добавить перед");
            QAction *action3 = menu.addAction("Добавить после");
            QAction *action4 = menu.addAction("Удалить");

            connect(action1, &QAction::triggered, [this, item]() {
                qDebug() << "Action 1 triggered for item: бебра " << item->text(0);
                emit onEditClickedSignal(item);
            });
            connect(action2, &QAction::triggered, [this, item]() {
                qDebug() << "Action 2 triggered for item: " << item->text(0);
                emit onAddClickedSignal(item, BEFORE);
            });
            connect(action3, &QAction::triggered, [this, item]() {
                qDebug() << "Action 3 triggered for item: " << item->text(0);
                emit onAddClickedSignal(item, AFTER);
            });
            connect(action4, &QAction::triggered, [this, item]() {
                qDebug() << "Action 4 triggered for item: " << item->text(0);
                emit onDeleteClickedSignal(item);
            });
            menu.exec(viewport()->mapToGlobal(pos));
    }
    else if(item != this->topLevelItem(0)){
        if(item->parent()->parent() == this->topLevelItem(0)){
            QMenu menu(this);
            QAction *action1 = menu.addAction("Редактировать");
            QAction *action2 = menu.addAction("Добавить перед");
            QAction *action3 = menu.addAction("Добавить после");
            QAction *action4 = menu.addAction("Удалить");
            connect(action1, &QAction::triggered, [this, item]() {
                qDebug() << "Action 1 triggered for item: " << item->text(0);
                emit onEditClickedSignal(item);
            });
            connect(action2, &QAction::triggered, [this, item]() {
                qDebug() << "Action 2 triggered for item: " << item->text(0);
                emit onAddClickedSignal(item, BEFORE);
            });
            connect(action3, &QAction::triggered, [this, item]() {
                qDebug() << "Action 3 triggered for item: " << item->text(0);
                emit onAddClickedSignal(item, AFTER);
            });
            connect(action4, &QAction::triggered, [this, item]() {
                qDebug() << "Action 4 triggered for item: " << item->text(0);
                emit onDeleteClickedSignal(item);
            });
            menu.exec(viewport()->mapToGlobal(pos));
        }
    }
}




