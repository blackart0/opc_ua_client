/////////////////////////////////////////////////////////////////////////////
// Name:        OPCUaClient.cpp
// Purpose:     OPCUaClient
// Author:      xcz
// Modified by:
// Copyright:   (c) Wind
// Licence:     MIT licence
/////////////////////////////////////////////////////////////////////////////
#include "OPCUaClient.h"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	wxImage::AddHandler(new wxPNGHandler);
	MyFrame* frame = new MyFrame();
	frame->Show(true);
	return true;
}

MyFrame::~MyFrame()
{
	if (m_Font) {
		delete m_Font;
	}
	if (m_uaClient) {
		UA_Client_disconnect(m_uaClient);
		UA_Client_delete(m_uaClient);
	}
}

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, L"KEPServerEX6Client", wxDefaultPosition, wxSize(800, 600))
{
	SetIcon(wxICON(sample));
	m_text = new wxTextCtrl(this, wxID_ANY, L"KEPServerEX6Client \n", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
	m_text->SetMinSize(wxSize(300, 100));
	delete wxLog::SetActiveTarget(new wxLogTextCtrl(m_text));
	m_Font = new wxFont(m_iFontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, m_sFontName);

	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_Menu_Hello, L"&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, L"&File");
	menuBar->Append(menuHelp, L"&Help");
	SetMenuBar(menuBar);

	CreateStatusBar(4);
	SetStatusText(wxT("Welcome to KEPServerEX6Client!"), 0);
	SetStatusText(wxString("power by Open62541: ") + UA_OPEN62541_VERSION, 1);

	wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY);
	notebook->SetFont(*m_Font);
	topsizer->Add(notebook, 1, wxEXPAND | wxALL, 5);

	wxButton* btnExit = new wxButton(this, wxID_EXIT, L"E&xit");
	btnExit->SetFont(*m_Font);
	wxBoxSizer* bSizer2 = new wxBoxSizer(wxHORIZONTAL);
	bSizer2->Add(btnExit, 0, wxRIGHT, 5);

	topsizer->Add(bSizer2, 0, wxALIGN_RIGHT | wxALL, 5);
	topsizer->Add(m_text, 0, wxEXPAND | wxALL, 5);

	// First page: -----------------------------------------------------------------------------
	wxPanel* panel1 = new wxPanel(notebook, wxID_ANY);
	panel1->SetMinSize(wxSize(1024, 400));
	notebook->AddPage(panel1, L"OPC UA");
	wxSizer* panelsizer1 = new wxBoxSizer(wxVERTICAL);
	//* definition of IP Addresses text validator
	wxString ipAddressFilter[11] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "." }; // authorized characters for IP Address
	wxArrayString arraystrIPAddress(11, ipAddressFilter);
	wxTextValidator txtvldIPAddress(wxFILTER_INCLUDE_CHAR_LIST); // text validator for IP Address
	txtvldIPAddress.SetIncludes(arraystrIPAddress); // sets authorized characters for IP Address
	// check of IP address
	// regular expression for IP address (4 [0-255] decimal numbers separated by periods)
	// [0-9]{1} : exactly 1 decimal digit ([0-9])
	// |[0-9]{2} : ...or exactly 2 decimal digits ([10-99])
	// |[0-1][0-9]{2} : ...or '0'/'1' followed by exactly 2 decimal digits ([000-199])
	// |2[0-4][0-9] : ...or 2 followed by '0'/'1'/'2'/'3'/'4' followed by 1 decimal digit ([200-249])
	// |25[0-5] : ...or '25' followed by '0'/'1'/'2'/'3'/'4'/'5' ([250-255])
	// (([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])\\.) : ...the whole followed by '.' (quoted)
	// (([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3} : ...the whole exactly 3 times
	// ^(([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3} : ...positioned at start
	// ([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5]) : [0-9] + [10-99] + [000-199] + [200-249] + [250-255]
	// ([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])$ : ...positioned at the end
	// wxRegEx regxIPAddr("^(([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])$");
	// if (!regxIPAddr.Matches(ipAddress)) {
	//    ...
	// }

	//* drawing of GUI
	m_textHost = new wxTextCtrl(panel1, wxID_ANY, L"127.0.0.1", wxDefaultPosition, wxSize(150, wxDefaultCoord), wxTE_CENTRE, txtvldIPAddress);
	m_textHost->SetToolTip(L"Four [0-255] decimal numbers separated by periods\nFormat: A.B.C.D\nEx.: 192.168.1.1");
	m_textPort = new wxTextCtrl(panel1, wxID_ANY, L"49320", wxDefaultPosition, wxSize(80, wxDefaultCoord), wxTE_CENTRE, txtvldIPAddress);
	m_btnConnect = new wxButton(panel1, ID_Btn_Connect, L"Connect");
	m_btnBrowse = new wxButton(panel1, ID_Btn_Browse, L"Browse");
	m_btnGetValue = new wxButton(panel1, ID_Btn_GetValue, L"GetValue");
	m_btnBrowse->Enable(false);
	m_btnGetValue->Enable(false);
	m_textHost->SetFont(*m_Font);
	m_textPort->SetFont(*m_Font);
	m_btnConnect->SetFont(*m_Font);
	m_btnBrowse->SetFont(*m_Font);
	m_btnGetValue->SetFont(*m_Font);

	wxSizer* sizerRow = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* stServer = new wxStaticText(panel1, wxID_ANY, L"Server: ");
	wxStaticText* stPort = new wxStaticText(panel1, wxID_ANY, L"Port: ");
	stServer->SetFont(*m_Font);
	stPort->SetFont(*m_Font);
	sizerRow->Add(stServer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 5);
	sizerRow->Add(m_textHost, 0, wxALIGN_CENTRE_VERTICAL | wxLEFT, 0);
	sizerRow->Add(stPort, 0, wxALIGN_CENTRE_VERTICAL | wxLEFT, 10);
	sizerRow->Add(m_textPort, 0, wxALIGN_CENTRE_VERTICAL | wxLEFT, 5);
	sizerRow->Add(m_btnConnect, 0, wxALIGN_CENTRE_VERTICAL | wxLEFT, 20);
	sizerRow->Add(m_btnBrowse, 0, wxALIGN_CENTRE_VERTICAL | wxLEFT, 10);
	sizerRow->Add(m_btnGetValue, 0, wxALIGN_CENTRE_VERTICAL | wxLEFT, 10);

	wxSizer* sizerRow1 = new wxBoxSizer(wxHORIZONTAL);
	//m_gridObjs
	m_gridObjs = new wxGrid(panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_gridObjs->SetFont(*m_Font);
	m_gridObjs->SetDefaultCellFont(*m_Font);
	m_gridObjs->CreateGrid(30, 20);
	m_gridObjs->SetMinSize(wxSize(280, 520));
	m_gridObjs->SetRowLabelSize(50);
	m_gridObjs->SetLabelBackgroundColour(wxColour(0xe9e4c1));
	m_gridObjs->EnableEditing(false);

	m_treeCtrl = new wxTreeCtrl(panel1, wxID_ANY, wxDefaultPosition, wxSize(200, -1), wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
	m_treeCtrl->SetFont(*m_Font);

	sizerRow1->Add(m_treeCtrl, 1, wxEXPAND | wxALL, 10);
	sizerRow1->Add(m_gridObjs, 0, wxEXPAND | wxALL, 10);
	panelsizer1->Add(sizerRow1, 1, wxEXPAND | wxALL, 10);
	panelsizer1->Add(sizerRow, 0, wxEXPAND | wxALL, 10);
	panel1->SetSizer(panelsizer1);

	// Second page: a text ctrl and a button
	wxPanel* panel2 = new wxPanel(notebook, wxID_ANY);
	notebook->AddPage(panel2, L"OPC DA");
	wxSizer* panelsizer2 = new wxBoxSizer(wxVERTICAL);
	panelsizer2->Add(new wxStaticText(panel2, wxID_ANY, L"开发中 ... ..."), 0, wxALIGN_CENTRE | wxLEFT, 10);
	//wxTextCtrl* text = new wxTextCtrl(panel, wxID_ANY, "TextLine 1.", wxDefaultPosition, wxSize(250, wxDefaultCoord));
	//panelsizer->Add(text, 0, wxEXPAND | wxALL, 10);
	//text = new wxTextCtrl(panel, wxID_ANY, "TextLine 2.", wxDefaultPosition, wxSize(250, wxDefaultCoord));
	//panelsizer->Add(text, 0, wxEXPAND | wxALL, 10);
	//wxButton* button2 = new wxButton(panel, wxID_ANY, "Hallo");
	//panelsizer2->Add(button2, 0, wxEXPAND | wxALL, 0);
	panel2->SetSizer(panelsizer2);

	SetSizerAndFit(topsizer);
	Layout();
	Center();

	Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Menu_Hello);
	Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

	Bind(wxEVT_BUTTON, &MyFrame::OnBtnConnect, this, ID_Btn_Connect);
	Bind(wxEVT_BUTTON, &MyFrame::OnBtnBrowse, this, ID_Btn_Browse);
	Bind(wxEVT_BUTTON, &MyFrame::OnBtnGetValue, this, ID_Btn_GetValue);
	Bind(wxEVT_BUTTON, &MyFrame::OnExit, this, wxID_EXIT);

	m_treeCtrl->Bind(wxEVT_TREE_SEL_CHANGED, &MyFrame::OnTreeSelChanged, this);
}

void MyFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(L"This is a OPC UA Client.", L"About OPCUaClient", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage(L"This is a OPC UA Client!");
}

void MyFrame::OnBtnConnect(wxCommandEvent& event)
{
	if (m_uaClient) {
		m_treeCtrl->DeleteAllItems();
		m_btnBrowse->Enable(false);
		m_btnGetValue->Enable(false);
		UA_Client_delete(m_uaClient); /* Disconnects the client internally */
	}
	m_uaClient = UA_Client_new();
	UA_ClientConfig_setDefault(UA_Client_getConfig(m_uaClient));

	m_sHost = m_textHost->GetValue();
	m_sPort = m_textPort->GetValue();
	wxString url = wxString::Format(L"opc.tcp://%s:%s", m_sHost, m_sPort);
	UA_StatusCode status = UA_Client_connect(m_uaClient, url.c_str());
	if (status != UA_STATUSCODE_GOOD) {
		//UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "The connection failed with status code %s", UA_StatusCode_name(status));
		wxLogMessage(wxString::Format(L"UA_Client_connect opc.tcp://%s:%s failed with status code %s", m_sHost, m_sPort, UA_StatusCode_name(status)));
		UA_Client_delete(m_uaClient);
		return;
	}
	else {
		GetServerName();
		//m_treeCtrl->AddRoot("OBJECTS");
		m_btnBrowse->Enable();
		m_btnGetValue->Enable();
		wxLogMessage(wxString::Format(L"UA_Client_connect opc.tcp://%s:%s successed!", m_sHost, m_sPort));
	}
}

