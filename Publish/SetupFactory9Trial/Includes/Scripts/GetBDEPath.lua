--[[
**********************************************************************************
Function:	GetBDEPath
Purpose:	Tests existance of BDE by paging the registry value
			"HKEY_LOCAL_MACHINE\\Software\\Borland\\Database Engine\\DLLPATH" and
			if it exists, gets the value (which is a folder path).
Arguments:	None.
Returns:	Folder Path if exists, or "" if it does not exist.
**********************************************************************************
]]


function GetBDEPath()
	-- *****( DECLARE LOCAL VARIABLES )*****
	local strReturn = "";
	local bFolderExists = false;
    local bKeyExists = Registry.DoesKeyExist(HKEY_LOCAL_MACHINE, "Software\\Borland\\Database Engine");
    local tPath = {};
    
    -- *****( CHECK IF THE REGISTRY KEY EXISTS )*****
    if bKeyExists then
    	-- *****( GET THE VALUE FROM THE REGISTRY KEY )*****
    	strReturn = Registry.GetValue(HKEY_LOCAL_MACHINE, "Software\\Borland\\Database Engine", "DLLPATH", false);
    	-- *****( CHECK IF THE VALUE RETRIEVED IS NOT AN EMPTY STRING )*****
    	if strReturn ~= "" then
    		-- *****( CONVERT REGISTRY VALUE TO [Drive]:\\[Folder] FORMAT )*****
    		tPath = String.SplitPath(strReturn);
        	strReturn = tPath.Drive .. tPath.Folder;
        	
        	-- *****( CHECK IF THE FOLDER PATH FROM THE REGISTRY EXISTS )*****
    		bFolderExists = Folder.DoesExist(strReturn);
    		if not bFolderExists then
    			Dialog.Message("", "DOESN'T EXIST!");
    			-- *****( FOLDER DOES NOT EXIST, RETURN AN EMPTY STRING )*****
    			strReturn = "";
    		end
    	end
   	end

    -- *****( RETURN BDE PATH.  IF BDE IS NOT INSTALLED, WILL RETURN "" )*****
    return strReturn;

end
