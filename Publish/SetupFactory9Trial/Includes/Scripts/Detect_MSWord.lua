--[[
**********************************************************************************
Function:	ir_GetWordVersion()
Purpose:	Detects Microsoft Word / Word Viewer
Arguments:	None.
Returns:	Version number, 0.0.0.0 if Word / Word Viewer does not exist
**********************************************************************************
]]


function ir_GetWordVersion()

--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%                                                                %%
--%%   LOCAL VARIABLE DECLARATION - CHANGE THESE FOR EACH PROGRAM   %%
--%%                                                                %%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	local tValidFileNames = {"winword", "wordview"};
	local strDefaultExtension = "doc";

--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%                                                                %%
--%%   LOCAL VARIABLE DECLARATION                                   %%
--%%                                                                %%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	local bOK = true;
	local bIsValidViewer = false;
	local strDefaultViewer = "";
	local strVersion = "";
	local j = 0;
	local name = 0;
	local tFileInfo = {};
	local tSplit_Path = {};

--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%                                                                %%
--%%   SET DEFAULT VIEWER VERSION TO 0.0.0.0                        %%
--%%                                                                %%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	strVersion ="0.0.0.0";

--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%                                                                %%
--%%   GET THE DEFAULT VIEWER FOR THE SPECIFIED EXTENSION           %%
--%%                                                                %%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	strDefaultViewer = File.GetDefaultViewer(strDefaultExtension);

--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%                                                                %%
--%%   IF THERE IS NO DEFAULT VIEWER, SET VERSION TO 0.0.0.0        %%
--%%                                                                %%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (Application.GetLastError() ~= 0) then
		strVersion = "0.0.0.0";
		bOK = false;
	end

--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%                                                                %%
--%%   IF THERE IS A DEFAULT VIEWER, GET THE FILENAME AND COMPARE   %%
--%%   THE FILENAME TO THE TABLE OF VALID NAMES ABOVE.  IF THE      %%
--%%   NAME IS VALID, GET THE VERSION NUMBER                        %%
--%%                                                                %%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if bOK then
		--assume viewer is not valid filename
		bIsValidViewer = false;
	
		tSplit_Path = String.SplitPath(strDefaultViewer);
	        
		--check default viewer against valid filenames
		for j, name in pairs (tValidFileNames) do
			--compares a caseless strDefaultViewerFile to a list of valid names
			if String.CompareNoCase(tSplit_Path.Filename, name)==0 then
				bIsValidViewer = true;
			end
		end
	
		--if default reader is valid filename
		if bIsValidViewer then
			--get version info (if file does not exist, version string will be empty)
			tFileInfo = File.GetVersionInfo(strDefaultViewer);

			--default viewer does exist, store version number
			strVersion = tFileInfo.FileVersion;	      
		end
	end

--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%                                                                %%
--%%   IF THE DEFAULT FILENAME IS INVALID, ASSIGN VERSION 0.0.0.0   %%
--%%                                                                %%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if not bIsValidViewer then
		strVersion = "0.0.0.0";
	end

--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%                                                                %%
--%%   RETURN THE VERSION NUMBER OF THE DEFAULT VIEWER.             %%
--%%   IF THERE IS NO VIEWER, OR THE VIEWER FILENAME DOES NOT MATCH %%
--%%   ONE OF THE ABOVE LISTED FILENAMES, 0.0.0.0 WILL BE RETURNED  %%
--%%                                                                %%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	return strVersion;

end