void MyFrame::OnBtnBrowse(wxCommandEvent& event)
{
	wxTreeItemId rootId = m_treeCtrl->GetRootItem();
	if (rootId.IsOk()) {
		int childCount = m_treeCtrl->GetChildrenCount(rootId);
		if (childCount > 0) {
			int ret = wxMessageBox(L"是否重新获取数据？", L"刷新数据请求", wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
			if (ret == wxNO) {
				return;
			}
			else {
				m_treeCtrl->DeleteChildren(rootId);
			}
		}
		Browse_nodes(m_uaClient, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), rootId);
		m_treeCtrl->Expand(rootId);
	}
}
void MyFrame::Browse_nodes(UA_Client* client, UA_NodeId nodeId, wxTreeItemId itemId)
{
	//UA_Client_Service_browse + UA_Client_readDataTypeAttribute + UA_Client_readValueAttribute，
	//	根据nodeclass区分类别，如果是UA_NODECLASS_OBJECT、UA_NODECLASS_VARIABLETYPE则递归调用，
	//	如果是UA_NODECLASS_VARIABLE则获取数据类型与值，接口封装为递归接口。可遍历所有节点。 
	//	具体处理需要依据实际需要进行调整不同的类型。
	UA_BrowseRequest bReq;
	UA_BrowseRequest_init(&bReq); // 初始化bReq
	bReq.requestedMaxReferencesPerNode = 0; // 限制查到的最大节点数，0 不限制
	//UA_BROWSEDIRECTION_FORWARD表示向下查找（即查找添加在节点下的节点），
	//UA_BROWSEDIRECTION_INVERSE表示向上查找（即查找节点的父节点），
	//UA_BROWSEDIRECTION_BOTH表示上下都进行查找
	bReq.nodesToBrowse = UA_BrowseDescription_new(); // 分配内存，并把地址赋给bReq.nodesToBrowse
	bReq.nodesToBrowse[0].browseDirection = UA_BROWSEDIRECTION_FORWARD;
	bReq.nodesToBrowse[0].includeSubtypes = UA_TRUE;//是否包含subtypes

	bReq.nodesToBrowseSize = 1; // 遍历的起始节点数量
	bReq.nodesToBrowse[0].nodeId = nodeId; // 遍历 nodeId 下的目录
	bReq.nodesToBrowse[0].nodeClassMask = UA_NODECLASS_OBJECT | UA_NODECLASS_VARIABLE | UA_NODECLASS_METHOD; // 只查看对象，变量和方法节点
	bReq.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL; //返回浏览到的节点包含的信息，名称、显示名称......，UA_BROWSERESULTMASK_ALL表示返回所有信息


	int rows = m_gridObjs->GetNumberRows();
	int cols = m_gridObjs->GetNumberCols();
	if (rows > 0) {
		m_gridObjs->DeleteRows(0, rows);
	}
	if (cols > 0) {
		m_gridObjs->DeleteCols(0, cols);
	}

	UA_BrowseResponse bResp = UA_Client_Service_browse(m_uaClient, bReq); // 开始遍历
	if (bResp.responseHeader.serviceResult == UA_STATUSCODE_GOOD) // 判断遍历结果
	{
		m_gridObjs->AppendCols(3);
		m_gridObjs->SetColLabelValue(0, L"Index");
		m_gridObjs->SetColLabelValue(1, L"nodeID");
		m_gridObjs->SetColLabelValue(2, L"BROWSE NAME");
		m_gridObjs->SetColSize(0, 30);
		m_gridObjs->SetColSize(1, 110);
		m_gridObjs->SetColSize(2, 80);
		//m_gridObjs->HideCol(1);
		// 打印遍历结果

		char s1[1024] = { "\0" };
		char s2[1024] = { "\0" };
		for (size_t i = 0; i < bResp.resultsSize; ++i) {
			for (size_t j = 0; j < bResp.results[i].referencesSize; ++j) {
				UA_ReferenceDescription* ref = &(bResp.results[i].references[j]);
				m_gridObjs->AppendRows();
				if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_NUMERIC) {
					memset(s1, 0, 1024);
					memcpy_s(s1, 1024, ref->browseName.name.data, (int)ref->browseName.name.length);
					m_gridObjs->SetCellValue(j, 0, wxString::Format(L"%d", (int)ref->nodeId.nodeId.namespaceIndex));
					m_gridObjs->SetCellValue(j, 1, wxString::Format(L"%d", (int)ref->nodeId.nodeId.identifier.numeric));
					m_gridObjs->SetCellValue(j, 2, wxString::FromUTF8(s1));
					wxTreeItemId childNode1 = m_treeCtrl->AppendItem(itemId, wxString::Format(L"%d", (int)ref->nodeId.nodeId.identifier.numeric));
				}
				else if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_STRING) {
					memset(s1, 0, 1024);
					memset(s2, 0, 1024);
					memcpy_s(s1, 1024, ref->nodeId.nodeId.identifier.string.data, (int)ref->nodeId.nodeId.identifier.string.length);
					memcpy_s(s2, 1024, ref->browseName.name.data, (int)ref->browseName.name.length);

					m_gridObjs->SetCellValue(j, 0, wxString::Format(L"%d", ref->nodeId.nodeId.namespaceIndex));
					m_gridObjs->SetCellValue(j, 1, wxString::FromUTF8(s1));
					m_gridObjs->SetCellValue(j, 2, wxString::FromUTF8(s2));

					wxTreeItemId childNode1 = m_treeCtrl->AppendItem(itemId, wxString::FromUTF8(s1));
				}
			}
		}
	}

	//UA_BrowseRequest_clear(&bReq);

	UA_BrowseResponse_clear(&bResp);
}
void MyFrame::OnBtnGetValue(wxCommandEvent& event)
{
	//---------------------------------------------------------------------------------------------------------------------------------
	/* Read the value attribute of the node. UA_Client_readValueAttribute is a
	 * wrapper for the raw read service available as UA_Client_Service_read. */
	UA_StatusCode status;
	UA_Variant value; /* Variants can hold scalar values and arrays of any type */
	UA_Variant_init(&value);
	char node_name_cc1[1024] = "XPY.DK.R0101"; 
	status = UA_Client_readValueAttribute(m_uaClient, UA_NODEID_STRING(2, node_name_cc1), &value);
	if (status == UA_STATUSCODE_GOOD) {
		if (UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT16])) {
			wxLogMessage(wxString::Format(L"the \"XPY.DK.R0101\" value is: %d", *(UA_Int16*)value.data));
		}
		else {
			wxLogMessage(L"XPY.DK.R0101 获取到数据，但数据类型不匹配");
		}
	}
	else {
		wxLogMessage(L"XPY.DK.R101 未取到数据(信号非GOOD)");
	}
	UA_BuildInfo;
	//---------------------------------------------------------------------------------------------------------------------------------
	// 0,2260,name:BuildInfo,UA_BuildInfo,type:UA_TYPES_BUILDINFO
	//---------------------------------------------------------------------------------------------------------------------------------
	char node_name_cc2[1024] = "_System._DateTimeLocal";
	status = UA_Client_readValueAttribute(m_uaClient, UA_NODEID_STRING(2, node_name_cc2), &value);
	if (status == UA_STATUSCODE_GOOD) {
		if (UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_DATETIME])) {
			UA_DateTimeStruct dts = UA_DateTime_toStruct(*(UA_DateTime*)value.data);
			wxLogMessage(wxString::Format(L"the \"_System._DateTimeLocal\" is: %04u.%02u.%02u %02u:%02u:%02u.%03u", dts.year, dts.month, dts.day, dts.hour, dts.min, dts.sec, dts.milliSec));
		}
		else {
			wxLogMessage(L"_System._DateTimeLocal 获取到数据，但数据类型不匹配");
		}
	}
	else {
		wxLogMessage(L"_System._DateTimeLocal 未取到数据(信号非GOOD)");
	}

	// Clean up 
	UA_Variant_clear(&value);

}

