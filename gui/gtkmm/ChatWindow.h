#ifndef EQUATOR_GUI_GTKMM_CHATWINDOW_H
#define EQUATOR_GUI_GTKMM_CHATWINDOW_H

#include <gtkmm/window.h>

#include "PanelNotebook.h"

namespace Eris {
	class Lobby;
	class Person;
}

class ChatWindow : public Gtk::Window
{
public:
	ChatWindow(Eris::Lobby & Lobby, const Glib::ustring & Self);
private:
	void initiateDialog(std::string AccountID);
	void privateTalk(Eris::Person * pPerson, const std::string & Message);
	ColoredNotebook m_Notebook;
	Eris::Lobby & m_lobby;
	Glib::ustring m_self;
};

#endif
