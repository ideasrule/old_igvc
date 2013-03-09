//=============================================================================
// Copyright © 2006 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with Point Grey Research, Inc. (PGR).
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: CompressorSelectionDlg.cpp,v 1.4 2006/10/26 20:57:39 demos Exp $
//=============================================================================

// CompressorSelectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CompressorSelectionDlg.h"
#include "vfw.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompressorSelectionDlg dialog


CCompressorSelectionDlg::CCompressorSelectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompressorSelectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCompressorSelectionDlg)
	//}}AFX_DATA_INIT

   m_picinfo = NULL;
   m_pcvCurrent = NULL;    
   m_cvChosen.lpState = NULL;
}


void CCompressorSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompressorSelectionDlg)
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_buttonConfigure);
	DDX_Control(pDX, IDC_COMBO_LIST_COMPRESSORS, m_comboListCompressors);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompressorSelectionDlg, CDialog)
	//{{AFX_MSG_MAP(CCompressorSelectionDlg)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, OnButtonConfigure)
	ON_CBN_SELCHANGE(IDC_COMBO_LIST_COMPRESSORS, OnSelchangeComboListCompressors)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompressorSelectionDlg message handlers

BOOL 
CCompressorSelectionDlg::OnInitDialog()
{
   CDialog::OnInitDialog();
   CString csDescription;

   for( int i = 0; i < m_iNumCompressors; i++ )
   {
      COMPVARS* pcompvars = new COMPVARS;
      pcompvars->fccHandler = m_picinfo[i].fccHandler;
      pcompvars->fccType = m_picinfo[i].fccType;
      pcompvars->lpState = NULL;
      pcompvars->cbState = 0;
      pcompvars->lQ = ICQUALITY_DEFAULT;
      pcompvars->cbSize = sizeof( COMPVARS );

	  csDescription.Format("%s",(CString)m_picinfo[ i ].szDescription);
	  m_comboListCompressors.InsertString( i, csDescription.GetBuffer(csDescription.GetLength()) );
      m_comboListCompressors.SetItemData( i, (unsigned long)pcompvars );
   }   

   m_comboListCompressors.SetCurSel( 0 );
   OnSelchangeComboListCompressors();
   
   return TRUE; 
}

CCompressorSelectionDlg::~CCompressorSelectionDlg()
{
   if( m_picinfo != NULL )
   {
      delete [] m_picinfo;
   }
   if( m_cvChosen.lpState != NULL )
   {
      delete [] m_cvChosen.lpState;
   }
}

void
CCompressorSelectionDlg::initialize( ICINFO* picinfo, int iNumICInfo )
{
   //
   // Add 1 because we are going to also use "No Compressor"
   //
   m_iNumCompressors = iNumICInfo + 1;

   m_picinfo = new ICINFO[ m_iNumCompressors ];   

   wcscpy( m_picinfo[ 0 ].szDescription, L"No Compressor" );
   m_picinfo[ 0 ].fccHandler = mmioFOURCC( 'D', 'I', 'B', ' ' );
   m_picinfo[ 0 ].dwFlags = 0x0;

   for( int i = 1; i < m_iNumCompressors; i++ )
   {
      m_picinfo[ i ] = picinfo[ i - 1 ];
   } 

   m_comboListCompressors.setICInfos( m_picinfo, m_iNumCompressors );


}

void 
CCompressorSelectionDlg::OnOK() 
{
   int iSel = m_comboListCompressors.GetCurSel();

   if( iSel > -1 )
   {            
      m_cvChosen = *m_pcvCurrent;
      m_cvChosen.lpState = new char[ m_pcvCurrent->cbState ];
      memcpy( m_cvChosen.lpState, m_pcvCurrent->lpState, m_pcvCurrent->cbState );      
   }   

   CDialog::OnOK();
}

bool
CCompressorSelectionDlg::getSelectedCompressor( COMPVARS* pcompvars )
{
   if( m_pcvCurrent != NULL )
   {  
      *pcompvars = m_cvChosen;      
      pcompvars->lpState = new char[ m_cvChosen.cbState ];
      memcpy( pcompvars->lpState, m_cvChosen.lpState, m_cvChosen.cbState );
      
      return true;
   }   

   return false;
}

