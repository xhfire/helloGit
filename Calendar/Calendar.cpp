// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
//#include "systdate.h"
#include <ATLComTime.h>

class CFrameWindowWnd : public CWindowWnd, public INotifyUI
{
public:
    CFrameWindowWnd() { };
    LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
    UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
    void OnFinalMessage(HWND /*hWnd*/) { delete this; };

    void Init() {}

    void OnPrepare() 
    {
		::GetLocalTime(&m_sysTime);
		DrawCalendar(m_sysTime);
    }

    void Notify(TNotifyUI& msg)
    {
        if( msg.sType == _T("windowinit") ) OnPrepare();
        else if( msg.sType == _T("return") ) 
		{
			m_sysTime.wYear  = SetTxtYear(0);
			m_sysTime.wMonth  = GetCmbMonth();
			DrawCalendar(m_sysTime);
		}
        else if( msg.sType == _T("click") ) 
		{
			CControlUI* pOne = static_cast<CControlUI*>(m_pm.FindControl(msg.ptMouse));
			if (_tcsicmp(pOne->GetClass(), _T("ButtonUI")) == 0)
			{
				//上一月
				if (_tcsicmp(pOne->GetName(), _T("BTN_UP_MONTH")) == 0)  
				{
					m_sysTime.wMonth = m_sysTime.wMonth-1 == 0 ? 12 : m_sysTime.wMonth-1;
					DrawCalendar(m_sysTime);
				}
				//上一年
				else if (_tcsicmp(pOne->GetName(), _T("BTN_UP_YEAR")) == 0)  
				{
					m_sysTime.wYear  = SetTxtYear(-1);
					m_sysTime.wMonth  = GetCmbMonth();
					DrawCalendar(m_sysTime);
				}
				//下一年
				else if (_tcsicmp(pOne->GetName(), _T("BTN_DOWN_YEAR")) == 0)  
				{
					m_sysTime.wYear  = SetTxtYear(1);
					m_sysTime.wMonth  = GetCmbMonth();
					DrawCalendar(m_sysTime);
				}
			}
		}
        else if( msg.sType == _T("itemselect") ) {
			if (_tcsicmp(msg.pSender->GetName(), _T("CMB_MONTH")) == 0)  
			{
				m_sysTime.wMonth  = GetCmbMonth();
				DrawCalendar(m_sysTime);
			}
        }
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if( uMsg == WM_CREATE ) {
            m_pm.Init(m_hWnd);
            CDialogBuilder builder;
            CControlUI* pRoot = builder.Create(_T("Calendar1.xml"), (UINT)0, NULL, &m_pm);
            ASSERT(pRoot && "Failed to parse XML");
            m_pm.AttachDialog(pRoot);
            m_pm.AddNotifier(this);
            Init();
            return 0;
        }
        else if( uMsg == WM_DESTROY ) {
            ::PostQuitMessage(0L);
        }
        else if( uMsg == WM_ERASEBKGND ) {
            return 1;
        }
        LRESULT lRes = 0;
        if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

	//绘制日历
	void DrawCalendar(SYSTEMTIME m_sysTime)
	{
		int iDay = 0;
		char cDay[3] = {0};
		//int iStartDay = m_sysTime.wDayOfWeek;
		m_sysTime.wDay = 1;
		int iStartDay = GetDayOfWeek(m_sysTime);
		//上月天数
		int iLastMonthStartDays = 31 - iStartDay;
		if(m_sysTime.wMonth>1)
			iLastMonthStartDays = GetMonthDays(m_sysTime.wYear,m_sysTime.wMonth - 1) - iStartDay;
		//本月天数
		int iMonthDays = GetMonthDays(m_sysTime.wYear,m_sysTime.wMonth);
		//下月天数
		int iNextMonthDays = 0;

		CControlUI* pButtonUI;
        CContainerUI* pControl = static_cast<CContainerUI*>(m_pm.FindControl(_T("ContainerUI3")));
		for(int i = 0;i < 42;i++)
		{
			pButtonUI = m_pm.FindSubControlByClass(pControl,_T("ButtonUI"),i);
			//上月
			if(i<iStartDay)
			{
				iLastMonthStartDays ++;
				sprintf(cDay,"%d",iLastMonthStartDays);
				pButtonUI->SetText(cDay);
				DrawBtnBackImage(pButtonUI,false);
			}
			else if(i>iStartDay-1&&iDay<iMonthDays)
			{
				iDay++;
				sprintf(cDay,"%d",iDay);
				pButtonUI->SetText(cDay);
				DrawBtnBackImage(pButtonUI,true);
			} 
			else 
			{
				iNextMonthDays ++;
				sprintf(cDay,"%d",iNextMonthDays);
				pButtonUI->SetText(cDay);
				DrawBtnBackImage(pButtonUI,false);
			}
		}
	}

	void DrawBtnBackImage(CControlUI* pButtonUI,bool isEnable)
	{
		if(isEnable)
		{
			pButtonUI->SetEnabled(true);
			pButtonUI->ApplyAttributeList(
				_T("normalimage=\"file='cells.png' source='0,0,81,81'\" ")\
				_T("hotimage=\"file='cells.png' source='0,81,81,162'\" "));
		}
		else
		{
			pButtonUI->SetBkImage(_T("calpad.jpg"));
			pButtonUI->ApplyAttributeList(
				_T("normalimage=\"file='calpad.jpg'\" ")\
				_T("hotimage=\"file='calpad.jpg'\" "));
		}
	}

	int GetMonthDays(int iY,int iM)
	{
		int iTotalDay = 31;		
		if(iM == 2) 
		{
			//闰年可以被4或者400整除 但是不能被100整除
			if(iY % 4 ==0 && iY % 100 !=0 || iY % 400 ==0) 
				iTotalDay= 29;
			else
				iTotalDay=28;
		}
		else if(iM==4 || iM==6 || iM==9||iM==11)
			iTotalDay = 30;
		return iTotalDay;
	}

	int GetDayOfWeek(SYSTEMTIME m_sysTime)
	{
		cTime.SetDate(m_sysTime.wYear,m_sysTime.wMonth,m_sysTime.wDay);
		return cTime.GetDayOfWeek()-1;
	}

	int GetCmbMonth()
	{
		CComboUI* pCmb = static_cast<CComboUI*>(m_pm.FindControl(_T("CMB_MONTH")));
		CDuiString cCurMonth = pCmb->GetText();
		return atoi(cCurMonth.Left(cCurMonth.GetLength()-1));
	}

	int SetTxtYear(int iY)
	{
		CEditUI* pTxt = static_cast<CEditUI*>(m_pm.FindControl(_T("TXT_YEAR")));
		CDuiString cCurYear = pTxt->GetText();
		int iCurYear= 0;
		iCurYear = atoi(cCurYear) + iY;
		cCurYear.Format("%d",iCurYear);
		pTxt->SetText(cCurYear);
		return iCurYear;
	}


public:
    CPaintManagerUI m_pm;
	SYSTEMTIME m_sysTime;
	COleDateTime cTime;
};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;

    CFrameWindowWnd* pFrame = new CFrameWindowWnd();
    if( pFrame == NULL ) return 0;
    pFrame->Create(NULL, _T("日历控件测试"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    pFrame->CenterWindow();
    pFrame->ShowWindow(true);
    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();
    return 0;
}