void MyFrame::GetServerName()
{
	UA_StatusCode status;
	UA_Variant value; /* Variants can hold scalar values and arrays of any type */
	UA_Variant_init(&value);
	char server_name[1024] = { '\0' };
	status = UA_Client_readValueAttribute(m_uaClient, UA_NODEID_NUMERIC(0, 2254), &value);
	if (status == UA_STATUSCODE_GOOD) {
		if (UA_Variant_hasArrayType(&value, &UA_TYPES[UA_TYPES_STRING])) {
			UA_String sss = *(UA_String*)value.data;
			memcpy_s(server_name, 1024, sss.data, sss.length - 9);
			m_treeCtrl->AddRoot(wxString::FromUTF8(server_name));
			wxLogMessage(wxString::FromUTF8(server_name));
		}
		else {
			wxLogMessage(L"\"ServerArray\" 获取到数据，但数据类型不匹配");
		}
	}
	else {
		wxLogMessage(L"\"ServerArray\" 未取到数据(信号非GOOD)");
	}
}

void MyFrame::OnTreeSelChanged(wxTreeEvent& event)
{
	// 获取当前选中的节点  
	wxTreeItemId selectedItemId = event.GetItem();
	if (!selectedItemId.IsOk())
		return; // 没有选中的节点  
	if (m_treeCtrl->GetChildrenCount(selectedItemId) > 0)
		return; // 已有子节点
	// 获取当前选中节点的文本（假设节点值存储在文本中）  
	wxString selectedText = m_treeCtrl->GetItemText(selectedItemId);

	char sNodeID[1024] = { '\0' };
	//memcpy_s(sNodeID, 1024, selectedText.c_str(), selectedText.length());
	wxCharBuffer buffer = selectedText.ToUTF8();
	strncpy(sNodeID, buffer.data(), strlen(buffer.data()));

	Browse_nodes(m_uaClient, UA_NODEID_STRING(2, sNodeID), selectedItemId);
	// 在选中节点下添加子节点  
	//wxTreeItemId newItemId = m_treeCtrl->AppendItem(selectedItemId, wxT("新子节点"));

	// 可选：设置新子节点的其他属性，如图像、数据等  
	// ...  

	// 展开包含新子节点的父节点  
	m_treeCtrl->Expand(selectedItemId);
}

