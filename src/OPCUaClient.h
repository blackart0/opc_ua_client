/////////////////////////////////////////////////////////////////////////////
// Name:        OPCUaClient.h
// Purpose:     OPCUaClient
// Author:      xcz
// Modified by:
// Copyright:   (c) Wind
// Licence:     MIT licence
/////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _OPCUaClient_H_
#define _OPCUaClient_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/event.h>
#include <wx/image.h>
#include <wx/graphics.h>
#include <wx/grid.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>

#include "open62541.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame();
    ~MyFrame();
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnBtnConnect(wxCommandEvent& event);
    void OnBtnBrowse(wxCommandEvent& event);
    void OnBtnGetValue(wxCommandEvent& event);
    void OnTreeSelChanged(wxTreeEvent& event);

    void Browse_nodes(UA_Client* client, UA_NodeId nodeId, wxTreeItemId itemId);
    void TreeAddNodes();
    UA_Client* m_uaClient = nullptr;
    int m_iFontSize = 9;
    wxString m_sHost = "";
    wxString m_sPort = "";
    wxString m_sFontName = L"微软雅黑";
    wxLog* m_log = nullptr;
    wxFont* m_Font = nullptr;
    wxGrid* m_gridObjs = nullptr;
    wxGrid* m_gridItems = nullptr;
    wxButton* m_btnConnect = nullptr;
    wxButton* m_btnBrowse = nullptr;
    wxButton* m_btnGetValue = nullptr;
    wxTextCtrl* m_text = nullptr;
    wxTextCtrl* m_textHost = nullptr;
    wxTextCtrl* m_textPort = nullptr;
    wxTreeCtrl* m_treeCtrl = nullptr;
};

enum enum_EVENTID
{
    ID_Menu_Hello = 1,
    ID_Btn_Connect,
    ID_Btn_Disconnect,
    ID_Btn_Browse,
    ID_Btn_GetValue,
};

#endif