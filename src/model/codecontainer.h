#ifndef CODECONTAINER_H
#define CODECONTAINER_H

#include "qdebug.h"
#include <QFile>
#include <QSaveFile>
#ifndef HEADLESS
#include <QMessageBox>
#endif
#include <QString>
#include <QTextStream>

#ifdef EELEDITOR_TEST_HOOKS
inline bool codeContainerSaveCommitFailureForTests = false;
inline void CodeContainerSetSaveCommitFailureForTests(bool fail)
{
    codeContainerSaveCommitFailureForTests = fail;
}
#endif

class CodeContainer
{
public:
    CodeContainer(){}
    CodeContainer(QString _path){
        path = _path;
        reloadCode();
    };
    bool reloadCode(){
        codeLoaded = false;
        QFile fl(path);
        if (!fl.open(QIODevice::ReadOnly))
            return false;
        code = fl.readAll();
        codeLoaded = true;
        return true;
    }
    bool save(QString cpath = "", QWidget* parent = nullptr, QString *errorMessage = nullptr){
        if(cpath.isEmpty())
            cpath = path;

        QSaveFile file(cpath);
        file.setDirectWriteFallback(false);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream << code;
            stream.flush();
            bool committed = stream.status() == QTextStream::Ok && file.flush();
#ifdef EELEDITOR_TEST_HOOKS
            if (committed)
            {
                if (codeContainerSaveCommitFailureForTests)
                    committed = false;
                else
                    committed = file.commit();
            }
#else
            if (committed)
                committed = file.commit();
#endif
            if (committed)
                return true;

            const QString reason = file.errorString().isEmpty()
                ? QStringLiteral("The atomic save could not be committed.") : file.errorString();
            const QString message = QStringLiteral("Failed to save script to %1. Reason: %2").arg(cpath, reason);
            if(errorMessage != nullptr)
                *errorMessage = message;
#ifndef HEADLESS
            if(parent != nullptr)
                QMessageBox::warning(parent, "File error", message);
#endif
            qWarning().noquote().nospace() << "Failed to commit eel script to " << cpath;
            qWarning().noquote().nospace() << "Error code: " << file.error() << "; reason: " << file.errorString();
            return false;
        }
        else {
            const QString reason = file.errorString().isEmpty()
                ? QStringLiteral("The destination could not be opened.") : file.errorString();
            const QString message = QStringLiteral("Failed to save script to %1. Reason: %2").arg(cpath, reason);
            if(errorMessage != nullptr)
                *errorMessage = message;
#ifndef HEADLESS
            if(parent != nullptr)
                QMessageBox::warning(parent, "File error", message);
#endif
            qWarning().noquote().nospace() << "Failed to save eel script to " << cpath;
            qWarning().noquote().nospace() << "Error code: " << file.error() << "; reason: " << file.errorString();
            return false;
        }
    };

    bool codeLoaded = false;
    QString code;
    QString path;
};

Q_DECLARE_METATYPE(CodeContainer*);

#endif // CODECONTAINER_H
