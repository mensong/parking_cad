--[[
**********************************************************************************
Function:	ir_GetQuicktimeVersion()
Purpose:	Detects Quicktime ActiveX
Arguments:	None.
Returns:	Version number, 0.0.0.0 if Quicktime ActiveX does not exist
**********************************************************************************
]]


--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%%                                                                 %%%
--%%%   This script tests for the presence of Quicktime ActiveX       %%%
--%%%                                                                 %%%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function ir_GetQuicktimeVersion()
	local bOK = true;

	--We're checking for Quicktime at this point . . .
	bOK = Registry.DoesKeyExist(HKEY_CLASSES_ROOT,"CLSID\\{02BF25D5-8C17-4B23-BC80-D3488ABDDC6B}\\InprocServer32");

	if (bOK) then
		strFileName = Registry.GetValue(HKEY_CLASSES_ROOT,"CLSID\\{02BF25D5-8C17-4B23-BC80-D3488ABDDC6B}\\InprocServer32","NoName",true);
		if (Application.GetLastError() ~= 0) then	
			bOK = false;
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