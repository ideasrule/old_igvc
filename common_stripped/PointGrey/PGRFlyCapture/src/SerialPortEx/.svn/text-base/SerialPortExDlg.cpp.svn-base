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
// $Id: SerialPortExDlg.cpp,v 1.8 2006/10/05 22:17:03 tvlaar Exp $
//=============================================================================


//=============================================================================
//
// SerialPortExDlg.
// This example illustrates how users can transmit and receive characters by 
// using the camera's serial buffer system.
// 
// This example creates the camera context and does the following:
//    Create a camera context handle,
//    Allocate a GUI handle to be used in all successive calls,
//    Display the camera selection dialog,
//    Initializes the selected camera on the bus and associates it with the given context,
//    Check to make sure that the serial port is actually supported,
//    Create a thread to receive data and display the data. 
//
// OnSendButton() is used to transmit data out of the camera's serial port based on user input,
//
// The receiveLoop() thread is used to get the connection parameters from the camera, 
//    update the dialog, verify the receive buffer status and determine the 
//    amount of data to be read, read the data and display the data in the window
//
// Users can use 'Set Register' button to set the serial port register values, 
// and use 'Get Register' button to get the serial port register values
//
//=============================================================================


//=============================================================================
// System Includes
//=============================================================================
#include "stdafx.h"
#include "math.h"

//=============================================================================
// Project Includes
//=============================================================================
#include "SerialPortEx.h"
#include "SerialPortExDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//=============================================================================
// Macro Definitions
//=============================================================================
#define _HANDLE_ERROR( error, function ) \
   if( error != FLYCAPTURE_OK ) \
   { \
      char szLocalBuffer[255]; \
      sprintf( szLocalBuffer, "%s(%d): %s (near line %d of %s)", function, \
      error, \
      flycaptureErrorToString( error ), __LINE__, __FILE__ ); \
      MessageBox(szLocalBuffer, "FlycaptureError", MB_OK); \
   } \

#define SERIAL_MODE_REG                0x2000
#define SERIAL_CONTROL_REG             0x2004
#define RECEIVE_BUFFER_STATUS_CONTROL  0x2008
#define TRANSMIT_BUFFER_STATUS_CONTROL 0x200C
#define SIO_DATA_REGISTER              0x2100
#define SIO_DATA_REGISTER_ALIAS        0x2104


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
   CAboutDlg();
   
   // Dialog Data
   //{{AFX_DATA(CAboutDlg)
   enum { IDD = IDD_ABOUTBOX };
   //}}AFX_DATA
   
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CAboutDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL
   
   // Implementation
protected:
   //{{AFX_MSG(CAboutDlg)
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
   //{{AFX_DATA_INIT(CAboutDlg)
   //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAboutDlg)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerialPortExDlg dialog

CSerialPortExDlg::CSerialPortExDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSerialPortExDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CSerialPortExDlg)
	m_csSendData = _T("");
	m_csReceiveData = _T("");
	m_csRegister = _T("");
	m_csValue_0_7 = _T("");
	m_csValue_16_23 = _T("");
	m_csValue_24_31 = _T("");
	m_csValue_8_15 = _T("");
	m_boolBroadcast = FALSE;
	//}}AFX_DATA_INIT
   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSerialPortExDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSerialPortExDlg)
   DDX_Text(pDX, IDC_SEND_WINDOW, m_csSendData);
	DDV_MaxChars(pDX, m_csSendData, 255);
   DDX_Text(pDX, IDC_RECEIVE_WINDOW, m_csReceiveData);
	DDX_Text(pDX, IDC_EDIT_REGISTER, m_csRegister);
	DDX_Text(pDX, IDC_EDIT_0_7, m_csValue_0_7);
	DDX_Text(pDX, IDC_EDIT_16_23, m_csValue_16_23);
	DDX_Text(pDX, IDC_EDIT_24_31, m_csValue_24_31);
	DDX_Text(pDX, IDC_EDIT_8_15, m_csValue_8_15);
	DDX_Check(pDX, IDC_CHECK_BROADCAST, m_boolBroadcast);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSerialPortExDlg, CDialog)
