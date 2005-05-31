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
#include <Eris/TypeInfo.h>

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
        Gtk::TreeModelColumn< Gdk::Color > TypeColor;
        
        TypeStoreColumns(void)
        {
            add(ID);
            add(Name);
            add(Type);
            add(TypeColor);
        }
    };
public:
    static Glib::RefPtr< TypeStore > create(void)
    {
        if(TypeStore::m_pColumns == 0)
        {
            m_pColumns = new(TypeStore::TypeStoreColumns);
            m_DisabledColor.set_rgb_p(0.7, 0.7, 0.7);
        }
        
        return Glib::RefPtr< TypeStore >(new TypeStore());
    }
    
    // the following five function add types to the tree but with different properties
    // Maybe someone will find a good way to pack it up into subcalls to decrease redundancy
    void vAdd(unsigned int iID, const Atlas::Message::MapType & Type, Glib::RefPtr< TypeStore > This)
    {
        // exit immediately if this type is already stored here
        if(m_Stored.find(Type.find("id")->second.asString()) != m_Stored.end())
        {
            return;
        }
        
        Gtk::TreeIter Iterator(append());
        Gtk::TreeRow TypeRow(*Iterator);
        
        TypeRow[Columns.ID] = iID;
        TypeRow[Columns.Name] = Type.find("id")->second.asString();
        TypeRow[Columns.Type] = Type;
        m_Stored.insert(std::make_pair(Type.find("id")->second.asString(), Gtk::TreeRowReference(This, get_path(Iterator))));
    }
    
    void vAdd(unsigned int iID, const Atlas::Message::MapType & Type, const std::string & sParent, Glib::RefPtr< TypeStore > This)
    {
        // exit immediately if this type is already stored here
        if(m_Stored.find(Type.find("id")->second.asString()) != m_Stored.end())
        {
            return;
        }
        
        std::map< std::string, Gtk::TreeRowReference >::iterator iParent(m_Stored.find(sParent));
        
        if(iParent == m_Stored.end())
        {
            std::cout << "FIXME: " << __FILE__ << ':' << __LINE__ << std::endl;
            
            throw;
        }
        
        Gtk::TreeIter Iterator(append(get_iter(iParent->second.get_path())->children()));
        Gtk::TreeRow TypeRow(*Iterator);
        
        TypeRow[Columns.ID] = iID;
        TypeRow[Columns.Name] = Type.find("id")->second.asString();
        TypeRow[Columns.Type] = Type;
        m_Stored.insert(std::make_pair(Type.find("id")->second.asString(), Gtk::TreeRowReference(This, get_path(Iterator))));
    }
    
    void vAdd(unsigned int iID, const std::string & sName, const std::string & sParent, Glib::RefPtr< TypeStore > This)
    {
        // exit immediately if this type is already stored here
        if(m_Stored.find(sName) != m_Stored.end())
        {
            return;
        }
        
        std::map< std::string, Gtk::TreeRowReference >::iterator iParent(m_Stored.find(sParent));
        
        if(iParent == m_Stored.end())
        {
            std::cout << "FIXME: " << __FILE__ << ':' << __LINE__ << std::endl;
            
            throw;
        }
        
        Gtk::TreeIter Iterator(append(get_iter(iParent->second.get_path())->children()));
        Gtk::TreeRow TypeRow(*Iterator);
        
        TypeRow[Columns.ID] = iID;
        TypeRow[Columns.Name] = sName;
        TypeRow[Columns.TypeColor] = m_DisabledColor;
        m_Stored.insert(std::make_pair(sName, Gtk::TreeRowReference(This, get_path(Iterator))));
    }
    
    void vAdd(unsigned int iID, const std::string & sName, Glib::RefPtr< TypeStore > This)
    {
        // exit immediately if this type is already stored here
        if(m_Stored.find(sName) != m_Stored.end())
        {
            return;
        }
        
        Gtk::TreeIter Iterator(append());
        Gtk::TreeRow TypeRow(*Iterator);
        
        TypeRow[Columns.ID] = iID;
        TypeRow[Columns.Name] = sName;
        TypeRow[Columns.TypeColor] = m_DisabledColor;
        m_Stored.insert(std::make_pair(sName, Gtk::TreeRowReference(This, get_path(Iterator))));
    }
    
    void vAdd(const Gtk::TreeRow & Row, Glib::RefPtr< TypeStore > This)
    {
        // exit immediately if this type is already stored here
        if(m_Stored.find(static_cast< Glib::ustring >(Row[Columns.Name])) != m_Stored.end())
        {
            return;
        }
        
        Gtk::TreeIter iParent(Row.parent());
        Gtk::TreeIter iIterator;
        
        if(iParent == true)
        {
            Gtk::TreeRow Parent(*iParent);
            std::map< std::string, Gtk::TreeRowReference >::iterator iStored(m_Stored.find(static_cast< Glib::ustring >(Parent[Columns.Name])));
            
            if(iStored == m_Stored.end())
            {
                vAdd(*iParent, This);
                iStored = m_Stored.find(static_cast< Glib::ustring >(Parent[Columns.Name]));
            }
            iIterator = append(get_iter(iStored->second.get_path())->children());
        }
        else
        {
            iIterator = append();
        }
        
        Gtk::TreeRow TypeRow(*iIterator);
        
        TypeRow[Columns.ID] = static_cast< unsigned int >(Row[Columns.ID]);
        TypeRow[Columns.Name] = static_cast< Glib::ustring >(Row[Columns.Name]);
        TypeRow[Columns.TypeColor] = static_cast< Gdk::Color >(Row[Columns.TypeColor]);
        TypeRow[Columns.Type] = static_cast< Atlas::Message::MapType >(Row[Columns.Type]);
        m_Stored.insert(std::make_pair(static_cast< Glib::ustring >(TypeRow[Columns.Name]), Gtk::TreeRowReference(This, get_path(iIterator))));
    }
    
    bool bExists(const std::string & sType)
    {
        return m_Stored.find(sType) != m_Stored.end();
    }
    
    TypeStoreColumns & Columns;
