#if !defined(AFX_COMPRESSORSELECTIONDLG_H__B417D969_55CE_4C56_9FCF_08D3253987E5__INCLUDED_)
#define AFX_COMPRESSORSELECTIONDLG_H__B417D969_55CE_4C56_9FCF_08D3253987E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompressorSelectionDlg.h : header file
//

#include "ComboBoxColour.h"
#include "flycap.h"


/////////////////////////////////////////////////////////////////////////////
// CCompressorSelectionDlg dialog

class CCompressorSelectionDlg : public CDialog
{
// Construction
public:
	CCompressorSelectionDlg(CWnd* pParent = NULL);   // standard constructor

        virtual ~CCompressorSelectionDlg();

// Dialog Data
	//{{AFX_DATA(CCompressorSelectionDlg)
	enum { IDD = IDD_DIALOG_COMPRESSOR_SELECTION };
	CButton	m_buttonConfigure;
	CComboBoxColour	m_comboListCompressors;
	//}}AFX_DATA

        void initialize( ICINFO* picinfo, int iNumICInfo );
        bool getSelectedCompressor( COMPVARS* pcompvars);
        void getSelectedCompressorName( char* pszName, size_t* piLength );        


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompressorSelectionDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCompressorSelectionDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();           
	afx_msg void OnButtonConfigure();
	afx_msg void OnSelchangeComboListCompressors();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

        ICINFO* m_picinfo;
        int     m_iNumCompressors;

        COMPVARS* m_pcvCurrent;
        COMPVARS  m_cvChosen;

        /* The size of the chosen compressor's state. */
        DWORD   m_dwStateSize;
        
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPRESSORSELECTIONDLG_H__B417D969_55CE_4C56_9FCF_08D3253987E5__INCLUDED_)