//{{AFX_MSG_MAP(CSerialPortExDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_SEND_BUTTON, OnSendButton)
	ON_BN_CLICKED(IDC_BUTTON_GET_REGISTER, OnButtonGetRegister)
	ON_BN_CLICKED(IDC_BUTTON_SET_REGISTER, OnButtonSetRegister)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
//
// Name: OnInitDialog()
//
// Description:
//   Create a camera context handle,
//   Allocate a GUI handle to be used in all successive calls,
//   Display the camera selection dialog,
//   Initializes the selected camera on the bus and associates it with the given context,
//   Check to make sure that the serial port is actually supported,
//   Create a thread to receive data and display the data. 
//
BOOL CSerialPortExDlg::OnInitDialog()
{
   CDialog::OnInitDialog();
   
   // Add "About..." menu item to system menu.
   
   // IDM_ABOUTBOX must be in the system command range.
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);
   
   CMenu* pSysMenu = GetSystemMenu(FALSE);
   if (pSysMenu != NULL)
   {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if (!strAboutMenu.IsEmpty())
      {
	 pSysMenu->AppendMenu(MF_SEPARATOR);
	 pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
   }
   
   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);			// Set big icon
   SetIcon(m_hIcon, FALSE);		// Set small icon

   FlyCaptureError error;
   unsigned long   ulValue;
   unsigned long   ulSerialNumber;
   INT_PTR         ipDialogStatus;

   // Create a camera context handle.
   error = flycaptureCreateContext(&m_context);
   _HANDLE_ERROR(error, "flycaptureCreateContext");
   if(error != FLYCAPTURE_OK)
   {
      return FALSE;
   }

   //Allocate a GUI handle to be used in all successive calls.
   CameraGUIError    guierror;
   guierror = ::pgrcamguiCreateContext( &m_guicontext );
   if( guierror != PGRCAMGUI_OK )
   {
      return FALSE;
   }
   
   //Display the camera selection dialog.
   guierror = ::pgrcamguiShowCameraSelectionModal(
      m_guicontext,
      m_context,
      &ulSerialNumber,
      &ipDialogStatus );
   if( guierror != PGRCAMGUI_OK )
   {
      return FALSE;
   }

   //Initializes the selected camera on the bus and associates it with the given context
   error = flycaptureInitializeFromSerialNumber(m_context, ulSerialNumber);
   _HANDLE_ERROR(error, "flycaptureInitializeFromSerialNumber");
   if(error != FLYCAPTURE_OK)
   {
      return FALSE;
   }

   //
   // Check to make sure that the serial port is actually supported
   //
   error = flycaptureGetCameraRegister(m_context,
      SERIAL_MODE_REG,
      &ulValue);

   if(error != FLYCAPTURE_OK ||
      ulValue == 0 )
   {
      MessageBox("This camera does not have support for serial "
	 "transactions as supported by this software.  Make sure "
	 "that the version of firmware is late enough.",
	 "Camera does not support serial port",
	 MB_OK);
      return FALSE;
   }

   //
   // Begin the receiving thread.
   //
   AfxBeginThread(threadReceive, this);

   
   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSerialPortExDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   if ((nID & 0xFFF0) == IDM_ABOUTBOX)
   {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   }
   else
   {
      CDialog::OnSysCommand(nID, lParam);
   }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CSerialPortExDlg::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting
      
      SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
      
      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;
      
      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSerialPortExDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}


