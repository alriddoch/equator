#ifndef EQUATOR_GUI_GTKMM_IMPORTTYPESWIZARD_H
#define EQUATOR_GUI_GTKMM_IMPORTTYPESWIZARD_H

#include <gtkmm/dialog.h>
#include <gtkmm/filechooserwidget.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/notebook.h>
#include <gtkmm/treeview.h>

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/RootOperation.h>

#include "../../app/Server.h"

namespace Gtk
{
    class Action;
    class ActionGroup;
    class UIManager;
}

namespace Eris
{
    class TypeInfo;
}

class TypeStore;
class UploadStatusStore;
class ImportTypesWizard;

class ImportTypesWizard : public Gtk::Dialog
{
public:
    ImportTypesWizard(Server & Server);
    ~ImportTypesWizard(void);
private:
    virtual void on_response(int iResponse);
    void vSelectionChanged(Gtk::TreeView * pTreeView);
    void vImportMessage(const Atlas::Objects::Operation::RootOperation & Operation);
    void vImportType(Eris::TypeInfo * pTypeInfo);
    void vUpdateType(Eris::TypeInfo * pTypeInfo);
    void vUploadTypesFromTree(Gtk::TreeNodeChildren Childs);
    void buttons(void);
    
    void vActionAdd(void);
    void vActionRemove(void);
    
    Server & m_Server;
    Glib::RefPtr< Gtk::Action > m_ActionAdd;
    Glib::RefPtr< Gtk::Action > m_ActionRemove;
    Glib::RefPtr< Gtk::ActionGroup > m_Actions;
    Glib::RefPtr< Gtk::UIManager > m_UIManager;
    Gtk::HBox m_HBox;
    Gtk::Notebook m_Notebook;
    Gtk::FileChooserWidget m_FileChooserWidget;
    Glib::RefPtr< TypeStore > m_TypesStore;
    Glib::RefPtr< TypeStore > m_ImportTypesStore;
    Glib::RefPtr< UploadStatusStore > m_UploadStatusStore;
    Gtk::TreeView m_Types;
    Gtk::TreeView m_ImportTypes;
    Gtk::TreeView m_UploadStatus;
    Gtk::ProgressBar m_UploadProgressBar;
    Gtk::ProgressBar m_LoadProgressBar;
    Gtk::Button * m_pBackButton;
    Gtk::Button * m_pCancelButton;
    Gtk::Button * m_pForwardButton;
    Gtk::Button * m_pOKButton;
    Gtk::CellRendererText m_StatusRenderer;
    Gtk::CellRendererText m_TypesRenderer;
    Gtk::CellRendererText m_ImportTypesRenderer;
    int m_iPageWithFileChooser;
    int m_iPageWithTypePicker;
    int m_iPageWithUploadStatus;
    
    class Picking;
    class Uploading;
    
    Picking * m_pPicking;
    Uploading * m_pUploading;
};

#endif
