#include "gatekateplugin.h"
#include "gatekateplugin.moc"


#include <ktexteditor/view.h>
#include <ktexteditor/document.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <klocale.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kaboutdata.h>
#include <kshell.h>
#include <kinputdialog.h>

K_PLUGIN_FACTORY(KatePluginGateFactory, registerPlugin<KatePluginGate>();)
K_EXPORT_PLUGIN(KatePluginGateFactory(KAboutData("kateGate","kateGate",ki18n("Hello World"), "0.1", ki18n("Example kate plugin"))) )

KatePluginGate::KatePluginGate( QObject* parent, const QList<QVariant>& )
    : Kate::Plugin( (Kate::Application*)parent, "kate-gate-plugin" )
{
}

KatePluginGate::~KatePluginGate()
{
}

Kate::PluginView *KatePluginGate::createView( Kate::MainWindow *mainWindow )
{
  return new KatePluginGateView( mainWindow );
}

KatePluginGateView::KatePluginGateView( Kate::MainWindow *mainWin )
    : Kate::PluginView( mainWin )
{
  setComponentData( KatePluginGateFactory::componentData() );
  setXMLFile( "plugins/kategate/ui.rc" );
  KAction *a = actionCollection()->addAction( "git_init" );
  a->setText( i18n("Init") );
  connect( a, SIGNAL( triggered(bool) ), this, SLOT( gitInit() ) );
  a = actionCollection()->addAction( "git_add_respository" );
  a->setText( i18n("Add remote repository") );
  connect( a, SIGNAL( triggered(bool) ), this, SLOT( gitRemoteAddRepo() ) );
  a = actionCollection()->addAction( "git_add_file" );
  a->setText( i18n("Add file") );
  connect( a, SIGNAL( triggered(bool) ), this, SLOT( gitAdd() ) );
  mainWindow()->guiFactory()->addClient( this );

  QWidget *toolview = mainWin->createToolView ("kate_private_plugin_gateplugin", Kate::MainWindow::Bottom, SmallIcon("utilities-terminal"), i18n("Gate"));
  mainWin->hideToolView(toolview);
  m_console = new KateConsole(mainWin, toolview); 
   
}

KatePluginGateView::~KatePluginGateView()
{
  mainWindow()->guiFactory()->removeClient( this );
}

void KatePluginGateView::gitInit()
{
  // creamos el repositorio
  m_console->sendInput( KShell::quoteArg("git") + " init" + '\n' ); //'\n');
}

void  KatePluginGateView::gitRemoteAddRepo()
{
  bool ok;
  QString res = KInputDialog::getText(i18n("add repository to git"),
		i18n("url"),
		QString::null,
		&ok,
		0,
		0,
		0,
		QString::null
	);
 if ( ok ){
    m_console->sendInput(  "git remote add origin " + KShell::quoteArg(res) +  '\n' );
 }
}

void KatePluginGateView::gitAdd()
{
  m_console->sendInput("git add " + KShell::quoteArg(mainWindow()->activeView()->document()->url().path() ) + '\n' );
 
}
void KatePluginGateView::slotInsertHello()
{
  if (!mainWindow()) {
    return;
  }

  KTextEditor::View *kv = mainWindow()->activeView();

  if (kv) {
    kv->insertText( "Hello World" );
  }
}

void KatePluginGateView::readSessionConfig( KConfigBase* config, const QString& groupPrefix )
{
  // If you have session-dependant settings, load them here.
  // If you have application wide settings, you have to read your own KConfig,
  // see the Kate::Plugin docs for more information.
  Q_UNUSED( config );
  Q_UNUSED( groupPrefix );
}

void KatePluginGateView::writeSessionConfig( KConfigBase* config, const QString& groupPrefix )
{
  // If you have session-dependant settings, save them here.
  // If you have application wide settings, you have to create your own KConfig,
  // see the Kate::Plugin docs for more information.
  Q_UNUSED( config );
  Q_UNUSED( groupPrefix );
}