//-----------------------------------------------------------------------------
//
// Name: OnSendButton()
//
// Description:
//   Verify the transmit buffer and send the data out ot the camera
//
void 
CSerialPortExDlg::OnSendButton() 
{
   UpdateData(TRUE);

   FlyCaptureError error;
   unsigned long   ulValue;

   //
   // Make sure that serial transmission is enabled
   //
   error = flycaptureGetCameraRegister(m_context, SERIAL_CONTROL_REG, &ulValue);
   _HANDLE_ERROR(error, "flycaptureGetCameraRegister");
   if(!(ulValue & 0x40000000))
   {
      ulValue = ulValue | 0x40000000;
      flycaptureSetCameraRegister(m_context, SERIAL_CONTROL_REG, ulValue);
   }

   //
   // Verify that the transmit buffer is ready.  Wait and try again if it isn't
   //
   int nRegCheckCount = 0;
   do
   {
      if( nRegCheckCount > 0 )
      {
	 Sleep(100);
      }

      error = flycaptureGetCameraRegister(m_context, 
	 SERIAL_CONTROL_REG, 
	 &ulValue);
      _HANDLE_ERROR(error, "flycaptureGetCameraRegister");
      nRegCheckCount++;
   } while(!(ulValue & 0x00800000) && nRegCheckCount<3);

   if(!(ulValue & 0x00800000))
   {
      MessageBox("Transmit buffer is not ready","ERROR",MB_OK);
   }

   //
   // Load the data into the buffer.  If it is only a single quadlet 
   // (4 bytes)or less, use the standard register write mechanism.  Otherwise, 
   // use the block write mechanism.
   //
   // Note that both mechanisms write in blocks of 4 bytes (quadlets) to
   // the buffer - it is not necessary to transmit all of the data but any 
   // data that isn't transmitted is lost.
   //
   // Note that the register that is being written to using the 
   // flycaptureWriteRegisterBlock call is an absolute offset.  This address
   // can be calculated by adding 0xFFFF F0F0 0000 to the register that is
   // being read - in this case 0x2104.  
   //
   if( m_csSendData.GetLength() <= 4 )
   {
      unsigned long szBuffer;
      char *pszBuffer = (char *)&szBuffer;
      sprintf(pszBuffer,"%s",m_csSendData);

      error = flycaptureSetCameraRegister(m_context,
	 SIO_DATA_REGISTER,
	 szBuffer);
      _HANDLE_ERROR(error, "flycaptureSetCameraRegister");
   }
   else
   {
      error = flycaptureWriteRegisterBlock(m_context,
	 0xFFFF,
	 0xF0F02104,
	 (unsigned long *)m_csSendData.GetBuffer(255),
	 // make sure that a multiple of 4 bytes are being written.
	 (unsigned long)ceil((double)m_csSendData.GetLength()/(double)4.0));
      _HANDLE_ERROR(error, "flycaptureWriteRegisterBlock");
   }

   //
   // Verify that the buffer is storing the data
   //
   error = flycaptureGetCameraRegister(m_context,
      TRANSMIT_BUFFER_STATUS_CONTROL,
      &ulValue);
   _HANDLE_ERROR(error, "flycaptureGetCameraRegister");

   if(((ulValue>>24) & 0xFF) != 
      (unsigned long)(4*ceil((double)m_csSendData.GetLength()/(double)4.0)))
   {
      // buffer isn't storing enough data.
   }

   //
   // Send the data out
   //
   error = flycaptureSetCameraRegister(m_context,
      TRANSMIT_BUFFER_STATUS_CONTROL,
      ulValue);
   _HANDLE_ERROR(error, "flycaptureSetCameraRegister");
}

UINT 
CSerialPortExDlg::threadReceive( void* pParam )
{
   CSerialPortExDlg* pDlg = (CSerialPortExDlg*)pParam;

   UINT ret = pDlg->receiveLoop();

   return ret; 
}


