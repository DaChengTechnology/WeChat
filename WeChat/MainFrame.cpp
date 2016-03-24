#include "StdAfx.h"
#include "MainFrame.h"
#include "resource.h"

#define WM_MYMSG_01    9999
#define WM_ICON	WM_USER + 1
CMainFrame::CMainFrame(void)
{
	icon = NULL;
}


CMainFrame::~CMainFrame(void)
{
	if(icon)
	{
		delete icon;
		icon = NULL;
	}
}

LPCTSTR CMainFrame::GetWindowClassName() const
{
	return _T("DUIMainFrame");
}

DuiLib::CDuiString CMainFrame::GetSkinFile()
{
	return _T("OwnerUI.xml");
}

DuiLib::CDuiString CMainFrame::GetSkinFolder()
{
	return _T("OwnerFolder");
}

void CMainFrame::OnPrepare()
{
	icon = new CDuiTrayIcon();
	icon->CreateTrayIcon(m_hWnd,/*IDI_WECHAT*/IDI_WE_CHAT,L"΢�źǺ�",WM_ICON);  //��������
	//icon->StartTwinkling();
	UpgrateFriends();
	UpgrateContacts();
}
 
void CMainFrame::Notify(TNotifyUI& msg)
{
	CDuiString SenderName = msg.pSender->GetName();
	if(msg.sType == _T("click"))				//��ť����¼�
	{
		if(SenderName == L"send")
			OnSendMessage();
	}
	else if(msg.sType == _T("windowinit"))
	{
		OnPrepare();
	}
	else if(msg.sType  == _T("itemclick"))
	{
		CFriendListUI* pFriendsList = static_cast<CFriendListUI*>(m_PaintManager.FindControl(L"friends"));
		if(pFriendsList != NULL && pFriendsList->GetItemIndex(msg.pSender) != -1)
			OnSelectFriendList(msg, pFriendsList);

		CContactUI* pContactsList = static_cast<CContactUI*>(m_PaintManager.FindControl(L"contacts"));
		if(pContactsList != NULL && pContactsList->GetItemIndex(msg.pSender) != -1)
			OnSelectContactList(msg, pContactsList);
	}
	__super::Notify(msg);
}

LRESULT CMainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch(uMsg)
	{
	case WM_KEYDOWN:
		lRes = OnKeyReturn(uMsg,wParam,lParam,bHandled);
		break;
	case WM_TIMER:
		lRes = OnOwnTimer(uMsg,wParam,lParam,bHandled);
		break;
	case WM_NCLBUTTONDBLCLK:
		lRes= 0;
		break;
	case WM_NCHITTEST:     
		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); 
		break;
	case WM_ICON:
		lRes = On_TroyIcon(uMsg, wParam, lParam, bHandled);   //����������Ϣ
		break;
	default:
		bHandled = FALSE;
	}
	if(bHandled ) return lRes;
	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CMainFrame::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	if( !::IsZoomed(*this) )//���ڷŴ�
	{
		RECT rcSizeBox = m_PaintManager.GetSizeBox();
		if( pt.y < rcClient.top + rcSizeBox.top )
		{
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
			return HTTOP;
		}
		else if( pt.y > rcClient.bottom - rcSizeBox.bottom )
		{
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}

		if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
		if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
	}

	RECT rcCaption = m_PaintManager.GetCaptionRect();
	//�������λ���ڱ�������
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(pt));
		if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
			_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
			_tcsicmp(pControl->GetClass(), _T("SliderUI")) != 0 &&
			_tcsicmp(pControl->GetClass(), _T("EditUI")) != 0 &&
			_tcsicmp(pControl->GetClass(), _T("RichEditUI")) != 0 &&
			_tcsicmp(pControl->GetClass(), _T("ComboUI")) != 0)
			return HTCAPTION;
	}
	return HTCLIENT;
}