private:
    TypeStore(void) :
        Gtk::TreeStore(*m_pColumns),
        Columns(*m_pColumns)
    {
        set_sort_column(Columns.ID, Gtk::SORT_ASCENDING);
    }
    
    // index for the stored types
    std::map< std::string, Gtk::TreeRowReference > m_Stored;
    
    static TypeStoreColumns * m_pColumns;
    static Gdk::Color m_DisabledColor;
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
            m_UpdatedColor.set_rgb_p(0.7, 0.8, 0.0);
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
Gdk::Color TypeStore::m_DisabledColor;
Gdk::Color UploadStatusStore::m_ImportedColor;
Gdk::Color UploadStatusStore::m_UpdatedColor;
Gdk::Color UploadStatusStore::m_ErrorColor;

class ImportTypesWizard::Uploading
{
public:
    Uploading(void);
    unsigned int m_uiUploadedTypes;
    std::map< std::string, Gtk::TreeRowReference > m_QueuedTypes;
    std::map< std::string, UploadStatusStore::Status > m_TypeOperations;
};

class ImportTypesWizard::Picking : public sigc::trackable
{
public:
    Picking(Server & Server, const Glib::ustring & sFileName, Gtk::ProgressBar & ProgressBar, Glib::RefPtr< TypeStore > TypeStore) :
        m_Decoder(sFileName),
        m_Server(Server),
        m_bSuccessful((sFileName != "") && (m_Decoder.bIsOpen() == true)),
        m_iID(0),
        m_ProgressBar(ProgressBar),
        m_TypesStore(TypeStore),
        m_iAddedTypes(0)
    {
        if(m_bSuccessful == true)
        {
            m_Server.m_connection->getTypeService()->BoundType.connect(sigc::mem_fun(this, &ImportTypesWizard::Picking::vBoundType));
            m_Decoder.vRead();
            m_TypesStore->clear();
            m_ProgressBar.set_fraction(0.0);
            
            std::stringstream ssText;
            
            ssText << "Loading types: 0 / " << m_Decoder.size();
            m_ProgressBar.set_text(ssText.str());
            
            XMLFileDecoder::iterator iType(m_Decoder.begin());
            std::set< std::string > Requests;
            
            while(iType != m_Decoder.end())
            {
                Atlas::Message::MapType::iterator iParents(iType->second.find("parents"));
                
                if((iParents == iType->second.end()) || (iParents->second.isList() == false) || (iParents->second.asList().size() == 0))
                {
                    m_ChildsFromFile.insert(std::make_pair("", iType->first));
                    
                    continue;
                }
                m_ChildsFromFile.insert(std::make_pair(iParents->second.asList()[0].asString(), iType->first));
                m_ParentsFromFile.insert(std::make_pair(iType->first, iParents->second.asList()[0].asString()));
                Requests.insert(iParents->second.asList()[0].asString());
                
                std::set< std::string >::iterator iChild(Requests.find(iType->first));
                
                if(iChild != Requests.end())
                {
                    Requests.erase(iChild);
                }
                ++iType;
            }
            
            std::set< std::string >::iterator iRequest(Requests.begin());
            Eris::TypeInfo * pTypeInfo(0);
            
            while(iRequest != Requests.end())
            {
                pTypeInfo = m_Server.m_connection->getTypeService()->getTypeByName(*iRequest);
                if(pTypeInfo->isBound() == true)
                {
                    bAddWithParents(pTypeInfo);
                }
                ++iRequest;
            }
        }
    }
    
