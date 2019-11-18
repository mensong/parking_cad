--[[
**********************************************************************************
Function:	ir_GetFlashAXVersion()
Purpose:	Detects the Flash ActiveX Control
Arguments:	None.
Returns:	Version number, 0.0.0.0 if Flash ActiveX Control does not exist
**********************************************************************************
]]




--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%%                                                                 %%%
--%%%   This script tests for the presence of Flash ActiveX           %%%
--%%%                                                                 %%%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function ir_GetFlashAXVersion()
	local bOK = true;	
	local strVersion = "0.0.0.0";
	bOK = Registry.DoesKeyExist(HKEY_CLASSES_ROOT,"CLSID\\{D27CDB6E-AE6D-11cf-96B8-444553540000}\\InprocServer32");

	
	if (bOK) then
		strFileName = Registry.GetValue(HKEY_CLASSES_ROOT,"CLSID\\{D27CDB6E-AE6D-11cf-96B8-444553540000}\\InprocServer32","NoName",true);
		if (Application.GetLastError() ~= 0) then	
			bOK = false;
		end
	end
	
	if (bOK) then
		-- Detect the version of the file here...
		verInfo = File.GetVersionInfo(strFileName);
		if(Application.GetLastError() ~= 0)then
			strVersion = "0.0.0.0";
		else
			-- OK, we have the file version
			strVersion = verInfo.FileVersion;
		end
	end
	
	if (bOK ~= true) then
		strVersion = "0.0.0.0";
	end
	
	return strVersion;
end