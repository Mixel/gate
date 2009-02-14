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
  //init
  KAction *a = actionCollection()->addAction( "git_init" );
  a->setText( i18n("Init") );
  connect( a, SIGNAL( triggered(bool) ), this, SLOT( gitInit() ) );
  //push remote 
  a = actionCollection()->addAction( "git_push" );
  a->setText( i18n("Push") );
  connect( a, SIGNAL( triggered(bool) ), this, SLOT( gitPush() ) );
 //checkout
  a = actionCollection()->addAction( "git_checkout" );
  a->setText( i18n("Checkout") );
  connect( a, SIGNAL( triggered(bool) ), this, SLOT( gitCheckout() ) );
  
  //add remote repository 
  a = actionCollection()->addAction( "git_add_respository" );
  a->setText( i18n("Add remote repository") );
  connect( a, SIGNAL( triggered(bool) ), this, SLOT( gitRemoteAddRepo() ) );
 //add file
  a = actionCollection()->addAction( "git_add_file" );
  a->setText( i18n("Add file") );
  connect( a, SIGNAL( triggered(bool) ), this, SLOT( gitAdd() ) );
 //commit
  a = actionCollection()->addAction( "git_commit" );
  a->setText( i18n("Commit") );
  connect( a, SIGNAL( triggered(bool) ), this, SLOT( gitCommit() ) );
//commit -a
 a = actionCollection()->addAction( "git_commit_all" );
  a->setText( i18n("Commit All changed files") );
  connect( a, SIGNAL( triggered(bool) ), this, SLOT( gitCommitAll() ) );





  mainWindow()->guiFactory()->addClient( this );

  

  QWidget *toolview = mainWin->createToolView ("kate_private_plugin_gateplugin", Kate::MainWindow::Bottom, SmallIcon("utilities-terminal"), i18n("Gate"));
  mainWin->hideToolView(toolview);
  m_console = new KateConsole(mainWin, toolview); 
   
}

KatePluginGateView::~KatePluginGateView()
{
  mainWindow()->guiFactory()->removeClient( this );
}



void KatePluginGateView::gitPush()
{
  // comando push, utilizando origin como repositorio y head, lo que le indica a git 
  // que utilice el branch actual
  m_console->sendInput( KShell::quoteArg("git") + "  push origin HEAD" + '\n' ); 
}

void KatePluginGateView::gitCheckout()
{
bool ok;
  QString res = KInputDialog::getText(i18n("Checkout"),
		i18n("Branch to checkout"), QString::null,&ok,0,0,0,QString::null);
 if ( ok ){
    m_console->sendInput(  "git checkout " + KShell::quoteArg(res) +  '\n' );
 }
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
		i18n("url"),QString::null,&ok,0,0,0,QString::null);
 if ( ok ){
    m_console->sendInput(  "git remote add origin " + KShell::quoteArg(res) +  '\n' );
 }
}

void KatePluginGateView::gitAdd()
{
  m_console->sendInput("git add " + KShell::quoteArg(mainWindow()->activeView()->document()->url().path() ) + '\n' );
}

void  KatePluginGateView::gitCommit()
{
  bool ok;
  QString res = KInputDialog::getText(i18n("Commit message"),
		i18n("Mesage"), QString::null,&ok,0,0,0,QString::null);
 if ( ok ){
    m_console->sendInput(  "git commit -m" + KShell::quoteArg(res) +  '\n' );
 }
}

void  KatePluginGateView::gitCommitAll()
{
  bool ok;
  QString res = KInputDialog::getText(i18n("Commit  message"),
		i18n("Mesage"), QString::null,&ok,0,0,0,QString::null);
 if ( ok ){
    m_console->sendInput(  "git commit -a -m" + KShell::quoteArg(res) +  '\n' );
 }
}

void KatePluginGateView::gitAddTag()
{
  m_console->sendInput("git add " + KShell::quoteArg(mainWindow()->activeView()->document()->url().path()) + '\n' );
}
/*
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
*/
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