    ~Picking(void)
    {
        std::cout << "Dangeling parents: " << m_ParentsFromFile.size() << std::endl;
        for(std::map< std::string, std::string >::iterator iParent(m_ParentsFromFile.begin()); iParent != m_ParentsFromFile.end(); ++iParent)
        {
            std::cout << "\t" << iParent->first << " => " << iParent->second << std::endl;
        }
        std::cout << "Dangeling childs: " << m_ChildsFromFile.size() << std::endl;
        for(std::map< std::string, std::string >::iterator iParent(m_ChildsFromFile.begin()); iParent != m_ChildsFromFile.end(); ++iParent)
        {
            std::cout << "\t" << iParent->first << " => " << iParent->second << std::endl;
        }
        std::cout << "Saved types: " << m_TypesStore->children().size() << std::endl;
    }
    
    bool bLoadingWasSuccessful(void)
    {
        return m_bSuccessful;
    }
    
    XMLFileDecoder m_Decoder;
private:
    void vAdd(const std::string & sParent, const std::string & sType)
    {
        XMLFileDecoder::iterator iFileType(m_Decoder.find(sType));
        
        if(sParent == "")
        {
            if(iFileType == m_Decoder.end())
            {
                // what we have here is a root type that is already on the server but not in the file
                m_TypesStore->vAdd(m_iID++, sType, m_TypesStore);
            }
            else
            {
                // here we can be sure that the root type is defined in the file
                m_TypesStore->vAdd(m_iID++, m_Decoder.find(sType)->second, m_TypesStore);
                ++m_iAddedTypes;
            }
        }
        else
        {
            if(iFileType == m_Decoder.end())
            {
                // what we have here is a type that is already on the server but not in the file
                m_TypesStore->vAdd(m_iID++, sType, sParent, m_TypesStore);
            }
            else
            {
                // here we can be sure that the type is defined in the file
                m_TypesStore->vAdd(m_iID++, m_Decoder.find(sType)->second, sParent, m_TypesStore);
                ++m_iAddedTypes;
            }
        }
        m_ProgressBar.set_fraction(static_cast< double >(m_iAddedTypes) / m_Decoder.size());
        
        std::stringstream ssText;
        
        ssText << "Loading types: " << m_iAddedTypes << " / " << m_Decoder.size();
        m_ProgressBar.set_text(ssText.str());
    }
    
