//=============================================================================
// Copyright © 2001 Point Grey Research, Inc. All Rights Reserved.
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
//
// Digiclops® is a registered Trademark of Point Grey Research Inc.
//=============================================================================
//=============================================================================
// $Id: DialogScrollMode.cpp,v 1.3 2006/10/04 00:15:46 tvlaar Exp $
//=============================================================================
#include "stdafx.h"
#include "timeslicedemo.h"
#include "timeslicedoc.h"
#include "DialogScrollMode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define _SLIDER_MAX 50


DialogScrollMode::DialogScrollMode( CWnd* pParent /*=NULL*/ )
   : CDialog( DialogScrollMode::IDD, pParent )
{
   //{{AFX_DATA_INIT(DialogScrollMode)
   m_iBoth              = 0;
   m_iCamera            = 0;
   m_iFrame             = 0;
   m_uiUpdateDelay      = 333;
   //}}AFX_DATA_INIT
   
   m_pDoc = NULL;
   
   m_uipTimerUID = 0;

   m_bContUpdateFrame = false;
   m_bContUpdateCamera = false;
}


void 
DialogScrollMode::DoDataExchange( CDataExchange* pDX )
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(DialogScrollMode)
   DDX_Control(pDX, IDC_SLIDER_FRAME, m_sliderFrame);
   DDX_Control(pDX, IDC_SLIDER_CAMERA, m_sliderCamera);
   DDX_Control(pDX, IDC_SLIDER_BOTH, m_sliderBoth);
   DDX_Control(pDX, IDC_EDIT_SCR_CAMERA, m_editCamera);
   DDX_Control(pDX, IDC_EDIT_SCR_FRAME, m_editFrame);
   DDX_Slider(pDX, IDC_SLIDER_BOTH, m_iBoth);
   DDX_Slider(pDX, IDC_SLIDER_CAMERA, m_iCamera);
   DDX_Slider(pDX, IDC_SLIDER_FRAME, m_iFrame);
   DDX_Text(pDX, IDC_EDIT_UPDATE_DELAY, m_uiUpdateDelay);
   DDV_MinMaxUInt(pDX, m_uiUpdateDelay, 0, 100000);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( DialogScrollMode, CDialog )
//{{AFX_MSG_MAP(DialogScrollMode)
ON_WM_HSCROLL()
ON_WM_SHOWWINDOW()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_RADIO_CAMERA, OnRadioCamera)
ON_BN_CLICKED(IDC_RADIO_FRAME, OnRadioFrame)
ON_BN_CLICKED(IDC_RADIO_BOTH, OnRadioBoth)
ON_BN_CLICKED(IDC_RADIO_NONE, OnRadioNone)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void 
DialogScrollMode::OnCancel() 
{
}


void 
DialogScrollMode::OnOK() 
{
   UpdateData( TRUE );

   ::KillTimer( GetSafeHwnd(), m_uipTimerUID );
   m_uipTimerUID = ::SetTimer( GetSafeHwnd(), 1, m_uiUpdateDelay, NULL );
}


void 
DialogScrollMode::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
   ASSERT( m_pDoc != NULL );

   static UINT uiPrevPosCamera   = _SLIDER_MAX / 2;
   static UINT uiPrevPosFrame    = _SLIDER_MAX / 2;
   static UINT uiPrevPosBoth     = _SLIDER_MAX / 2;

   int iDelta;

   if( 
      nSBCode != SB_LINELEFT &&
      nSBCode != SB_LINERIGHT &&
      nSBCode != SB_THUMBPOSITION &&
      nSBCode != SB_THUMBTRACK )
   {
      return;
   }

   char pszNumText[ 16 ];

   switch( pScrollBar->GetDlgCtrlID() )
   {
   case IDC_SLIDER_CAMERA:
      switch( nSBCode )
      {
      case SB_LINELEFT:
         iDelta = -1;         
         break;

      case SB_LINERIGHT:
         iDelta = 1;
         break;

      default:
         iDelta = nPos - uiPrevPosCamera;
         break;
      }

      m_pDoc->scrollModifyCamera( iDelta );

      sprintf( pszNumText, "%d", m_pDoc->m_iScrollModeCamera );
      m_editCamera.SetWindowText( pszNumText );

      uiPrevPosCamera = m_sliderCamera.GetPos();
      break;

   case IDC_SLIDER_FRAME:
      switch( nSBCode )
      {
      case SB_LINELEFT:
         iDelta = -1;         
         break;

      case SB_LINERIGHT:
         iDelta = 1;
         break;

      default:
         iDelta = nPos - uiPrevPosFrame;
         break;

      }

      m_pDoc->scrollModifyFrame( iDelta );
      
      sprintf( pszNumText, "%d", m_pDoc->m_iScrollModeLoc );
      m_editFrame.SetWindowText( pszNumText );

      uiPrevPosFrame = m_sliderFrame.GetPos();
      break;

   case IDC_SLIDER_BOTH:
      switch( nSBCode )
      {
      case SB_LINELEFT:
         iDelta = -1;         
         break;
         
      case SB_LINERIGHT:
         iDelta = 1;
         break;
         
      default:
         iDelta = nPos - uiPrevPosBoth;
         break;
      }
            
      m_pDoc->scrollModifyCamera( iDelta );         
      sprintf( pszNumText, "%d", m_pDoc->m_iScrollModeCamera );
      m_editCamera.SetWindowText( pszNumText );
      uiPrevPosCamera = m_sliderCamera.GetPos();
      m_sliderCamera.SetPos( m_sliderCamera.GetPos() + iDelta );        
      
      m_pDoc->scrollModifyFrame( iDelta );         
      sprintf( pszNumText, "%d", m_pDoc->m_iScrollModeLoc );
      m_editFrame.SetWindowText( pszNumText );
      uiPrevPosFrame = m_sliderFrame.GetPos();
      m_sliderFrame.SetPos( m_sliderFrame.GetPos() + iDelta );        
      
      uiPrevPosBoth = m_sliderBoth.GetPos();
      break;
      
   default:
      ASSERT( FALSE );
      break;
   }

   
   CDialog::OnHScroll( nSBCode, nPos, pScrollBar );
}


