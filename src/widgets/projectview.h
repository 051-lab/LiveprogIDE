#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <QListWidget>
#include <model/codecontainer.h>

class ProjectView : public QListWidget
{
    Q_OBJECT
public:
    ProjectView(QWidget* parent = nullptr);
    ~ProjectView() override;
    void addFile(QString path);
    void closeFile(QString path);
    void closeCurrentFile();
    CodeContainer *getCurrentFile();
    void setCurrentFilePath(const QString& path);
private slots:
    void updatedCurrentFile(QListWidgetItem* item);
signals:
    void currentFileUpdated(CodeContainer* prev,CodeContainer* cur);
private:
    CodeContainer* previousCont = nullptr;
};

#endif // PROJECTVIEW_H
