--[[
**********************************************************************************
Function:	ir_GetRealPlayerVersion()
Purpose:	Detects RealPlayer ActiveX
Arguments:	None.
Returns:	Version number, 0.0.0.0 if RealPlayer ActiveX does not exist
**********************************************************************************
]]

--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%%                                                                 %%%
--%%%   This script tests for the presence of RealPlayer ActiveX      %%%
--%%%                                                                 %%%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function ir_GetRealPlayerVersion()
	local bOK = true;

	--We're checking for Quicktime at this point . . .
	bOK = Registry.DoesKeyExist(HKEY_CLASSES_ROOT,"CLSID\\{CFCDAA03-8BE4-11CF-B84B-0020AFBBCCFA}\\InprocServer32");

	if (bOK) then
		strFileName = Registry.GetValue(HKEY_CLASSES_ROOT,"CLSID\\{CFCDAA03-8BE4-11CF-B84B-0020AFBBCCFA}\\InprocServer32","NoName",true);
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