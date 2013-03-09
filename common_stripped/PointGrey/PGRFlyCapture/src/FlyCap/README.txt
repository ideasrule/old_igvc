Disabled Help Menu in FlyCap when built from Example Workspace
-----------------------------------------------------------------

Point Grey Research has moved to an HTMLHelp based help system.  This is 
Microsoft's currently recommended help system and provides a relatively 
flexible help framework with a well known interface. It has been integrated 
into the Camera Control Dialog via a context sensitive help button (a small 
blue and white question mark located in the bottom left corner) and into the 
FlyCap application via a new help menu. For more information on HTMLHelp, refer
to:

http://msdn.microsoft.com/library/en-us/htmlhelp/html/vsconhh1start.asp

Using HTMLHelp from within an application is a fairly simple process.  It 
requires including the header file "htmlhelp.h" and linking to the library 
"htmlhelp.lib".  These files are included by default in most recent Visual 
Studio installs, but were an optional component in older versions like Visual 
C++ 6.0.  Since some users may not have access to these files, it has been 
decided that the default configuration will disable the help menu.

If you would like this menu enabled in your build of FlyCap, you must remove 
the preprocessor definition "_PGRDIST" from the FlyCap project settings and add 
"htmlhelp.lib" to the list of libraries in the linker settings.  If you do not 
have the library or header file available, they are included in the HTML Help 
Workshop install.  This is Microsoft's HTMLHelp authoring solution.  The install 
will typically update your environment paths, but if your build environment cannot 
find the files it may be neccessary to copy them to the include and lib 
directories of your PGR Flycapture Install.For more informatio on the HTML Help 
Workshop, refer to:

http://go.microsoft.com/fwlink/?LinkId=14188

The context sensitive help button is implemented in the lower level libraries and 
so does not require linking at the end user level.  It will be available regardless.