// ######
// ## TreeMainWindow.cpp
// ## ==================
// ## 07.04.2013: Creation
// ## 02.06.2019: Version v2
// ## 09.07.2022: Prise en compte QT57
// ######

#include "TreeMainWindow.h"
#include "TreeWidget.h"
#include <QCryptographicHash>

#include "FileOpen.xpm"
#include "FileSave.xpm"

//
// <<<< TreeMainWindow::TreeMainWindow
//
TreeMainWindow::TreeMainWindow(QWidget *p_Parent)
    : QMainWindow(p_Parent) {
    QToolBar *toolbar = addToolBar("Outils");
    toolbar->addAction(QIcon("icon.png"), "Choisir Dossier", this, SLOT(slot_Choisir_Dossier_Racine()));
    toolbar->addAction("Sauvegarder Cartographie", this, SLOT(slot_Save_TreeMap()));
    toolbar->addAction("Comparer Cartographies", this, SLOT(slot_Comparer_Cartographies()));
    toolbar->addAction("Quitter", this, SLOT(slot_QUIT()));

    //-- MENU

    _Menu = new QMenu(this);
    _Menu->setTitle("&Fichier");
    _Menu->addAction(QPixmap((const char **) FileSave), "Enregistrer Cartographie", this, SLOT(slot_Save_TreeMap()),
                     Qt::ALT | Qt::Key_E);
    _Menu->addAction("Comparer Cartographies", this, SLOT(slot_Comparer_Cartographies()), Qt::ALT | Qt::Key_C);

    this->menuBar()->addMenu(_Menu);

    _Menu->addAction(QPixmap((const char **) FileOpen), "Choisir Dossier racine", this,
                     SLOT(slot_Choisir_Dossier_Racine()), Qt::ALT | Qt::Key_S);
    _Menu->addAction("Aide (F1)", this, SLOT(slot_ShowHelp()), Qt::Key_F1);
    _Menu->addAction("Recharger (F2)", this, SLOT(slot_Reload()), Qt::Key_F2);
    _Menu->addAction("Ouvrir fichier sélectionné (F3)", this, SLOT(slot_OpenSelectedFile()), Qt::Key_F3);

    _Menu->addSeparator();

    _Menu->addAction("Quitter", this, SLOT(slot_QUIT()), Qt::ALT | Qt::Key_Q);

    //-- SPLITTER

    _Splitter = new QSplitter(this);

    this->setCentralWidget(_Splitter);

    //-- TREEWIDGET Source et Destination

    _TW_Dossier = new TreeWidget(_Splitter);
    _TextEdit = new TextEdit(_Splitter);
    _TextEdit->setStyleSheet("background-color: #f9f9f9; font-family: Monospace; font-size: 12px;");
    _TextEdit->setReadOnly(true);

#ifdef WIN32
    QByteArray BA_HOME(getenv("HOMEDRIVE"));
    BA_HOME.append(getenv("HOMEPATH"));
#else
    QByteArray BA_HOME(getenv("HOME"));
#endif

    QString DefautDIR(BA_HOME);

    Choisir_Dossier_Racine(DefautDIR);

    //-- SLOTS
    connect(_TW_Dossier, SIGNAL(SIGNAL_Statut_Fichier(QString)), this, SLOT(slot_Statut_Fichier(QString)));

    connect(_TW_Dossier, SIGNAL(SIGNAL_RightClicked(QTreeWidgetItem *, int)), this,
            SLOT(slot_PopupContextMenu_TreeView(QTreeWidgetItem *, int)));
}

// >>>> TreeMainWindow::TreeMainWindow

//
// <<<< TreeMainWindow::Choisir_Dossier_Racine
//
void TreeMainWindow::Choisir_Dossier_Racine(QString p_Dossier) {
    QFileInfo FI_Racine(p_Dossier);

    if (FI_Racine.isSymLink()) {
        _Path_Dossier_Racine = FI_Racine.symLinkTarget();
    } else {
        _Path_Dossier_Racine = p_Dossier;
    }

    _TW_Dossier->Add_FirstChild(_Path_Dossier_Racine);
}

// >>>> TreeMainWindow::Choisir_Dossier_Racine

