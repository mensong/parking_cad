--[[	
**********************************************************************************
Function:	g_ConfirmFreeSpaceOverride
Purpose:	Warns the user that there isn't enough space on the selected drive and
			asks if they really want to continue with the installation.
Arguments:	None.
Returns:	(boolean) true if the user wants to continue or false if not
**********************************************************************************
--]]
function g_ConfirmFreeSpaceOverride()
	local strTitle = SetupData.GetLocalizedString("MSG_WARNING");
	local strPrompt = SetupData.GetLocalizedString("MSG_NOT_ENOUGH_FREE_SPACE")
			  .."\r\n\r\n"
			  ..SetupData.GetLocalizedString("MSG_CONFIRM_CONTINUE");
	strPrompt = SessionVar.Expand(strPrompt);

	local nResult = Dialog.Message(strTitle, strPrompt, MB_YESNO, MB_ICONEXCLAMATION, MB_DEFBUTTON2);
	return (nResult == IDYES);
end


--[[	
**********************************************************************************
Function:	g_ConfirmSetupAbort
Purpose:	Shows a message to the user that confirms that they want to abort the 
			setup.  Called from most screen's On Cancel event by default.
Arguments:	None.
Returns:	(boolean) true if the user wants to abort the setup or false if not
**********************************************************************************
--]]
function g_ConfirmSetupAbort()
	local strTitle = SetupData.GetLocalizedString("MSG_CONFIRM");
	local strPrompt = SetupData.GetLocalizedString("MSG_CONFIRM_ABORT");
	local nResult = Dialog.Message(strTitle, strPrompt, MB_YESNO, MB_ICONQUESTION, MB_DEFBUTTON2);
	return (nResult == IDYES);
end


--[[	
**********************************************************************************
Function:	g_ContainsValidPathChars
Purpose:	Checks whether a string contains valid path characters, which is
			anything EXCEPT these characters: / * ? " < > |
Arguments:	(string) strText - The string to validate
Returns:	(boolean) true if the string is valid, false if it contains any invalid characters
**********************************************************************************
--]]
function g_ContainsValidPathChars(strText)
	-- note: this uses the internal lua function string.find which supports
	--       pattern matching
	return string.find(strText, "[/%*%?\"<>|]") == nil;
end


--[[	
**********************************************************************************
Function:	g_ContainsValidRelativePathChars
Purpose:	Checks whether a string contains valid relative path characters, which is
			anything EXCEPT these characters: / * ? " < > | :
Arguments:	(string) strText - The string to validate
Returns:	(boolean) true if the string is valid, false if it contains any invalid characters
**********************************************************************************
--]]
function g_ContainsValidRelativePathChars(strText)
	-- note: this uses the internal lua function string.find which supports
	--       pattern matching
	return string.find(strText, "[/%*%?\"<>|:]") == nil;
end


--[[	
**********************************************************************************
Function:	g_EditFieldFolderBrowse
Purpose:	Browses for a folder and fills an edit field with the path. It uses
			the edit field's text as the initial folder in the folder browse dialog.
Arguments:	(number) nIDEditField - The ID of the edit field
			(string) strPrompt - The prompt for the folder browse dialog
Returns:	Nothing.
**********************************************************************************
--]]
function g_EditFieldFolderBrowse(nIDEditField, strPrompt)

	-- get the current properties of the edit field
	local tbEditProps = DlgEditField.GetProperties(nIDEditField);
	if(not tbEditProps) then
		-- The edit field is not accessible or does not exist
		return;
	end

	-- display a folder browse dialog, using the current contents of the edit
	-- field as the initial folder path (the folder to start browsing from)
	local strInitialFolder = tbEditProps.Text;
	local strTargetFolder = Dialog.FolderBrowse(strPrompt, strInitialFolder);
	if((strTargetFolder == "") or (strTargetFolder == "CANCEL")) then
		return;
	end
	
	-- replace the contents of the edit field with the folder path that was selected
	tbEditProps.Text = strTargetFolder;
	DlgEditField.SetProperties(nIDEditField, tbEditProps);
end


--[[	
**********************************************************************************
Function:	g_EditFieldIsEmpty
Purpose:	Determine whether there is any text in an edit field.
Arguments:	(number) nIDEditField - The ID of the edit field control.
Returns:	true if the edit field is empty, or false if there is text in it.
**********************************************************************************
--]]
function g_EditFieldIsEmpty(nIDEditField)
	return String.Length((DlgEditField.GetProperties(nIDEditField).Text)) == 0;
end