//-----------------------------------------------------------------------------
//
// Name: updateConnectionParameters()
//
// Description:
//   Get connection parameters from the camera and update the dialog
//
void
CSerialPortExDlg::updateConnectionParameters()
{
   FlyCaptureError error;
   unsigned long   ulValue;

   error = flycaptureGetCameraRegister(m_context,
      SERIAL_MODE_REG,
      &ulValue);
   _HANDLE_ERROR(error, "flycaptureGetCameraRegister");

   //
   // Determine Baud Rate
   //
   switch( ulValue>>24 )
   {
   case 0:
      SetDlgItemText(IDC_BAUD_RATE,"300");
      break;
   case 1:
      SetDlgItemText(IDC_BAUD_RATE,"600");
      break;
   case 2:
      SetDlgItemText(IDC_BAUD_RATE,"1200");
      break;
   case 3:
      SetDlgItemText(IDC_BAUD_RATE,"2400");
      break;
   case 4:
      SetDlgItemText(IDC_BAUD_RATE,"4800");
      break;
   case 5:
      SetDlgItemText(IDC_BAUD_RATE,"9600");
      break;
   case 6:
      SetDlgItemText(IDC_BAUD_RATE,"19200");
      break;
   case 7:
      SetDlgItemText(IDC_BAUD_RATE,"38400");
      break;
   case 8:
      SetDlgItemText(IDC_BAUD_RATE,"57600");
      break;
   case 9:
      SetDlgItemText(IDC_BAUD_RATE,"1152000");
      break;
   case 10:
      SetDlgItemText(IDC_BAUD_RATE,"2304000");
      break;
   default:
      SetDlgItemText(IDC_BAUD_RATE,"Unknown");
      break;
   }

   //
   // Determine character length
   //
   switch((ulValue>>16)&0xFF)
   {
   case 7:
      SetDlgItemText(IDC_CHAR_LENGTH,"7");
      break;
   case 8:
      SetDlgItemText(IDC_CHAR_LENGTH,"8");
      break;
   default:
      SetDlgItemText(IDC_CHAR_LENGTH,"Unknown");
      break;
   }

   //
   // Determine parity
   //
   switch((ulValue>>14)&0x3)
   {
   case 0:
      SetDlgItemText(IDC_PARITY,"None");
      break;
   case 1:
      SetDlgItemText(IDC_PARITY,"Odd");
      break;
   case 2:
      SetDlgItemText(IDC_PARITY,"Even");
      break;
   default:
      SetDlgItemText(IDC_PARITY,"Unknown");
      break;
   }
   
   //
   // Determine stop bits
   //
   switch((ulValue>>12)&0x3)
   {
   case 0:
      SetDlgItemText(IDC_STOP_BITS,"1");
      break;
   case 1:
      SetDlgItemText(IDC_STOP_BITS,"1.5");
      break;
   case 2:
      SetDlgItemText(IDC_STOP_BITS,"2");
      break;
   default:
      SetDlgItemText(IDC_STOP_BITS,"Unknown");
      break;
   }

   //
   // Set buffer size
   //
   char szBufferSize[8];
   sprintf(szBufferSize,"%d",ulValue&0xFF);
   SetDlgItemText(IDC_BUFFER_SIZE,szBufferSize);
}