//
// <<<< TreeMainWindow::Statut_Fichier
//
void TreeMainWindow::Statut_Fichier(QString p_Path) {
    QFileInfo qFI(p_Path);

    QDate mDate = qFI.lastModified().date();
    QTime mTime = qFI.lastModified().time();

    QString qDate = mDate.toString("dd.MM.yyyy");
    QString qTime = mTime.toString("hh:mm:ss");

    QString infoDate = QString("Modifié le %1 à %2").arg(qDate).arg(qTime);

    QString infoSize;
    if (qFI.isFile()) {
        infoSize = QString(" | Taille: %1 octets").arg(qFI.size());
    } else if (qFI.isDir()) {
        QDir dir(p_Path);
        QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        int nbFiles = list.size();
        infoSize = QString(" | %1 éléments").arg(nbFiles);
    }

    QString finalStatus = QString("%1 - %2%3").arg(p_Path).arg(infoDate).arg(infoSize);

    this->statusBar()->showMessage(finalStatus);
}

// >>>> TreeMainWindow::Statut_Fichier

//
// <<<< TreeMainWindow::slot_Choisir_Dossier_Racine
//
void TreeMainWindow::slot_Choisir_Dossier_Racine() {
    QFileDialog F_Dialog(this);

#ifdef QT57
    F_Dialog.setFileMode(QFileDialog::DirectoryOnly);
#else
    F_Dialog.setFileMode(QFileDialog::Directory);
    F_Dialog.setOptions(QFileDialog::ShowDirsOnly);
#endif

    F_Dialog.setAcceptMode(QFileDialog::AcceptOpen);
    F_Dialog.setSizeGripEnabled(true);

#ifdef WIN32
    QByteArray BA_HOME(getenv("HOMEDRIVE"));
    BA_HOME.append(getenv("HOMEPATH"));
#else
    QByteArray BA_HOME(getenv("HOME"));
#endif

    QString DefautDIR(BA_HOME);

    F_Dialog.setDirectory(DefautDIR);

    QString qDIR = F_Dialog.getExistingDirectory(this, "Sélection Dossier Source");

    if (!qDIR.isEmpty()) {
        Choisir_Dossier_Racine(qDIR);
    }
}

// >>>> TreeMainWindow::slot_Choisir_Dossier_Racine

//
// <<<< TreeMainWindow::slot_Statut_Fichier
//
void TreeMainWindow::slot_Statut_Fichier(QString p_Path) {
    this->Statut_Fichier(p_Path);
}

// >>>> TreeMainWindow::slot_Statut_Fichier

//
// <<<< TreeMainWindow::slot_PopupContextMenu_TreeView
//
/*
*Affiche un menu contextuel lors d’un clic droit sur un élément de l’arborescence :
Si c’est un dossier, propose de l’ouvrir.
Si c’est un fichier, propose de l’ouvrir avec l’application système,
et s’il s’agit d’un fichier texte, de l’afficher dans l’éditeur.
    */
void TreeMainWindow::slot_PopupContextMenu_TreeView(QTreeWidgetItem *p_Item, int) {
    if (!p_Item)
        return;

    QString PathName = _TW_Dossier->Get_PathName(p_Item);
    QFileInfo FI_Path(PathName);

    FI_Path.setCaching(false);

    while (FI_Path.isSymLink()) {
        PathName = FI_Path.symLinkTarget();
        FI_Path.setFile(PathName);
    }

    if (PathName.isEmpty())
        return;

    QMenu PopupM("PopupMenu TreeView");

    QAction *X_Action_DIR = NULL;
    QAction *X_Action_TXT = NULL;
    QAction *X_Action_Open = NULL;

    if (FI_Path.isDir()) {
        X_Action_DIR = PopupM.addAction("Ouvrir ce dossier");
    } else if (FI_Path.isFile()) {
        X_Action_Open = PopupM.addAction("Ouvrir avec l'application système");

        QFile Fd_Test(PathName);
        if (Fd_Test.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray data = Fd_Test.read(512); // on lit les 512 premiers octets
            Fd_Test.close();

            bool isText = true;
            for (char c: data) {
                if ((c < 32 && c != '\n' && c != '\r' && c != '\t') || c == 127) {
                    isText = false;
                    break;
                }
            }

            if (isText) {
                X_Action_TXT = PopupM.addAction("Afficher le fichier");
            }
        }
    }

    QPoint PM_Point = QCursor::pos() + QPoint(12, 8);
    QAction *ACT_x = PopupM.exec(PM_Point);

    if (!ACT_x)
        return;

    if (ACT_x == X_Action_DIR) {
        this->Choisir_Dossier_Racine(PathName);
    } else if (ACT_x == X_Action_Open) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(PathName));
    } else if (ACT_x == X_Action_TXT) {
        QFile Fd_R(PathName);
        if (Fd_R.open(QIODevice::ReadOnly)) {
            QTextStream TS_R(&Fd_R);
            QString S_TEXT = TS_R.readAll();
            Fd_R.close();

            _TextEdit->setReadOnly(false);
            _TextEdit->setText(S_TEXT);
            _TextEdit->setReadOnly(true);
        }
    }
}

