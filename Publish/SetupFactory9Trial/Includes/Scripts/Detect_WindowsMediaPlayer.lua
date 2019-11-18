--[[
**********************************************************************************
Function:	ir_GetWMPVersion()
Purpose:	Detects Windows Media Player
Arguments:	None.
Returns:	Version number, 0.0.0.0 if the Windows Media Player ActiveX does not exist
**********************************************************************************
]]

--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%%                                                                 %%%
--%%%   This script tests for the presence of Windows Media Player.   %%%
--%%%   The registry key below exists for WMP Version 6.4+            %%%
--%%%                                                                 %%%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function ir_GetWMPVersion()
	local bOK = true;

	--We're checking for Media player 7.0+ at this point . . .
	bOK = Registry.DoesKeyExist(HKEY_CLASSES_ROOT,"CLSID\\{6BF52A52-394A-11d3-B153-00C04F79FAA6}\\InprocServer32");

	if (bOK) then
		strFileName = Registry.GetValue(HKEY_CLASSES_ROOT,"CLSID\\{6BF52A52-394A-11d3-B153-00C04F79FAA6}\\InprocServer32","NoName",true);
		if (Application.GetLastError() ~= 0) then	
			bOK = false;
		end
	else
	--version 7.0+ returned false, so we're now checking for version 6.4 . . . 
		bOK = Registry.DoesKeyExist(HKEY_CLASSES_ROOT,"CLSID\\{22D6F312-B0F6-11D0-94AB-0080C74C7E95}\\InprocServer32");
		
		if (bOK) then
			strFileName = Registry.GetValue(HKEY_CLASSES_ROOT,"CLSID\\{22D6F312-B0F6-11D0-94AB-0080C74C7E95}\\InprocServer32","NoName",true);
			if (Application.GetLastError() ~= 0) then	
				bOK = false;
			end
		end
	end


	--If a version was found, we now get the version information
	if (bOK) then
		verInfo = File.GetVersionInfo(strFileName);
		if(Application.GetLastError() ~= 0)then
			strVersion = "0.0.0.0";
		else
			-- OK, we have the file version
			strVersion = verInfo.FileVersion;
		end
	end
	
	--If there was no version found, bOK = false, and verison is set to "0.0.0.0"
	if (bOK ~= true) then
		strVersion = "0.0.0.0";
	end
	
	return strVersion;
end
