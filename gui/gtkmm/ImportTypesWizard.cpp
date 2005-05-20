#include "ImportTypesWizard.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include <gdkmm/color.h>

#include <gtkmm/action.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/treerowreference.h>
#include <gtkmm/treestore.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/uimanager.h>

#include <Atlas/Codecs/Bach.h>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation.h>

#include <Eris/Account.h>
#include <Eris/Connection.h>
#include <Eris/Response.h>

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

class UploadStatusStore : public Gtk::TreeStore
{
private:
    class UploadStatusStoreColumns : public Gtk::TreeModelColumnRecord
    {
    public:
        Gtk::TreeModelColumn< Glib::ustring > Name;
        Gtk::TreeModelColumn< Glib::ustring > Status;
        Gtk::TreeModelColumn< Gdk::Color > StatusColor;
        
        UploadStatusStoreColumns(void)
        {
            add(Name);
            add(Status);
            add(StatusColor);
        }
    };
public:
    enum Status
    {
        STATUS_IMPORT,
        STATUS_UPDATE,
        STATUS_ERROR
    };
    
    static Glib::RefPtr< UploadStatusStore > create(void)
    {
        if(UploadStatusStore::m_pColumns == 0)
        {
            m_pColumns = new(UploadStatusStore::UploadStatusStoreColumns);
            m_ImportedColor.set_rgb_p(0.0, 0.8, 0.0);
            m_UpdatedColor.set_rgb_p(0.5, 1.0, 0.0);
            m_ErrorColor.set_rgb_p(0.8, 0.0, 0.0);
        }
        
        return Glib::RefPtr< UploadStatusStore >(new UploadStatusStore());
    }
    
    void vAddStatus(const Glib::ustring & sName, Status Status, const Glib::ustring & sMessage = "")
    {
        Gtk::TreeRow TypeRow(*append());
        
        TypeRow[Columns.Name] = sName;
        switch(Status)
        {
        case STATUS_IMPORT:
            {
                TypeRow[Columns.Status] = "Imported";
                TypeRow[Columns.StatusColor] = m_ImportedColor;
                
                break;
            }
        case STATUS_UPDATE:
            {
                TypeRow[Columns.Status] = "Updated";
                TypeRow[Columns.StatusColor] = m_UpdatedColor;
                
                break;
            }
        case STATUS_ERROR:
            {
                TypeRow[Columns.Status] = "Error";
                TypeRow[Columns.StatusColor] = m_ErrorColor;
                if(sMessage != "")
                {
                    Gtk::TreeRow MessageRow(*append(TypeRow.children()));
                    
                    MessageRow[Columns.Name] = sMessage;
                }
                
                break;
            }
        }
    }
    
    UploadStatusStoreColumns & Columns;
private:
    UploadStatusStore(void) :
        Gtk::TreeStore(*m_pColumns),
        Columns(*m_pColumns)
    {
    }
    
    static UploadStatusStoreColumns * m_pColumns;
    static Gdk::Color m_ImportedColor;
    static Gdk::Color m_UpdatedColor;
    static Gdk::Color m_ErrorColor;
};

TypeStore::TypeStoreColumns * TypeStore::m_pColumns = 0;
UploadStatusStore::UploadStatusStoreColumns * UploadStatusStore::m_pColumns = 0;
Gdk::Color UploadStatusStore::m_ImportedColor;
Gdk::Color UploadStatusStore::m_UpdatedColor;
Gdk::Color UploadStatusStore::m_ErrorColor;

