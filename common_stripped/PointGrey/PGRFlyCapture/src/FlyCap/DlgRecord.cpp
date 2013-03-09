//=============================================================================
// Copyright © 2004 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with Point Grey Research, Inc. (PGR).
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: DlgRecord.cpp,v 1.9 2007/02/12 18:03:35 soowei Exp $
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include "stdafx.h"

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "flycap.h"
#include "DlgRecord.h"
#include "PGRAviFile.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CDlgRecord::CDlgRecord(CWnd* pParent /*=NULL*/)
   : CDialog(CDlgRecord::IDD, pParent)
{
   //{{AFX_DATA_INIT(CDlgRecord)
   m_iFrames = 0;
   m_iLengthTime = 0;
   m_csPath = _T("");
   m_dFramerate = 0.0;
   m_iCompressionQuality = 85;
   //}}AFX_DATA_INIT
   
   
   // this is a bogus comment.
}


void 
CDlgRecord::DoDataExchange( CDataExchange* pDX )
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CDlgRecord)
   DDX_Control(pDX, IDC_EDIT_JPG_COMPRESSION_QUALITY, m_editCompressionQuality);
   DDX_Control(pDX, IDC_EDIT_SELECTED_COMPRESSOR, m_editSelectedCompressor);
   DDX_Text(pDX, IDC_EDIT_LENGTH, m_iFrames);
   DDV_MinMaxInt(pDX, m_iFrames, 1, 2147483647);
   DDX_Text(pDX, IDC_EDIT_LENGTH_TIME, m_iLengthTime);
   DDV_MinMaxInt(pDX, m_iLengthTime, 1, 2147483647);
   DDX_Text(pDX, IDC_EDIT_PATH, m_csPath);
   DDX_Text(pDX, IDC_EDIT_FRAMERATE, m_dFramerate);
   DDV_MinMaxDouble(pDX, m_dFramerate, 0., 999999999.);
   DDX_Control(pDX, IDC_RADIO_TIME, m_buttonTime);
   DDX_Control(pDX, IDC_RADIO_FRAMES, m_buttonFrames);
   DDX_Control(pDX, IDC_RADIO_STREAM, m_buttonStream);
   DDX_Control(pDX, IDC_EDIT_LENGTH, m_editFrames);
   DDX_Control(pDX, IDC_EDIT_LENGTH_TIME, m_editLengthTime);
   DDX_Control(pDX, IDC_RADIO_VIDEO, m_buttonVideo);
   DDX_Control(pDX, IDC_RADIO_IMAGES, m_buttonImages);
   DDX_Control(pDX, IDC_RADIO_IMAGE_BMP, m_buttonImageBMP);
   DDX_Control(pDX, IDC_RADIO_IMAGE_PPM, m_buttonImagePPM);
   DDX_Control(pDX, IDC_RADIO_IMAGE_PGM, m_buttonImagePGM);
   DDX_Control(pDX, IDC_RADIO_IMAGE_RAW, m_buttonImageRAW);
   DDX_Control(pDX, IDC_RADIO_IMAGE_JPG, m_buttonImageJPG);
   DDX_Text(pDX, IDC_EDIT_JPG_COMPRESSION_QUALITY, m_iCompressionQuality);
   DDV_MinMaxInt(pDX, m_iCompressionQuality, 0, 100);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRecord, CDialog)
//{{AFX_MSG_MAP(CDlgRecord)
   ON_BN_CLICKED(IDC_RADIO_TIME, OnRadioTime)
   ON_BN_CLICKED(IDC_RADIO_FRAMES, OnRadioFrames)
   ON_BN_CLICKED(IDC_RADIO_STREAM, OnRadioStream)
   ON_BN_CLICKED(IDC_RADIO_IMAGES, OnRadioImages)
   ON_BN_CLICKED(IDC_RADIO_VIDEO, OnRadioVideo)
   ON_BN_CLICKED(IDC_RADIO_IMAGE_BMP, OnRadioImageBmp)
   ON_BN_CLICKED(IDC_RADIO_IMAGE_PPM, OnRadioImagePpm)
   ON_BN_CLICKED(IDC_RADIO_IMAGE_PGM, OnRadioImagePgm)
   ON_BN_CLICKED(IDC_RADIO_IMAGE_RAW, OnRadioImageRaw)
   ON_BN_CLICKED(IDC_BUTTON_SELECT_COMPRESSOR, OnButtonSelectCompressor)
   ON_BN_CLICKED(IDC_RADIO_IMAGE_JPG, OnRadioImageJpg)
	ON_EN_KILLFOCUS(IDC_EDIT_JPG_COMPRESSION_QUALITY, OnKillfocusEditJpgCompressionQuality)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL 
