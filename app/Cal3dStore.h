// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef EQUATOR_APP_CAL3D_STORE_H
#define EQUATOR_APP_CAL3D_STORE_H

#include "Layer.h"
#include "GlView.h"

#include "visual/Model.h"

class Cal3dStoreOptions;

namespace Gtk {
  class FileChooserDialog;
}

class Cal3dStore : public Layer {
  private:
    GlView::rmode_t m_renderMode;

    Cal3dModel m_cal3dModel;

    int m_selectionList;
    int m_selectionStack;
    int m_lowVertexSelection;
    int m_highVertexSelection;
    bool m_validDrag;

    Cal3dStoreOptions * m_optionsBox;
    // WFMath::Vector<3> m_dragPoint;

    int m_nameCount;
    int m_nameDict;

    void import_response(int, Gtk::FileChooserDialog *);
  public:
    Cal3dStore(Model &);

    void loadModel(const std::string &);
    void loadFile(const std::string &);

    virtual void options();
    virtual void importFile();
    virtual void exportFile();
    virtual void selectInvert();
    virtual void selectAll();
    virtual void selectNone();

    virtual void draw(GlView & view);
    virtual bool animate(GlView & view);
    virtual void select(GlView & view, int x, int y);
    virtual void select(GlView & view, int x, int y, int w, int h);
    virtual void pushSelection();
    virtual void popSelection();
    virtual void dragStart(GlView & view, int x, int y);
    virtual void dragUpdate(GlView & view, const WFMath::Vector<3> &);
    virtual void dragEnd(GlView & view, const WFMath::Vector<3> &);
    virtual void insert(const PosType &);
    virtual void align(Alignment );
};

#endif // EQUATOR_APP_CAL3D_STORE_H
