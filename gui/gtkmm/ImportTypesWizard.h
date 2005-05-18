#ifndef EQUATOR_GUI_GTKMM_IMPORTTYPESWIZARD_H
#define EQUATOR_GUI_GTKMM_IMPORTTYPESWIZARD_H

#include <gtkmm/dialog.h>
#include <gtkmm/filechooserwidget.h>
#include <gtkmm/notebook.h>
#include <gtkmm/treeview.h>

namespace Gtk
{
    class Action;
    class ActionGroup;
    class UIManager;
}

class TypeStore;

class ImportTypesWizard : public Gtk::Dialog
{
public:
    ImportTypesWizard(void);
    ~ImportTypesWizard(void);
private:
    virtual void on_response(int iResponse);
    void buttons(void);
    
    void vActionAdd(void);
    void vActionRemove(void);
    
    Glib::RefPtr< Gtk::Action > m_ActionAdd;
    Glib::RefPtr< Gtk::Action > m_ActionRemove;
    Glib::RefPtr< Gtk::ActionGroup > m_Actions;
    Glib::RefPtr< Gtk::UIManager > m_UIManager;
    Gtk::HBox m_HBox;
    Gtk::Notebook m_Notebook;
    Gtk::FileChooserWidget m_FileChooserWidget;
    Glib::RefPtr< TypeStore > m_TypesStore;
    Glib::RefPtr< TypeStore > m_ImportTypesStore;
    Gtk::TreeView m_Types;
    Gtk::TreeView m_ImportTypes;
    Glib::ustring m_sFileName;
    Gtk::Button * m_pBackButton;
    Gtk::Button * m_pForwardButton;
    Gtk::Button * m_pOKButton;
    int m_iPageWithFileChooser;
    int m_iPageWithTypePicker;
};

#endif
