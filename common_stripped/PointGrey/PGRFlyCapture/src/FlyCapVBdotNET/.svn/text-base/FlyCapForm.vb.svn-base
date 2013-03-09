'=============================================================================
' Copyright © 2006 Point Grey Research, Inc. All Rights Reserved.
' 
' This software is the confidential and proprietary information of Point
' Grey Research, Inc. ("Confidential Information").  You shall not
' disclose such Confidential Information and shall use it only in
' accordance with the terms of the license agreement you entered into
' with Point Grey Research, Inc. (PGR).
' 
' PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
' SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
' IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
' PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
' SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
' THIS SOFTWARE OR ITS DERIVATIVES.
'=============================================================================
'=============================================================================
' $Id: FlyCapForm.vb,v 1.8 2008/01/10 22:36:32 demos Exp $
'=============================================================================


Option Strict Off
Option Explicit On 

Friend Class FlyCapForm
	Inherits System.Windows.Forms.Form
#Region "Windows Form Designer generated code "
	Public Sub New()
		MyBase.New()
		If m_vb6FormDefInstance Is Nothing Then
			If m_InitializingDefInstance Then
				m_vb6FormDefInstance = Me
			Else
				Try 
					'For the start-up form, the first instance created is the default instance.
					If System.Reflection.Assembly.GetExecutingAssembly.EntryPoint.DeclaringType Is Me.GetType Then
						m_vb6FormDefInstance = Me
					End If
				Catch
				End Try
			End If
		End If
		'This call is required by the Windows Form Designer.
		InitializeComponent()
	End Sub
	'Form overrides dispose to clean up the component list.
	Protected Overloads Overrides Sub Dispose(ByVal Disposing As Boolean)
		If Disposing Then
			If Not components Is Nothing Then
				components.Dispose()
			End If
		End If
		MyBase.Dispose(Disposing)
	End Sub
	'Required by the Windows Form Designer
	Private components As System.ComponentModel.IContainer
	Public ToolTip1 As System.Windows.Forms.ToolTip
	Public WithEvents GetCameraInfo As System.Windows.Forms.Button
	Public WithEvents FlycaptureImageBox As System.Windows.Forms.PictureBox
    Public WithEvents SaveImage As System.Windows.Forms.Button
	Public WithEvents ToggleSettingsDialog As System.Windows.Forms.Button
    Public WithEvents SaveImageMenuItem As System.Windows.Forms.MenuItem
	Public WithEvents ExitMenuItem As System.Windows.Forms.MenuItem
	Public WithEvents FileMenu As System.Windows.Forms.MenuItem
	Public MainMenu1 As System.Windows.Forms.MainMenu
	'NOTE: The following procedure is required by the Windows Form Designer
	'It can be modified using the Windows Form Designer.
	'Do not modify it using the code editor.
    Friend WithEvents GrabImageButton As System.Windows.Forms.CheckBox
    Friend WithEvents NewContext As System.Windows.Forms.Button
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Dim resources As System.Resources.ResourceManager = New System.Resources.ResourceManager(GetType(FlyCapForm))
        Me.ToolTip1 = New System.Windows.Forms.ToolTip(Me.components)
        Me.GetCameraInfo = New System.Windows.Forms.Button
        Me.SaveImage = New System.Windows.Forms.Button
        Me.ToggleSettingsDialog = New System.Windows.Forms.Button
        Me.FlycaptureImageBox = New System.Windows.Forms.PictureBox
        Me.MainMenu1 = New System.Windows.Forms.MainMenu
        Me.FileMenu = New System.Windows.Forms.MenuItem
        Me.SaveImageMenuItem = New System.Windows.Forms.MenuItem
        Me.ExitMenuItem = New System.Windows.Forms.MenuItem
        Me.GrabImageButton = New System.Windows.Forms.CheckBox
        Me.NewContext = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'GetCameraInfo
        '
        Me.GetCameraInfo.BackColor = System.Drawing.SystemColors.Control
        Me.GetCameraInfo.Cursor = System.Windows.Forms.Cursors.Default
        Me.GetCameraInfo.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.GetCameraInfo.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GetCameraInfo.Image = CType(resources.GetObject("GetCameraInfo.Image"), System.Drawing.Image)
        Me.GetCameraInfo.Location = New System.Drawing.Point(96, 0)
        Me.GetCameraInfo.Name = "GetCameraInfo"
        Me.GetCameraInfo.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.GetCameraInfo.Size = New System.Drawing.Size(24, 24)
        Me.GetCameraInfo.TabIndex = 4
        Me.GetCameraInfo.TextAlign = System.Drawing.ContentAlignment.BottomCenter
        Me.ToolTip1.SetToolTip(Me.GetCameraInfo, "Show camera information")
        '
        'SaveImage
        '
        Me.SaveImage.BackColor = System.Drawing.SystemColors.Control
        Me.SaveImage.Cursor = System.Windows.Forms.Cursors.Default
        Me.SaveImage.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.SaveImage.ForeColor = System.Drawing.SystemColors.ControlText
        Me.SaveImage.Image = CType(resources.GetObject("SaveImage.Image"), System.Drawing.Image)
        Me.SaveImage.Location = New System.Drawing.Point(24, 0)
        Me.SaveImage.Name = "SaveImage"
        Me.SaveImage.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.SaveImage.Size = New System.Drawing.Size(24, 24)
        Me.SaveImage.TabIndex = 1
        Me.SaveImage.TextAlign = System.Drawing.ContentAlignment.BottomCenter
        Me.ToolTip1.SetToolTip(Me.SaveImage, "Save an image to disk")
        '
        'ToggleSettingsDialog
        '
        Me.ToggleSettingsDialog.BackColor = System.Drawing.SystemColors.Control
        Me.ToggleSettingsDialog.Cursor = System.Windows.Forms.Cursors.Default
        Me.ToggleSettingsDialog.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.ToggleSettingsDialog.ForeColor = System.Drawing.SystemColors.ControlText
        Me.ToggleSettingsDialog.Image = CType(resources.GetObject("ToggleSettingsDialog.Image"), System.Drawing.Image)
        Me.ToggleSettingsDialog.Location = New System.Drawing.Point(48, 0)
        Me.ToggleSettingsDialog.Name = "ToggleSettingsDialog"
        Me.ToggleSettingsDialog.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.ToggleSettingsDialog.Size = New System.Drawing.Size(24, 24)
        Me.ToggleSettingsDialog.TabIndex = 2
        Me.ToggleSettingsDialog.TextAlign = System.Drawing.ContentAlignment.BottomCenter
        Me.ToolTip1.SetToolTip(Me.ToggleSettingsDialog, "Show the camera settings dialog")
        '
        'FlycaptureImageBox
        '
        Me.FlycaptureImageBox.BackColor = System.Drawing.SystemColors.Control
        Me.FlycaptureImageBox.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.FlycaptureImageBox.Cursor = System.Windows.Forms.Cursors.Default
        Me.FlycaptureImageBox.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.FlycaptureImageBox.ForeColor = System.Drawing.SystemColors.ControlText
        Me.FlycaptureImageBox.Location = New System.Drawing.Point(0, 24)
        Me.FlycaptureImageBox.Name = "FlycaptureImageBox"
        Me.FlycaptureImageBox.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.FlycaptureImageBox.Size = New System.Drawing.Size(640, 480)
        Me.FlycaptureImageBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize
        Me.FlycaptureImageBox.TabIndex = 3
        Me.FlycaptureImageBox.TabStop = False
        '
        'MainMenu1
        '
        Me.MainMenu1.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.FileMenu})
        '
        'FileMenu
        '
        Me.FileMenu.Index = 0
        Me.FileMenu.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.SaveImageMenuItem, Me.ExitMenuItem})
        Me.FileMenu.Text = "File"
        '
        'SaveImageMenuItem
        '
        Me.SaveImageMenuItem.Index = 0
        Me.SaveImageMenuItem.Text = "Save Image"
        '
        'ExitMenuItem
        '
        Me.ExitMenuItem.Index = 1
        Me.ExitMenuItem.Text = "Exit"
        '
        'GrabImageButton
        '
        Me.GrabImageButton.Appearance = System.Windows.Forms.Appearance.Button
        Me.GrabImageButton.Cursor = System.Windows.Forms.Cursors.Default
        Me.GrabImageButton.Image = CType(resources.GetObject("GrabImageButton.Image"), System.Drawing.Image)
        Me.GrabImageButton.Location = New System.Drawing.Point(72, 0)
        Me.GrabImageButton.Name = "GrabImageButton"
        Me.GrabImageButton.Size = New System.Drawing.Size(24, 24)
        Me.GrabImageButton.TabIndex = 3
        '
        'NewContext
        '
        Me.NewContext.Image = CType(resources.GetObject("NewContext.Image"), System.Drawing.Image)
        Me.NewContext.Location = New System.Drawing.Point(0, 0)
        Me.NewContext.Name = "NewContext"
        Me.NewContext.Size = New System.Drawing.Size(24, 24)
        Me.NewContext.TabIndex = 5
        '
        'FlyCapForm
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.AutoScroll = True
        Me.BackColor = System.Drawing.SystemColors.Control
        Me.ClientSize = New System.Drawing.Size(648, 505)
        Me.Controls.Add(Me.NewContext)
        Me.Controls.Add(Me.GrabImageButton)
        Me.Controls.Add(Me.GetCameraInfo)
        Me.Controls.Add(Me.FlycaptureImageBox)
        Me.Controls.Add(Me.ToggleSettingsDialog)
        Me.Controls.Add(Me.SaveImage)
        Me.Cursor = System.Windows.Forms.Cursors.Default
        Me.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Location = New System.Drawing.Point(11, 57)
        Me.Menu = Me.MainMenu1
        Me.Name = "FlyCapForm"
        Me.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Text = "FlyCapVBdotNET"
        Me.ResumeLayout(False)

    End Sub
