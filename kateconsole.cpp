/* This file is part of the KDE project
   Copyright (C) 2001 Christoph Cullmann <cullmann@kde.org>
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2002 Anders Lund <anders.lund@lund.tdcadsl.dk>
   Copyright (C) 2007 Anders Lund <anders@alweb.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kateconsole.h"
#include "kateconsole.moc"

#include <kicon.h>
#include <kiconloader.h>
#include <ktexteditor/document.h>
#include <ktexteditor/view.h>

#include <kde_terminal_interface.h>
#include <kshell.h>
#include <kparts/part.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <KDialog>


#include <kurl.h>
#include <klibloader.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
//Added by qt3to4:
#include <QShowEvent>

#include <QCheckBox>
#include <QVBoxLayout>

#include <kpluginloader.h>
#include <kaboutdata.h>
#include <kpluginfactory.h>
#include <kauthorized.h>


KateConsole::KateConsole (Kate::MainWindow *mw, QWidget *parent)
    : KVBox (parent), KXMLGUIClient()
    , m_part (0)
    , m_mw (mw)
    , m_toolView (parent)
{
  // this must be called before putting anything into actionCollection()
  setComponentData (KComponentData("kate"));

  QAction* a = actionCollection()->addAction("katekonsole_tools_pipe_to_terminal");
  a->setIcon(KIcon("pipe"));
  a->setText(i18n("&Pipe to Terminal"));
  connect(a, SIGNAL(triggered()), this, SLOT(slotPipeToConsole()));

  a = actionCollection()->addAction("katekonsole_tools_sync");
  a->setText(i18n("S&ynchronize Terminal with Current Document"));
  connect(a, SIGNAL(triggered()), this, SLOT(slotManualSync()));

  a = actionCollection()->addAction("katekonsole_tools_toggle_focus");
  a->setIcon(KIcon("utilities-terminal"));
  a->setText(i18n("&Focus Terminal"));
  connect(a, SIGNAL(triggered()), this, SLOT(slotToggleFocus()));

  setXMLFile("plugins/katekonsole/ui.rc");
  m_mw->guiFactory()->addClient (this);

 // readConfig();
}

KateConsole::~KateConsole ()
{
  m_mw->guiFactory()->removeClient (this);
  if (m_part)
    disconnect ( m_part, SIGNAL(destroyed()), this, SLOT(slotDestroyed()) );
}

void KateConsole::loadConsoleIfNeeded()
{
  if (m_part) return;

  if (!window() || !parentWidget()) return;
  if (!window() || !isVisibleTo(window())) return;

  KPluginFactory *factory = KPluginLoader("libkonsolepart").factory();

  if (!factory) return;

  m_part = static_cast<KParts::ReadOnlyPart *>(factory->create<QObject>(this, this));

  if (!m_part) return;

  // start the terminal
  qobject_cast<TerminalInterface*>(m_part)->showShellInDir( QString() );

  KGlobal::locale()->insertCatalog("konsole");

  setFocusProxy(m_part->widget());
  m_part->widget()->show();

  connect ( m_part, SIGNAL(destroyed()), this, SLOT(slotDestroyed()) );

  slotSync();
}

void KateConsole::slotDestroyed ()
{
  m_part = 0;

  // hide the dockwidget
  if (parentWidget())
  {
    m_mw->hideToolView (m_toolView);
    m_mw->centralWidget()->setFocus ();
  }
}

void KateConsole::showEvent(QShowEvent *)
{
  if (m_part) return;

  loadConsoleIfNeeded();
}

void KateConsole::cd (const KUrl &url)
{
  sendInput("cd " + KShell::quoteArg(url.path()) + '\n');
}

void KateConsole::sendInput( const QString& text )
{
  loadConsoleIfNeeded();
 
  if (!m_part) return;
  
  TerminalInterface *t = qobject_cast<TerminalInterface *>(m_part);

  if (!t) return;
  t->sendInput (text);
}

void KateConsole::slotPipeToConsole ()
{
  if (KMessageBox::warningContinueCancel
      (m_mw->window()
       , i18n ("Do you really want to pipe the text to the console? This will execute any contained commands with your user rights.")
       , i18n ("Pipe to Terminal?")
       , KGuiItem(i18n("Pipe to Terminal")), KStandardGuiItem::cancel(), "Pipe To Terminal Warning") != KMessageBox::Continue)
    return;

  KTextEditor::View *v = m_mw->activeView();

  if (!v)
    return;

  if (v->selection())
    sendInput (v->selectionText());
  else
    sendInput (v->document()->text());
}

void KateConsole::slotSync()
{
  if (m_mw->activeView() ) {
    if ( m_mw->activeView()->document()->url().isValid()
      && m_mw->activeView()->document()->url().isLocalFile() ) {
      cd(KUrl( m_mw->activeView()->document()->url().directory() ));
    } else if ( !m_mw->activeView()->document()->url().isEmpty() ) {
      sendInput( "### " + i18n("Sorry, can not cd into '%1'", m_mw->activeView()->document()->url().directory() ) + '\n' );
    }
  }
}

void KateConsole::slotManualSync()
{
  slotSync();
  if ( ! m_part || ! m_part->widget()->isVisible() )
    m_mw->showToolView( parentWidget() );
}
void KateConsole::slotToggleFocus()
{
  QAction *action = actionCollection()->action("katekonsole_tools_toggle_focus");
  if ( ! m_part ) {
    m_mw->showToolView( parentWidget() );
    action->setText( i18n("Defocus Terminal") );
    return; // this shows and focuses the konsole
  }

  if ( ! m_part ) return;

  if (m_part->widget()->hasFocus()) {
    if (m_mw->activeView())
      m_mw->activeView()->setFocus();
      action->setText( i18n("Focus Terminal") );
  } else {
    // show the view if it is hidden
    if (parentWidget()->isHidden())
      m_mw->showToolView( parentWidget() );
    else // should focus the widget too!
      m_part->widget()->setFocus( Qt::OtherFocusReason );
    action->setText( i18n("Defocus Terminal") );
  }
}
/*
void KateConsole::readConfig()
{
  if ( KConfigGroup(KGlobal::config(), "Konsole").readEntry("AutoSyncronize", false) )
    connect( m_mw, SIGNAL(viewChanged()), SLOT(slotSync()) );
  else
    disconnect( m_mw, SIGNAL(viewChanged()), this, SLOT(slotSync()) );
}

*/



// kate: space-indent on; indent-width 2; replace-tabs on;

