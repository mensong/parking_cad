--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
--%%%                                                                 %%%
--%%%   This script tests for the presence of Internet Explorer       %%%
--%%%                                                                 %%%
--%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

--[[
**********************************************************************************
Function:	ir_GetIEVersion()
Purpose:	Detects Internet Explorer
Arguments:	None.
Returns:	Version number, 0.0.0.0 if Internet Explorer does not exist
**********************************************************************************
]]


function ir_GetIEVersion()
	local bOK = true;
	local strVersion = "0.0.0.0";
	
	-- Check to ensure key exists
	bOK = Registry.DoesKeyExist(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Internet Explorer");
	
	if (bOK) then
		-- Get version of IE (4 and above)
		strVersion = Registry.GetValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Internet Explorer","Version",true);
		if (Application.GetLastError() ~= 0) then	
			-- Version 3 and lower are stored in a different reg value
			strVersion = Registry.GetValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Internet Explorer","IVer",true);
			if (Application.GetLastError() ~= 0) then	
				bOK = false;
			else
				-- assign correct version number based on value in registry
				if strVersion == "103" then
					strVersion = "3.0";
				elseif strVersion == "102" then
					strVersion = "2.5";
				elseif strVersion == "101" then
					strVersion = "2.0";
				elseif strVersion == "100" then
					strVersion = "1.0";
				end
			end
		end
	end
	return strVersion;
end