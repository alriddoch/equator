#ifndef EQUATOR_GUI_GTKMM_IMPORTTYPESWIZARD_H
#define EQUATOR_GUI_GTKMM_IMPORTTYPESWIZARD_H

#include <gtkmm/dialog.h>
#include <gtkmm/filechooserwidget.h>
#include <gtkmm/notebook.h>
#include <gtkmm/treeview.h>

#include <Atlas/Message/Element.h>

namespace Gtk
{
    class Action;
    class ActionGroup;
    class UIManager;
}

class TypeStore;
class ImportTypesWizard;

class TypeIterator
{
    friend class ImportTypesWizard;
public:
    TypeIterator(const TypeIterator & TypeIterator);
    TypeIterator & operator++(void);
    bool operator!=(const TypeIterator & Other) const;
    Atlas::Message::MapType operator*(void) const;
private:
    TypeIterator(Glib::RefPtr< TypeStore > & Store, const Gtk::TreeIter & TreeIter);
    Glib::RefPtr< TypeStore > & m_Store;
    Gtk::TreeIter m_TreeIter;
};

class ImportTypesWizard : public Gtk::Dialog
{
public:
    typedef TypeIterator iterator;
    
    ImportTypesWizard(void);
    ~ImportTypesWizard(void);
    
    iterator begin(void);
    iterator end(void);
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
