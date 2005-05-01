#include "PanelNotebook.h"

#include <gtkmm/action.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/button.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>
#include <gtkmm/uimanager.h>

#include <Eris/Person.h>
#include <Eris/Room.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
//  Panel                                                                                        //
///////////////////////////////////////////////////////////////////////////////////////////////////

Panel::Panel(const std::string & Caption) :
	m_caption(Caption)
{
}

Panel::~Panel(void)
{
}

const std::string Panel::getCaption(void) const
{
	return m_caption;
}

void Panel::pushMessage(const std::string & Message)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  ColoredNotebook                                                                              //
///////////////////////////////////////////////////////////////////////////////////////////////////

ColoredNotebook::ColoredNotebook(void)
{
	m_ContentNormal.set_rgb_p(0.0, 0.0, 0.0);
	m_ContentChanged.set_rgb_p(0.0, 0.0, 1.0);
	// don't know why, but the protected on_switch_page does not work.
	Gtk::Notebook::signal_switch_page().connect(sigc::mem_fun(*this, &ColoredNotebook::onSwitchPage));
}

void ColoredNotebook::add(const std::string & sIdentifier, Panel * pPanel)
{
	std::map< std::string, Panel * >::iterator iPanel(m_Panels.find(sIdentifier));
	
	if(iPanel == m_Panels.end())
	{
		m_Panels[sIdentifier] = pPanel;
		
		int iPageNum(append_page(pPanel->getWidget(), pPanel->getCaption()));
		
		m_PageIdentifiers[iPageNum] = sIdentifier;
		pPanel->Changed.connect(sigc::bind(sigc::mem_fun(*this, &ColoredNotebook::panelContentChanged), &(pPanel->getWidget())));
	}
}

Panel * ColoredNotebook::get(const std::string & sIdentifier)
{
	std::map< std::string, Panel * >::iterator iPanel(m_Panels.find(sIdentifier));
	
	return ((iPanel == m_Panels.end()) ? (0) : (iPanel->second));
}

void ColoredNotebook::switchTo(const std::string & sIdentifier)
{
	Panel * pPanel(get(sIdentifier));
	
	if(pPanel != 0)
	{
		set_current_page(page_num(pPanel->getWidget()));
	}
}

const std::string & ColoredNotebook::getActivePanelIdentifier(void) const
{
	static std::string sEmpty = "";
	std::map< int, std::string >::const_iterator iPage(m_PageIdentifiers.find(get_current_page()));
	
	if(iPage == m_PageIdentifiers.end())
	{
		return sEmpty;
	}
	else
	{
		return iPage->second;
	}
}

Panel * ColoredNotebook::getActivePanel(void)
{
	return m_Panels[m_PageIdentifiers[get_current_page()]];
}

void ColoredNotebook::panelContentChanged(Gtk::Widget * pChild)
{
	if(page_num(*pChild) != get_current_page())
	{
		Gtk::Widget * pLabel(get_tab_label(*pChild));
		
		if(pLabel == 0)
		{
			throw;
		}
		pLabel->modify_fg(Gtk::STATE_NORMAL, m_ContentChanged);
		pLabel->modify_fg(Gtk::STATE_ACTIVE, m_ContentChanged);
	}
}

