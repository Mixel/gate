#ifndef _PLUGIN_KATE_GATE_H_
#define _PLUGIN_KATE_GATE_H_

#include <kate/mainwindow.h>
#include <kate/plugin.h>
#include "kateconsole.h"

#include <kxmlguiclient.h>

class QShowEvent;

namespace KParts
{
  class ReadOnlyPart;
}

namespace KateMDI
{
 }


class KatePluginGate : public Kate::Plugin
{
  Q_OBJECT

  public:
    explicit KatePluginGate( QObject* parent = 0, const QList<QVariant>& = QList<QVariant>() );
    virtual ~KatePluginGate();
      Kate::PluginView *createView( Kate::MainWindow *mainWindow );
  private:
};

class KatePluginGateView : public Kate::PluginView, public KXMLGUIClient
{
    Q_OBJECT

  public:
    KatePluginGateView( Kate::MainWindow *mainWindow );
    ~KatePluginGateView();

    virtual void readSessionConfig( KConfigBase* config, const QString& groupPrefix );
    virtual void writeSessionConfig( KConfigBase* config, const QString& groupPrefix );

  public slots:  
    void gitInit();
    void gitPush();
    void gitRemoteAddRepo();
    void gitAdd();
    void gitCommit();
    void gitCommitAll();
    void gitAddTag();
  private:
   KateConsole *m_console;
};

#endif
