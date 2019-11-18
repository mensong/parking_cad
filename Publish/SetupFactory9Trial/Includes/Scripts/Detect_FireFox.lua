--[[
**********************************************************************************
Function:	ir_GetFireFoxVersion()
Purpose:	Detects Mozilla FireFox Web Browser
Arguments:	Boolean(optional). eg: ir_GetFireFoxVersion(true)
Returns:	Version number, 0.0.0.0 or 0.0.0 if Mozilla FireFox does not exist
		    Or a Table of All Three Version Values if 'bTable' switch is 'true'
		    ( {"0.0.0.0","0.0.0.0","0.0.0"} if Mozilla FireFox does not exist) 
**********************************************************************************
]]


--[[%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  %%%                                                                 %%%
  %%%   This script tests for the presence of Mozilla FireFox         %%%
  %%%   ( the Default code returns the User Friendly version )        %%%
  %%%                                                                 %%%
  %%%   example calls to this function at bottom of this script       %%%
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
]]


function ir_GetFireFoxVersion(bool)

	local bOK = false;
	local strVersion;
	
	local strVersion_Build;
	local strversion_Gecko;
	local strversion_Friendly;	

	--[[Note this function will return a Table of All Three version values If ' bTable ' boolean is 'true'
	    If is set to true it will ignore and override all of the other boolean switches and return a 'table' ]]

	local bTable;
	local tVersion_Info = {"0.0.0.0","0.0.0.0","0.0.0"};
	
	if ((bool ~= nil) and (bool == true)) then
		bTable = true;-- return a table of string values
	else
		bTable = false; -- default to return a string value	
	end

	--[[Only One of below 3 boolean values should be set to true,
	    the default code is to return the 'friendly build' ]]
	
	local bBuild = false; -- core build version
	local bGecko = false; -- gecko version
	local bFriendly = true; -- friendly version (default)
	
	-- check for the presence of FireFox regkey, determine subkey to access
	local FFkey = "Software\\Mozilla\\Mozilla Firefox";

	bOK = Registry.DoesKeyExist(HKEY_LOCAL_MACHINE,FFkey);

	if (bOK) then

		strVersion_Friendly = Registry.GetValue(HKEY_LOCAL_MACHINE, FFkey, "CurrentVersion"); --get friendly version
		strversion_Gecko = Registry.GetValue(HKEY_LOCAL_MACHINE, FFkey, ""); --get Gecko version
		if (Application.GetLastError() ~= 0) or (strVersion_Friendly == "") or (strversion_Gecko == "") then	
			bOK = false;

		else

		-- Detect for and get the version of the file (firefox.exe) here...

			local strFileName = Registry.GetValue(HKEY_LOCAL_MACHINE, FFkey.."\\"..strVersion_Friendly.."\\Main", "PathToExe");
			local tverInfo = File.GetVersionInfo(strFileName);
			if(Application.GetLastError() ~= 0) then
				bOK = false;
			else
				-- target file exists, now return the version based on the boolean(true) setting above

				if bTable then
					--return a table of all three version values (strings) - {core,gecko,friendly}
					local tVersion_Info = {tverInfo.FileVersion,strversion_Gecko,tverInfo.ProductVersion};
					return tVersion_Info;
				end
				-- otherwise return only one of the version values (string)  
				if bFriendly then
					--user friendly version (0.0.0 format eg: 3.6.13)
					return tverInfo.ProductVersion;
				end
				if bBuild then
					--core build version(0.0.0.0 format)
					return tverInfo.FileVersion;
				end
				if bGecko then
					--gecko version(0.0.0.0 format)
					return strversion_Gecko;
				end				

			end

		end
		
	end

	--If there was no version info found or an error occurred, set the version info to "zeros" by required format:
	if (bOK ~= true) then

		if (bTable == true) then
			return tVersion_Info;
		end		
						
		if bFriendly then
			strVersion = "0.0.0";
		else
			strVersion = "0.0.0.0";
		end

	end
	
	return strVersion; 
end


--[[
**********************************************************************************
Example calls:

local ffver = ir_GetFireFoxVersion(); -- (default)
Dialog.Message("Firefox Version",ffver);

local tffver = ir_GetFireFoxVersion(true); -- sets 'bTable' to 'true'
Dialog.Message("Firefox Version","Core: "..tffver[1].."\r\nGecko: "..tffver[2].."\r\nFriendly: "..tffver[3]);
 
**********************************************************************************
]]