void ColoredNotebook::onSwitchPage(GtkNotebookPage * pPage, guint iPageNum)
{
	Gtk::Widget * pChild(get_nth_page(iPageNum));
	
	if(pChild == 0)
	{
		throw;
	}
	
	Gtk::Widget * pLabel(get_tab_label(*pChild));
	
	if(pLabel == 0)
	{
		throw;
	}
	pLabel->modify_fg(Gtk::STATE_NORMAL, m_ContentNormal);
	pLabel->modify_fg(Gtk::STATE_ACTIVE, m_ContentNormal);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  TextPanel                                                                                    //
///////////////////////////////////////////////////////////////////////////////////////////////////

TextPanel::TextPanel(const std::string & Caption) :
	Panel(Caption),
	m_verticalAdjustment(0, 0, 0, 0, 0, 0),
	m_textBuffer(Gtk::TextBuffer::create()),
	m_panelLength(0.0)
{
	m_verticalAdjustment.signal_changed().connect(sigc::mem_fun(*this, &TextPanel::adjustToBottom));
	m_scroller.set_vadjustment(m_verticalAdjustment);
	m_scroller.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
	
	Gtk::TextView * pTextView = manage(new Gtk::TextView(m_textBuffer));
	
	pTextView->set_wrap_mode(Gtk::WRAP_WORD);
	pTextView->set_editable(false);
	pTextView->set_cursor_visible(false);
	pTextView->show();
	m_scroller.add(*pTextView);
	m_scroller.show();
}

void TextPanel::pushMessage(const std::string & Message)
{
	m_textBuffer->insert(m_textBuffer->end(), Message + "\n");
	Changed();
}

Gtk::Widget & TextPanel::getWidget(void)
{
	return m_scroller;
}

void TextPanel::adjustToBottom(void)
{
	if(m_panelLength != m_verticalAdjustment.get_upper())
	{
		double bottomPage = m_panelLength - m_verticalAdjustment.get_page_size();
		
		m_panelLength = m_verticalAdjustment.get_upper();
		if(m_verticalAdjustment.get_value() >= bottomPage)
		{
			m_verticalAdjustment.set_value(m_verticalAdjustment.get_upper());
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  PersonPanel                                                                                  //
///////////////////////////////////////////////////////////////////////////////////////////////////

PersonPanel::PersonPanel(Eris::Person & Person, const Glib::ustring & Self) :
	TextPanel(Person.getName()),
	m_vBox(false, 0),
	m_person(Person),
	m_self(Self)
{
	m_vBox.pack_start(TextPanel::getWidget(), true, true, 0);
	m_entry.signal_activate().connect(sigc::mem_fun(this, &PersonPanel::sendMessage));
	m_entry.show();
	m_vBox.pack_end(m_entry, false, false, 0);
	m_vBox.show();
}

PersonPanel::~PersonPanel(void)
{
}

void PersonPanel::pushMessage(const std::string & Message)
{
	TextPanel::pushMessage("<" + m_person.getName() + "> " + Message);
}

void PersonPanel::explicitePushMessage(const std::string & Message)
{
	TextPanel::pushMessage(Message);
}

Gtk::Widget & PersonPanel::getWidget(void)
{
	return m_vBox;
}

void PersonPanel::sendMessage(void)
{
	m_person.msg(m_entry.get_text());
	explicitePushMessage("<" + m_self + "> " + m_entry.get_text());
	m_entry.set_text("");
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  UserStore                                                                                    //
///////////////////////////////////////////////////////////////////////////////////////////////////

UserStore::UserStoreColumns * UserStore::m_pColumns = 0;

UserStore::UserStore(Eris::Room & Room) :
	Gtk::ListStore(*m_pColumns),
	Columns(*m_pColumns)
{
	Room.Appearance.connect(sigc::mem_fun(this, &UserStore::appearance));
	Room.Disappearance.connect(sigc::mem_fun(this, &UserStore::disappearance));
	std::vector< Eris::Person * > People = Room.getPeople();
	
	for(std::vector< Eris::Person * >::iterator iPerson = People.begin(); iPerson != People.end(); ++iPerson)
	{
		Gtk::TreeRow User(*append());
		
		User[Columns.ID] = (*iPerson)->getAccount();
		User[Columns.Name] = (*iPerson)->getName();
	}
}

Glib::RefPtr< UserStore > UserStore::create(Eris::Room & Room)
{
	if(UserStore::m_pColumns == 0)
	{
		m_pColumns = new(UserStore::UserStoreColumns);
	}
	
	return Glib::RefPtr< UserStore >(new UserStore(Room));
}

void UserStore::appearance(Eris::Room * pRoom, Eris::Person * pPerson)
{
	Gtk::TreeRow User(*append());
	
	User[Columns.ID] = pPerson->getAccount();
	User[Columns.Name] = pPerson->getName();
}

void UserStore::disappearance(Eris::Room * pRoom, Eris::Person * pPerson)
{
	Gtk::TreeIter User(children().begin());
	
	while(User)
	{
		if((*User)[Columns.ID] == pPerson->getAccount())
		{
			erase(User);
			
			return;
		}
		++User;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  RoomPanel                                                                                    //
///////////////////////////////////////////////////////////////////////////////////////////////////

static const Glib::ustring g_sUI = 
"<ui>"
"  <toolbar name='ToolBar'>"
"    <toolitem action='Dialog'/>"
"  </toolbar>"
"</ui>";

RoomPanel::RoomPanel(Eris::Room & Room) :
	TextPanel(Room.getName()),
	m_userStore(UserStore::create(Room)),
	m_userView(m_userStore),
	m_room(Room),
	m_UIManager(Gtk::UIManager::create()),
	m_actions(Gtk::ActionGroup::create()),
	m_actionDialog(Gtk::Action::create("Dialog", "Dialog", "Opens a private chat panel."))
{
	Room.Entered.connect(sigc::mem_fun(this, &RoomPanel::entered));
	Room.Appearance.connect(sigc::mem_fun(this, &RoomPanel::appearance));
	Room.Disappearance.connect(sigc::mem_fun(this, &RoomPanel::disappearance));
	Room.Speech.connect(sigc::mem_fun(this, &RoomPanel::speech));
	Room.Emote.connect(sigc::mem_fun(this, &RoomPanel::emote));
	m_actions->add(m_actionDialog, sigc::mem_fun(this, &RoomPanel::dialogPressed));
	m_UIManager->insert_action_group(m_actions);
	m_UIManager->add_ui_from_string(g_sUI);
	m_userView.append_column("Name", m_userStore->Columns.Name);
	m_userView.property_can_focus() = false;
	m_userView.get_selection()->signal_changed().connect(sigc::mem_fun(this, &RoomPanel::selectionChanged));
	m_userView.show();
	TextPanel::getWidget().property_can_focus() = false;
	
	Gtk::VBox * pVBox1(manage(new Gtk::VBox(false, 0)));
	
	pVBox1->pack_start(TextPanel::getWidget(), true, true, 0);
	m_entry.signal_activate().connect(sigc::mem_fun(this, &RoomPanel::sendMessage));
	m_entry.show();
	pVBox1->pack_end(m_entry, false, false, 0);
	pVBox1->show();
	m_paned.pack1(*pVBox1, true, true);
	
	Gtk::VBox * pVBox2(manage(new Gtk::VBox(false, 0)));
	
	pVBox2->pack_start(m_userView, true, true, 0);
	pVBox2->pack_start(*(m_UIManager->get_widget("/ToolBar")), false, false, 0);
	pVBox2->show();
	m_paned.pack2(*pVBox2, false, false);
	m_paned.set_position(600);
	m_paned.show();
	selectionChanged();
}

RoomPanel::~RoomPanel(void)
{
}

Gtk::Widget & RoomPanel::getWidget(void)
{
	return m_paned;
}

void RoomPanel::entered(Eris::Room * pRoom)
{
	pushMessage("You entered the room.");
}

void RoomPanel::appearance(Eris::Room * pRoom, Eris::Person * pPerson)
{
	pushMessage("\"" + pPerson->getName() + "\" entered the room.");
}

void RoomPanel::disappearance(Eris::Room * pRoom, Eris::Person * pPerson)
{
	pushMessage("\"" + pPerson->getName() + "\" left the room.");
}

void RoomPanel::speech(Eris::Room * pRoom, Eris::Person * pPerson, const std::string & Message)
{
	if(pPerson != 0)
	{
		pushMessage("<" + pPerson->getName() + "> " + Message);
	}
	else
	{
		pushMessage("<unknown> " + Message);
	}
}

void RoomPanel::emote(Eris::Room * pRoom, Eris::Person * pPerson, const std::string & Message)
{
	pushMessage(pPerson->getName() + " " + Message);
}

void RoomPanel::sendMessage(void)
{
	const Glib::ustring & sText(m_entry.get_text());
	
	if(sText.substr(0, 4) == "/me ")
	{
		m_room.emote(sText.substr(4));
	}
	else
	{
		m_room.say(sText);
	}
	m_entry.set_text("");
}

void RoomPanel::dialogPressed(void)
{
	Gtk::TreeIter Iterator(m_userView.get_selection()->get_selected());
	
	if(Iterator == true)
	{
		Glib::ustring sID((*Iterator)[m_userStore->Columns.ID]);
		
		InitiateDialog(sID);
	}
}

void RoomPanel::selectionChanged(void)
{
    Gtk::TreeIter Iterator(m_userView.get_selection()->get_selected());
    
	m_actionDialog->set_sensitive(Iterator);
}