void 
CCompressorSelectionDlg::OnButtonConfigure() 
{
   int iSel = m_comboListCompressors.GetCurSel();

   if( iSel > -1 )
   {      
      m_pcvCurrent = (COMPVARS*)m_comboListCompressors.GetItemData( iSel );
   }  
   else
   {
      return;
   }  
   
   m_pcvCurrent->hic = ICOpen( m_pcvCurrent->fccType, 
         m_pcvCurrent->fccHandler, ICMODE_QUERY );  

   if( m_pcvCurrent->hic )
   {
      //
      // Double check to see if this compressor can be configured.
      //
      if( ICQueryConfigure( m_pcvCurrent->hic ) )
      {         
         if( m_pcvCurrent != NULL )
         {
            if( (m_pcvCurrent->lpState != NULL) && (m_pcvCurrent->cbState > 0) )
            {
               ICSetState( m_pcvCurrent->hic, m_pcvCurrent->lpState, m_pcvCurrent->cbState );
            }
         }         
         
         ICConfigure( m_pcvCurrent->hic, GetSafeHwnd() );         
                          
         m_pcvCurrent->cbState = ICGetStateSize( m_pcvCurrent->hic );
         
         if( m_pcvCurrent->cbState > 0 )
         {
            if( m_pcvCurrent->lpState != NULL )
            {
               delete m_pcvCurrent->lpState;
               m_pcvCurrent->lpState = NULL;
            }

            m_pcvCurrent->lpState = new char[ m_pcvCurrent->cbState ];

            if( m_pcvCurrent->lpState != NULL )
            {
               ICGetState( m_pcvCurrent->hic, m_pcvCurrent->lpState, m_pcvCurrent->cbState );               
            }
         }         
        
         ICClose( m_pcvCurrent->hic );
         return;
      }
      ICClose( m_pcvCurrent->hic );
   }

   ::AfxMessageBox( "Unable to configure this compressor." );
}

void 
CCompressorSelectionDlg::OnSelchangeComboListCompressors() 
{
   int iSel = m_comboListCompressors.GetCurSel();

   if( iSel > -1 )
   {      
      m_pcvCurrent = 
         (COMPVARS*)m_comboListCompressors.GetItemData( iSel );
   }  
   else
   {
      m_buttonConfigure.EnableWindow( FALSE );
      return;
   }

   HIC hic = ICOpen( m_pcvCurrent->fccType, 
         m_pcvCurrent->fccHandler, ICMODE_QUERY );    

   if( hic )
   {
      //
      // Check to see if this compressor can be configured.
      //
      if( ICQueryConfigure( hic ) )
      {
         m_buttonConfigure.EnableWindow( TRUE );         
         ICClose( hic ); 
         return;
      }
      ICClose( hic );
   }
   
   m_buttonConfigure.EnableWindow( FALSE );
}

BOOL 
CCompressorSelectionDlg::DestroyWindow() 
{ 

   for( int i = 0; i < m_iNumCompressors; i++ )
   {
      COMPVARS* pcompvars =
         (COMPVARS*)m_comboListCompressors.GetItemData( i );

      if( pcompvars != NULL )
      {
         if( pcompvars->lpState != NULL )
         {
            delete pcompvars->lpState;
         }
         
         delete pcompvars;
      }
   }

   return CDialog::DestroyWindow();
}

void
CCompressorSelectionDlg::getSelectedCompressorName( char* pszName, size_t* piLength )
{
   HIC hic = ICOpen( m_cvChosen.fccType, 
      m_cvChosen.fccHandler, ICMODE_QUERY );
   
   ICINFO icinfo;
   
   if( hic ) 
   { 
      ICGetInfo( hic, &icinfo, sizeof( ICINFO ) );
      ICClose( hic );  
      
      CString csName(icinfo.szDescription);
      
      if( pszName == NULL )
      {
         // 
         // Length of the string plus the NULL terminator.
         //
         *piLength = csName.GetLength() + 1;
         return;
      }
      
      memset( pszName, 0x0, *piLength );
      strncpy( pszName, csName, *piLength-1 );
   }
   else
   {
      if( pszName == NULL )
      {
         // 
         // Length of the string plus the NULL terminator.
         //
         *piLength = strlen( "No Compressor" ) + 1;
         return;
      }
      memset( pszName, 0x0, *piLength );
      strncpy( pszName, "No Compressor", strlen( "No Compressor" ) + 1 );
   }

   return;

}