CControlUI* CMainFrame::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("FriendList")) == 0)   //�Զ���ؼ����б���
	{
		return new CFriendListUI(m_PaintManager);
	}
	return NULL;
}

void CMainFrame::UpgrateContacts()
{
	CContactUI* pContactsList = static_cast<CContactUI*>(m_PaintManager.FindControl(L"contacts"));
	if(pContactsList != NULL)
	{
		if (!contact_.empty())					//��� �б���Ⱥ+���ں�+���ѣ�
			contact_.clear();

		ContactListItemInfo item={0};
		item.nick_name = _T("���ں�");
		item.folder = true;
		item.id = OTHERS;
		Node* root_parent = pContactsList->AddNode(item, NULL);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("���ں�");
		item.logo = _T("logo.png");
		item.folder = false;
		item.id = PUBLIC_NO;
		pContactsList->AddNode(item, root_parent);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("Ⱥ��");
		item.folder = true;
		item.id = GROUP_CHAT;
		Node* root_parent1 = pContactsList->AddNode(item, NULL);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("10��ȫ1");
		item.logo = _T("qun1.png");
		item.group_id = 1;		//1��Ⱥ
		item.folder = false;
		item.id = GROUP_CHAT;
		pContactsList->AddNode(item, root_parent1);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("�Ÿ硢���ԡ�����");
		item.logo = _T("qun2.png");
		item.group_id = 2;		//1��Ⱥ
		item.folder = false;
		item.id = GROUP_CHAT;
		pContactsList->AddNode(item, root_parent1);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("��������齫");
		item.logo = _T("qun3.png");
		item.group_id = 3;		//1��Ⱥ
		item.folder = false;
		item.id = GROUP_CHAT;
		pContactsList->AddNode(item, root_parent1);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("�Ƹ�ȫ���Ԯ�ź��Ӫ");
		item.logo = _T("qun4.png");
		item.group_id = 4;		//1��Ⱥ
		item.folder = false;
		item.id = GROUP_CHAT;
		pContactsList->AddNode(item, root_parent1);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("C");
		item.folder = true;
		item.id = OTHERS;
		Node* root_parent2 = pContactsList->AddNode(item, NULL);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("����");
		item.logo = _T("c1_1.png");
		item.description = L"����֪��Ȩ����Ӧ����ӵ�в�֪��Ȩ";
		item.back_name = L"�ݸ�";
		item.weixin_id = L"zhe5960";
		item.zone = L"�㽭 ����";
		item.group_id = 1;		//1��Ⱥ
		item.folder = false;
		item.id = FRIENDS;
		pContactsList->AddNode(item, root_parent2);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("������");
		item.logo = _T("c2.png");
		item.description = L"���߷�����˳�ƶ�Ϊ��";
		item.back_name = L"";
		item.weixin_id = L"czllry";
		item.zone = L"�㽭 ����";
		item.group_id = 1;		//1��Ⱥ
		item.folder = false;
		item.id = FRIENDS;
		pContactsList->AddNode(item, root_parent2);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("��С��");
		item.logo = _T("c3.png");
		item.description = L"";
		item.back_name = L"�̷�";
		item.group_id = 2;		//1��Ⱥ
		item.weixin_id = L"";
		item.zone = L"�㽭 ��ɽ";
		item.folder = false;
		item.id = FRIENDS;
		pContactsList->AddNode(item, root_parent2);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("��ǿ");
		item.description = L"���ʲ������";
		item.group_id = 2;		//1��Ⱥ
		item.back_name = L"";
		item.weixin_id = L"cq_52apple1314";
		item.zone = L"�㽭 ����";
		item.logo = _T("c4.png");
		item.folder = false;
		item.id = FRIENDS;
		pContactsList->AddNode(item, root_parent2);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("������qs��");
		item.description = L"��� ����";
		item.back_name = L"�½�";
		item.group_id = 3;		//1��Ⱥ
		item.weixin_id = L"qinshoujie";
		item.zone = L"�㽭 ��ɽ";
		item.logo = _T("c5.png");
		item.folder = false;
		item.id = FRIENDS;
		pContactsList->AddNode(item, root_parent2);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		memset(&item,0,sizeof(ContactListItemInfo));
		item.nick_name = _T("D");
		item.id = OTHERS;
		item.folder = true;
		Node* root_parent3 = pContactsList->AddNode(item, NULL);		//���Ӹ��ڵ㣬���������ڵ㣨����
		contact_.push_back(item);

		for (int i=0; i<5; ++i)
		{
			item.nick_name = _T("����");
			item.logo = _T("c1.png");
			item.description = L"����֪��Ȩ����Ӧ����ӵ�в�֪��Ȩ";
			item.back_name = L"�ݸ�";
			item.group_id = 1;		//1��Ⱥ
			item.weixin_id = L"zhe5960";
			item.zone = L"�㽭 ����";
			item.folder = false;
			item.id = FRIENDS;
			pContactsList->AddNode(item, root_parent3);		//���Ӹ��ڵ㣬���������ڵ㣨����
			contact_.push_back(item);

			item.nick_name = _T("������");
			item.logo = _T("c2.png");
			item.description = L"���߷�����˳�ƶ�Ϊ��";
			item.back_name = L"";
			item.weixin_id = L"czllry";
			item.group_id = 1;		//1��Ⱥ
			item.zone = L"�㽭 ����";
			item.folder = false;
			item.id = FRIENDS;
			pContactsList->AddNode(item, root_parent3);		//���Ӹ��ڵ㣬���������ڵ㣨����
			contact_.push_back(item);

			item.nick_name = _T("��С��");
			item.logo = _T("c3.png");
			item.description = L"";
			item.back_name = L"�̷�";
			item.group_id = 2;		//1��Ⱥ
			item.weixin_id = L"";
			item.zone = L"�㽭 ��ɽ";
			item.folder = false;
			item.id = FRIENDS;
			pContactsList->AddNode(item, root_parent3);		//���Ӹ��ڵ㣬���������ڵ㣨����
			contact_.push_back(item);

			item.nick_name = _T("��ǿ");
			item.description = L"���ʲ������";
			item.group_id = 2;		//1��Ⱥ
			item.back_name = L"";
			item.weixin_id = L"cq_52apple1314";
			item.zone = L"�㽭 ����";
			item.logo = _T("c4.png");
			item.folder = false;
			item.id = FRIENDS;
			pContactsList->AddNode(item, root_parent3);		//���Ӹ��ڵ㣬���������ڵ㣨����
			contact_.push_back(item);

			item.nick_name = _T("������qs��");
			item.description = L"��� ����";
			item.group_id = 3;		//1��Ⱥ
			item.back_name = L"�½�";
			item.weixin_id = L"qinshoujie";
			item.zone = L"�㽭 ��ɽ";
			item.logo = _T("c5.png");
			item.folder = false;
			item.id = FRIENDS;
			pContactsList->AddNode(item, root_parent3);		//���Ӹ��ڵ㣬���������ڵ㣨����
			contact_.push_back(item);
		}
	}
}

