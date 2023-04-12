*** BEFORE STARTING THE PROJECT IN VISUAL STUDIOS MAKE SURE TO CONSIDER THESE PRECONDITIONS ***


If you get this error: Error	LNK2019	unresolved external symbol __imp__WSAStartup@8 referenced in function _init_winsock	
Follow these steps:

The problem is that winsock is not included by default in Visual Studios, so we have to link it.

-Right-click on your project in the Solution Explorer and select "Properties" from the context menu.

-In the Properties window, select "Linker" > "Input" from the left-hand menu.

-In the "Additional Dependencies" field, add "ws2_32.lib" (without quotes) to the list of libraries. (Should look similar to this: kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;ws2_32.lib;%(AdditionalDependencies))

- Click "APPLY" and "OK" to close the Properties window.

Rebuild your project.