--[[	
**********************************************************************************
Function:	g_FillComboBoxWithDriveDisplayNames
Purpose:	Fills a combo box with a list of drives, using their display names if
			possible, and either selects a specific item by index or (if none is specified) 
			re-selects the previously selected item.
Arguments:	(number) nCtrlID - The ID of the combo box control.
			(table) tbDrives - A numerically indexed table containing the drive letters you want to add.
			(number) nSelect - The index of the item you want selected afterwards. (optional)
Returns:	Nothing.
**********************************************************************************
--]]
function g_FillComboBoxWithDriveDisplayNames(nCtrlID, tbDrives, nSelect)

	local tbProps = DlgComboBox.GetProperties(CTRL_COMBOBOX_01);
	if(not tbProps) then
		return;
	end

	-- if no selection index was passed, 
	-- then get the last index that was selected
	if(not nSelect) then
		nSelect = tbProps.Selected;
		
		Debug.Print("nSelect = "..nSelect.."\r\n");
		if(nSelect < 1) then
			nSelect = 1;
		end
		Debug.Print("nSelect = "..nSelect.."\r\n");
	end

	-- empty the combo box
	DlgComboBox.RemoveItem(nCtrlID, -1);

	-- fill the combo box with the drives in tbDrives,
	-- using each drive's display name if possible
	for i, strDrive in pairs (tbDrives) do
	
		local tbInfo = Drive.GetInformation(strDrive);
		local strItem;
		if(tbInfo) then
			-- use the drive's display name
			strItem = tbInfo.DisplayName;
		else
			-- couldn't get the drive's info, so just use the drive letter & colon, e.g. 'C:'
			strItem = String.TrimRight(strDrive, "\\");
		end

		local nIndex = DlgComboBox.AddItem(nCtrlID, strItem);
		DlgComboBox.SetItemData(nCtrlID, nIndex, strDrive);
	end
	
	-- select the appropriate item
	DlgComboBox.SetProperties(nCtrlID, {Selected=nSelect});
end


--[[	
**********************************************************************************
Function:	g_FillComboBoxWithShortcutFolders
Purpose:	Fills the specified combo box control with the existing Start Menu >
			Programs folders, taking the current user profile setting into account.
Arguments:	(number) nComboBoxID - The ID of the combo box.
Returns:	Nothing.
**********************************************************************************
--]]
function g_FillComboBoxWithShortcutFolders(nComboBoxID)
	
	local tbProps = DlgComboBox.GetProperties(nComboBoxID);
	if not tbProps then
		return;
	end

	local strReselect = tbProps.Text;
	
	-- empty the list
	DlgComboBox.RemoveItem(nComboBoxID,-1);
	
	local strBaseFolder;
	if _UsePerUserFolders then
		strBaseFolder = SessionVar.Expand("%StartProgramsFolder%");
	else
		strBaseFolder = SessionVar.Expand("%StartProgramsFolderCommon%");
	end
	local nBaseFolderLength = String.Length(strBaseFolder);
	local tbFolders = Folder.Find(strBaseFolder, "*", true);
	
	if tbFolders then
		-- add all of the folder names to the list
		for i, strFolder in pairs (tbFolders) do
			local nFolderLength = String.Length(strFolder);
			local strFolderName = String.Right(strFolder, (nFolderLength - nBaseFolderLength));
			strFolderName = String.TrimLeft(strFolderName, "\\");
			
			-- insert the item at the end of the list
			DlgComboBox.InsertItem(nComboBoxID, -1, strFolderName);
		end
	end
	
	DlgComboBox.SetProperties(nComboBoxID, {Text=strReselect});
end


