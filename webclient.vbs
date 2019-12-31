Function ReadFile(filepath) 
	On Error Resume Next
    Set fs = CreateObject("Scripting.FileSystemObject") 
    Set file = fs.OpenTextFile(filepath, 1, false) 
    ReadFile = file.ReadAll 
    file.Close
    set fs = Nothing 
end Function

Sub GetUrl(url)
	On Error Resume Next
	Set xPost = CreateObject("Microsoft.XMLHTTP")
	xPost.Open "GET", url, True
	xPost.Send
	WScript.sleep 2000
	Set xPost = Nothing
End Sub

Sub PostUrl(url, data)
	On Error Resume Next
	Set xPost = CreateObject("Microsoft.XMLHTTP")
	xPost.Open "POST", url, True
	xPost.Send data
	WScript.sleep 2000
	Set xPost = Nothing
End Sub

Set objArgs = WScript.Arguments
If objArgs.Count = 1 Then
	GetUrl(objArgs(0))
End If

If objArgs.Count = 2 Then
	Dim log
	log = ReadFile(objArgs(1))
	log = log & vbCrLf
	log = Replace(log, "\", "\\")
	log = Replace(log, """", "\""")
	log = Replace(log, vbCrLf, "\\r\\n")
	log = Replace(log, vbCr, "\\r")
	log = Replace(log, vbTab, "\\t")
	log = Replace(log, Chr(8), "\\b")
	Dim json
	json = "{""name"":""log"", ""value"":""" + log + """}"
	'MsgBox json
	PostUrl objArgs(0), json
End If