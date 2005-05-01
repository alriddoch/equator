#ifndef EQUATOR_GUI_GTKMM_PANELNOTEBOOK_H
#define EQUATOR_GUI_GTKMM_PANELNOTEBOOK_H

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/liststore.h>
#include <gtkmm/notebook.h>
#include <gtkmm/paned.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>

namespace Gtk {
	class Action;
	class ActionGroup;
	class ListStore;
	class TextBuffer;
	class UIManager;
}

namespace Eris {
	class Person;
	class Room;
}

class Panel
{
public:
	Panel(const std::string & Caption);
	virtual ~Panel(void);
	const std::string getCaption(void) const;
	virtual void pushMessage(const std::string & Message);
	virtual Gtk::Widget & getWidget(void) = 0;
	sigc::signal< void > Changed;
private:
	Panel(const Panel & Panel);
	std::string m_caption;
};

class ColoredNotebook : public Gtk::Notebook
{
public:
	ColoredNotebook(void);
	void add(const std::string & sIdentifier, Panel * pPanel);
	Panel * get(const std::string & sIdentifier);
	void switchTo(const std::string & sIdentifier);
	const std::string & getActivePanelIdentifier(void) const;
	Panel * getActivePanel(void);
private:
	void panelContentChanged(Gtk::Widget * pChild);
	void onSwitchPage(GtkNotebookPage * pPage, guint iPageNum);
	std::map< std::string, Panel * > m_Panels;
	std::map< int, std::string > m_PageIdentifiers;
	Gdk::Color m_ContentNormal;
	Gdk::Color m_ContentChanged;
};

class TextPanel : public Panel
{
public:
	TextPanel(const std::string & Caption);
	virtual void pushMessage(const std::string & Message);
	virtual Gtk::Widget & getWidget(void);
private:
	void adjustToBottom(void);
	Gtk::Adjustment m_verticalAdjustment;
	Gtk::ScrolledWindow m_scroller;
	Glib::RefPtr< Gtk::TextBuffer > m_textBuffer;
	double m_panelLength;
};

class PersonPanel : public TextPanel, public sigc::trackable
{
public:
	PersonPanel(Eris::Person & Person, const Glib::ustring & Self);
	virtual void pushMessage(const std::string & Message);
	virtual Gtk::Widget & getWidget(void);
	virtual ~PersonPanel(void);
private:
	void explicitePushMessage(const std::string & Message);
	void sendMessage(void);
	PersonPanel(const PersonPanel & Panel);
	Gtk::VBox m_vBox;
	Gtk::Entry m_entry;
	Eris::Person & m_person;
	Glib::ustring m_self;
};

class UserStore : public Gtk::ListStore
{
private:
	class UserStoreColumns : public Gtk::TreeModelColumnRecord
	{
	public:
		Gtk::TreeModelColumn< Glib::ustring > ID;
		Gtk::TreeModelColumn< Glib::ustring > Name;
	
		UserStoreColumns(void)
		{
			add(ID);
			add(Name);
		}
	};
public:
	static Glib::RefPtr< UserStore > create(Eris::Room & Room);
	UserStoreColumns & Columns;
private:
	UserStore(Eris::Room & Room);
	void appearance(Eris::Room * pRoom, Eris::Person * pPerson);
	void disappearance(Eris::Room * pRoom, Eris::Person * pPerson);
	static UserStoreColumns * m_pColumns;
};

class RoomPanel : public TextPanel, public sigc::trackable
{
public:
	RoomPanel(Eris::Room & Room);
	virtual Gtk::Widget & getWidget(void);
	virtual ~RoomPanel(void);
	sigc::signal< void, std::string > InitiateDialog;
private:
	void entered(Eris::Room * pRoom);
	void appearance(Eris::Room * pRoom, Eris::Person * pPerson);
	void disappearance(Eris::Room * pRoom, Eris::Person * pPerson);
	void speech(Eris::Room * pRoom, Eris::Person * pPerson, const std::string & Message);
	void emote(Eris::Room * pRoom, Eris::Person * pPerson, const std::string & Message);
	void sendMessage(void);
	void dialogPressed(void);
	void selectionChanged(void);
	RoomPanel(const RoomPanel & Panel);
	Gtk::HPaned m_paned;
	Glib::RefPtr< UserStore > m_userStore;
	Gtk::TreeView m_userView;
	Gtk::Entry m_entry;
	Eris::Room & m_room;
	Glib::RefPtr< Gtk::UIManager > m_UIManager;
	Glib::RefPtr< Gtk::ActionGroup > m_actions;
	Glib::RefPtr< Gtk::Action > m_actionDialog;
};

#endif