//
//
//// 写操作
//void OPC_api::OPC_writeValue(const char* node_name_cc)
//{
//	char* node_name = const_cast <char*>(node_name_cc);
//	OPC_test = OPC_test + 100;
//	UA_Variant_setScalar(&value, &OPC_test, &UA_TYPES[UA_TYPES_UINT32]);
//	status = UA_Client_writeValueAttribute(client, UA_NODEID_STRING(2, node_name), &value);
//	if (status != UA_STATUSCODE_GOOD)
//	{
//		cout << "write Failed" << endl;
//	}
//	else
//	{
//		cout << "write Successful" << endl;
//	}
//	cout << "OPC_test更改为 :" << OPC_test << endl;
//}

//// 创建一个自定义的wxTreeItemData派生类实例  
//MyTreeItemData* myData = new MyTreeItemData(/* 构造函数参数 */);
//
//// 假设你已经有了一个有效的wxTreeItemId叫做itemId  
//treeCtrl->SetItemData(itemId, myData);
//
//// ...  
//
//// 稍后在程序的某个地方，你想获取这个数据  
//wxTreeItemData* retrievedData = treeCtrl->GetItemData(itemId);
//if (retrievedData) {
//	MyTreeItemData* myRetrievedData = dynamic_cast<MyTreeItemData*>(retrievedData);
//	if (myRetrievedData) {
//		// 现在你可以安全地使用myRetrievedData了  
//	}
// 示例中的MyTreeItemData是一个假设的类，它应该继承自wxTreeItemData。你需要根据你的具体需求来实现这个类。
//}