BOOL 
DialogScrollMode::OnInitDialog() 
{
   CDialog::OnInitDialog();

   ((CButton*)GetDlgItem( IDC_RADIO_NONE ))->SetCheck( 1 );

   m_sliderCamera.SetRange( 0, _SLIDER_MAX, TRUE );
   m_sliderCamera.SetPos( _SLIDER_MAX / 2 );

   m_sliderFrame.SetRange( 0, _SLIDER_MAX, TRUE );
   m_sliderFrame.SetPos( _SLIDER_MAX / 2 );

   m_sliderBoth.SetRange( 0, _SLIDER_MAX, TRUE );
   m_sliderBoth.SetPos( _SLIDER_MAX / 2 );

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


void 
DialogScrollMode::OnShowWindow( BOOL bShow, UINT nStatus ) 
{
   CDialog::OnShowWindow( bShow, nStatus );

   char pszNumText[ 16 ];
   
   sprintf( pszNumText, "%d", m_pDoc->m_iScrollModeCamera );
   m_editCamera.SetWindowText( pszNumText );
   
   sprintf( pszNumText, "%d", m_pDoc->m_iScrollModeLoc );
   m_editFrame.SetWindowText( pszNumText );
}


void 
DialogScrollMode::OnTimer( UINT_PTR nIDEvent ) 
{
   char pszNumText[ 16 ];

   static bool bIncrCamera = true;
   static bool bIncrFrame = true;

   int iDelta;

   if( m_bContUpdateCamera )
   {
      //
      // Modify camera slider.
      //
      iDelta = bIncrCamera ? 1: -1;

      m_pDoc->scrollModifyCamera( iDelta );         
      sprintf( pszNumText, "%d", m_pDoc->m_iScrollModeCamera );
      m_editCamera.SetWindowText( pszNumText );
      m_sliderCamera.SetPos( m_sliderCamera.GetPos() + iDelta );

      if( bIncrCamera )
      {
         if( m_sliderCamera.GetPos() >= m_sliderCamera.GetRangeMax() )
         {
            bIncrCamera = false;
         }
      }
      else
      {
         if( m_sliderCamera.GetPos() <= m_sliderCamera.GetRangeMin() )
         {
            bIncrCamera = true;
         }      
      }
   }


   if( m_bContUpdateFrame )
   {
      //
      // Modify frame slider.
      //
      iDelta = bIncrFrame ? 1: -1;
      
      m_pDoc->scrollModifyFrame( iDelta );         
      sprintf( pszNumText, "%d", m_pDoc->m_iScrollModeLoc );
      m_editFrame.SetWindowText( pszNumText );
      m_sliderFrame.SetPos( m_sliderFrame.GetPos() + iDelta );        
      
      if( bIncrFrame )
      {
         if( m_sliderFrame.GetPos() >= m_sliderFrame.GetRangeMax() )
         {
            bIncrFrame = false;
         }
      }
      else
      {
         if( m_sliderFrame.GetPos() <= m_sliderFrame.GetRangeMin() )
         {
            bIncrFrame = true;
         }      
      }
   }
   

   CDialog::OnTimer( nIDEvent );
}


void 
DialogScrollMode::OnRadioCamera() 
{
   UpdateData( TRUE );

   m_bContUpdateCamera = true;
   m_bContUpdateFrame = false;

   if( m_uipTimerUID == 0 )
   {
      m_uipTimerUID = ::SetTimer( GetSafeHwnd(), 1, m_uiUpdateDelay, NULL );
   }
}


void 
DialogScrollMode::OnRadioFrame() 
{
   UpdateData( TRUE );

   m_bContUpdateCamera = false;
   m_bContUpdateFrame = true;

   if( m_uipTimerUID == 0 )
   {
      m_uipTimerUID = ::SetTimer( GetSafeHwnd(), 1, m_uiUpdateDelay, NULL );
   }

}


void 
DialogScrollMode::OnRadioBoth() 
{
   UpdateData( TRUE );

   m_bContUpdateFrame = true;
   m_bContUpdateCamera = true;

   if( m_uipTimerUID == 0 )
   {
      m_uipTimerUID = ::SetTimer( GetSafeHwnd(), 1, m_uiUpdateDelay, NULL );
   }

}


void 
DialogScrollMode::OnRadioNone() 
{   
   UpdateData( TRUE );

   m_bContUpdateCamera = false;
   m_bContUpdateFrame = false;

   if( m_uipTimerUID != 0 )
   {      
      ::KillTimer( GetSafeHwnd(), m_uipTimerUID );
      m_uipTimerUID = 0;
   }
   
   m_sliderCamera.SetPos( _SLIDER_MAX / 2 );
   m_sliderFrame.SetPos( _SLIDER_MAX / 2 );
   
   char pszNumText[ 16 ];
   
   sprintf( pszNumText, "%d", m_pDoc->m_iScrollModeCamera );
   m_editCamera.SetWindowText( pszNumText );
   
   sprintf( pszNumText, "%d", m_pDoc->m_iScrollModeLoc );
   m_editFrame.SetWindowText( pszNumText );
}