void CMainFrame::UpgrateFriends()
{
	CFriendListUI* pFriendsList = static_cast<CFriendListUI*>(m_PaintManager.FindControl(L"friends"));
	if(pFriendsList != NULL)
	{
		if (!friends_.empty())					//��� �����ϵ�б�
			friends_.clear();
		if(pFriendsList->GetCount() > 0)
			pFriendsList->RemoveAll();

		FriendListItemInfo item = {0};
		item.nick_name = _T("������");
		item.logo = _T("c2.png");
		item.id = LAST_CONTACT;
		item.folder = false;
		pFriendsList->AddNode(item, NULL);		//���Ӹ��ڵ㣬���������ڵ㣨����
		friends_.push_back(item);

		for (int i=0; i<10; ++i)
		{
			memset(&item,0,sizeof(FriendListItemInfo));
			item.nick_name = _T("������");
			item.logo = _T("c2.png");
			item.id = LAST_CONTACT;
			pFriendsList->AddNode(item, NULL);		//���Ӹ��ڵ㣬���������ڵ㣨����
			friends_.push_back(item);
		}
	}
}

DuiLib::ContactListItemInfo* CMainFrame::FindContactItem(CDuiString &name)
{
	for (unsigned int i=0; i<contact_.size(); ++i)
	{
		if(contact_[i].nick_name == name)
			return &contact_[i];
	}
	return NULL;
}