//-----------------------------------------------------------------------------
//
// Name: receiveLoop()
//
// Description:
//   Keep receiving data and displaying the data
//
UINT 
CSerialPortExDlg::receiveLoop()
{
   FlyCaptureError error;
   char            szBuffer[1024];
   unsigned long   ulValueA;
   unsigned long   ulValueB;
   int             nCount =0;

   while(true)
   {
      Sleep(100);

      //
      // update the connection parameters.
      //
      updateConnectionParameters();
	
      // get the text that is currently in the receive window
      //
      CString csBuffer;
      GetDlgItemText(IDC_RECEIVE_WINDOW,csBuffer);

      //
      // make sure that receive is enabled - if it isn't, enable it now.
      //
      error = flycaptureGetCameraRegister(m_context,
	 SERIAL_CONTROL_REG,
	 &ulValueA);
      _HANDLE_ERROR( error, "flycaptureGetCameraRegister");
      if(!(ulValueA & 0x80000000))
      {
	 ulValueA = ulValueA | 0x80000000;
	 error = flycaptureSetCameraRegister(m_context, SERIAL_CONTROL_REG, ulValueA);
	 _HANDLE_ERROR( error, "flycaptureSetCameraRegister");
	 sprintf(szBuffer,"%s%3d. Enabled receive\r\n", csBuffer.Right(255), nCount);
	 goto UPDATE_DISPLAY;
      }

      //
      // verify that there are no receive data framing errors.
      //
      error = flycaptureGetCameraRegister(m_context,
	 SERIAL_CONTROL_REG,
	 &ulValueA);
      _HANDLE_ERROR( error, "flycaptureGetCameraRegister");
      if(ulValueA & 0x000E0000)
      {
	 //
	 // we have had one of three data receive errors
	 //
	 sprintf(szBuffer,"%s%3d. Data receive error (%x)\r\n", 
	    csBuffer.Right(255), 
	    nCount, 
	    ulValueA);

	 //
	 // clear any errors by writing the appropriate bits back to 0.
	 //
	 error = flycaptureSetCameraRegister(m_context,
	    SERIAL_CONTROL_REG,
	    (ulValueA & 0xFFF1FFFF));
	 _HANDLE_ERROR( error, "flycaptureSetCameraRegister");
	 goto UPDATE_DISPLAY;
      }

      //
      // verify that the recieve buffer is ready to be read
      //
      error = flycaptureGetCameraRegister(m_context,
	 SERIAL_CONTROL_REG,
	 &ulValueA);
      _HANDLE_ERROR( error, "flycaptureGetCameraRegister");
      if( !(ulValueA & 0x80200000))
      {
	 //
	 // buffer is not ready to be read.
	 //
	 sprintf(szBuffer,"%s%3d. receive buffer is not ready\r\n", 
	    csBuffer.Right(255), nCount);
	 goto UPDATE_DISPLAY;
      }

      //
      // determine the amount of data that is ready to be read
      //
      error = flycaptureGetCameraRegister(m_context,
	 RECEIVE_BUFFER_STATUS_CONTROL ,
	 &ulValueA);
      _HANDLE_ERROR( error, "flycaptureGetCameraRegister");
      if(!(ulValueA && 0xFF000000))
      {
	 continue; 
      }

      //
      // send the characters to the data access register
      //
      error = flycaptureSetCameraRegister(m_context,
	 RECEIVE_BUFFER_STATUS_CONTROL,
	 (ulValueA>>8 & 0x00FF0000));
      _HANDLE_ERROR( error, "flycaptureSetCameraRegister");

      //
      // verify that the characters are ready to be read
      //
      error = flycaptureGetCameraRegister(m_context,
	 RECEIVE_BUFFER_STATUS_CONTROL,
	 &ulValueB);
      _HANDLE_ERROR( error, "flycaptureGetCameraRegister");

      if(((ulValueA>>8)&0x00FF0000)!=(ulValueB&0x00FF0000))
      {
	 continue;
      }

      //
      // If there is one quadlet or less (4 bytes or less) to read, then read
      // them using a standard register read. Otherwise, read the data out from 
      // the data access registers using a block read.
      //
      // Note that the register that is being read from using the 
      // flycaptureReadRegisterBlock call is an absolute offset.  This address
      // can be calculated by adding 0xFFFF F0F0 0000 to the register that is
      // being read - in this case 0x2104.  
      //
      // Also note that the flycaptureReadRegisterBlock only allows quadlets to
      // be read (i.e. mulitples of 4 bytes).  This shouldn't pose a problem but 
      // is the reason for the call to 'ceil'
      //
      if((ulValueA>>24)<=4)
      {
	 error = flycaptureGetCameraRegister(m_context,
	    SIO_DATA_REGISTER,
	    &ulValueA);
	 _HANDLE_ERROR( error, "flycaptureSetCameraRegister");
	 
	 sprintf(szBuffer,"%s%3d. %c%c%c%c\r\n", csBuffer.Right(255), nCount,
	    (ulValueA>>0)&0xFF,
	    (ulValueA>>8)&0xFF,
	    (ulValueA>>16)&0xFF,
	    (ulValueA>>24)&0xFF);
      }
      else
      {
	 char szLocalBuffer[255];
	 error = flycaptureReadRegisterBlock(m_context,
	    0xFFFF,
	    0xF0F02104,
	    (unsigned long*)&szLocalBuffer[0],
	    (unsigned long)ceil((double)(ulValueA>>24)/(double)4.0));
	 _HANDLE_ERROR( error, "flycaptureReadRegisterBlock");
	
	 //
	 // put an end character on the buffer just in case some extra garbage was
	 // read as a result of reading in multiples of 4.
	 //
	 szLocalBuffer[ulValueA>>24]='\0';
	 sprintf(szBuffer,"%s%3d. %s\r\n", csBuffer.Right(255), nCount, szLocalBuffer);
      }


UPDATE_DISPLAY:
      //
      // write the text to the window and then scroll down to the latest line.
      //
      SetDlgItemText(IDC_RECEIVE_WINDOW,szBuffer);
      CEdit *pEdit = (CEdit*)GetDlgItem(IDC_RECEIVE_WINDOW);
      pEdit->LineScroll(pEdit->GetLineCount()-1,0);
      nCount++;
   }
}