ImportTypesWizard::ImportTypesWizard(Server & Server) :
    Gtk::Dialog("Import types ...", false, true),
    m_Server(Server),
    m_ActionAdd(Gtk::Action::create("Add", Gtk::Stock::GO_FORWARD, "Add", "Adds the selected type onto the server.")),
    m_ActionRemove(Gtk::Action::create("Remove", Gtk::Stock::GO_BACK, "Remove", "Remove the selected type from the server.")),
    m_Actions(Gtk::ActionGroup::create()),
    m_UIManager(Gtk::UIManager::create()),
    m_FileChooserWidget(Gtk::FILE_CHOOSER_ACTION_OPEN),
    m_TypesStore(TypeStore::create()),
    m_ImportTypesStore(TypeStore::create()),
    m_UploadStatusStore(UploadStatusStore::create()),
    m_Types(m_TypesStore),
    m_ImportTypes(m_ImportTypesStore),
    m_UploadStatus(m_UploadStatusStore)
{
    m_Actions->add(m_ActionAdd, sigc::mem_fun(this, &ImportTypesWizard::vActionAdd));
    m_Actions->add(m_ActionRemove, sigc::mem_fun(this, &ImportTypesWizard::vActionRemove));
    m_UIManager->insert_action_group(m_Actions);
    m_UIManager->add_ui_from_string(g_sUI);
    set_default_size(600, 400);
    get_vbox()->pack_start(m_HBox, true, true, 12);
    m_HBox.pack_start(m_Notebook, true, true, 12);
    m_Notebook.set_show_tabs(false);
    m_Notebook.set_show_border(false);
    m_pCancelButton = add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    m_pBackButton = add_button(Gtk::Stock::GO_BACK, RESPONSE_PREVIOUS);
    m_pForwardButton = add_button(Gtk::Stock::GO_FORWARD, RESPONSE_NEXT);
    m_pOKButton = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    {
        // page with file chooser widget
        m_iPageWithFileChooser = m_Notebook.append_page(m_FileChooserWidget);
        m_FileChooserWidget.set_select_multiple(false);
        m_FileChooserWidget.signal_file_activated().connect(sigc::bind(sigc::mem_fun(this, &Gtk::Dialog::response), RESPONSE_NEXT));
    }
    {
        // page with two tree views and a tollbar in between
        m_Types.append_column("Names", m_TypesStore->Columns.Name);
        m_Types.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
        m_Types.signal_row_activated().connect(sigc::hide(sigc::hide(sigc::mem_fun(this, &ImportTypesWizard::vActionAdd))));
        m_ImportTypes.append_column("Name", m_ImportTypesStore->Columns.Name);
        m_ImportTypes.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
        m_ImportTypes.signal_row_activated().connect(sigc::hide(sigc::hide(sigc::mem_fun(this, &ImportTypesWizard::vActionRemove))));
        
        Gtk::HBox * pHBox(manage(new Gtk::HBox(false, 8)));
        Gtk::ScrolledWindow * pScroller1(manage(new Gtk::ScrolledWindow()));
        
        pScroller1->add(m_Types);
        
        Gtk::ScrolledWindow * pScroller2(manage(new Gtk::ScrolledWindow()));
        
        pScroller2->add(m_ImportTypes);
        pHBox->pack_start(*pScroller1, true, true, 0);
        
        Gtk::Toolbar * pToolbar(dynamic_cast< Gtk::Toolbar * >(m_UIManager->get_widget("/ToolBar")));
        
        pToolbar->set_orientation(Gtk::ORIENTATION_VERTICAL);
        pToolbar->set_toolbar_style(Gtk::TOOLBAR_ICONS);
        pHBox->pack_start(*pToolbar, false, false, 0);
        pHBox->pack_end(*pScroller2, true, true, 0);
        m_iPageWithTypePicker = m_Notebook.append_page(*pHBox);
    }
    {
        // page with progressbar and upload status display
        Gtk::VBox * pVBox(manage(new Gtk::VBox(false, 8)));
        Gtk::ScrolledWindow * pScroller(manage(new Gtk::ScrolledWindow()));
        
        m_UploadStatus.append_column("Name", m_UploadStatusStore->Columns.Name);
        
        Gtk::TreeViewColumn * pNameColumn(m_UploadStatus.get_column(0));
        
        pNameColumn->set_expand(true);
        m_UploadStatus.append_column("Status", m_StatusRenderer);
        
        Gtk::TreeViewColumn * pStatusColumn(m_UploadStatus.get_column(1));
        
        m_StatusRenderer.property_foreground_set() = true;
        pStatusColumn->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
        pStatusColumn->set_fixed_width(100);
        pStatusColumn->add_attribute(m_StatusRenderer.property_text(), m_UploadStatusStore->Columns.Status);
        pStatusColumn->add_attribute(m_StatusRenderer.property_foreground_gdk(), m_UploadStatusStore->Columns.StatusColor);
        pScroller->add(m_UploadStatus);
        pVBox->pack_start(*pScroller, true, true, 0);
        pVBox->pack_end(m_ProgressBar, false, false, 0);
        m_iPageWithUploadStatus = m_Notebook.append_page(*pVBox);
    }
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
            // pre-actions
            if(m_Notebook.get_current_page() == m_iPageWithFileChooser)
            {
                if(bLoadFile(m_FileChooserWidget.get_filename()) == false)
                {
                    break;
                }
            }
            m_Notebook.next_page();
            // post-actions
            if(m_Notebook.get_current_page() == m_iPageWithUploadStatus)
            {
                std::stringstream ssText;
                
                m_uiUploadedTypes = 0;
                ssText << "Imported Types: 0 / " << m_ImportTypesStore->children().size();
                m_ProgressBar.set_text(ssText.str());
                
                Gtk::TreeIter iType(m_ImportTypesStore->children().begin());
                
                while(iType != m_ImportTypesStore->children().end())
                {
                    Atlas::Objects::Operation::Create Create;
                    Atlas::Message::ListType Arguments;
                    Atlas::Message::MapType Type((*iType)[m_ImportTypesStore->Columns.Type]);
                    
                    Type["ruleset"] = "mason";
                    Arguments.push_back(Type);
                    Create->setArgsAsList(Arguments);
                    Create->setFrom(m_Server.m_account->getId());
                    
                    long lSerialNumber(Eris::getNewSerialno());
                    
                    Create->setSerialno(lSerialNumber);
                    m_Server.m_connection->getResponder()->await(lSerialNumber, this, &ImportTypesWizard::vImportMessage);
                    m_Server.m_connection->send(Create);
                    ++iType;
                }
            }
            
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

bool ImportTypesWizard::bLoadFile(const Glib::ustring & sFileName)
{
    if(sFileName == "")
    {
        return false;
    }
    
    XMLFileDecoder Decoder(sFileName);
    
    if(Decoder.bIsOpen() == false)
    {
        std::cout << "Error opening \"" << sFileName << "\"." << std::endl;
        
        return false;
    }
    Decoder.vRead();
    // FIXME: some warning or error if reading wasn't successful
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
    
    return true;
}

void ImportTypesWizard::buttons(void)
{
    if((m_Notebook.get_current_page() + 1 == m_Notebook.get_n_pages()) || (m_Notebook.get_n_pages() == 1))
    {
        m_pForwardButton->set_sensitive(false);
        m_pOKButton->set_sensitive((m_Notebook.get_current_page() != m_iPageWithUploadStatus) || (m_uiUploadedTypes == m_ImportTypesStore->children().size()));
    }
    else
    {
        m_pForwardButton->set_sensitive(true);
        m_pOKButton->set_sensitive(false);
    }
    if((m_Notebook.get_current_page() == 0) || (m_Notebook.get_current_page() == -1) || (m_Notebook.get_current_page() == m_iPageWithUploadStatus))
    {
        m_pBackButton->set_sensitive(false);
    }
    else
    {
        m_pBackButton->set_sensitive(true);
    }
    m_pCancelButton->set_sensitive(!m_pOKButton->is_sensitive());
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

void ImportTypesWizard::vImportMessage(const Atlas::Objects::Operation::RootOperation & Operation)
{
    const std::list< std::string > & Parents(Operation->getParents());
    
    if(std::find(Parents.begin(), Parents.end(), "error") != Parents.end())
    {
        Atlas::Message::MapType Arg0Map;
        Atlas::Message::MapType Arg1Map;
        
        Operation->getArgs()[0]->addToMessage(Arg0Map);
        Operation->getArgs()[1]->addToMessage(Arg1Map);
        m_UploadStatusStore->vAddStatus(Arg1Map["args"].asList()[0].asMap()["id"].asString(), UploadStatusStore::STATUS_ERROR, Arg0Map["message"].asString());
    }
    else if(std::find(Parents.begin(), Parents.end(), "info") != Parents.end())
    {
        Atlas::Message::MapType Arg0Map;
        
        Operation->getArgs()[0]->addToMessage(Arg0Map);
        m_UploadStatusStore->vAddStatus(Arg0Map["id"].asString(), UploadStatusStore::STATUS_IMPORT);
    }
    else
    {
        std::stringstream debugStream;
        Atlas::Codecs::Bach debugCodec(debugStream, *(m_Server.m_connection));
        Atlas::Objects::ObjectsEncoder debugEncoder(debugCodec);
        
        debugEncoder.streamObjectsMessage(Operation);
        std::cout << "Got info:" << debugStream.str() << std::endl;
        
        return;
    }
    ++m_uiUploadedTypes;
    
    std::stringstream ssText;
    
    ssText << "Imported Types: " << m_uiUploadedTypes << " / " << m_ImportTypesStore->children().size();
    m_ProgressBar.set_text(ssText.str());
    m_ProgressBar.set_fraction(static_cast< double >(m_uiUploadedTypes) / m_ImportTypesStore->children().size());
    buttons();
}