    // this function is recursive in direction of the root
    bool bAddWithParents(const Eris::TypeInfo * pTypeInfo)
    {
        // this function will add the type identified by pTypeInfo with all its parents to the type store
        assert(pTypeInfo->isBound());
        
        const Eris::TypeInfoSet & Parents(pTypeInfo->getParents());
        
        if(Parents.size() == 0)
        {
            vAdd("", pTypeInfo->getName());
        }
        else
        {
            const Eris::TypeInfo * pParent(*(Parents.begin()));
            
            if(m_ParentsFromFile.count(pTypeInfo->getName()) > 0)
            {
                // this type is from the file and requires a parent type
                // FIXME: what about more than one??
                std::multimap< std::string, std::string >::iterator iParent(m_ParentsFromFile.find(pTypeInfo->getName()));
                const std::string & sParent(iParent->second);
                
                if(pParent->getName() != sParent)
                {
                    // interesting: the file and the server have different information about the parents
                    std::cout << "Type \"" << pTypeInfo->getName() << "\" is specified with the wrong parent \"" << sParent << "\". The server says \"" << pParent->getName() << "\" is the parent." << std::endl;
                    // what we have to do is delete the information from the file and take the server's
                    std::multimap< std::string, std::string >::iterator iLowerChild(m_ChildsFromFile.lower_bound(sParent));
                    std::multimap< std::string, std::string >::iterator iUpperChild(m_ChildsFromFile.upper_bound(sParent));
                    
                    while(iLowerChild != iUpperChild)
                    {
                        if(iLowerChild->second == pTypeInfo->getName())
                        {
                            m_ChildsFromFile.erase(iLowerChild);
                        }
                        ++iLowerChild;
                    }
                    // FIXME: what about multiple parents?
                    m_ParentsFromFile.erase(iParent);
                    m_Decoder.erase(m_Decoder.find(pTypeInfo->getName()));
                    // if not a single type is waiting for this type to be its parent -> we don't need it
                    //   this is OK to be only file types because only those can be leafs in the tree we are building up
                    if(m_ChildsFromFile.count(pTypeInfo->getName()) == 0)
                    {
                        return false;
                    }
                    // inserting the item at the server's position will be done below
                }
            }
            // the type (and all its childs) have been added already
            if(m_TypesStore->bExists(pTypeInfo->getName()) == true)
            {
                return true;
            }
            // if there is no tree row for the parent yet
            if(m_TypesStore->bExists(pParent->getName()) == false)
            {
                if(bAddWithParents(pParent) == false)
                {
                    return false;
                }
            }
            vAdd(pParent->getName(), pTypeInfo->getName());
        }
        // add all types from the file that are waiting for pTypeInfo
        vAddChilds(pTypeInfo->getName());
        
        return true;
    }
    
    // this function is recursive in direction of the leaves
    void vAddChilds(const std::string & sType)
    {
        std::multimap< std::string, std::string >::iterator iLowerChild(m_ChildsFromFile.lower_bound(sType));
        std::multimap< std::string, std::string >::iterator iUpperChild(m_ChildsFromFile.upper_bound(sType));
        
        while(iLowerChild != iUpperChild)
        {
            vAdd(iLowerChild->first, iLowerChild->second);
            vAddChilds(iLowerChild->second);
            m_ParentsFromFile.erase(m_ParentsFromFile.find(iLowerChild->second));
            
            std::multimap< std::string, std::string >::iterator iDeleteChild(iLowerChild);
            
            ++iLowerChild;
            m_ChildsFromFile.erase(iDeleteChild);
        }
    }
    
    void vBoundType(Eris::TypeInfo * pTypeInfo)
    {
        bAddWithParents(pTypeInfo);
    }
    
