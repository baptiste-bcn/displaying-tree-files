// ######
// ## TreeAppli.cpp
// ## =============
// ## 07.04.2013: Creation
// ## 02.06.2019: Version v2
// ## 09.07.2022: Prise en compte QT57
// ######

#include "TreeMainWindow.h"
#include "TreeWidget.h"

//
// <<<< main
//
int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  TreeMainWindow *Window = new TreeMainWindow();

  Window->setWindowTitle(QString::fromUtf8("QTreeWidget List Directory ;)"));

  int BX = 150;

#ifdef QT57

  QDesktopWidget *Desktop = QApplication::desktop();

  int WW = Desktop->screenGeometry().width() - 2 * BX;
  int HH = Desktop->screenGeometry().height() - 2 * BX;

#else

  QSize SizeS = Window->screen()->availableSize();

  int HH = SizeS.height() - 2 * BX;
  int WW = SizeS.width() - 2 * BX;

#endif

  Window->resize(WW, HH);
  Window->move(BX, BX);

  Window->show();

  return app.exec();
}
// >>>> main