#End Region 
#Region "Upgrade Support "
	Private Shared m_vb6FormDefInstance As FlyCapForm
	Private Shared m_InitializingDefInstance As Boolean
	Public Shared Property DefInstance() As FlyCapForm
		Get
			If m_vb6FormDefInstance Is Nothing OrElse m_vb6FormDefInstance.IsDisposed Then
				m_InitializingDefInstance = True
				m_vb6FormDefInstance = New FlyCapForm()
				m_InitializingDefInstance = False
			End If
			DefInstance = m_vb6FormDefInstance
		End Get
		Set
			m_vb6FormDefInstance = Value
		End Set
	End Property
#End Region 

    '==========================================================================
    ' This code provides an example of how to access PGR's FlyCapture DLLs from
    ' a VB .NET environment. The examples include the simple data types such as 
    ' booleans, ints, longs, char arrays, etc, as well as structures, 
    ' enumerations, and most importantly, images.
    '
    ' Note that this is only an example and that some actions have alternative
    ' solutions.  For example, instead of using delegate callback functions,
    ' one could use explicit threading.  Also, instead of displaying the image
    ' as a Bitmap, one may want to look at the function "BitBlt".  This method
    ' was not chosen in this example because of issues with repainting the 
    ' image after another window has been dragged over it.
	'
    ' To use this example, you simply need to place the two DLLs 
    ' (PGRFlycapture.dll and pgrflycapturegui.dll) into the VB directory where 
    ' you are running, or in the system32 directory, so they can be found.
	'
    ' To use any of the other API calls, it should be straightforward to use 
    ' the examples provided herein, to help you to generate the appropriate 
    ' data structures and prototypes. Further information regarding conversion 
    ' of data types can be found in the following Microsoft article entitled 
    ' "How To Write C DLLs and Call Them from Visual Basic":
	'     http://support.microsoft.com/kb/106553/EN-US/
    ' You should not need to use that, however, as this code should contain at
    ' least one example for each of the fundamental data types.
	'
    ' It may be necessary to define other "converted" FlyCapture structures
    ' or enumerations depending on your needs and which DLL functions you
    ' require.  The converted FlyCapture definitions below should be a 
    ' sufficient list of examples of how to do this.
    '
    ' In order to run the executable after compiling, you must change the
    ' .NET Security settings as follows:
    '   Start->Settings->Control Panel->Administrative Tools->
    '   Microsoft .NET Framework 1.1 Wizards->Adjust .NET Security->
    '   Make changes to this computer->My Computer->Full Trust
    '
    ' If you are running Microsoft Development Environment 2003 or previous
    ' versions, then a post-build command is not supported.  In order to 
    ' workaround this, the FlyCapVBdotNETPostBuild project in this solution
    ' is dependent on the FlyCapVBdotNET and will also execute the neccessary
    ' post-build commands.  When building this solution, use the 
    ' FlyCapVBdotNETPostBuild project
    '==========================================================================

    ' ===================== Delegate (Callback) Functions =====================
    ' GrabImageLoop function pointer
    Public Delegate Sub GrabImageLoopCallback()
    ' New instance of the GrabImageLoop function pointer
    Public delegGrabLoop As New GrabImageLoopCallback(AddressOf GrabImageLoop)
    '==========================================================================
   
    ' Exit the form from the menu rather than the X in the upper right corner
    Public Sub ExitMenuItem_Click(ByVal eventSender As System.Object, _
                                  ByVal eventArgs As System.EventArgs) _
                                  Handles ExitMenuItem.Click

        Me.Close()

    End Sub

    ' The contexts must be destroyed before exiting the app.
    Private Sub FlyCapDestroyContext()

        Dim ret As Integer
        Dim sMsg As String

        If bValidContexts Then
            ret = flycaptureDestroyContext(flycapContext)
            If (ret <> 0) Then
                sMsg = "Error in flycaptureDestroyContext: " & _
                    flycaptureErrorToString(ret)
                MsgBox(sMsg)
            End If

            ret = pgrcamguiDestroyContext(camguiContext)
            If (ret <> CameraGUIError.PGRCAMGUI_OK) Then
                sMsg = "Return value: " & flycaptureErrorToString(ret)
                MsgBox(sMsg)
            End If
        End If

        bValidContexts = False

    End Sub

    ' All the necessary initialization to set up the contexts. After this, 
    ' we are ready for grabbing, and any other direct interaction with the 
    ' live context, such as setting parameters, retrieving camera 
    ' information, etc.
    Private Function Flycapture_Initialization()

        Dim ret As Integer
        Dim sMsg As String
        Dim serial As Integer
        Dim dialogStatus As Integer

		' Cross Thread errors occur when debugging through Visual Studio.
		' This is the recommended fix from Visual Studio.
        Control.CheckForIllegalCrossThreadCalls = False

        ' Create the flycap context
        ret = flycaptureCreateContext(flycapContext)
        bValidContexts = True
        If ret <> 0 Then
            sMsg = "flycaptureCreateContext Error: " & _
                flycaptureErrorToString(ret)
            MsgBox(sMsg)
            Return -1
        End If

        ' create the context that controls the camera dialogs
        ret = pgrcamguiCreateContext(camguiContext)
        If ret <> CameraGUIError.PGRCAMGUI_OK Then
            sMsg = "pgrcamguiCreateContext Error: " & _
                flycaptureErrorToString(ret)
            MsgBox(sMsg)
            Return -1
        End If

        ' Show the camera selection dialog - allow the user to choose which
        ' camera to interact with
        ret = pgrcamguiShowCameraSelectionModal(camguiContext, _
            flycapContext, serial, dialogStatus)
        ' If the Cancel button is pressed on the Camera Selection Dialog,
        ' then destoy the contexts and exit.
        If dialogStatus <> 1 Then
            Call FlyCapDestroyContext()
            Return -1
        End If
        If ret <> CameraGUIError.PGRCAMGUI_OK Then
            sMsg = "pgrcamguiShowCameraSelectionModal Error: " & _
                flycaptureErrorToString(ret)
            MsgBox(sMsg)
            Return -1
        End If

        ' Initialize the chosen camera
        ret = flycaptureInitializeFromSerialNumber(flycapContext, serial)
        If ret <> 0 Then
            sMsg = "flycaptureInitialize Error: " & _
                flycaptureErrorToString(ret)
            MsgBox(sMsg)
            Return -1
        End If

        ' Create the settings dialog for the specific camera type chosen
        ret = pgrcamguiCreateSettingsDialog(camguiContext, 0, flycapContext)
        If ret <> CameraGUIError.PGRCAMGUI_OK Then
            sMsg = "pgrcamguiCreateSettingsDialog Error: " & _
                flycaptureErrorToString(ret)
            MsgBox(sMsg)
            Return -1
        End If

        Return 0

    End Function

    ' This creates an empty bitmap for flycapRGBImage.pData to point to.
    Private Sub InitBitmapStructure(ByRef nRows As Integer, _
                                    ByRef nCols As Integer)

        Dim bmi As BITMAPINFO ' bitmap header
        Dim pvBits As IntPtr ' pointer to DIB section
        Dim nBytes As Integer

        nBytes = nRows * nCols * 3 ' for R,G,B

        bmi.bmiHeader.biSize = 40 ' sizeof(BITMAPINFOHEADER) = 40
        bmi.bmiHeader.biWidth = nCols
        bmi.bmiHeader.biHeight = nRows
        bmi.bmiHeader.biPlanes = 1
        bmi.bmiHeader.biBitCount = 24 ' three 8-bit components
        bmi.bmiHeader.biCompression = 0 ' BI_RGB = 0
        bmi.bmiHeader.biSizeImage = nBytes
        CreateDIBSection(New IntPtr(0), bmi, DIB_RGB_COLORS, pvBits, 0, 0)

        ' This is where we set the "to be converted" image data pointer to 
        ' our newly created bitmap data pointer
        flycapRGBImage.pData = pvBits

    End Sub

    ' Here we initialize the bitmap structure that will be used to display 
    ' the images and call the flycapture initialization code.
    Private Sub FlyCapForm_Load(ByVal eventSender As System.Object, _
                                ByVal eventArgs As System.EventArgs) _
                                Handles MyBase.Load

        ' Initialize the flycapture structures
        Dim ret As Integer
        ret = Flycapture_Initialization()
        If ret <> 0 Then
            Me.Close()
            Exit Sub
        End If

        ' Initialize the flycapRGBImage data pointer to a new Bitmap
        InitBitmapStructure(480, 640)

        ' Indicate that we have yet to run flycaptureStart.
        bFlyCapStarted = False
        ' Indicate that the thread need not stop if called.
        bStopThread = False

        ' Start grabbing images
        GrabImageButton.Checked = True

    End Sub

    ' Stop flycapture and destroy the contexts
    Private Sub FlyCapForm_Closing(ByVal eventSender As System.Object, _
                                   ByVal eventArgs As _
                                    System.ComponentModel.CancelEventArgs) _
                                   Handles MyBase.Closing

        Dim Cancel As Short = eventArgs.Cancel

        Dim settingsState As Short
        Dim ret As Integer

        ' Stop the grab thread if it is running
        bStopThread = True

        If bValidContexts Then
            ret = pgrcamguiGetSettingsWindowState(camguiContext, settingsState)
            If ret = 0 Then
                If settingsState = 1 Then
                    Call ToggleSettingsDialog_Click(ToggleSettingsDialog, _
                        New System.EventArgs)
                End If
            End If
        End If

        If bFlyCapStarted = True Then
            flycaptureStop(flycapContext)
        End If

        Call FlyCapDestroyContext()

        eventArgs.Cancel = Cancel

    End Sub

    ' Here is an example of retrieving string data from the pgrflycapture dll, 
    ' including string data
    Private Sub GetCameraInfo_Click(ByVal eventSender As System.Object, _
                                    ByVal eventArgs As System.EventArgs) _
                                    Handles GetCameraInfo.Click
        Dim ret As Integer
        Dim sMsg As String
        Dim flycapInfo As FlyCaptureInfo

        ret = flycaptureGetCameraInformation(flycapContext, flycapInfo)

        If ret <> 0 Then
            sMsg = "Error in flycaptureGetCameraInformation: " & _
                flycaptureErrorToString(ret)
            MsgBox(sMsg)
        Else
            If flycapInfo.CameraType = _
                    FlycaptureCameraType.FLYCAPTURE_BLACK_AND_WHITE Then
                sMsg = "B&W " & flycapInfo.pszModelString & Chr(13) & _
                    Chr(10) & "Serial#:" & Str(flycapInfo.SerialNumber)
            Else
                sMsg = "Colour " & flycapInfo.pszModelString & Chr(13) & _
                    Chr(10) & "Serial#:" & Str(flycapInfo.SerialNumber)
            End If
            MsgBox(sMsg)
        End If

    End Sub

    ' The main routine for accessing images from within the FlyCapture DLL.
    ' Starts flycapture if necessary, grabs an image, and converts it to the
    ' correct format for viewing in Windows.
    Private Function GrabImage()
        Dim ret As Integer
        Dim sMsg As String
        Dim prevRows As Integer
        Dim prevColumns As Integer
        Dim newImage As Bitmap

        ' Start flycap if necessary
        If bFlyCapStarted = False Then
            ret = flycaptureStart(flycapContext, FLYCAPTURE_VIDEOMODE_ANY, _
                FLYCAPTURE_FRAMERATE_ANY)
            If ret <> 0 Then
                sMsg = "Error in flycaptureStart: " & _
                    flycaptureErrorToString(ret)
                MsgBox(sMsg)
                Exit Function
            Else
                bFlyCapStarted = True
            End If
        End If

        ' Save the current dimensions of the image
        prevRows = flycapImage.iRows
        prevColumns = flycapImage.iCols

        ' Grab the image
        flycaptureGrabImage2(flycapContext, flycapImage)

        ' Resize the form and reinitialize the bitmap if 
        ' the image dimensions change.
        If flycapImage.iRows <> prevRows _
                Or flycapImage.iCols <> prevColumns Then
            InitBitmapStructure(flycapImage.iRows, flycapImage.iCols)
            Me.Height = flycapImage.iRows + HEIGHT_DIFFERENCE
            Me.Width = flycapImage.iCols + WIDTH_DIFFERENCE
            FlycaptureImageBox.Height = flycapRGBImage.iRows
            FlycaptureImageBox.Width = flycapRGBImage.iCols
        End If

        ' Make sure we ask for the correct image format, then convert to it.
        flycapRGBImage.pixelFormat = FLYCAPTURE_BGR
        ret = flycaptureConvertImage(flycapContext, flycapImage, _
                                     flycapRGBImage)
        If ret <> 0 Then
            sMsg = "flycaptureConvertImage error: " & _
                flycaptureErrorToString(ret)
            MsgBox(sMsg)
            Exit Function
        End If

        ' Show the new image
        newImage = New Bitmap(flycapRGBImage.iCols, flycapRGBImage.iRows, _
            flycapRGBImage.iRowInc, Imaging.PixelFormat.Format24bppRgb, _
            flycapRGBImage.pData)
        FlycaptureImageBox.Image = newImage
        FlycaptureImageBox.Refresh()

    End Function

    ' Save the converted image to disk. To save to a different file format, 
    ' select one of the implemented FlyCaptureImageFileFormat's 
    ' (enumeration defined above)
    Private Sub SaveImageMenuItem_Click(ByVal eventSender As System.Object, _
                                        ByVal eventArgs As System.EventArgs) _
                                        Handles SaveImage.Click

        Dim ret As Integer
        Dim sMsg As String
        Dim fName As String

        Dim cdlSave As New SaveFileDialog()

        cdlSave.Filter = "Windows Bitmap (*.bmp)|*.bmp"
        cdlSave.Title = "Save an image"
        cdlSave.ShowDialog()

        fName = cdlSave.FileName

        ret = flycaptureSaveImage(flycapContext, flycapRGBImage, fName, _
            FlyCaptureImageFileFormat.FLYCAPTURE_FILEFORMAT_BMP)

        If ret <> 0 Then
            sMsg = "Error in flycaptureSaveImage: " & _
                flycaptureErrorToString(ret)
            MsgBox(sMsg)
        End If

    End Sub

    ' Show/hide the PGR camera settings dialog. This dialog allows the user to
    ' set various camera parameters by interacting with a GUI object.
    Private Sub ToggleSettingsDialog_Click(ByVal eventSender As System.Object, _
                                           ByVal eventArgs As System.EventArgs) _
                                           Handles ToggleSettingsDialog.Click

        Dim ret As Integer
        Dim sMsg As String
        Dim hwnd As Integer

        hwnd = GetActiveWindow()

        ret = pgrcamguiToggleSettingsWindowState(camguiContext, hwnd)

    End Sub

    ' This is the function called from BeginInvoke. It continues to call
    ' GrabImage until the global boolean bStopThread is set to False
    Private Sub GrabImageLoop()

        Do While True
            If bStopThread = True Then
                Exit Sub
            End If
            GrabImage()
        Loop

    End Sub

    ' Start or stop the image grabbing routine with this button.
    ' Calling BeginInvoke method will start a thread from the thread pool 
    ' and will call the GrabLoop without blocking.
    Private Sub GrabImageButton_CheckedChanged(ByVal sender As System.Object, _
                                               ByVal e As System.EventArgs) _
                                        Handles GrabImageButton.CheckedChanged

        If GrabImageButton.Checked Then
            ' Start grabbing
            bStopThread = False
            delegGrabLoop.BeginInvoke(Nothing, Nothing)
        Else
            ' Stop grabbing
            bStopThread = True
        End If

    End Sub

    ' Start grabbing images with a new context (or different camera)
    Private Sub NewContext_Click(ByVal sender As System.Object, _
                                 ByVal e As System.EventArgs) _
                                 Handles NewContext.Click

        ' Stop grabbing images, destoy the context, and load a new one.
        GrabImageButton.Checked = False
        Call FlyCapDestroyContext()
        Call FlyCapForm_Load(Nothing, Nothing)

    End Sub

    Private Sub SaveImageMenuItem_Click_1(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SaveImageMenuItem.Click
        Call SaveImageMenuItem_Click(Nothing, Nothing)
    End Sub
End Class