// >>>> TreeMainWindow::slot_PopupContextMenu_TreeView

//
// <<<< TreeMainWindow::slot_QUIT
//
void TreeMainWindow::slot_QUIT() {
    //--  ? Enregistrer_Arborescence( QString p_PathFichier )

    qApp->quit();
}

// >>>> TreeMainWindow::slot_QUIT

void TreeMainWindow::slot_Save_TreeMap() {
    QString filePath = QFileDialog::getSaveFileName(this, "Enregistrer la cartographie", "", "Fichiers Texte (*.txt)");

    if (!filePath.isEmpty()) {
        _TW_Dossier->Save_TreeMap(filePath);
        statusBar()->showMessage("Cartographie enregistrée dans " + filePath, 5000);
    }
}

void TreeMainWindow::slot_Comparer_Cartographies() {
    QString dir1 = QFileDialog::getExistingDirectory(this, "Choisir le premier dossier");
    if (dir1.isEmpty())
        return;

    QString dir2 = QFileDialog::getExistingDirectory(this, "Choisir le deuxième dossier");
    if (dir2.isEmpty())
        return;

    QMap<QString, QString> map1 = buildDirectoryMap(dir1);
    QMap<QString, QString> map2 = buildDirectoryMap(dir2);

    QString result;

    // Fichiers supprimés (présents dans dir1 mais pas dir2)
    for (const QString &path: map1.keys()) {
        if (!map2.contains(path)) {
            result += "Supprimé : " + path + "\n";
        }
    }

    // Fichiers ajoutés (présents dans dir2 mais pas dir1)
    for (const QString &path: map2.keys()) {
        if (!map1.contains(path)) {
            result += "Ajouté : " + path + "\n";
        }
    }

    // Fichiers modifiés
    for (const QString &path: map1.keys()) {
        if (map2.contains(path)) {
            if (map1[path] != map2[path]) {
                result += "Modifié : " + path + "\n";
            }
        }
    }

    if (result.isEmpty()) {
        result = "Aucune différence détectée.";
    }

    QMessageBox::information(this, "Résultat Comparaison", result);
}

QMap<QString, QString> TreeMainWindow::buildDirectoryMap(const QString &rootPath) {
    QMap<QString, QString> map;

    QDir dir(rootPath);
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::DirsFirst);

    for (const QFileInfo &fi: list) {
        QString relativePath = fi.absoluteFilePath().mid(rootPath.length());

        if (fi.isDir()) {
            // Recurse into subdirectories
            QMap<QString, QString> childMap = buildDirectoryMap(fi.absoluteFilePath());
            for (auto it = childMap.begin(); it != childMap.end(); ++it) {
                map.insert(relativePath + it.key(), it.value());
            }
        } else if (fi.isFile()) {
            QString checksum = computeChecksum(fi.absoluteFilePath());
            map.insert(relativePath, checksum);
        }
    }

    return map;
}

QString TreeMainWindow::computeChecksum(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return QString();

    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(&file);
    return hash.result().toHex();
}

void TreeMainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_F1:
            QMessageBox::information(this, "Aide",
                                     "F2 : Recharger\nF3 : Ouvrir le fichier sélectionné\n\nClic droit : Menu contextuel");
            break;
        case Qt::Key_F2:
            this->Choisir_Dossier_Racine(_Path_Dossier_Racine);
            break;
        case Qt::Key_F3: {
            QTreeWidgetItem *currentItem = _TW_Dossier->currentItem();
            if (currentItem) {
                QString path = _TW_Dossier->Get_PathName(currentItem);
                if (QFileInfo(path).isFile()) {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
                }
            }
            break;
        }
        default:
            QMainWindow::keyPressEvent(event);
    }
}

void TreeMainWindow::slot_ShowHelp() {
    QMessageBox::information(this, "Aide",
                             "F1 : Aide\nF2 : Recharger le dossier racine\nF3 : Ouvrir le fichier sélectionné");
}

void TreeMainWindow::slot_Reload() {
    this->Choisir_Dossier_Racine(_Path_Dossier_Racine);
}

void TreeMainWindow::slot_OpenSelectedFile() {
    QTreeWidgetItem *currentItem = _TW_Dossier->currentItem();
    if (currentItem) {
        QString path = _TW_Dossier->Get_PathName(currentItem);
        if (QFileInfo(path).isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        } else {
            QMessageBox::warning(this, "Erreur", "L'élément sélectionné n'est pas un fichier.");
        }
    } else {
        QMessageBox::warning(this, "Erreur", "Aucun fichier sélectionné.");
    }
}