//-----------------------------------------------------------------------------
//
// Name: OnButtonGetRegister()
//    
// Description:
//    Press 'Get Register' button to get the serial port register values
//    The register address is in 'Register' edit box
//    
void CSerialPortExDlg::OnButtonGetRegister() 
//Get the serial port register values using API function
{
   FlyCaptureError error;
   unsigned long ulRegister = 0X800;
   unsigned long ulValue;
   
   char*	pszTopString;
   char	pszTempString[100];  
   
   UpdateData( TRUE );
   
   //Get the register address from the edit box
   sprintf( pszTempString, "0x%s", (LPCTSTR)m_csRegister );
   ulRegister = strtoul( pszTempString, &pszTopString, 0 );
   
   //Get the 4 bytes register values from the camera
   error = flycaptureGetCameraRegister(m_context, ulRegister, &ulValue);
   _HANDLE_ERROR( error, "flycaptureGetCameraRegister");
   
   //Display the 4 bytes values in the edit boxes
   sprintf( pszTempString,"%02X", (ulValue>>24) & 0xFF );
   m_csValue_0_7 = pszTempString;
   
   sprintf( pszTempString,"%02X", (ulValue>>16) & 0xFF );
   m_csValue_8_15 = pszTempString;
   
   sprintf( pszTempString,"%02X", (ulValue>>8) & 0xFF );
   m_csValue_16_23 = pszTempString;
   
   sprintf( pszTempString,"%02X", (ulValue>>0) & 0xFF );
   m_csValue_24_31 = pszTempString;
   
   UpdateData( FALSE );
}



//-----------------------------------------------------------------------------
//
// Name: OnButtonSetRegister()
//    
// Description:
//    Press 'Set Register' button to set the serial port register values
//    The register address is in 'Register' edit box
//
void CSerialPortExDlg::OnButtonSetRegister() 
{
   FlyCaptureError error;
   unsigned long ulRegister;
   unsigned long ulValue;
   char*	pszTopString;
   char	pszTempString[ 100 ];
   
   UpdateData( TRUE );
   
   //Get the register address from the edit box
   sprintf ( pszTempString, "0x%s", (LPCTSTR)m_csRegister );
   ulRegister = strtol( pszTempString, &pszTopString, 0 );
   
   //Get the 4 bytes register values from the edit boxes 
   sprintf(pszTempString, "0x%02s%02s%02s%02s", (LPCTSTR)m_csValue_0_7, 
      (LPCTSTR)m_csValue_8_15, 
      (LPCTSTR)m_csValue_16_23, 
      (LPCTSTR)m_csValue_24_31 ); 
   ulValue = strtoul( pszTempString, &pszTopString, 0 );
   
   //Check the Broadcast flag, if it is true, set any registers for all the cameras on the bus
   if (m_boolBroadcast)
      error = flycaptureSetCameraRegister(m_context, ulRegister, ulValue);
   else
      error = flycaptureSetCameraRegisterBroadcast(m_context, ulRegister, ulValue);
   
   _HANDLE_ERROR( error, "flycaptureGetCameraRegister");
   
   
}