    Server & m_Server;
    bool m_bSuccessful;
    unsigned int m_iID;
    Gtk::ProgressBar & m_ProgressBar;
    Glib::RefPtr< TypeStore > m_TypesStore;
    unsigned int m_iAddedTypes;
    
    // maps a type's name to the names of its children
    //   only contains information gathered from the file
    std::multimap< std::string, std::string > m_ChildsFromFile;
    
    // maps a type's name to the names of its parent(s)
    //   only contains information gathered from the file
    std::multimap< std::string, std::string > m_ParentsFromFile;
};

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
    m_UploadStatus(m_UploadStatusStore),
    m_pPicking(0),
    m_pUploading(0)
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
        m_Types.append_column("Names", m_TypesRenderer);
        m_Types.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
        m_Types.signal_row_activated().connect(sigc::hide(sigc::hide(sigc::mem_fun(this, &ImportTypesWizard::vActionAdd))));
        m_Types.get_selection()->signal_changed().connect(sigc::bind(sigc::mem_fun(this, &ImportTypesWizard::vSelectionChanged), &m_Types));
        
        Gtk::TreeViewColumn * pTypesColumn(m_Types.get_column(0));
        
        m_TypesRenderer.property_foreground_set() = true;
        pTypesColumn->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
        pTypesColumn->set_fixed_width(100);
        pTypesColumn->add_attribute(m_TypesRenderer.property_text(), m_TypesStore->Columns.Name);
        pTypesColumn->add_attribute(m_TypesRenderer.property_foreground_gdk(), m_TypesStore->Columns.TypeColor);
        m_ImportTypes.append_column("Name", m_ImportTypesRenderer);
        m_ImportTypes.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
        m_ImportTypes.signal_row_activated().connect(sigc::hide(sigc::hide(sigc::mem_fun(this, &ImportTypesWizard::vActionRemove))));
        
        Gtk::TreeViewColumn * pImportTypesColumn(m_ImportTypes.get_column(0));
        
        m_ImportTypesRenderer.property_foreground_set() = true;
        pImportTypesColumn->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
        pImportTypesColumn->set_fixed_width(100);
        pImportTypesColumn->add_attribute(m_ImportTypesRenderer.property_text(), m_ImportTypesStore->Columns.Name);
        pImportTypesColumn->add_attribute(m_ImportTypesRenderer.property_foreground_gdk(), m_ImportTypesStore->Columns.TypeColor);
        
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
        
        Gtk::VBox * pVBox(manage(new Gtk::VBox(false, 8)));
        
        pVBox->pack_start(*pHBox, true, true, 0);
        pVBox->pack_start(m_LoadProgressBar, false, false, 0);
        m_iPageWithTypePicker = m_Notebook.append_page(*pVBox);
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
        pVBox->pack_end(m_UploadProgressBar, false, false, 0);
        m_iPageWithUploadStatus = m_Notebook.append_page(*pVBox);
    }
    get_vbox()->show_all();
    buttons();
}