--[[	
**********************************************************************************
Function:	g_GetDriveLetters
Purpose:	Fills a table with the drive letters (e.g. 'C:') for all of the drives
			that match a specific list of drive types, or for all of the drives on
			the system if no list of drive types is specified.
Arguments:	Call this function with the list of drive types you want to include
			in the table, e.g. g_GetTableOfDriveLetters(DRIVE_FIXED, DRIVE_REMOTE)
Returns:	(table) A numerically indexed table of drive letters.
**********************************************************************************
--]]
function g_GetDriveLetters(...)
	local tbDrives = {};
	local nDriveCount = 0;
	local tbAllDrives = Drive.Enumerate();
	for driveIdx, strDrive in pairs (tbAllDrives) do
	
		-- remove the trailing backslash (e.g. change 'C:\' to 'C:')
		strDrive = String.TrimRight(strDrive, "\\");

		if(arg.n > 0) then
			local nType = Drive.GetType(strDrive);
			for argIdx, argValue in ipairs(arg) do
				if(nType == argValue) then
					nDriveCount = nDriveCount + 1;
					tbDrives[nDriveCount] = strDrive;
					break;
				end		
			end
		else
			nDriveCount = nDriveCount + 1;
			tbDrives[nDriveCount] = strDrive;
		end
	end

	return tbDrives;
end


--[[	
**********************************************************************************
Function:	g_GetFreeSpaceInBytes
Purpose:	Calculates the amount of disk space available on the specified drive.
Arguments:	(string) strDrive - A string whose first character is the drive letter
							    e.g. C:, C:\Program Files\Your Product
Returns:	The amount of free space on the drive, in bytes.
**********************************************************************************
--]]
function g_GetFreeSpaceInBytes(strDrive)
	local nFreeSpace = Drive.GetFreeSpace(strDrive);
	if(nFreeSpace ~= -1) then
		-- convert from MB to bytes
		nFreeSpace = nFreeSpace * 1024 * 1024;
	else
		nFreeSpace = 0;
	end
	return nFreeSpace;
end


--[[	
**********************************************************************************
Function:	g_GetLocalizedNumericChars
Purpose:	Get a string containing the valid numeric characters for the current locale.
Arguments:	None.
Returns:	A string containing the valid numeric characters for the current locale.
**********************************************************************************
--]]
function g_GetLocalizedNumericChars()
	local strNumericChars = Registry.GetValue(HKEY_CURRENT_USER,"Control Panel\\International","sMonDecimalSep")
	                      ..Registry.GetValue(HKEY_CURRENT_USER,"Control Panel\\International","sPositiveSign")
	                      ..Registry.GetValue(HKEY_CURRENT_USER,"Control Panel\\International","sNegativeSign")
	                      ..Registry.GetValue(HKEY_CURRENT_USER,"Control Panel\\International","sNativeDigits");
	
	return strNumericChars;
end


--[[	
**********************************************************************************
Function:	g_HandleSystemReboot
Purpose:	Reboots the system if necessary. Usually called as the last command in
			the "On Shutdown" event.
Arguments:	None.
Returns:	None.
**********************************************************************************
--]]
function g_HandleSystemReboot()
	if(_NeedsReboot) then
		if(_SilentInstall) then
			-- Always reboot on silent install, if needed
			System.Reboot();
		else
			-- Ask the user if they want to reboot now
			local strMsg = "";
			if(_DoingUninstall)then
				strMsg = SetupData.GetLocalizedString("MSG_REBOOT_NEEDED_UNINSTALL");
			else
				strMsg = SetupData.GetLocalizedString("MSG_REBOOT_NEEDED");
			end
			local nResult = Dialog.Message(SetupData.GetLocalizedString("MSG_NOTICE"),strMsg,MB_YESNO,MB_ICONQUESTION);
			if(nResult == IDYES)then
				System.Reboot();
			end
		end							
	end
end


--[[	
**********************************************************************************
Function:	g_IsSerialNumberInList
Purpose:	Determines whether a serial number is valid by searching through a
			specific serial number list or all serial number lists in the project.
Arguments:	(string) strSerial - the serial number you want to validate
			(string) strListName - the name of the list you want to search in,
			                       or nil if you want to search through all lists
Returns:	true if the serial is valid, false if it isn't.
**********************************************************************************
--]]
-- search through the specified serial number list for a match
function g_IsSerialNumberInList(strSerial, strListName)

	if (strListName == nil) then

		-- get the names of all available serial number lists
		local tbSerialLists = SetupData.GetSerialListNames();
		
		-- check the serial number against each available list
		-- and return true if we find a match in any of them
		for i, list_name in pairs (tbSerialLists) do
		    if SetupData.IsValidSerialNumber(list_name, strSerial) then
				-- serial number is valid		        
		        return true;
		    end
		end
	
		-- if we made it this far, the serial number wasn't found in any of the lists	
		return false;
		
	else

		-- check the serial number against the specified list
		return SetupData.IsValidSerialNumber(strListName,strSerial);

	end
end


--[[
**********************************************************************************
Function:	g_IsValidPath
Purpose:	Determines whether a string contains a valid, fully-qualified path.
            Note: checks format of path string for proper UNC or DOS formatting,
                  but does NOT verify whether path exists.
Arguments:	(string) strPath - the path you want to validate
Returns:	true if the path format is valid, false if it isn't.
**********************************************************************************
]]
function g_IsValidPath(strPath)

	-- note: this uses the internal lua function string.find which supports
	--       pattern matching
	
	--[[ check for UNC paths (\\ followed by at least one valid path character)
	  
	^ 					must be at beginning of string
	\\\\ 				2 slashes, escaped (\\)
	[^/%*%?\"<>|:\\]	any valid path character except a backslash, i.e. anything EXCEPT these characters:	/ * ? " < > | : \ 
	[^/%*%?\"<>|:]+ 	one or more occurrences of any valid path character, i.e. anything EXCEPT these characters: / * ? " < > | :
	$					must go to end of string
	
	]]
	if string.find(strPath, "^\\\\[^/%*%?\"<>|:\\][^/%*%?\"<>|:]+$") then
		-- also check that there are no double backslashes in the path beyond the first two
		return string.find(strPath, "\\\\", 3) == nil;
	end

	--[[ check for DOS paths (a letter followed by a : followed by a \)
	  
	^ 					must be at beginning of string
	%a 					any single letter (uppercase or lowercase)
	\\ 					1 backslash, escaped (\)
	[^/%*%?\"<>|:]* 	zero or more occurrences of any valid path character, i.e. anything EXCEPT these characters: / * ? " < > | :
	$					must go to end of string
	
	]]
	if(string.find(strPath, "^%a:\\[^/%*%?\"<>|:]*$") ~= nil) then
		-- also check that there are no double backslashes in the path
		return string.find(strPath, "\\\\", 3) == nil;
	end
	
	-- not a fully qualified path, whatever it is
	return false;
end


--[[	
**********************************************************************************
Function:	g_LicenseAgreementScreen_UpdateNextButton
Purpose:	Enables the Next button on the License Agreement screen if the user has
			the 'I agree' option selected.
Arguments:	None.
Returns:	Nothing.
**********************************************************************************
--]]
function g_LicenseAgreementScreen_UpdateNextButton()
	local bEnableNext = false;
	local tbProps = DlgRadioButton.GetProperties(CTRL_RADIOBTN_AGREE);
	if(tbProps) then
		bEnableNext = tbProps.Checked;
	end
	
	DlgButton.SetProperties(CTRL_BUTTON_NEXT, {Enabled=bEnableNext});
end


--[[	
**********************************************************************************
Function:	g_UpdateStaticTextCtrl
Purpose:	Updates the text in a static text control, expanding any session variables in it.
Arguments:	(number) nCtrlID - The ID of the static text control to update.
          	(string) strStringID - The message ID for the text that appears in 
          						   the static text control, i.e. the original
          						   text that contains the session variable(s).
Returns:	Nothing.
**********************************************************************************
--]]
function g_UpdateStaticTextCtrl(nCtrlID, strStringID)
	local tbProps = DlgStaticText.GetProperties(nCtrlID);
	if tbProps then
		tbProps.Text = Screen.GetLocalizedString(strStringID);
		tbProps.Text = SessionVar.Expand(tbProps.Text);
		DlgStaticText.SetProperties(nCtrlID, tbProps);
	end
end


--[[	
**********************************************************************************
Function:	g_ValidateEditField
Purpose:	Perform some simple validation on an input object.
Arguments:	(number) nIDEditField - The ID of the edit field control.
          	(number) nMinChars - The minimum number of characters required.
          	(number) nMaxChars - The maximum number of characters allowed. (nil for no limit)
          	(string) strValidChars - a string containing the list of valid characters, or nil
          	                         if no character checking should be performed.
									 Note: To test for numeric characters, use the
									       g_GetLocalizedNumericChars function to get the 
									       valid numeric characters for the current locale.
Returns:	(boolean) true if the edit field is meets the criteria, false if it doesn't.
**********************************************************************************
--]]
function g_ValidateEditField(nIDEditField, nMinChars, nMaxChars, strValidChars)
	local strText = DlgEditField.GetProperties(nIDEditField).Text;
	local nLen = String.Length(strText);
	
	if(nLen < nMinChars) then
		return false;
	end
	
	if(nMaxChars and (nLen > nMaxChars)) then
		return false;
	end

	if(strValidChars) then
		if(String.TrimRight(strText, strValidChars) ~= "") then
			-- the string contains characters not in strValidChars
			return false;
		end
	end

	return true;
end

--[[	
**********************************************************************************
Function:	g_OnRegisterFileFailed
Purpose:	Called from the setup when a file fails COM or TypeLib registration.
Arguments:	(number) nRegType - The type of registration. 0 = COM (DllRegisterServer), 1 = TypeLib
          	(string) strFilename - The full path and filename of the file that failed registration.
          	(string) strErrorMsg - The translated error message.
          	(number) nErrorCode - The error code.
Returns:	(boolean) true if the setup should continue or false to abort
**********************************************************************************
--]]
function g_OnRegisterFileFailed(nRegType, strFilename, strErrorMsg, nErrorCode)
	local strMessage = "";
	
	if(nRegType == 0)then
		strMessage = SetupData.GetLocalizedString("ERR_REGISTER_COM");
	else
		strMessage = SetupData.GetLocalizedString("ERR_REGISTER_TLB");
	end
	
	strMessage = strMessage.." "..strFilename.."\r\n"..strErrorMsg.." ("..nErrorCode..")";
	
	if(not _SilentInstall)then
		Dialog.Message(SetupData.GetLocalizedString("MSG_NOTICE"),strMessage,MB_OK,MB_ICONEXCLAMATION);
	end

	-- Continue with the installation.  Change to false to abort the install.
	return true;
end