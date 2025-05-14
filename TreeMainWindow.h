 //######
//## TreeMainWindow.h
//## ================
//## 07.04.2013: Création
//## 02.06.2019: Version v2
//## 09.07.2022: Prise en compte QT57
//######

#ifndef TREEMAINWINDOW_H
#define TREEMAINWINDOW_H

#include "TreeWidget.h"
#include "TextEdit.h"

class TreeMainWindow: public QMainWindow
{
  Q_OBJECT

public:
  TreeMainWindow( QWidget * p_Parent = 0 );

  void Choisir_Dossier_Racine( QString p_Dossier );

  void Statut_Fichier( QString p_Path );

public slots:
  void slot_Choisir_Dossier_Racine();
  void slot_Statut_Fichier( QString p_Path );
  void slot_Save_TreeMap();

  void slot_PopupContextMenu_TreeView( QTreeWidgetItem * p_item, int p_Column );

  void slot_QUIT();

protected:
  QString           _Path_Dossier_Racine;

  QMenu            *_Menu;

  QSplitter        *_Splitter;

  TreeWidget       *_TW_Dossier;
  TextEdit         *_TextEdit;
};

#endif