ImportTypesWizard::~ImportTypesWizard(void)
{
    delete m_pPicking;
    m_pPicking = 0;
    delete m_pUploading;
    m_pUploading = 0;
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
                delete m_pPicking;
                m_pPicking = new ImportTypesWizard::Picking(m_Server, m_FileChooserWidget.get_filename(), m_LoadProgressBar, m_TypesStore);
                if(m_pPicking->bLoadingWasSuccessful() == false)
                {
                    break;
                }
                m_ImportTypesStore->clear();
            }
            m_Notebook.next_page();
            // post-actions
            if(m_Notebook.get_current_page() == m_iPageWithTypePicker)
            {
            }
            else if(m_Notebook.get_current_page() == m_iPageWithUploadStatus)
            {
                delete m_pUploading;
                m_pUploading = new ImportTypesWizard::Uploading();
                
                std::stringstream ssText;
                
                ssText << "Imported Types: 0 / " << m_ImportTypesStore->children().size();
                m_UploadProgressBar.set_text(ssText.str());
                m_Server.m_connection->getTypeService()->BoundType.connect(sigc::mem_fun(this, &ImportTypesWizard::vUpdateType));
                m_Server.m_connection->getTypeService()->BadType.connect(sigc::mem_fun(this, &ImportTypesWizard::vImportType));
                vUploadTypesFromTree(m_ImportTypesStore->children());
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

void ImportTypesWizard::vUploadTypesFromTree(Gtk::TreeNodeChildren Children)
{
    for(Gtk::TreeIter iType(Children.begin()); iType != Children.end(); ++iType)
    {
        Atlas::Message::MapType Type((*iType)[m_ImportTypesStore->Columns.Type]);
        
        // upload the type itself first of all, so we won't have dependency problems
        if(Type.empty() == false)
        {
            Glib::ustring sID((*iType)[m_ImportTypesStore->Columns.Name]);
            
            Eris::TypeInfoPtr TypeInfo(m_Server.m_connection->getTypeService()->getTypeByName(sID));
            
            m_Server.m_connection->getTypeService()->getTypeByName(sID);
            
            Gtk::TreeRowReference RowRef(m_ImportTypesStore, Gtk::TreePath(iType));
            
            m_pUploading->m_QueuedTypes[TypeInfo->getName()] = RowRef;
            if(TypeInfo->isBound() == true)
            {
                vUpdateType(TypeInfo);
            }
        }
        vUploadTypesFromTree(iType->children());
    }
}

void ImportTypesWizard::buttons(void)
{
    if((m_Notebook.get_current_page() + 1 == m_Notebook.get_n_pages()) || (m_Notebook.get_n_pages() == 1))
    {
        m_pForwardButton->set_sensitive(false);
        m_pOKButton->set_sensitive((m_Notebook.get_current_page() != m_iPageWithUploadStatus) || ((m_pUploading != 0) && (m_pUploading->m_uiUploadedTypes == m_ImportTypesStore->children().size())));
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
        
        m_ImportTypesStore->vAdd(Row, m_ImportTypesStore);
        SelectedRows.push_back(Gtk::TreeRowReference(m_TypesStore, *iPath));
        ++iPath;
    }
    while(SelectedRows.begin() != SelectedRows.end())
    {
        if((m_TypesStore->get_iter(SelectedRows.back().get_path()))->children().size() == 0)
        {
            m_TypesStore->erase(m_TypesStore->get_iter(SelectedRows.back().get_path()));
        }
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
        
        m_TypesStore->vAdd(Row, m_TypesStore);
        SelectedRows.push_back(Gtk::TreeRowReference(m_ImportTypesStore, *iPath));
        ++iPath;
    }
    while(SelectedRows.begin() != SelectedRows.end())
    {
        m_ImportTypesStore->erase(m_ImportTypesStore->get_iter(SelectedRows.back().get_path()));
        SelectedRows.pop_back();
    }
}

void ImportTypesWizard::vSelectionChanged(Gtk::TreeView * pTypeView)
{
    std::vector< Gtk::TreePath > SelectedRows(pTypeView->get_selection()->get_selected_rows());
    std::vector< Gtk::TreePath >::iterator iSelectedRow(SelectedRows.begin());
    
    while(iSelectedRow != SelectedRows.end())
    {
        Gtk::TreeRow Row(*(pTypeView->get_model()->get_iter(*iSelectedRow)));
        Glib::RefPtr< TypeStore > Store(Glib::RefPtr< TypeStore >::cast_dynamic(pTypeView->get_model()));
        Atlas::Message::MapType MapType(Row[Store->Columns.Type]);
        
        if(MapType.empty() == true)
        {
            pTypeView->get_selection()->unselect(*iSelectedRow);
        }
        ++iSelectedRow;
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
        
        std::string sID(Arg1Map["args"].asList()[0].asMap()["id"].asString());
        
        m_UploadStatusStore->vAddStatus(sID, UploadStatusStore::STATUS_ERROR, Arg0Map["message"].asString());
        m_pUploading->m_TypeOperations.erase(m_pUploading->m_TypeOperations.find(sID));
    }
    else if(std::find(Parents.begin(), Parents.end(), "info") != Parents.end())
    {
        Atlas::Message::MapType Arg0Map;
        
        Operation->getArgs()[0]->addToMessage(Arg0Map);
        
        std::string sID(Arg0Map["id"].asString());
        std::map< std::string, UploadStatusStore::Status >::iterator iOperation(m_pUploading->m_TypeOperations.find(sID));
        
        m_UploadStatusStore->vAddStatus(sID, iOperation->second);
        m_pUploading->m_TypeOperations.erase(iOperation);
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
    ++m_pUploading->m_uiUploadedTypes;
    
    std::stringstream ssText;
    
    ssText << "Imported Types: " << m_pUploading->m_uiUploadedTypes << " / " << m_ImportTypesStore->children().size();
    m_UploadProgressBar.set_text(ssText.str());
    m_UploadProgressBar.set_fraction(static_cast< double >(m_pUploading->m_uiUploadedTypes) / m_ImportTypesStore->children().size());
    buttons();
}

void ImportTypesWizard::vImportType(Eris::TypeInfo * pType)
{
    std::map< std::string, Gtk::TreeRowReference >::iterator iType(m_pUploading->m_QueuedTypes.find(pType->getName()));
    
    if(iType != m_pUploading->m_QueuedTypes.end())
    {
        Atlas::Objects::Operation::Create Create;
        Atlas::Message::ListType Arguments;
        Atlas::Message::MapType Type((*(m_ImportTypesStore->get_iter(iType->second.get_path())))[m_ImportTypesStore->Columns.Type]);
        
        Type["ruleset"] = "mason";
        Arguments.push_back(Type);
        Create->setArgsAsList(Arguments);
        Create->setFrom(m_Server.m_account->getId());
        
        long lSerialNumber(Eris::getNewSerialno());
        
        Create->setSerialno(lSerialNumber);
        m_Server.m_connection->getResponder()->await(lSerialNumber, this, &ImportTypesWizard::vImportMessage);
        m_Server.m_connection->send(Create);
        m_pUploading->m_TypeOperations[pType->getName()] = UploadStatusStore::STATUS_IMPORT;
        m_pUploading->m_QueuedTypes.erase(iType);
    }
}

void ImportTypesWizard::vUpdateType(Eris::TypeInfo * pType)
{
    std::map< std::string, Gtk::TreeRowReference >::iterator iType(m_pUploading->m_QueuedTypes.find(pType->getName()));
    
    if(iType != m_pUploading->m_QueuedTypes.end())
    {
        Atlas::Objects::Operation::Set Set;
        Atlas::Message::ListType Arguments;
        Atlas::Message::MapType Type((*(m_ImportTypesStore->get_iter(iType->second.get_path())))[m_ImportTypesStore->Columns.Type]);
        
        Type["ruleset"] = "mason";
        Arguments.push_back(Type);
        Set->setArgsAsList(Arguments);
        Set->setFrom(m_Server.m_account->getId());
        
        long lSerialNumber(Eris::getNewSerialno());
        
        Set->setSerialno(lSerialNumber);
        m_Server.m_connection->getResponder()->await(lSerialNumber, this, &ImportTypesWizard::vImportMessage);
        m_Server.m_connection->send(Set);
        m_pUploading->m_TypeOperations[pType->getName()] = UploadStatusStore::STATUS_UPDATE;
        m_pUploading->m_QueuedTypes.erase(iType);
    }
}

ImportTypesWizard::Uploading::Uploading(void) :
    m_uiUploadedTypes(0)
{
}
