#include "projectview.h"

#include <QFileSystemModel>

ProjectView::ProjectView(QWidget* parent) :
    QListWidget(parent)
{
    connect(this,&QListWidget::currentItemChanged,this,&ProjectView::updatedCurrentFile);
}

ProjectView::~ProjectView()
{
    /* Removing the current item emits currentItemChanged.  During parent
     * destruction the EELEditor receiver is already past its derived
     * destructor, so no UI transition can be delivered safely. */
    blockSignals(true);
    while(count() > 0)
    {
        QListWidgetItem* listItem = takeItem(0);
        delete listItem->data(Qt::UserRole).value<CodeContainer*>();
        delete listItem;
    }
    previousCont = nullptr;
}

void ProjectView::addFile(QString path){
    closeFile(path);
    CodeContainer* cont = new CodeContainer(path);
    QListWidgetItem* item = new QListWidgetItem();
    QFile file(path);
    item->setText(QFileInfo(file.fileName()).fileName());
    item->setToolTip(path);
    item->setData(Qt::UserRole,QVariant::fromValue<CodeContainer*>(cont));
    item->setSelected(true);
    addItem(item);
    this->setCurrentItem(item);
}

void ProjectView::closeFile(QString path){
    for(int i = count() - 1; i >= 0; i--){
        if(path == item(i)->toolTip()){
            QListWidgetItem* listItem = takeItem(i);
            CodeContainer* container = listItem->data(Qt::UserRole).value<CodeContainer*>();
            if(previousCont == container)
                previousCont = nullptr;
            delete container;
            delete listItem;
            break;
        }
    }
}

CodeContainer* ProjectView::getCurrentFile(){
    if(selectedItems().count() < 1)
        return nullptr;

    return selectedItems().first()->data(Qt::UserRole).value<CodeContainer*>();
}

void ProjectView::setCurrentFilePath(const QString& path)
{
    auto* container = getCurrentFile();
    auto* current = currentItem();
    if (container == nullptr || current == nullptr || path.isEmpty())
        return;

    container->path = path;
    current->setToolTip(path);
    current->setText(QFileInfo(path).fileName());
}

void ProjectView::updatedCurrentFile(QListWidgetItem* item){
    if(item == nullptr) {
        emit currentFileUpdated(previousCont,nullptr);
        return;
    }
    auto container = item->data(Qt::UserRole).value<CodeContainer*>(); 
    emit currentFileUpdated(previousCont,container);
    previousCont = container;
}

void ProjectView::closeCurrentFile(){
    if(selectedItems().count() < 1)
        return;

    QString path = selectedItems().first()->data(Qt::UserRole).value<CodeContainer*>()->path;
    closeFile(path);
}