CDlgRecord::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_buttonFrames.SetCheck( TRUE );
   m_buttonVideo.SetCheck( TRUE );
   m_buttonImageBMP.EnableWindow( FALSE );
   m_buttonImageJPG.EnableWindow( FALSE );
   m_buttonImagePPM.EnableWindow( FALSE );
   m_buttonImagePGM.EnableWindow( FALSE );
   m_buttonImageRAW.EnableWindow( FALSE );
   m_saveOutput = AVI;
   OnRadioFrames();
   
   COMPVARS compvars;
   
   // Retreive the selected compressor.
   if( m_compressorDlg.getSelectedCompressor( &compvars ) )
   {    
      // Use this compressor.
      size_t iLength;
      m_compressorDlg.getSelectedCompressorName( NULL, &iLength );
      
      if( iLength > 0 )
      {
         char* pszName = new char[ iLength ];
         m_compressorDlg.getSelectedCompressorName( pszName, &iLength );
         
         m_editSelectedCompressor.SetWindowText( 
            pszName );
         
         delete pszName;
      }
      
      m_pAVI->setCompressor( &compvars );
   }
   else
   {         
      m_editSelectedCompressor.SetWindowText( "No compressor" );
   }
      
   return TRUE;  
}

void
CDlgRecord::OnRadioFrames()
{
   m_saveFormat = FRAMES;
   m_editLengthTime.EnableWindow( FALSE );  
   m_editFrames.EnableWindow( TRUE );
}

void
CDlgRecord::OnRadioTime()
{
   m_saveFormat = TIME;
   m_editFrames.EnableWindow( FALSE );
   m_editLengthTime.EnableWindow( TRUE );  
}

void
CDlgRecord::OnRadioStream()
{
   m_saveFormat = STREAM;
   m_editFrames.EnableWindow( FALSE );
   m_editLengthTime.EnableWindow( FALSE );
}

void CDlgRecord::OnRadioImages() 
{
   m_buttonImageBMP.EnableWindow( TRUE );
   m_buttonImageJPG.EnableWindow( TRUE );
   m_buttonImagePPM.EnableWindow( TRUE );   
   m_buttonImagePGM.EnableWindow( TRUE ); 
   m_buttonImageRAW.EnableWindow( TRUE ); 
   m_buttonImageBMP.SetCheck( TRUE );
   m_saveOutput = BMP;
}

void CDlgRecord::OnRadioVideo() 
{	
   m_saveOutput = AVI;
   m_buttonImageBMP.EnableWindow( FALSE );
   m_buttonImageJPG.EnableWindow( FALSE );
   m_buttonImagePPM.EnableWindow( FALSE );
   m_buttonImagePGM.EnableWindow( FALSE );
   m_buttonImageRAW.EnableWindow( FALSE );
   m_buttonImageBMP.SetCheck( FALSE );
   m_buttonImagePPM.SetCheck( FALSE );
   m_buttonImagePGM.SetCheck( FALSE );
   m_buttonImageRAW.SetCheck( FALSE );
}

void CDlgRecord::OnRadioImageBmp() 
{
   m_saveOutput = BMP;
}

void CDlgRecord::OnRadioImageJpg() 
{
   m_saveOutput = JPG;   
}

void CDlgRecord::OnRadioImagePpm() 
{
   m_saveOutput = PPM;
}

void CDlgRecord::OnRadioImagePgm() 
{
   m_saveOutput = PGM;
}

void CDlgRecord::OnRadioImageRaw() 
{
   m_saveOutput = RAW;
}

void CDlgRecord::OnButtonSelectCompressor() 
{
   INT_PTR bRet = m_compressorDlg.DoModal();

   if( bRet == IDOK )
   {      
      COMPVARS compvars;

      // Retreive the selected compressor.
      if( m_compressorDlg.getSelectedCompressor( &compvars ) )
      {    
         // Use this compressor.
         size_t iLength;
         m_compressorDlg.getSelectedCompressorName( NULL, &iLength );

         if( iLength > 0 )
         {
            char* pszName = new char[ iLength ];
            m_compressorDlg.getSelectedCompressorName( pszName, &iLength );

            m_editSelectedCompressor.SetWindowText( 
               pszName );

            delete pszName;
         }

         m_pAVI->setCompressor( &compvars );
      }
      else
      {         
         m_editSelectedCompressor.SetWindowText( "No compressor" );
      }  
   } 
}

void CDlgRecord::setAVIMember( PGRAviFile* pAVI )
{
   m_pAVI = pAVI;
}

void CDlgRecord::OnKillfocusEditJpgCompressionQuality() 
{
   CString csCompressionQuality;
   m_editCompressionQuality.GetWindowText( csCompressionQuality );

   m_iCompressionQuality = atoi( csCompressionQuality );
}
