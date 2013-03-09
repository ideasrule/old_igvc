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
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: ComboBoxColour.cpp,v 1.6 2007/12/13 23:35:19 soowei Exp $
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
#include "ComboBoxColour.h"
#include "vfw.h"

#define CINEPAK_BY_RADIUS      mmioFOURCC( 'c', 'v', 'i', 'd' )
#define WINDOWS_MEDIA_VIDEO_9  mmioFOURCC( 'W', 'M', 'V', '3' )
#define NO_COMPRESSION         mmioFOURCC( 'D', 'I', 'B', ' ' )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CComboBoxColour::CComboBoxColour()
{
}

CComboBoxColour::~CComboBoxColour()
{
}


BEGIN_MESSAGE_MAP(CComboBoxColour, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxColour)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxColour message handlers


void 
CComboBoxColour::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   ASSERT(lpDrawItemStruct->CtlType == ODT_COMBOBOX);
   //ICINFO* pICInfo = (ICINFO*) lpDrawItemStruct->itemData;
   COMPVARS* pcv = (COMPVARS*)lpDrawItemStruct->itemData;

   ICINFO icinfo = m_picinfo[ lpDrawItemStruct->itemID ];
   /*
   HIC hic = ICOpen( pcv->fccType, pcv->fccHandler, ICMODE_QUERY );
   ICINFO icinfo;

   if( hic ) 
   {     
      ICGetInfo( hic, &icinfo, sizeof( ICINFO ) );
      ICClose( hic ); 
   } */


   CDC dc;

   dc.Attach(lpDrawItemStruct->hDC);

   // Save these value to restore them when done drawing.
   COLORREF crOldTextColor = dc.GetTextColor();
   COLORREF crOldBkColor = dc.GetBkColor();

   bool bSelected = false;

   // If this item is selected, set the background color 
   // and the text color to appropriate values. Erase
   // the rect by filling it with the background color.
   if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
      (lpDrawItemStruct->itemState  & ODS_SELECTED))
   {
      bSelected = true;
      dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
      dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
      dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
   }
   else
      dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);

   if( pcv->fccHandler == WINDOWS_MEDIA_VIDEO_9 ||
       pcv->fccHandler == NO_COMPRESSION )
   {
	  // Since this type of codec is supported (known to work), 
	  // change the background colour to light blue if it is not
	  // selected. (Selected items are already dark blue).
	  if( !bSelected )
      {
         dc.SetBkColor( RGB( 198,226,255 ) );
         dc.FillSolidRect(&lpDrawItemStruct->rcItem, RGB( 198,226,255 ) );
      }
   }

   // Draw the text.
   CString csDesc(icinfo.szDescription);

   dc.DrawText(
      csDesc,
      (int)csDesc.GetLength(),
      &lpDrawItemStruct->rcItem,
      DT_CENTER|DT_SINGLELINE|DT_VCENTER);

   // Reset the background color and the text color back to their
   // original values.
   dc.SetTextColor(crOldTextColor);
   dc.SetBkColor(crOldBkColor);

   dc.Detach();

}

void 
CComboBoxColour::setICInfos( ICINFO* picinfo, int iNumICInfo )
{
   m_picinfo = new ICINFO[ iNumICInfo ];

   for( int i = 0; i < iNumICInfo; i++ )
   {
      m_picinfo[ i ] = picinfo[ i ];
   }
}
