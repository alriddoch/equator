#include "ChatWindow.h"

#include <iostream>

#include <gtkmm/window.h>

#include <Eris/Lobby.h>
#include <Eris/Person.h>

ChatWindow::ChatWindow(Eris::Lobby & Lobby, const Glib::ustring & Self) :
	m_lobby(Lobby),
	m_self(Self)
{
	add(m_Notebook);
	m_Notebook.show();
	
	RoomPanel * pLobbyPanel(new RoomPanel(Lobby));
	
	pLobbyPanel->InitiateDialog.connect(sigc::mem_fun(this, &ChatWindow::initiateDialog));
	m_Notebook.add(Lobby.getId(), pLobbyPanel);
	m_Notebook.property_can_focus() = false;
	Lobby.PrivateTalk.connect(sigc::mem_fun(this, &ChatWindow::privateTalk));
}

void ChatWindow::initiateDialog(std::string AccountID)
{
	Panel * pPanel(m_Notebook.get(AccountID));
	
	if(pPanel == 0)
	{
		m_Notebook.add(AccountID, new PersonPanel(*(m_lobby.getPerson(AccountID)), m_self));
	}
	m_Notebook.switchTo(AccountID);
}

void ChatWindow::privateTalk(Eris::Person * pPerson, const std::string & Message)
{
	Panel * pPanel(m_Notebook.get(pPerson->getAccount()));
	
	if(pPanel == 0)
	{
		m_Notebook.add(pPerson->getAccount(), new PersonPanel(*pPerson, m_self));
	}
	else
	{
		pPanel->pushMessage(Message);
	}
}
