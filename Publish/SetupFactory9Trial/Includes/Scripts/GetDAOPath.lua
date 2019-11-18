--[[
**********************************************************************************
Function:	GetDAOPath
Purpose:	Tests existance of values from "Software\\Microsoft\\Shared Tools\\"
			and if any exist, gets the value (which is a folder path).
Arguments:	None.
Returns:	Folder Path if exists, or "" if it does not exist.
**********************************************************************************
]]


function GetDAOPath()
    -- *****( DECLARE LOCAL VARIABLES )*****
    local strReturn = "";
    local tValuesToTest = {"DAO", "DAO360.dll", "DAO350.dll", "DAO350"};
    local tPath = {};
    
    -- *****( TRAVERSE THROUGH VALUE TABLE )*****
    for nIndex, Value in pairs (tValuesToTest) do
        -- *****( SEARCH FOR EXISTANCE OF REGISTRY VALUE )*****
        bKeyExists = Registry.DoesKeyExist(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Shared Tools\\" .. Value);
        
        -- *****( IF THAT REGISTRY VALUE EXISTS, GET PATH VALUE AND BREAK FROM LOOP )*****
        if (bKeyExists) then
            strReturn = Registry.GetValue(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Shared Tools\\" .. Value, "Path", false);
            break;
        end
    end
    -- *****( IF A VALUE WAS FOUND, DO THIS )*****
    if strReturn ~= "" then
        -- *****( CONVERT REGISTRY VALUE TO [Drive]:\\[Folder] FORMAT )*****
        tPath = String.SplitPath(strReturn);
        strReturn = tPath.Drive .. tPath.Folder;
        
        -- *****( CHECK IF THE FOLDER PATH FROM THE REGISTRY EXISTS )*****
        bFolderExists = Folder.DoesExist(strReturn);
        if not bFolderExists then
            strReturn = "";
        end
               
    end
    
     -- *****( RETURN DAO PATH.  IF DAO IS NOT INSTALLED, WILL RETURN "" )*****
    return strReturn;
    
end