std::vector<ContactListItemInfo*> CMainFrame::FindContactItemBygroupID(UINT id)
{
	std::vector<ContactListItemInfo*> res;
	for (UINT i=0; i<contact_.size(); ++i)
	{
		if(contact_[i].group_id == id && contact_[i].id == FRIENDS)
		{
			res.push_back(&contact_[i]);
		}
	}
	return res;
}

void CMainFrame::OnSelectFriendList(TNotifyUI& msg, CFriendListUI* pFriendsList)
{
	if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
	{
		Node* node = (Node*)msg.pSender->GetTag();
		if(node == NULL) return;
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("default_bk")));
		if(pControl == NULL) return;

		if(node->data().type_ == LAST_CONTACT)			//�����ϵ��
		{
			CTileLayoutUI* m_group_list = static_cast<CTileLayoutUI*>(m_PaintManager.FindControl(L"group_list"));	
			if(m_group_list)
			{
				pControl->SelectItem(3);
			}
		}
	}
}

void CMainFrame::OnSelectContactList(TNotifyUI& msg, CContactUI* pFriendsList)
{
	if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
	{
		Node* node = (Node*)msg.pSender->GetTag();
		if(node == NULL) return;
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("default_bk")));
		if(pControl == NULL) return;

		if(node->data().type_ == GROUP_CHAT)				//ѡ����Ⱥ��
		{
			CTileLayoutUI* m_group_list = static_cast<CTileLayoutUI*>(m_PaintManager.FindControl(L"group_list"));			
			if(m_group_list)
			{
				static std::vector<CButtonUI*> vec;
				if(m_group_list->GetCount() > 0)
					m_group_list->RemoveAll();
				if(vec.size() > 0)
					vec.clear();
				std::vector<ContactListItemInfo*> res = FindContactItemBygroupID(node->data().group_id);
				if(res.size() > 0)
				{
					TCHAR szPath[MAX_PATH]={0};
					RECT rcTextPadding={0,73,0,0};
					for (UINT i=0; i<res.size();++i)
					{
						CButtonUI *pBtn = new CButtonUI;
						_stprintf(szPath, _T("file='%s' dest='5,10,66,71'"),res[i]->logo);
						pBtn->SetBkImage(szPath);
						pBtn->SetHotImage(L"file='bk_hover.png' corner='5,10,5,37' hole='true'");
						pBtn->SetPushedImage(L"file='bk_hover.png' corner='5,10,5,37' hole='true'");
						pBtn->SetTextPadding(rcTextPadding);
						pBtn->SetText(res[i]->nick_name);
						pBtn->SetFixedHeight(108);
						pBtn->SetFixedWidth(71);
						m_group_list->Add(pBtn);
						vec.push_back(pBtn);
					}
				}
			}
			pControl->SelectItem(2);
		}
		else if(node->data().type_ == PUBLIC_NO)		//ѡ���˹��ں�
		{
			//icon->StopTwinkling();
			pControl->SelectItem(0);
		}
		else if(node->data().type_ == FRIENDS)			//ѡ���˺���
		{
			CButtonUI* m_friend_face = static_cast<CButtonUI*>(m_PaintManager.FindControl(L"friend_face"));
			CLabelUI* m_friend_name = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"friend_name"));
			CLabelUI* m_friend_sign = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"friend_sign"));
			CLabelUI* m_back_name = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"back_name"));
			CLabelUI* m_weixin_id = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"weixin_id"));
			CLabelUI* m_zone = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"zone"));

			ContactListItemInfo* item = FindContactItem(node->data().text_);
			if(item != NULL)
			{
				m_friend_face->SetNormalImage(item->logo);
				m_friend_name->SetText(item->nick_name);
				if(item->description.IsEmpty())
					m_friend_sign->SetVisible(false);
				else
				{
					m_friend_sign->SetVisible(true);
					m_friend_sign->SetText(item->description);
				} 
				if(item->back_name.IsEmpty())
				{
					m_back_name->SetText(L"������ӱ�ע");
					//m_back_name->SetBkColor(1000);
				}
				else
					m_back_name->SetText(item->back_name);
				if(item->weixin_id.IsEmpty())
					m_weixin_id->SetVisible(false);
				else
				{
					m_weixin_id->SetVisible(true);
					m_weixin_id->SetText(item->weixin_id);
				}
				m_zone->SetText(item->zone);
			}
			pControl->SelectItem(1);
		}
	}
}

