-- ****( INITIALIZE TABLE )****
TextFileEx = {};


-- %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
-- Name:	TextFileEx.CountLines
-- Purpose:	Counts the lines in a specified text file
-- Values:	sFile = string, full path to file
-- Returns:	Number of lines in specified text file, or nil on fail
-- %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
TextFileEx.CountLines = function (sFile)
	-- ****( DECLARE LOCAL VARIABLES )****
	local tTextFileContents = {};
	local bExists = false;
	local nLines = 0;
	-- ****( CHECK IF FILE EXISTS )****
	bExists = File.DoesExist(sFile);
	if bExists then
		-- ****( THE FILE EXISTS, READ IT INTO A TABLE )****
		tTextFileContents = TextFile.ReadToTable(sFile);
		-- ****( COUNT THE NUMBER OF LINES IN THE TABLE )****
		nLines = Table.Count(tTextFileContents);
	else
		-- ****( THE FILE DID NOT EXIST, SET #LINES TO NIL )****
		nLines = nil;
	end
	-- ****( RETURN THE NUMBER OF LINES, NIL ON FAIL )****
	return nLines;
end	


-- %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
-- Name:	TextFileEx.DeleteLine
-- Purpose:	Removes a specific line from a specified text file
-- Values:	sFile = string, full path to file
--			nLine = number, line number to remove
-- Returns:	Contents of removed line, or nil on fail
-- %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
TextFileEx.DeleteLine = function (sFile, nLine)
	-- ****( DECLARE LOCAL VARIABLES )****
	local tTextFileContents = {};
	local bExists = false;
	local sRemoved = "";
	-- ****( CHECK IF FILE EXISTS )****
	bExists = File.DoesExist(sFile);
	if bExists then
		-- ****( THE FILE EXISTS, LOAD IT INTO A TABLE )****
		tTextFileContents = TextFile.ReadToTable(sFile);
		-- ****( CHECK THAT LINE TO REMOVE EXISTS IN TEXT FILE )****
		if nLine > Table.Count(tTextFileContents)  then
			-- ****( LINE DOESN'T EXIST, SET REMOVED TO NIL )****
			sRemoved = nil;
		else
			-- ****( LINE EXISTS, REMOVE FROM TABLE, WRITE TABLE TO FILE )****
			sRemoved = Table.Remove(tTextFileContents, nLine);
			TextFile.WriteFromTable(sFile, tTextFileContents, false);
		end
	else
		-- ****( THE FILE DOESN'T EXIST, SET REMOVED TO NIL )****
		sRemoved = nil;
	end
	-- ****( RETURN THE REMOVED LINE, OR NIL IF FAIL )****
	return sRemoved;
end	


-- %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
-- Name:	TextFileEx.FindLine
-- Purpose:	Find the first line number where a string is in a file
-- Values:	sFile = string, full path to file
--			sSearchFor = string, to search for
--			nStartLine = number, line number to start at
-- Returns:	Location of found string, or nil on fail
-- %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
TextFileEx.FindLine = function (sFile, sSearchFor, nStartLine)
	-- ****( DECLARE LOCAL VARIABLES )****
	local tTextFileContents = {};
	local bExists = false;
	local nFoundAt = nil;
	-- ****( CHECK IF FILE EXISTS )****
	bExists = File.DoesExist(sFile);
	if bExists then
		-- ****( FILE EXISTS, READ INTO TABLE )****
		tTextFileContents = TextFile.ReadToTable(sFile);
		-- ****( GET THE NUMEBR OF LINES FROM THE TABLE )****
		nLines = Table.Count(tTextFileContents);
		-- ****( IF THE LINE TO START AT IS > THAN TOTAL LINES, RETURN NIL )****
		if (nStartLine > nLines)  then
			nFoundAt = nil;
		else
			-- ****( TRAVERSE THROUGH TABLE FROM STARTLINE TO END, OR UNTIL FOUND )****
			for Count = nStartLine, nLines do
				-- ****( AT CURRENT LINE, CHECK IF MATCHES REQUESTED STRING )****
				if String.CompareNoCase(tTextFileContents[Count], sSearchFor) == 0 then
					-- ****( STRING WAS FOUND, SET VARIABLE AND BREAK LOOP )****
					nFoundAt = Count;
					break;
				end
			end
		end
	else
		-- ****( FILE DID NOT EXIST, RETURN NIL )****
		nFoundAt = nil;
	end
	-- ****( RETURN LOCATION OF FOUND STRING, OR NIL ON FAIL )****
	return nFoundAt;
end	


-- %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
-- Name:	TextFileEx.GetLine
-- Purpose:	Retrieves contents of specific line from textfile
-- Values:	sFile = string, full path to file
-- 			nLine = number, line number to retrieve
-- Returns:	Contents of line requested, or nil on fail
-- %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
TextFileEx.GetLine = function (sFile, nLine)
	-- ****( DECLARE LOCAL VARIABLES )****
	local tTextFileContents = {};
	local bExists = false;
	local sLineContents
	-- ****( CHECK IF FILE EXISTS )****
	bExists = File.DoesExist(sFile);
	if bExists then
		-- ****( FILE EXISTS, READ INTO TABLE )****
		tTextFileContents = TextFile.ReadToTable(sFile);
		-- ****( CHECK IF LINE REQUESTED > TOTAL LINES )****
		if (nLine > Table.Count(tTextFileContents))  then
			-- ****( LINE REQUESTED IS > TOTAL LINES, RETURN NIL )****
			sLineContents = nil;
		else
			-- ****( LINE REQUESTED EXISTS, GET CONTENTS )****
			sLineContents = tTextFileContents[nLine];
		end
	else
		-- ****( FILE DID NOT EXIST, RETURN NIL )****
		sLineContents = nil;
	end
	-- ****( RETURN CONTENTS OF LINE, OR ON FAIL RETURN NIL )****
	return sLineContents;
end	


-- %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
-- Name:	TextFileEx.InsertLine
-- Purpose:	Inserts string into specific location in textfile
-- Values:	sFile = string, full path to file
-- 			nLine = number, location to insert string into
--			sToInsert = string, contents to insert into file
-- Returns:	True if successful, False if fail
-- %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
TextFileEx.InsertLine = function (sFile, nLine, sToInsert)
	-- ****( DECLARE LOCAL VARIABLES )****
	local tTextFileContents = {};
	local bExists = false;
	local bReturn = false;
	-- ****( CHECK IF FILE EXISTS )****
	bExists = File.DoesExist(sFile);
	if bExists then
		-- ****( FILE EXISTS, READ INTO TABLE )****
		tTextFileContents = TextFile.ReadToTable(sFile);
		-- ****( GET THE NUMBER OF LINES FROM THE TABLE )****
		nLines = Table.Count(tTextFileContents);
		-- ****( CHECK IF REQUESTED LINE EXCEEDS LINE COUNT )****
		if (nLine > nLines)  then
			-- ****( REQUESTED LINE EXCEEDS, INSERT AT END OF TABLE, WRITE FILE )****
			nLine =  nLines + 1;
			Table.Insert(tTextFileContents, nLine, sToInsert);
			TextFile.WriteFromTable(sFile, tTextFileContents, false);
			bReturn = true;
		else
			-- ****( INSERT LINE AT SPECIFIED POSITION, WRITE FILE )****
			Table.Insert(tTextFileContents, nLine, sToInsert);
			TextFile.WriteFromTable(sFile, tTextFileContents, false);
			bReturn = true;
		end
	else
		-- ****( FILE DID NOT EXIST, RETURN FALSE )****
		bReturn = false;
	end
	-- ****( RETURN TRUE IF OK, FALSE IF FAIL )****
	return bReturn
end	