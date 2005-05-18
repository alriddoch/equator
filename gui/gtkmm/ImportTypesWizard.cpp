#include "ImportTypesWizard.h"

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include <gtkmm/action.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/treerowreference.h>
#include <gtkmm/treestore.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/uimanager.h>

#include <Atlas/Message/Element.h>
#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Codecs/XML.h>

#define RESPONSE_NEXT 1
#define RESPONSE_PREVIOUS 2

static const Glib::ustring g_sUI = ""
"<ui>"
"  <toolbar name='ToolBar'>"
"    <toolitem action='Add'/>"
"    <toolitem action='Remove'/>"
"  </toolbar>"
"</ui>";

class XMLFileDecoder : public std::map< std::string, Atlas::Message::MapType >, public Atlas::Message::DecoderBase
{
public:
    typedef std::map< std::string, Atlas::Message::MapType >::iterator iterator;
    
    XMLFileDecoder(const std::string & sFilename) :
        m_File(sFilename.c_str(), std::ios::in),
        m_Codec(m_File, *this)
    {
    }
    
    void vRead()
    {
        while(!m_File.eof())
        {
            m_Codec.poll();
        }
    }
    
    bool bIsOpen()
    {
        return m_File.is_open();
    }
private:
    std::fstream m_File;
    Atlas::Codecs::XML m_Codec;
    
    virtual void messageArrived(const Atlas::Message::MapType & Object)
    {
        Atlas::Message::MapType::const_iterator iID(Object.find("id"));
        
        if(iID == Object.end())
        {
            std::cerr << " *** equator: object without id attribute." << std::endl;
        }
        insert(std::make_pair(iID->second.asString(), Object));
    }
};

class TypeStore : public Gtk::TreeStore
{
private:
    class TypeStoreColumns : public Gtk::TreeModelColumnRecord
    {
    public:
        Gtk::TreeModelColumn< unsigned int > ID;
        Gtk::TreeModelColumn< Glib::ustring > Name;
        Gtk::TreeModelColumn< Atlas::Message::MapType > Type;
        
        TypeStoreColumns(void)
        {
            add(ID);
            add(Name);
            add(Type);
        }
    };
public:
    static Glib::RefPtr< TypeStore > create(void)
    {
        if(TypeStore::m_pColumns == 0)
        {
            m_pColumns = new(TypeStore::TypeStoreColumns);
        }
        
        return Glib::RefPtr< TypeStore >(new TypeStore());
    }
    
    void vAddType(unsigned int iID, const Atlas::Message::MapType & Type)
    {
        Gtk::TreeRow TypeRow(*append());
        
        TypeRow[Columns.ID] = iID;
        TypeRow[Columns.Name] = Type.find("id")->second.asString();
        TypeRow[Columns.Type] = Type;
    }
    
    TypeStoreColumns & Columns;
private:
    TypeStore(void) :
        Gtk::TreeStore(*m_pColumns),
        Columns(*m_pColumns)
    {
        set_sort_column(Columns.ID, Gtk::SORT_ASCENDING);
    }
    
    static TypeStoreColumns * m_pColumns;
};

TypeStore::TypeStoreColumns * TypeStore::m_pColumns = 0;

ImportTypesWizard::ImportTypesWizard(void) :
    Gtk::Dialog("Import types ...", false, true),
    m_ActionAdd(Gtk::Action::create("Add", Gtk::Stock::GO_FORWARD, "Add", "Adds the selected type onto the server.")),
    m_ActionRemove(Gtk::Action::create("Remove", Gtk::Stock::GO_BACK, "Remove", "Remove the selected type from the server.")),
    m_Actions(Gtk::ActionGroup::create()),
    m_UIManager(Gtk::UIManager::create()),
    m_FileChooserWidget(Gtk::FILE_CHOOSER_ACTION_OPEN),
    m_TypesStore(TypeStore::create()),
    m_ImportTypesStore(TypeStore::create()),
    m_Types(m_TypesStore),
    m_ImportTypes(m_ImportTypesStore)
{
    m_Actions->add(m_ActionAdd, sigc::mem_fun(this, &ImportTypesWizard::vActionAdd));
    m_Actions->add(m_ActionRemove, sigc::mem_fun(this, &ImportTypesWizard::vActionRemove));
    m_UIManager->insert_action_group(m_Actions);
    m_UIManager->add_ui_from_string(g_sUI);
    set_default_size(600, 400);
    get_vbox()->pack_start(m_HBox, true, true, 12);
    m_HBox.pack_start(m_Notebook, true, true, 12);
    m_Types.append_column("Names", m_TypesStore->Columns.Name);
    m_Types.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
    m_Types.signal_row_activated().connect(sigc::hide(sigc::hide(sigc::mem_fun(this, &ImportTypesWizard::vActionAdd))));
    m_ImportTypes.append_column("Name", m_ImportTypesStore->Columns.Name);
    m_ImportTypes.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
    m_ImportTypes.signal_row_activated().connect(sigc::hide(sigc::hide(sigc::mem_fun(this, &ImportTypesWizard::vActionRemove))));
    
    Gtk::HBox * pHBox(manage(new Gtk::HBox(false)));
    Gtk::ScrolledWindow * pScroller1(manage(new Gtk::ScrolledWindow()));
    
    pScroller1->add(m_Types);
    
    Gtk::ScrolledWindow * pScroller2(manage(new Gtk::ScrolledWindow()));
    
    pScroller2->add(m_ImportTypes);
    pHBox->pack_start(*pScroller1, true, true, 0);
    
    Gtk::Toolbar * pToolbar(dynamic_cast< Gtk::Toolbar * >(m_UIManager->get_widget("/ToolBar")));
    
    pToolbar->set_orientation(Gtk::ORIENTATION_VERTICAL);
    pToolbar->set_toolbar_style(Gtk::TOOLBAR_ICONS);
    pHBox->pack_start(*pToolbar, false, false, 8);
    pHBox->pack_end(*pScroller2, true, true, 0);
    m_iPageWithFileChooser = m_Notebook.append_page(m_FileChooserWidget);
    m_iPageWithTypePicker = m_Notebook.append_page(*pHBox);
    m_Notebook.set_show_tabs(false);
    m_Notebook.set_show_border(false);
    m_FileChooserWidget.set_select_multiple(false);
    m_FileChooserWidget.signal_file_activated().connect(sigc::bind(sigc::mem_fun(this, &Gtk::Dialog::response), RESPONSE_NEXT));
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    m_pBackButton = add_button(Gtk::Stock::GO_BACK, RESPONSE_PREVIOUS);
    m_pForwardButton = add_button(Gtk::Stock::GO_FORWARD, RESPONSE_NEXT);
    m_pOKButton = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    get_vbox()->show_all();
    buttons();
}