void CMainFrame::OnSendMessage()
{
	//���촰����������
	CBubbleChat* pBubbleList = static_cast<CBubbleChat*>(m_PaintManager.FindControl(L"Bubble_Chat"));
	BubbleItemInfo item;
	memset(&item,0,sizeof(BubbleItemInfo));
	CRichEditUI* pInputEdit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(L"input_edit"));
	if(pBubbleList)
	{	
		memset(item.buf,0,MAX_PATH);
		wcscpy(item.buf,pInputEdit->GetText().GetData());
		if(wcslen(item.buf)>0)
		{
			pBubbleList->AddNode(item,NULL);
			//��� ��������
			//pInputEdit->SetSel(0,-1);
			//pInputEdit->Clear();
			pInputEdit->SetFocus();
			pInputEdit->SetText(L"");
			SetTimer(m_hWnd,WM_MYMSG_01,80,NULL);
		}
		else
			MessageBox(m_hWnd,L"���ܷ��Ϳհ���Ϣ",L"error",MB_OK);

		//::SendMessage(*this,WM_MYMSG_01,0,0);
		//pBubbleList->PageDown();
	}
}

LRESULT CMainFrame::OnKeyReturn(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam == VK_RETURN)  //���»س���
	{
		CRichEditUI* pInputEdit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(L"input_edit"));
		if(pInputEdit && pInputEdit->IsFocused())
			OnSendMessage();	
	}
	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnOwnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CBubbleChat* pBubbleList = static_cast<CBubbleChat*>(m_PaintManager.FindControl(L"Bubble_Chat"));
	if(wParam == WM_MYMSG_01 && pBubbleList)
	{
		SIZE sz = pBubbleList->GetScrollPos();
		int cy = pBubbleList->GetScrollRange().cy;
		if(sz.cy == cy)
			KillTimer(m_hWnd,WM_MYMSG_01);
		else
		{
			sz.cy = cy;
			pBubbleList->SetScrollPos(sz);
		}
	}
	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_MENUCLICK)
	{
		int a = 1;
// 		CButtonUI* pControl = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("setting_quit")));
 		CDuiString *strMenuName = (CDuiString*)wParam;
// 		BOOL bChecked = (BOOL)lParam;	
 		if (*strMenuName == _T("quit")) 
 		{
 			Close();
		}
 		delete strMenuName;
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CMainFrame::On_TroyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(lParam)
	{
	case WM_RBUTTONDOWN:
		{
			CMenuWnd* pMenu = new CMenuWnd();   //����� ʲôʱ�� �ͷ��ڴ��
			tagPOINT point;
			GetCursorPos(&point);
			point.y = point.y - 165;
			pMenu->Init(NULL, _T("menu_troy.xml"), point, &m_PaintManager,NULL);
		}
	}
	bHandled = FALSE;
	return 0;
}