ImportTypesWizard::~ImportTypesWizard(void)
{
}

void ImportTypesWizard::on_response(int iResponse)
{
    switch(iResponse)
    {
    case RESPONSE_NEXT:
        {
            if(m_Notebook.get_current_page() == m_iPageWithFileChooser)
            {
                if(m_FileChooserWidget.get_filename() == "")
                {
                    break;
                }
                if(m_FileChooserWidget.get_filename() != m_sFileName)
                {
                    XMLFileDecoder Decoder(m_FileChooserWidget.get_filename());
                    
                    if(Decoder.bIsOpen() == false)
                    {
                        std::cout << "Error opening \"" << m_FileChooserWidget.get_filename() << "\"." << std::endl;
                    }
                    Decoder.vRead();
                    // FIXME: some warning or error if reading wasn't successful
                    m_sFileName = m_FileChooserWidget.get_filename();
                    // don't delete content before here since loading may fail
                    m_TypesStore->clear();
                    m_ImportTypesStore->clear();
                    XMLFileDecoder::iterator iType(Decoder.begin());
                    unsigned int iID = 0;
                    
                    while(iType != Decoder.end())
                    {
                        m_TypesStore->vAddType(iID++, iType->second);
                        ++iType;
                    }
                }
            }
            m_Notebook.next_page();
            
            break;
        }
    case RESPONSE_PREVIOUS:
        {
            m_Notebook.prev_page();
            // go back
            
            break;
        }
    default:
        {
            return;
        }
    }
    buttons();
}

void ImportTypesWizard::buttons(void)
{
    if((m_Notebook.get_current_page() + 1 == m_Notebook.get_n_pages()) || (m_Notebook.get_n_pages() == 1))
    {
        m_pBackButton->set_sensitive(true);
        m_pForwardButton->hide();
        m_pOKButton->show();
    }
    else
    {
        m_pBackButton->set_sensitive(false);
        m_pForwardButton->show();
        m_pOKButton->hide();
    }
}

void ImportTypesWizard::vActionAdd(void)
{
    Glib::RefPtr< Gtk::TreeSelection > Selection(m_Types.get_selection());
    std::vector< Gtk::TreePath > SelectedPaths(Selection->get_selected_rows());
    std::vector< Gtk::TreePath >::iterator iPath(SelectedPaths.begin());
    std::vector< Gtk::TreeRowReference > SelectedRows;
    
    while(iPath != SelectedPaths.end())
    {
        Gtk::TreeRow Row(*(m_TypesStore->get_iter(*iPath)));
        
        m_ImportTypesStore->vAddType(Row[m_TypesStore->Columns.ID], Row[m_TypesStore->Columns.Type]);
        SelectedRows.push_back(Gtk::TreeRowReference(m_TypesStore, *iPath));
        ++iPath;
    }
    while(SelectedRows.begin() != SelectedRows.end())
    {
        m_TypesStore->erase(m_TypesStore->get_iter(SelectedRows.back().get_path()));
        SelectedRows.pop_back();
    }
}

void ImportTypesWizard::vActionRemove(void)
{
    Glib::RefPtr< Gtk::TreeSelection > Selection(m_ImportTypes.get_selection());
    std::list< Gtk::TreeModel::Path > SelectedPaths(Selection->get_selected_rows());
    std::list< Gtk::TreeModel::Path >::iterator iPath(SelectedPaths.begin());
    std::vector< Gtk::TreeRowReference > SelectedRows;
    
    while(iPath != SelectedPaths.end())
    {
        Gtk::TreeRow Row(*(m_ImportTypesStore->get_iter(*iPath)));
        
        m_TypesStore->vAddType(Row[m_ImportTypesStore->Columns.ID], Row[m_ImportTypesStore->Columns.Type]);
        SelectedRows.push_back(Gtk::TreeRowReference(m_ImportTypesStore, *iPath));
        ++iPath;
    }
    while(SelectedRows.begin() != SelectedRows.end())
    {
        m_ImportTypesStore->erase(m_ImportTypesStore->get_iter(SelectedRows.back().get_path()));
        SelectedRows.pop_back();
    }
}
